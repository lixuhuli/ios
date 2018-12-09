#include "StdAfx.h"
#include "TaskUnzipFile.h"
#include <PublicLib\DeCompress.h>
#include "ParamDefine.h"
#include "GlobalData.h"

#include "CPP/Common/IntToString.h"
#include "CPP/Common/MyInitGuid.h"
#include "CPP/Common/StringConvert.h"

#include "CPP/Windows/DLL.h"
#include "CPP/Windows/FileDir.h"
#include "CPP/Windows/FileFind.h"
#include "CPP/Windows/FileName.h"
#include "CPP/Windows/NtCheck.h"
#include "CPP/Windows/PropVariant.h"
#include "CPP/Windows/PropVariantConv.h"

#include "CPP/7zip/Common/FileStreams.h"
#include "CPP/7zip/Archive/IArchive.h"
#include "CPP/7zip/IPassword.h"

using namespace NWindows;

DEFINE_GUID(CLSID_CFormat7z, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00);
//////////////////////////////////////////////////////////////
// Archive Extracting callback class


static HRESULT IsArchiveItemProp(IInArchive *archive, UInt32 index, PROPID propID, bool &result)
{
    NCOM::CPropVariant prop;
    RINOK(archive->GetProperty(index, propID, &prop));
    if (prop.vt == VT_BOOL){
        result = VARIANT_BOOLToBool(prop.boolVal);
    }
    else if (prop.vt == VT_EMPTY){
        result = false;
    }
    else{
        return E_FAIL;
    }
    return S_OK;
}

static HRESULT IsArchiveItemFolder(IInArchive *archive, UInt32 index, bool &result)
{
    return IsArchiveItemProp(archive, index, kpidIsDir, result);
}



//////////////////////////////////////////////////////////////
// Archive Open callback class
class CArchiveOpenCallback: public IArchiveOpenCallback, public ICryptoGetTextPassword, public CMyUnknownImp
{
public:
    MY_UNKNOWN_IMP1(ICryptoGetTextPassword)


        STDMETHOD(SetTotal)(const UInt64 *files, const UInt64 *bytes);
    STDMETHOD(SetCompleted)(const UInt64 *files, const UInt64 *bytes);
    STDMETHOD(CryptoGetTextPassword)(BSTR *password);

    bool PasswordIsDefined;
    UString Password;

    CArchiveOpenCallback() : PasswordIsDefined(false) {}
};

STDMETHODIMP CArchiveOpenCallback::SetTotal(const UInt64 * /* files */, const UInt64 * /* bytes */)
{
    return S_OK;
}

STDMETHODIMP CArchiveOpenCallback::SetCompleted(const UInt64 * /* files */, const UInt64 * /* bytes */)
{
    return S_OK;
}

STDMETHODIMP CArchiveOpenCallback::CryptoGetTextPassword(BSTR *password)
{
    if (!PasswordIsDefined){
        // You can ask real password here from user
        // Password = GetPassword(OutStream);
        // PasswordIsDefined = true;
        return E_ABORT;
    }
    return StringToBstr(Password, password);
}


static const wchar_t *kCantDeleteOutputFile = L"ERROR: Can not delete output file ";

static const char *kTestingString    =  "Testing     ";
static const char *kExtractingString =  "Extracting  ";
static const char *kSkippingString   =  "Skipping    ";

static const char *kUnsupportedMethod = "Unsupported Method";
static const char *kCRCFailed = "CRC Failed";
static const char *kDataError = "Data Error";
static const char *kUnknownError = "Unknown Error";

static const wchar_t *kEmptyFileAlias = L"[Content]";

class CArchiveExtractCallback: public IArchiveExtractCallback, public ICryptoGetTextPassword, public CMyUnknownImp {
public:
    MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

    // IProgress
    STDMETHOD(SetTotal)(UInt64 size);
    STDMETHOD(SetCompleted)(const UInt64 *completeValue);

    // IArchiveExtractCallback
    STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode);
    STDMETHOD(PrepareOperation)(Int32 askExtractMode);
    STDMETHOD(SetOperationResult)(Int32 resultEOperationResult);

    // ICryptoGetTextPassword
    STDMETHOD(CryptoGetTextPassword)(BSTR *aPassword);

private:
    CMyComPtr<IInArchive> _archiveHandler;
    UString _directoryPath;  // Output directory
    UString _filePath;       // name inside arcvhive
    UString _diskFilePath;   // full path to file on disk
    bool _extractMode;
    struct CProcessedFileInfo
    {
        FILETIME MTime;
        UInt32 Attrib;
        bool isDir;
        bool AttribDefined;
        bool MTimeDefined;
    } _processedFileInfo;

    COutFileStream *_outFileStreamSpec;
    CMyComPtr<ISequentialOutStream> _outFileStream;
    UncompressCallback _lpProgressCallback;
    void* _ProgressParam;

public:
    void Init(IInArchive *archiveHandler, const UString &directoryPath, const UncompressCallback &lpProgressCallback, void* ProgressParam);

    UInt64 nFilesize;
    UInt64 NumErrors;
    bool PasswordIsDefined;
    UString Password;

    CArchiveExtractCallback() : PasswordIsDefined(false) {}
};

void CArchiveExtractCallback::Init(IInArchive *archiveHandler, const UString &directoryPath, const UncompressCallback &lpProgressCallback, void* ProgressParam)
{
    NumErrors = 0;
    _archiveHandler = archiveHandler;
    _directoryPath = directoryPath;
    _lpProgressCallback = lpProgressCallback;
    _ProgressParam = ProgressParam;
    NFile::NName::NormalizeDirPathPrefix(_directoryPath);
}

STDMETHODIMP CArchiveExtractCallback::SetTotal(UInt64 size)
{
    nFilesize = size;
    return S_OK;
}
STDMETHODIMP CArchiveExtractCallback::SetCompleted(const UInt64 * completeValue)
{
    auto percent = static_cast<float>((float)(*completeValue) / (float)nFilesize * 100.0f);
    _lpProgressCallback((int)percent, _ProgressParam);
    return S_OK;
}
STDMETHODIMP CArchiveExtractCallback::GetStream(UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode)
{
    *outStream = 0;
    _outFileStream.Release();

    {
        // Get Name
        NCOM::CPropVariant prop;
        RINOK(_archiveHandler->GetProperty(index, kpidPath, &prop));

        UString fullPath;
        if (prop.vt == VT_EMPTY){
            fullPath = kEmptyFileAlias;
        }
        else{
            if (prop.vt != VT_BSTR){
                return E_FAIL;
            }
            fullPath = prop.bstrVal;
        }
        _filePath = fullPath;
    }

    if (askExtractMode != NArchive::NExtract::NAskMode::kExtract){
        return S_OK;
    }
    {
        // Get Attrib
        NCOM::CPropVariant prop;
        RINOK(_archiveHandler->GetProperty(index, kpidAttrib, &prop));
        if (prop.vt == VT_EMPTY){
            _processedFileInfo.Attrib = 0;
            _processedFileInfo.AttribDefined = false;
        }
        else{
            if (prop.vt != VT_UI4){
                return E_FAIL;
            }
            _processedFileInfo.Attrib = prop.ulVal;
            _processedFileInfo.AttribDefined = true;
        }
    }

    RINOK(IsArchiveItemFolder(_archiveHandler, index, _processedFileInfo.isDir));

    {
        // Get Modified Time
        NCOM::CPropVariant prop;
        RINOK(_archiveHandler->GetProperty(index, kpidMTime, &prop));
        _processedFileInfo.MTimeDefined = false;
        switch(prop.vt){
        case VT_EMPTY:{
            // _processedFileInfo.MTime = _utcMTimeDefault;
            break;
                      }
        case VT_FILETIME:{
            _processedFileInfo.MTime = prop.filetime;
            _processedFileInfo.MTimeDefined = true;
            break;
                         }
        default:{
            return E_FAIL;
                }
        }
    }
    {
        // Get Size
        NCOM::CPropVariant prop;
        RINOK(_archiveHandler->GetProperty(index, kpidSize, &prop));
        bool newFileSizeDefined = (prop.vt != VT_EMPTY);
        UInt64 newFileSize;
        if (newFileSizeDefined){
            ConvertPropVariantToUInt64(prop, newFileSize);
        }
    }

    {
        // Create folders for file
        int slashPos = _filePath.ReverseFind(WCHAR_PATH_SEPARATOR);
        if (slashPos >= 0){
            //NFile::NDirectory::CreateComplexDirectory(_directoryPath + _filePath.Left(slashPos));
        }
    }

    UString fullProcessedPath = _directoryPath + _filePath;
    _diskFilePath = fullProcessedPath;

    if (_processedFileInfo.isDir){
        //NFile::NDirectory::CreateComplexDirectory(fullProcessedPath);
    }
    else{
        NFile::NFind::CFileInfo fi;
        if (fi.Find(fullProcessedPath)){
            //if (!NFile::NDirectory::DeleteFileAlways(fullProcessedPath)){
            //    return E_ABORT;
            //}
        }

        _outFileStreamSpec = new COutFileStream;
        CMyComPtr<ISequentialOutStream> outStreamLoc(_outFileStreamSpec);
        if (!_outFileStreamSpec->Open(fullProcessedPath, CREATE_ALWAYS)){
            return E_ABORT;
        }
        _outFileStream = outStreamLoc;
        *outStream = outStreamLoc.Detach();
    }
    return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::PrepareOperation(Int32 askExtractMode)
{
    _extractMode = false;
    switch (askExtractMode)
    {
    case NArchive::NExtract::NAskMode::kExtract:  _extractMode = true; break;
    };
    switch (askExtractMode)
    {
    case NArchive::NExtract::NAskMode::kExtract: break;
    case NArchive::NExtract::NAskMode::kTest: break;
    case NArchive::NExtract::NAskMode::kSkip: break;
    };
    return S_OK;
}
STDMETHODIMP CArchiveExtractCallback::SetOperationResult(Int32 operationResult)
{
    switch(operationResult)
    {
    case NArchive::NExtract::NOperationResult::kOK:
        break;
    default:
        {
            NumErrors++;
            switch(operationResult)
            {
            case NArchive::NExtract::NOperationResult::kUnsupportedMethod:
                break;
            case NArchive::NExtract::NOperationResult::kCRCError:
                break;
            case NArchive::NExtract::NOperationResult::kDataError:
                break;
            default:
                break;
            }
        }
    }

    if (_outFileStream != NULL){
        if (_processedFileInfo.MTimeDefined){
            _outFileStreamSpec->SetMTime(&_processedFileInfo.MTime);
        }
        RINOK(_outFileStreamSpec->Close());
    }
    _outFileStream.Release();
    if (_extractMode && _processedFileInfo.AttribDefined){
        //NFile::NDirectory::MySetFileAttributes(_diskFilePath, _processedFileInfo.Attrib);
    }
    return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::CryptoGetTextPassword(BSTR *password)
{
    if (!PasswordIsDefined){
        // You can ask real password here from user
        // Password = GetPassword(OutStream);
        // PasswordIsDefined = true;
        return E_ABORT;
    }
    return StringToBstr(Password, password);
}



namespace TaskCenter{
	CTaskUnzipFile::CTaskUnzipFile(const MSG& msg, UINT uMsg, const wstring& strZipFile, const wstring& strOutDir)
		: ITask(msg)
		, m_pNeedExit(NULL)
		, m_strZipFile(strZipFile)
		, m_strOutDir(strOutDir)
		, m_uMsg(uMsg)
	{
		m_type = TaskUnzipFile;
		m_bUsed = TRUE;
	}

	CTaskUnzipFile::~CTaskUnzipFile()
	{
	}

	void CTaskUnzipFile::Init(const MSG& msg, UINT uMsg, const wstring& strZipFile, const wstring& strOutDir)
	{
		m_msg = msg;
		m_uMsg = uMsg;
		m_bUsed = TRUE;
		m_strZipFile = strZipFile;
		m_strOutDir = strOutDir;
	}

	void CTaskUnzipFile::SetNeedExit(BOOL* pNeedExit)
	{
		m_pNeedExit = pNeedExit;
	}

	void CTaskUnzipFile::Run() {
		bool bSuccess = false;

        NWindows::NDLL::CLibrary lib;

        do {
            auto dll_path = CGlobalData::Instance()->GetRunPath() + L"7z.dll";
            if (!lib.Load(dll_path.c_str())){
                break;
            }

            Func_CreateObject createObjectFunc = (Func_CreateObject)lib.GetProc("CreateObject");
            if (createObjectFunc == nullptr){
                break;
            }

            CMyComPtr<IInArchive> archive;
            if (createObjectFunc(&CLSID_CFormat7z, &IID_IInArchive, (void **)&archive) != S_OK){
                break;
            }

            CInFileStream *fileSpec = new CInFileStream;
            CMyComPtr<IInStream> file = fileSpec;

            UString archiveName = GetUnicodeString(m_strZipFile.c_str());

            if (fileSpec && !fileSpec->Open(archiveName)){
                break;
            }

            CArchiveOpenCallback *openCallbackSpec = new CArchiveOpenCallback;
            CMyComPtr<IArchiveOpenCallback> openCallback(openCallbackSpec);
            openCallbackSpec->PasswordIsDefined = false;

            if (archive->Open(file, 0, openCallback) != S_OK){
                break;
            }

            CArchiveExtractCallback *extractCallbackSpec = new CArchiveExtractCallback;
            CMyComPtr<IArchiveExtractCallback> extractCallback(extractCallbackSpec);
            extractCallbackSpec->Init(archive, m_strOutDir.c_str(), UncompressCallback, this); // second parameter is output folder path
            extractCallbackSpec->PasswordIsDefined = false;

            UInt32 numItems = 0;
            archive->GetNumberOfItems(&numItems);

            HRESULT result = archive->Extract(NULL, (UInt32)(Int32)(-1), false, extractCallback);
            if (result != S_OK){
                OUTPUT_XYLOG(LEVEL_ERROR, L"解压失败：文件：%s，解压到：%s", m_strZipFile.c_str(), m_strOutDir.c_str());
                break;
            }

            bSuccess = true;

        } while(false);

        if (lib.IsLoaded()) lib.Free();

		::PostMessage(m_msg.hwnd, m_uMsg, bSuccess ? 0 : 1, 0);

		m_msg.hwnd = nullptr;
	}

	void CTaskUnzipFile::OnLoadProgress(int nPercent)
	{
		if (nPercent < 0)
			nPercent = 0;
		else if (nPercent > 100)
			nPercent = 100;
		//::PostMessage(m_msg.hwnd, m_msg.message, (WPARAM)0, nPercent);
	}

	void CTaskUnzipFile::UncompressCallback(int nCurrentPercent, void* pParam)
	{
		CTaskUnzipFile* pTask = (CTaskUnzipFile*)pParam;
		if (pTask)
			pTask->OnLoadProgress(nCurrentPercent);
	}

}
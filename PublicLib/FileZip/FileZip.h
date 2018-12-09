#pragma once
#include <windows.h>
#include <atlstr.h>
#include <map>
#include <vector>

#ifdef LIBXYHELPER_EXPORT
#define LIBXYHELPER_API __declspec(dllexport)
#else
#define LIBXYHELPER_API __declspec(dllimport)
#endif

namespace PublicLib{

	typedef void(*ZipCallback)(int nCurrentStep, int ntotalSteps, void* pParam);

	typedef struct STZipInfo
	{
		ULONGLONG						m_ullIndex;
		CString							m_strName;
		std::map<CString, STZipInfo>	m_mapSubFileInfo;
	}*LPSTZipFileInfo;


	class  CFileZip_LibZip
	{
	public:
		CFileZip_LibZip(void);
		virtual ~CFileZip_LibZip(void);

		bool Create(LPCWSTR lpszZipFilePath);//创建一个Zip文件
		bool Open(LPCWSTR lpszZipFilePath);	//打开一个Zip文件
		bool Save();
		void Close();

		bool UnZip(LPCWSTR lpszSaveDir, ZipCallback pCallback = NULL);
		bool UnZipOneFolder(LPCWSTR  lpszFolderName, LPCWSTR  lpszSaveDir);
		bool UnZipOneFile(LPCWSTR  lpszFileName, LPCWSTR  lpszSaveFilePath);


		bool UnZipOneFileToMem(LPCWSTR  lpszFileName, BYTE * pBuffer, DWORD dwBufferLength, DWORD * pdwRetLength);


		bool AppendFolder(LPCWSTR  lpszName);
		bool AppendFile(LPCWSTR  lpszFilePath, LPCWSTR  lpszName);
		bool AppendBuffer(char* buffer, unsigned int buffer_length, LPCWSTR lpszName);
		bool AppendDirectory(LPCWSTR  lpszDirName, LPCWSTR  lpszName = NULL);	//将一个目录下的文件添加到压缩包 lpszName如果为空则添加到根目录

		bool DeleteFile(LPCWSTR  lpszName);	//删除文件或目录 

		bool GetZipFileList(std::map<CString, STZipInfo>& mapFileList);
		bool GetZipFileList(std::vector<CString>& file_list);
		bool GetZipFileList(std::vector<std::wstring>& file_list);
		ULONGLONG GetZipFileCount();

	protected:
		void*		m_pZipFile;
		CStringW	m_strFilePath;

	protected:
		__int64 GetFileIndexByName(LPCWSTR lpszName);
		CString GetFileNameByIndex(ULONGLONG ullIndex);

		bool UnZipOneFile(ULONGLONG ullIndex, LPCWSTR lpszSaveFilePath);
		DWORD UnZipOneFile(ULONGLONG ullIndex, BYTE * pData, DWORD dwDataLen);
		bool ZipDeleteFile(ULONGLONG ullIndex);


		//以下为辅助功能函数，为了不与调用方函数名冲突，放到类里面相对方便一些
		CStringW AnsyToUnicode(const CStringA &str);
		CStringW Utf8ToUnicode(const CStringA &str);
		CStringA UnicodeToAnsi(const CStringW& wstr);
		CStringA UnicodeToUtf8(const CStringW& wstr);

		bool CreateMultipleDirectory(const CString & szPath);
		BOOL GetSplitString(CString SourceString, CString SplitFlag, std::vector<CString> &DestStringArray);
		bool AddFileToFileListMap(ULONGLONG ullIndex, const CString& strFileName, std::map<CString, STZipInfo>& mapFileList);
		bool EnumerateDirectoryFileToMap(CString strDirectory, CString strDirectoryBase, std::map<CString, CString> & mapFiles);
	};


	class CFileZip_LiteZip
	{
	public:
		CFileZip_LiteZip();

		int CreateZip(const std::wstring& zip_file);
		int OpenZip(const std::wstring& zip_file);
		int AddZipBuffer(std::wstring& name, std::string& data, unsigned int data_length);
		int AddZipFolder(std::wstring& path);
		int Close();



	private:
		void* hz_;
	};

	class CFileZip_LiteUnzip
	{
	public:
		int OpenZip(const std::wstring& zip_file);

		int UnzipFile(std::vector<std::string>& vectInfoList);

		int UnZipOneFile(const std::wstring& file_name, std::string& file_data);

		bool ZipFileIsExist(const std::wstring& strFileName);

		int EnumDirFileList(const std::wstring& dir, std::vector<std::wstring>& file_list);

		int Close();

		CFileZip_LiteUnzip();
	protected:
		void* hz_;
	};

	class CIpa_LiteUnzip : public CFileZip_LiteUnzip
	{
	public:
		int GetIpaMainDir(std::wstring& dir);

		bool HaveDoc();

		int GetIpaChineseDir(std::wstring& dir);


	};
}
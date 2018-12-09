#pragma once
#include "Task.h"
#include "GlobalData.h"
#include "HttpInterface.h"
#include "CxImg/xImageApi.h"
#include "CxImg/ximage.h"

namespace TaskCenter{
    class CTaskGetVerificationCode
        : public ITask {
    public:
        CTaskGetVerificationCode(const MSG& msg, const wstring& strPhoneNum, const wstring& strCheckType)
            : ITask(msg) {
            m_type = TaskGetVerificationCode;
            m_bUsed = TRUE;
            phone_number_ = strPhoneNum;
            check_type_ = strCheckType;
        }

        void Init(const MSG& msg, const wstring& strPhoneNum, const wstring& strCheckType) {
            m_msg = msg;
            m_bUsed = TRUE;
            phone_number_ = strPhoneNum;
            check_type_ = strCheckType;
        }

    protected:
        virtual void Run() {
            PublicLib::CHttpClient http;
            http.AddHttpHeader(L"version", L"1_1.0");
            http.AddHttpHeader(L"Device-Id", L"356261050135555");
            http.AddHttpHeader(L"Unique-Code", L"1AEE004A-73D0-4427-B1C4-B771A5CA1732");
            http.AddHttpHeader(L"Access-Token", L"A2838C35DAAAE9058E521930B8D27F9A0538C7F8D7CD248A263820E6A6B27FB12058141FDA3CEF5D98BDC24C53F07B2B");
            http.AddHttpHeader(L"Guoren-App-Interal-Tags", L"Salem501200");

            wstring strPost;
            strPost.append(L"mobile=");
            strPost += phone_number_;
            strPost.append(L"&type=");
            strPost += check_type_;

            string strData = PublicLib::UToUtf8(strPost);

            m_strResult = http.Request(URL_GET_VERIFICATION, PublicLib::Post, strData.c_str(), strData.size());
            if (m_strResult.empty()){
                OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
            }
        }

    private:
        wstring phone_number_;
        wstring check_type_;
    };

    class CTaskUserCheckCode
        : public ITask {
    public:
        CTaskUserCheckCode(const MSG& msg, const wstring& strPhoneNum, const wstring& strVerificationCode, const wstring& strActiveCode)
            : ITask(msg) {
                m_type = TaskGetUserCheckCode;
                m_bUsed = TRUE;
                phone_number_ = strPhoneNum;
                verification_code_ = strVerificationCode;
                active_code_ = strActiveCode;
        }

        void Init(const MSG& msg, const wstring& strPhoneNum, const wstring& strVerificationCode, const wstring& strActiveCode) {
            m_msg = msg;
            m_bUsed = TRUE;
            phone_number_ = strPhoneNum;
            verification_code_ = strVerificationCode;
            active_code_ = strActiveCode;
        }

    protected:
        virtual void Run() {
            PublicLib::CHttpClient http;
            http.AddHttpHeader(L"version", L"1_1.0");
            http.AddHttpHeader(L"Device-Id", L"356261050135555");
            http.AddHttpHeader(L"Unique-Code", L"1AEE004A-73D0-4427-B1C4-B771A5CA1732");
            http.AddHttpHeader(L"Access-Token", L"A2838C35DAAAE9058E521930B8D27F9A0538C7F8D7CD248A263820E6A6B27FB12058141FDA3CEF5D98BDC24C53F07B2B");
            http.AddHttpHeader(L"Guoren-App-Interal-Tags", L"Salem501200");

            wstring strPost;
            strPost.append(L"mobile=");
            strPost += phone_number_;
            strPost.append(L"&inviteCode=");
            strPost += active_code_;
            strPost.append(L"&code=");
            strPost += verification_code_;

            string strData = PublicLib::UToUtf8(strPost);

            m_strResult = http.Request(URL_USER_CHECKCODE, PublicLib::Post, strData.c_str(), strData.size());
            if (m_strResult.empty()){
                OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
            }
        }

    private:
        wstring phone_number_;
        wstring verification_code_;
        wstring active_code_;
    };

    class CTaskUserCheckMobileCode
        : public ITask {
    public:
        CTaskUserCheckMobileCode(const MSG& msg, const wstring& strPhoneNum, const wstring& strCode, const wstring& strCheckType)
            : ITask(msg) {
                m_type = TaskGetUserCheckMobileCode;
                m_bUsed = TRUE;
                phone_number_ = strPhoneNum;
                code_ = strCode;
                check_type_ = strCheckType;
        }

        void Init(const MSG& msg, const wstring& strPhoneNum, const wstring& strCode, const wstring& strCheckType) {
            m_msg = msg;
            m_bUsed = TRUE;
            phone_number_ = strPhoneNum;
            code_ = strCode;
            check_type_ = strCheckType;
        }

    protected:
        virtual void Run() {
            PublicLib::CHttpClient http;
            http.AddHttpHeader(L"version", L"1_1.0");
            http.AddHttpHeader(L"Device-Id", L"356261050135555");
            http.AddHttpHeader(L"Unique-Code", L"1AEE004A-73D0-4427-B1C4-B771A5CA1732");
            http.AddHttpHeader(L"Access-Token", L"A2838C35DAAAE9058E521930B8D27F9A0538C7F8D7CD248A263820E6A6B27FB12058141FDA3CEF5D98BDC24C53F07B2B");
            http.AddHttpHeader(L"Guoren-App-Interal-Tags", L"Salem501200");

            wstring strPost;
            strPost.append(L"mobile=");
            strPost += phone_number_;
            strPost.append(L"&code=");
            strPost += code_;
            strPost.append(L"&type=");
            strPost += check_type_;

            string strData = PublicLib::UToUtf8(strPost);

            m_strResult = http.Request(URL_USER_CHECKMOBILECODE, PublicLib::Post, strData.c_str(), strData.size());
            if (m_strResult.empty()){
                OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
            }
        }

    private:
        wstring phone_number_;
        wstring code_;
        wstring check_type_;
    };

    class CTaskUserRegister
        : public ITask {
    public:
        CTaskUserRegister(const MSG& msg, const wstring& strPhoneNum, const wstring& strVerificationCode, const wstring& strActiveCode, const wstring& strPassword)
            : ITask(msg) {
                m_type = TaskGetUserRegister;
                m_bUsed = TRUE;
                phone_number_ = strPhoneNum;
                verification_code_ = strVerificationCode;
                active_code_ = strActiveCode;
                password_ = strPassword;
        }

        void Init(const MSG& msg, const wstring& strPhoneNum, const wstring& strVerificationCode, const wstring& strActiveCode, const wstring& strPassword) {
            m_msg = msg;
            m_bUsed = TRUE;
            phone_number_ = strPhoneNum;
            verification_code_ = strVerificationCode;
            active_code_ = strActiveCode;
            password_ = strPassword;
        }

    protected:
        virtual void Run() {
            PublicLib::CHttpClient http;
            http.AddHttpHeader(L"version", L"1_1.0");
            http.AddHttpHeader(L"Device-Id", L"356261050135555");
            http.AddHttpHeader(L"Unique-Code", L"1AEE004A-73D0-4427-B1C4-B771A5CA1732");
            http.AddHttpHeader(L"Access-Token", L"A2838C35DAAAE9058E521930B8D27F9A0538C7F8D7CD248A263820E6A6B27FB12058141FDA3CEF5D98BDC24C53F07B2B");
            http.AddHttpHeader(L"Guoren-App-Interal-Tags", L"Salem501200");

            wstring strPost;
            strPost.append(L"mobile=");
            strPost += phone_number_;
            strPost.append(L"&inviteCode=");
            strPost += active_code_;
            strPost.append(L"&code=");
            strPost += verification_code_;
            strPost.append(L"&password=");
            strPost += password_;

            string strData = PublicLib::UToUtf8(strPost);

            m_strResult = http.Request(URL_USER_REGISTER, PublicLib::Post, strData.c_str(), strData.size());
            if (m_strResult.empty()){
                OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
            }
        }

    private:
        wstring phone_number_;
        wstring verification_code_;
        wstring active_code_;
        wstring password_;
    };

    class CTaskUserLogin
        : public ITask {
    public:
        CTaskUserLogin(const MSG& msg, const wstring& strAccountId, const wstring& strPassword)
            : ITask(msg) {
                m_type = TaskGetUserLogin;
                m_bUsed = TRUE;
                account_id_ = strAccountId;
                password_ = strPassword;
        }

        void Init(const MSG& msg, const wstring& strAccountId, const wstring& strPassword) {
            m_msg = msg;
            m_bUsed = TRUE;
            account_id_ = strAccountId;
            password_ = strPassword;
        }

    protected:
        virtual void Run() {
            PublicLib::CHttpClient http;
            http.AddHttpHeader(L"version", L"1_1.0");
            http.AddHttpHeader(L"Device-Id", L"356261050135555");
            http.AddHttpHeader(L"Unique-Code", L"1AEE004A-73D0-4427-B1C4-B771A5CA1732");
            http.AddHttpHeader(L"Access-Token", L"A2838C35DAAAE9058E521930B8D27F9A0538C7F8D7CD248A263820E6A6B27FB12058141FDA3CEF5D98BDC24C53F07B2B");
            http.AddHttpHeader(L"Guoren-App-Interal-Tags", L"Salem501200");

            wstring strPost;
            strPost.append(L"account=");
            strPost += account_id_;
            strPost.append(L"&password=");
            strPost += password_;

            string strData = PublicLib::UToUtf8(strPost);

            m_strResult = http.Request(URL_USER_LOGIN, PublicLib::Post, strData.c_str(), strData.size());
            if (m_strResult.empty()){
                OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
            }
        }

    private:
        wstring account_id_;
        wstring password_;
    };

    class CTaskUserCodeLogin
        : public ITask {
    public:
        CTaskUserCodeLogin(const MSG& msg, const wstring& strPhoneNum, const wstring& strCode)
            : ITask(msg) {
                m_type = TaskGetUserCodeLogin;
                m_bUsed = TRUE;
                phone_number_ = strPhoneNum;
                code_ = strCode;
        }

        void Init(const MSG& msg, const wstring& strPhoneNum, const wstring& strCode) {
            m_msg = msg;
            m_bUsed = TRUE;
            phone_number_ = strPhoneNum;
            code_ = strCode;
        }

    protected:
        virtual void Run() {
            PublicLib::CHttpClient http;
            http.AddHttpHeader(L"version", L"1_1.0");
            http.AddHttpHeader(L"Device-Id", L"356261050135555");
            http.AddHttpHeader(L"Unique-Code", L"1AEE004A-73D0-4427-B1C4-B771A5CA1732");
            http.AddHttpHeader(L"Access-Token", L"A2838C35DAAAE9058E521930B8D27F9A0538C7F8D7CD248A263820E6A6B27FB12058141FDA3CEF5D98BDC24C53F07B2B");
            http.AddHttpHeader(L"Guoren-App-Interal-Tags", L"Salem501200");

            wstring strPost;
            strPost.append(L"mobile=");
            strPost += phone_number_;
            strPost.append(L"&code=");
            strPost += code_;

            string strData = PublicLib::UToUtf8(strPost);

            m_strResult = http.Request(URL_USER_CODE_LOGIN, PublicLib::Post, strData.c_str(), strData.size());
            if (m_strResult.empty()){
                OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
            }
        }

    private:
        wstring phone_number_;
        wstring code_;
    };

    class CTaskUserModifyPassword
        : public ITask {
    public:
        CTaskUserModifyPassword(const MSG& msg, const wstring& strPhoneNum, const wstring& strCode, const wstring& strPassword)
            : ITask(msg) {
                m_type = TaskGetUserModifyPassword;
                m_bUsed = TRUE;
                phone_number_ = strPhoneNum;
                code_ = strCode;
                password_ = strPassword;
        }

        void Init(const MSG& msg, const wstring& strPhoneNum, const wstring& strCode, const wstring& strPassword) {
            m_msg = msg;
            m_bUsed = TRUE;
            phone_number_ = strPhoneNum;
            code_ = strCode;
            password_ = strPassword;
        }

    protected:
        virtual void Run() {
            PublicLib::CHttpClient http;
            http.AddHttpHeader(L"version", L"1_1.0");
            http.AddHttpHeader(L"Device-Id", L"356261050135555");
            http.AddHttpHeader(L"Unique-Code", L"1AEE004A-73D0-4427-B1C4-B771A5CA1732");
            http.AddHttpHeader(L"Access-Token", L"A2838C35DAAAE9058E521930B8D27F9A0538C7F8D7CD248A263820E6A6B27FB12058141FDA3CEF5D98BDC24C53F07B2B");
            http.AddHttpHeader(L"Guoren-App-Interal-Tags", L"Salem501200");

            wstring strPost;
            strPost.append(L"mobile=");
            strPost += phone_number_;
            strPost.append(L"&code=");
            strPost += code_;
            strPost.append(L"&password=");
            strPost += password_;

            string strData = PublicLib::UToUtf8(strPost);

            m_strResult = http.Request(URL_USER_MODIFYPASSWORD, PublicLib::Post, strData.c_str(), strData.size());
            if (m_strResult.empty()){
                OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
            }
        }

    private:
        wstring phone_number_;
        wstring code_;
        wstring password_;
    };

	class CTaskUserInfo
		:public ITask {
	public:
		CTaskUserInfo(const MSG& msg, int nUid, const string& strToken, bool bUpdateInfo)
			: ITask(msg)
			, m_nUid(nUid)
			, m_strToken(strToken)
			, m_bUpdateInfo(bUpdateInfo){
			m_type = TaskUserInfo;
			m_bUsed = TRUE;
		}

		void Init(const MSG& msg, int nUid, const string& strToken, bool bUpdateInfo) {
			m_msg = msg;
			m_nUid = nUid;
			m_strToken = strToken;
			m_bUpdateInfo = bUpdateInfo;
			m_bUsed = TRUE;
		}

		void GetParam(OUT int& nUid, OUT string& strToken) {
			nUid = m_nUid;
			strToken = m_strToken;
		}

		bool IsUpdateInfo(){ return m_bUpdateInfo; }

	protected:
		virtual void Run() {
            PublicLib::CHttpClient http;
            http.AddHttpHeader(L"version", L"1_1.0");
            http.AddHttpHeader(L"Device-Id", L"356261050135555");
            http.AddHttpHeader(L"Unique-Code", L"1AEE004A-73D0-4427-B1C4-B771A5CA1732");
            http.AddHttpHeader(L"Access-Token", L"A2838C35DAAAE9058E521930B8D27F9A0538C7F8D7CD248A263820E6A6B27FB12058141FDA3CEF5D98BDC24C53F07B2B");
            http.AddHttpHeader(L"Guoren-App-Interal-Tags", L"Salem501200");

            string strPost;
            strPost.append("token=");
            strPost += m_strToken;
            strPost.append("&uid=");
            char szUid[100] = { 0 };
            sprintf_s(szUid, "%u", m_nUid);
            strPost.append(szUid);

            string strData = strPost;

			m_strResult = http.Request(URL_USER_INFO, PublicLib::Post, strPost.c_str(), strPost.size());
			if (m_strResult.empty()) {
				OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
			}
		}

	private:
		int m_nUid;
		bool m_bUpdateInfo;
		string m_strToken;
	};

	class CTaskUserIco
		: public ITask
	{
	public:
		CTaskUserIco(const MSG& msg, const wstring& strUrl, const wstring& strUserPath)
			: ITask(msg)
			, m_strUrl(strUrl)
			, m_strUserPath(strUserPath)
		{
			m_type = TaskUserIco;
			m_bUsed = TRUE;
		}

		void Init(const MSG& msg, const wstring& strUrl, const wstring& strUserPath)
		{
			m_msg = msg;
			m_strUrl = strUrl;
			m_strUserPath = strUserPath;
			m_bUsed = TRUE;
		}
		const wstring& GetIcoPath()const
		{
			return m_strUserPath;
		}
        const wstring& GetIcoUrl()const
        {
            return m_strUrl;
        }

	protected:
		virtual void Run()
		{
			PublicLib::CHttpClient http;
			byte* pImgData = NULL;
			DWORD dwImgSize = 0;

			if (http.DownLoadMem(m_strUrl.c_str(), (void**)&pImgData, dwImgSize)) {
                if (!pImgData) {
                    m_strResult = "0";
                    return;
                }

                std::unique_ptr<CxImage> img;
                CxImage press_img;
                bool sucess = false;

                auto PressImage = [&](const CxImage& temp) -> bool {
                    img.reset(new CxImage(pImgData, dwImgSize, temp.GetType()));
                    if (!img || !img->IsValid()) return false;

                    img->Resample(42, 42, 1, &press_img);

                    if (!press_img.IsValid()) return false;

                    press_img.Save(m_strUserPath.c_str(), press_img.GetType());
                    return true;
                };


                do {
                    CxImage temp;
                    if (temp.CheckFormat(pImgData, dwImgSize, CXIMAGE_FORMAT_PNG)) {
                        sucess = PressImage(temp);
                    }
                    else if (temp.CheckFormat(pImgData, dwImgSize, CXIMAGE_FORMAT_JPG)) {
                        sucess = PressImage(temp);
                    }
                    else if (temp.CheckFormat(pImgData, dwImgSize, CXIMAGE_FORMAT_GIF)) {
                        sucess = PressImage(temp);
                    }

                    temp.Destroy();
                } while (false);

                if (img) {
                    img->Destroy();
                    img.reset();
                }
                press_img.Destroy();

				free(pImgData);
				m_strResult = sucess ? "1" : "0";
			}
			else
			{
				OUTPUT_XYLOG(LEVEL_ERROR, L"游戏图标下载失败，URL = %s", m_strUrl.c_str());
				m_strResult = "0";
			}
		}

	private:
		wstring m_strUserPath;
		wstring m_strUrl;
	};

	class CTaskUploadAvatar
		:public ITask
	{
	public:
		CTaskUploadAvatar(const MSG& msg, int nUid, const string& strToken, const wstring& strImgPath)
			: ITask(msg)
			, m_nUid(nUid)
			, m_strToken(strToken)
			, m_strImgPath(strImgPath)
		{
			m_type = TaskUploadAvatar;
			m_bUsed = TRUE;
		}

		void Init(const MSG& msg, int nUid, const string& strToken, const wstring& strImgPath)
		{
			m_msg = msg;
			m_nUid = nUid;
			m_strToken = strToken;
			m_strImgPath = strImgPath;
			m_bUsed = TRUE;
		}
		const wstring& GetImgPath()const
		{
			return m_strImgPath;
		}

	protected:
		virtual void Run()
		{
			int nPos = m_strImgPath.rfind(L".");
			if (nPos == wstring::npos)
			{
				OUTPUT_XYLOG(LEVEL_ERROR, L"文件扩展名错误");
				return;
			}
			wstring strExt = m_strImgPath.substr(nPos + 1);
			FILE* fp = NULL;
			_wfopen_s(&fp, m_strImgPath.c_str(), L"rb");
			if (NULL == fp)
			{
				OUTPUT_XYLOG(LEVEL_ERROR, L"打开文件：%s失败！", m_strImgPath.c_str());
				return;
			}
			fseek(fp, 0, SEEK_END);
			int nSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			if (nSize < 1)
			{
				fclose(fp);
				OUTPUT_XYLOG(LEVEL_ERROR, L"文件：%s为空！", m_strImgPath.c_str());
				return;
			}
			char* pBuffer = (char*)malloc(nSize);
			int nRead = fread(pBuffer, 1, nSize, fp);
			fclose(fp);
			string CRLF = "\r\n";
			string Boundary = "-----------------------------67491722032265";
			string StartBoundary = "--" + Boundary + CRLF;
			string EndBoundary = "--" + Boundary + "--" + CRLF;
			string strType = "Content-Type: image/jpeg";
			if (wcsicmp(strExt.c_str(), L"png") == 0)
				strType = "Content-Type: image/png";
			string strPostData = StartBoundary + "Content-Disposition: form-data; name=\"files\"; filename=\"" + "11.jpg" + "\"" + CRLF
				+ strType + CRLF + CRLF;
			strPostData.append(pBuffer, nRead);
			strPostData += CRLF;
			strPostData += StartBoundary + "Content-Disposition: form-data; name=\"token\"\r\n\r\n" + m_strToken + CRLF;
			char szUid[30] = { 0 };
			sprintf_s(szUid, "%u", m_nUid);
			strPostData += StartBoundary + "Content-Disposition: form-data; name=\"uid\"\r\n\r\n" + szUid + CRLF;
			string strDeviceID = PublicLib::UToUtf8(CGlobalData::Instance()->GetGuid());
			strPostData += StartBoundary + "Content-Disposition: form-data; name=\"device_id\"\r\n\r\n" + strDeviceID + CRLF;
			strPostData += EndBoundary;
			free(pBuffer);
			PublicLib::CHttpClient http(PublicLib::ContentFile);
			wstring str(L"multipart/form-data; boundary=");
			str += PublicLib::AToU(Boundary);
			http.AddHttpHeader(L"Cache-Control", L"no-cache");
			http.AddHttpHeader(L"Accept-Encoding", L"gzip, deflate");
			http.AddHttpHeader(L"Content-Type", str.c_str());
			m_strResult = http.Request(URL_UOLODA_AVATAR, PublicLib::Post, strPostData.c_str(), strPostData.size());
			if (m_strResult.empty())
			{
				OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
			}
		}

	private:
		int m_nUid;
		string m_strToken;
		wstring m_strImgPath;
	};
}
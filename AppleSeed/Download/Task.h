#pragma once
#include "TaskCallback.h"
#include <string>
using std::wstring;
using std::string;
#include <PublicLib/HttpLib.h>
using PublicLib::CHttpDownload;

// �����ļ�����
enum FileType {
	Ft_Unknow = 0,	// δ֪
	Ft_Game,		// ģ������Ϸ

    Ft_Icon,        // ��Ϸͼ������
};

enum GameType {
	GameUnknow = 0,
	GameFBA = 31,	// �ֻ�
	GameNDS,		// NDS
	GameGBA,		// GBA
	GamePSP,
	GameFC,
	GameSFC = 43,
	GameMD = 51,
	GameGBAEx,		// GBA��ǿ��
	GamePS,
	GameWSC,
	GameGBC = 56,
	GameN64,
	GameONS,
	GameDC = 60,
};

static const struct GameTypeList {
	GameType type;
	wstring strGameName;

} g_gameTypes[] = {
	{ GameFBA, L"�ֻ�" },
	{ GameNDS, L"NDS" },
	{ GameGBA, L"GBA" },
	{ GamePSP, L"PSP" },
	{ GameFC, L"FC" },
	{ GameSFC, L"SFC" },
	{ GameMD, L"MD" },
	{ GameGBAEx, L"GBA��ǿ��" },
	{ GamePS, L"PS" },
	{ GameWSC, L"WSC" },
	{ GameGBC, L"GBC" },
	{ GameN64, L"N64" },
	{ GameONS, L"ONS" },
	{ GameDC, L"DC" },
};

static inline wstring GetGameName(GameType type) {
	for (int i = 0; i < _countof(g_gameTypes); ++i) {
		if (g_gameTypes[i].type == type)
			return g_gameTypes[i].strGameName;
	}
	return L"";
}

static inline GameType GetGameType(const string& strTagID) {
	int nID = atoi(strTagID.c_str());
	for (int i = 0; i < _countof(g_gameTypes); ++i) {
		if ((int)g_gameTypes[i].type == nID)
			return g_gameTypes[i].type;
	}
	return GameUnknow;
}

struct ITask {
	string		strUrl;			// �������ӵ�ַ
    string      strIconUrl;     // ����ͼ���ַ����json��ҳ�˵���
	wstring		strSavePath;	// ���ر���·��
	wstring		strName;		// ����������
    wstring		strVer;		    // ��Ϸ�ڲ��汾��
	wstring		strVersion;		// ��Ϸ�汾�ţ�����Ϸ�汾���ƺ��ڲ��汾����������İ汾��
    wstring		strSourceVer;   // ��Դ�ļ��汾��
    wstring		strConfigVer;   // �����ļ��汾��
	__int64		nGameID;		// ��ϷID
	__int64		nDate;			// �������ʱ��
	__int64		nTotalSize;		// �������С
	__int64		nLoadSize;		// �����ش�С
	FileType	type;			// ��������
	TaskState	state;			// ����״̬
	__int64		nLastTime;		// ʱ���
	__int64		nSize;
	int			nSpeed;			//�����ٶ� ��λ KB/S
	CHttpDownload *pHttp;		//���ؿ�ָ��
	BOOL		bShowOnLoadWnd;	//�Ƿ���ʾ�����ش���
	BOOL		bUseCbList;		//�Ƿ�ʹ�ûص�����
	CCallbackList cbList;		//���ػص�����
	string		strGameInfo;
	string		strGameTags;
	string		strPkgName;
	string		strSyncMem;
	wstring		strRomPath;
	GameType	gameType;
    bool        favorite;
    int         nLoadWay;       //1-��Ĭ����    2-����ҳ����
	ITask()
		: nTotalSize(0)
		, nLoadSize(0)
		, nLastTime(0)
		, state(Ts_Init)
		, type(Ft_Game)
		, pHttp(NULL)
		, bShowOnLoadWnd(TRUE)
		, bUseCbList(FALSE)
		, nDate(0)
		, gameType(GameUnknow)
		, nSize(0)
		, nSpeed(0)
		, nGameID(0)
        , favorite(false) 
        , nLoadWay(1) {

	}

	virtual ~ITask() {
		if (pHttp) {
			pHttp->Stop();
			pHttp = NULL;
		}
	}

	void clear() {
		strUrl.clear();
		nDate = 0;
		nTotalSize = 0;
		nLoadSize = 0;
		state = Ts_Init;
		type = Ft_Unknow;
	}

    std::wstring GetVerName() {
        if (strVersion.empty()) return L"";

        auto pos = strVersion.rfind(L".");
        if (pos == wstring::npos) return L"";

        auto ver_name = strVersion.substr(0, pos);
        return ver_name;
    }

    int GetTag() {
        switch (type) {
        case Ft_Unknow: return 0;
        case Ft_Game: return 1;
        case Ft_Icon: return 2;
        default:
            return 0;
        }
    }
};
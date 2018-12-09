#pragma once
#include "TaskCallback.h"
#include <string>
using std::wstring;
using std::string;
#include <PublicLib/HttpLib.h>
using PublicLib::CHttpDownload;

// 下载文件类型
enum FileType {
	Ft_Unknow = 0,	// 未知
	Ft_Game,		// 模拟器游戏

    Ft_Icon,        // 游戏图标下载
};

enum GameType {
	GameUnknow = 0,
	GameFBA = 31,	// 街机
	GameNDS,		// NDS
	GameGBA,		// GBA
	GamePSP,
	GameFC,
	GameSFC = 43,
	GameMD = 51,
	GameGBAEx,		// GBA加强版
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
	{ GameFBA, L"街机" },
	{ GameNDS, L"NDS" },
	{ GameGBA, L"GBA" },
	{ GamePSP, L"PSP" },
	{ GameFC, L"FC" },
	{ GameSFC, L"SFC" },
	{ GameMD, L"MD" },
	{ GameGBAEx, L"GBA加强版" },
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
	string		strUrl;			// 下载链接地址
    string      strIconUrl;     // 下载图标地址，给json网页端调用
	wstring		strSavePath;	// 下载保存路径
	wstring		strName;		// 下载项名称
    wstring		strVer;		    // 游戏内部版本号
	wstring		strVersion;		// 游戏版本号，将游戏版本名称和内部版本号组合起来的版本号
    wstring		strSourceVer;   // 资源文件版本号
    wstring		strConfigVer;   // 配置文件版本号
	__int64		nGameID;		// 游戏ID
	__int64		nDate;			// 下载完成时间
	__int64		nTotalSize;		// 下载项大小
	__int64		nLoadSize;		// 已下载大小
	FileType	type;			// 下载类型
	TaskState	state;			// 下载状态
	__int64		nLastTime;		// 时间戳
	__int64		nSize;
	int			nSpeed;			//下载速度 单位 KB/S
	CHttpDownload *pHttp;		//下载库指针
	BOOL		bShowOnLoadWnd;	//是否显示到下载窗口
	BOOL		bUseCbList;		//是否使用回调队列
	CCallbackList cbList;		//下载回调队列
	string		strGameInfo;
	string		strGameTags;
	string		strPkgName;
	string		strSyncMem;
	wstring		strRomPath;
	GameType	gameType;
    bool        favorite;
    int         nLoadWay;       //1-静默下载    2-详情页下载
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
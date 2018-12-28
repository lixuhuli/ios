#pragma once

#define WM_USER_MSG_BASE WM_USER + 1000

//定义主窗口消息
enum{
	WM_MAINWND_MSG_BEGIN = WM_USER_MSG_BASE,
	WM_MAINWND_MSG_COMMON,             // 普通消息，禁止在这个消息前面添加新的消息！
    WM_MAINWND_MSG_NOTIFYICON,
    WM_MAINWND_MSG_SETTING,
    WM_MAINWND_MSG_MENU,
    WM_MAINWND_MSG_SEARCH,
    WM_MAINWND_MSG_PAGE_LOCALGAMES,
    WM_MAINWND_MSG_PAGE_GAMEROOM,
    WM_MAINWND_MSG_GAMEHALL,
    WM_MAINWND_MSG_GAMEHALL_PING,
    WM_MAINWND_MSG_GAMEHALL_SEARCH,
    WM_MAINWND_MSG_AREA_COUNT,

	WM_MAINWND_MSG_TASK_BEGIN,
    WM_MAINWND_MSG_GETSERVER,          // 获取游戏服务器配置信息
    WM_MAINWND_MSG_USER_LOGIN,         // 用户登录
    WM_MAINWND_MSG_USERINFO,           // 获取用户信息
    WM_MAINWND_MSG_USER_ICO,           // 获取用户头像
    WM_MAINWND_MSG_GAMELIST,
    WM_MAINWND_MSG_IM_GET_TOKEN,
    WM_MAINWND_MSG_CONTEST,
    WM_MAINWND_MSG_DOWNLOAD_FILE,
    WM_MAINWND_MSG_RANKBATTLE_GAMES,
    WM_MAINWND_MSG_UPLOAD_RECORD,
    WM_MAINWND_MSG_IM_CHECKMSG,
    WM_MAINWND_MSG_CHECK_UPDATE_GAME,
	WM_MAINWND_MSG_TASK_END,

    WM_MAINWND_MSG_MSGBOX1,
    WM_MAINWND_MSG_IM,
    WM_MAINWND_MSG_PLAYER_ICO,
    WM_MAINWND_MSG_CONTEST_ICO,
    WM_MAINWND_MSG_PASSWORD_WND,     // 弹出设置房间密码窗口
    WM_MAINWND_MSG_ACCOUNT_WND,	     // 弹出完善账号窗口
    WM_MAINWND_MSG_GAME_CHANGED,

    WM_MAINWND_MSG_SHOW_BOTTOM_BAR,  // 显示底部窗口 新增消息

    WM_MAINWND_MSG_DOWNLOAD_MIRRORSYSTEM,

    WM_MAINWND_MSG_FILE_UNZIPING,
    WM_MAINWND_MSG_FILE_UNZIP,

    WM_MAINWND_MSG_IOSENGINE_UPDATING,
    WM_MAINWND_MSG_IOSENGINE_UPDATE,
    WM_MAINWND_MSG_IOSENGINE_APPLIACTION,

    WM_MAINWND_MSG_GET_KEYBOARD,

    WM_MAINWND_MSG_EMULATOR_ALREADY,
    WM_MAINWND_MSG_LOAD_IOS_ENGINE,

    WM_MAINWND_MSG_EXIT,

    WM_MAINWND_MSG_UPDATE_IOSWND_POS,

    WM_MAINWND_MSG_EMULATOR_VIP = WM_USER + 1502,   // go into vip

    WM_MAINWND_MSG_END,
};

// 模拟器消息
enum {
    WM_ENUMWND_MSG_KEY_BOSS = WM_USER + 1500,
    WM_ENUMWND_MSG_GAMEHALL_EXIT = WM_USER + 1501,
    WM_ENUMWND_MSG_VIP_STATUS = WM_USER + 1503,
};

// 用户注册登录消息
enum{
    WM_USERWND_MSG_BEGIN = WM_USER + 1000,
    WM_USERWND_MSG_GET_VERIFICATION,
    WM_USERWND_MSG_USER_CHECKCODE,
    WM_USERWND_MSG_USER_REGISTER,
    WM_USERWND_MSG_USER_LOGIN,
    WM_USERWND_MSG_USER_CODE_LOGIN,
    WM_USERWND_MSG_USER_FIND_PASSWORD,
    WM_USERWND_MSG_GET_VERIFICATION2,
    WM_USERWND_MSG_USER_CHECKMOBILECODE,
    WM_USERWND_MSG_USER_MODIFYPASSWORD,

    WM_USERWND_MSG_END,
};

// 键盘映射消息
enum{
    WM_KEYWND_MSG_BEGIN = WM_USER + 1000,
    WM_KEYWND_MSG_REMOVE_KEY,

    WM_KEYWND_MSG_END,
};

// WpGameRoomIM
enum
{
    LpIMLoginSuccess = 0,				//登录IM成功
    LpIMLoginFail,						//登录IM失败
    LpIMLogout,							//登出IM
    LpIMChatRoomRequestFail,			//获取聊天室登录信息失败
    LpIMChatRoomEnterSuccess,			//进入聊天室成功
    LpIMChatRoomEnterFail,				//进入聊天室失败
    LpIMChatRoomExit,					//退出聊天室
    LpIMDisConnect,						//断线
    LpIMSendMsgSuccess,					//发送消息成功
    LpIMSendMsgFail,					//发送消息失败
};


//WM_MAINWND_MSG_GAME_CHANGED
//WM_MAINWND_MSG_PAGE_LOCALGAMES
enum
{
	WpLGAddGame = 0,
	WpLGDeleteGame,
	WpLGDeleteSelectGames,
	WpLGDeleteOneGames,
};

//WM_MAINWND_MSG_COMMON
enum
{
	WpCommonFindPwd = 1,
	WpCommonLogin,
	WpCommonEnterRoom,
	WpCommonEnterLiveRoom,
	WpCommonCreateRoom,
	WpCommonEnterCollection,
	WpCommonCloseFeedback,
    WpCommonActivated,
    WpCommonClosePayWnd,
	WpCommonCloseUserInfo,
	WpCommonRefreshGamehallWeb,
	WpCommonRefreshAppWeb,
	WpCommonRefreshHomeWeb,
	WpCommonBackToGamehallWeb,//返回大厅网页
	WpCommonLogout,
	WpCommonGotoGameInfoWeb,
	WpCommonExitGameRoom,//退出房间
	WpCommonGotoLoadWeb,
	WpCommonRunEmulator,
	WpCommonPartition,
	WpCommonFilterRoom,
	WpCommonUpdateLoadCount,
	WpCommonCreateRoomNoCopper,//铜板不足
	WpCommonReloadAppWeb,
	WpCommonTokenError,
	WpCommonDiskNoSpace,
	WpCommonUserInfo,
	WpCommonGotoContest,//跳转排位赛页面
	WpCommonGotoGameHall,//跳转对战大厅页面
	WpUpdateRecentList,
	WpCommonShowRankRule,
	WpCommonSelectAvatarImg,
	WpCommonUpdateUserIco,
	WpCommonUpdateRankWeb,
    WpCommonGotoGameCommentWeb,
    WpCommonGotoGameForumWeb,
    WpCommonUpdateTopButton,
    WpCommonLaunchGame,
    WpCommonUserModifyPassword,
    WpCommonUserForm,
    WpCommonSwitchAccount,
    WpCommonModifyUserInfo,
};

// WM_MAINWND_MSG_ACCOUNT_WND
enum {
    WpLogin = 0,
    WpLoginQuickJoin,
    WpLoginCreate,
    WpLoginJoin,
    WpLoginSearch,
    WpLoginSearchOut,
};

// WM_MAINWND_MSG_IM
enum {
    WpIMRecvMsg = 0,
    WpIMSendMsg,
    WpIMChatRoomInfo,
    WpIMChatRoomNotify,
};

// WM_MAINWND_MSG_MSGBOX1
enum {
    WpMsgbox1InputRoomID = 0,
    WpMsgbox1AccountBlock,
    WpMsgbox1NoRankFile,
    WpMsgbox1IMSendMsgError,
};

//WM_MAINWND_MSG_DOWNLOAD_FILE
enum
{
    WpDownloadFileRank = 0,
    WpDownloadFileAD,
};

//WM_MAINWND_MSG_RANK
enum
{
    WpContestNormal = 1,
    WpContestMsgPro,
};

// 主窗口COPYDATA消息ID定义
enum
{
    ID_MW_COPYDATA_BEGIN = 0,
    ID_MW_COPYDATA_JS_RUN_GAME,				//运行游戏
    ID_MW_COPYDATA_JS_TOAST,				//Toast
    ID_MW_COPYDATA_JS_LOGIN,				//登录验证
    ID_MW_COPYDATA_JS_COMPLET_ACCOUNT,		//完善账号
    ID_MW_COPYDATA_JS_STARTGAME,			//开始游戏
    ID_MW_COPYDATA_JS_MODIFY_ACCOUNT,		//修改昵称或手机号
    ID_MW_COPYDATA_JS_SEARCH_ROOM,			//搜索房间
    ID_MW_COPYDATA_JS_ENTER_ROOM,
    ID_MW_COPYDATA_JS_ENTER_LIVE_ROOM,
    ID_MW_COPYDATA_JS_RANK_PAGE,
    ID_MW_COPYDATA_JS_DATA_POST,
    ID_MW_COPYDATA_JS_GAME_STATUS,
    ID_MW_COPYDATA_JS_MODIFY_AVATAR,
    ID_MW_COPYDATA_JS_PAY_CONFIRM,
    ID_MW_COPYDATA_JS_GET_USER_STATUS,
    ID_MW_COPYDATA_JS_SHOWMSG,			  // 弹窗提示
    ID_MW_COPYDATA_JS_SHOWTOAST,		  // toast提示
    ID_MW_COPYDATA_JS_SHOWCONFIRM,		  // confirm提示
};


//定义下载中心隐藏窗口消息
enum
{
	WM_DLWND_MSG_BEGIN = WM_USER + 1000,
	WM_DLWND_MSG_UPDATE_LOADED,
	WM_DLWND_MSG_LOAD_PROGRESS,	//更新下载进度
	WM_DLWND_MSG_LOAD_FINISH,	//下载完成
	WM_DLWND_MSG_LOAD_ERROR,	//下载失败
	WM_DLWND_MSG_TASK_TRY_START,//尝试开始任务
	WM_DLWND_MSG_WEBHELPER,
	WM_DLWND_MSG_UNZIPING,
	WM_DLWND_MSG_UNZIP,
    WM_DLWND_MSG_INSTALL_APKING,
    WM_DLWND_MSG_INSTALL_APK,
    WM_DLWND_MSG_INSTALL_ANDROID_EMULATOR,
    WM_DLWND_MSG_IPA_INSTALL,
    WM_DLWND_MSG_IPA_INSTALLING,
};

enum {
    TIMER_ID_BEGIN = 600,
    TIMER_ID_CHECKEMULATOR,
    TIMER_ID_UPDATE_TIME,
    TIMER_ID_SCROLL_CHATROOM,
    //TIMER_ID_UPDATE_USER,
    TIMER_ID_CHECK_UPDATE,
    TIMER_ID_CONTEST_AD,
    TIMER_ID_UPDATE_HOUR,
    TIMER_ID_DOWNLOAD_PROGRESS,
    TIMER_ID_VERIFICATION_COUNT_DOWN,
    TIMER_ID_VERIFICATION_COUNT_DOWN2,
};

//WM_MAINWND_MSG_GAMEHALL
enum {
	WpConnectError = 0,	//无法连接到服务器
	WpLoginError,		//登录失败
	WpDisConnect,		//与服务器断开连接了
	WpPing,				//网络延时计算
	WpSubsRoom,			//订阅房间成功
	WpSubsRoomError,	//订阅房间失败
	WpPartitionError,
	WpSearchRoomError,
	WpCheckPassword,
	WpCheckPasswordError,
	WpQuickJoin,
	WpQuickJoinError,
	WpAccountKickOut,
};

//WM_MAINWND_MSG_PAGE_GAMEROOM
enum {
	WpGameRoomBase = 0,
	WpGameRoomUpdateList,
	WpGameRoomDeleteList,
	WpGameRoomUpdating,
	WpGameRoomUpdateFinish,
	WpGameRoomPartition,
	//WpGameRoomFilterGameName,
	WpGameRoomFilterRoomType,
	WpGameRoomUserInfo,
	WpGameRoomQuickJoinError,
	WpGameRoomSubsRoom,
	WpGameRoomIM,
};

// WM_HOTKEY
enum {
    ID_HOTKEY_BOS = 0,
};
#pragma once



//#define TEST
#ifdef TEST
//请求游戏详情
#define URL_REQUEST_GAME	L"http://testpcv1.ctapi.papa91.com/pc/game/gameinfov4"

//联想关键字搜索
#define URL_SEARCH_AUTO		L"http://testpcv1.ctapi.papa91.com/pc/search/searchauto"//L"http://testanv1.ctapi.papa91.com/search/searchauto"

//关键字搜索
#define URL_SEARCH_WORD		L"http://testpcv1.ctapi.papa91.com/pc/search/searchinfo"//L"http://testanv1.ctapi.papa91.com/search/searchinfo"

//大家都在搜
#define URL_RECOMMEND		L"http://testpcv1.ctapi.papa91.com/pc/game/randomgame"//L"http://testanv1.ctapi.papa91.com/game/randomgame"

//获取服务器IP、端口号
#define URL_GETSERVER		L"http://testbattle.papa91.com/lobby/login?"

//游客登录
#define URL_GUEST_LOGIN		L"http://testbattle.papa91.com/user/tourist/login"//L"http://testanv2.cjapi.papa91.com/user/tourist/login"

//用户信息
#define URL_USER_INFO		L"http://testanv3frapi.papa91.com/battle_pc/info"//L"http://testanv3cjapi.papa91.com/user/info"//L"http://testanv2.cjapi.papa91.com/user/info"

//获取游戏合集列表
#define URL_GET_GAMELIST	L"http://testpcv1.ctapi.papa91.com/pc/battle/battle_room_game_list_cfg"//L"http://testanv9.ctapi.papa91.com/battle/battle_room_game_list_cfg"

//创建房间，获取房间信息
#define URL_CREATE_ROOM		L"http://testbattle.papa91.com/lobby/create_room?"

//上传头像
#define URL_UOLODA_AVATAR		L"http://testanv3frapi.papa91.com/battle_pc/upfile"

#else
//请求游戏详情
#define URL_REQUEST_GAME	L"http://testguoren.abcs8.com/v1/gameinfo/index"

// 安卓游戏请求 目前只支持测试环境
#define URL_REQUEST_ANDROID_GAME_TEST	L"http://testpcv1.ctapi.papa91.com/pc/game/gameinfov4"

//联想关键字搜索
#define URL_SEARCH_AUTO		L"http://pcv1.ctapi.papa91.com/pc/search/searchauto"

//关键字搜索
#define URL_SEARCH_WORD		L"http://pcv1.ctapi.papa91.com/pc/search/searchinfo"

//大家都在搜
#define URL_RECOMMEND		L"http://pcv1.ctapi.papa91.com/pc/game/randomgame"

//获取服务器IP、端口号
#define URL_GETSERVER		L"http://anv3btapi.papa91.com/lobby/login?"

//游客登录
#define URL_GUEST_LOGIN		L"http://pcv3cjapi.5fun.cn/user/tourist/login"

//注册验证码获取
#define URL_GET_VERIFICATION   L"http://testguoren.abcs8.com/v1/user/getMobileCode"

//用户验证
#define URL_USER_CHECKCODE     L"http://testguoren.abcs8.com/v1/user/checkCode"

//短信验证
#define URL_USER_CHECKMOBILECODE     L"http://testguoren.abcs8.com/v1/user/checkMobileCode"

//用户注册
#define URL_USER_REGISTER      L"http://testguoren.abcs8.com/v1/user/reg"

//用户登录
#define URL_USER_LOGIN         L"http://testguoren.abcs8.com/v1/user/login"

//短信登录
#define URL_USER_CODE_LOGIN    L"http://testguoren.abcs8.com/v1/user/codeLogin"

//修改密码
#define URL_USER_MODIFYPASSWORD      L"http://testguoren.abcs8.com/v1/user/modifyPassword"

//用户信息
#define URL_USER_INFO		L"http://testguoren.abcs8.com/v1/user/getUserData"

//查询游戏更新接口
#define URL_CHECK_IOS_ENGINE_UPDATE		  L"http://testguoren.abcs8.com/v1/upgrade/index"

//查询游戏一次性更新接口
#define URL_CHECK_IOS_ENGINE_ALL_UPDATE	  L"http://testguoren.abcs8.com/v1/upgrade/allUpgrade"



//获取游戏合集列表
#define URL_GET_GAMELIST	L"http://pcv1.ctapi.papa91.com/pc/battle/battle_room_game_list_cfg"

//创建房间，获取房间信息
#define URL_CREATE_ROOM		L"http://anv3btapi.papa91.com/lobby/create_room?"

//上传头像
#define URL_UOLODA_AVATAR	L"http://anv3frapi.papa91.com/battle_pc/upfile"
#endif

//获取排位赛房间、对战房间游戏列表
#define URL_GET_RANKBATTLE_GAMES		L"http://anv3btapi.papa91.com/battle_pc/battle_support_game/index"


//查询游戏更新接口
#define URL_CHECK_GAME_UPDATE			L"http://anv3btapi.papa91.com/battle_pc/check_version/index"


//游戏库左侧导航配置接口
#define URL_GAME_TAB					L"http://anv3frapi.papa91.com/battle_pc/game_lib_menu/index_v3" //L"http://anv3frapi.papa91.com/battle_pc/game_lib_menu"

//////////////////////////////////////////////////////////////////////////////////////
//IM接口定义

//创建TOKEN
#define URL_IM_CREATE_TOKEN		L"http://im-service.5fun.com/imkeeper/wyyxid/createwyyxid"

//更新TOKEN
#define URL_IM_UPDATE_TOKEN		L"http://im-service.5fun.com/imkeeper/wyyxid/updatewyyxidandtoken"

//封禁网易云通信ID
#define URL_IM_BLOCK_TOKEN		L"http://im-service.5fun.com/imkeeper/wyyxid/unbanwyyxid"

//更新用户信息
#define URL_IM_UPDATE_USERINFO	L"http://im-service.5fun.com/imkeeper/userinfo/updateuserinfo"

//获取用户信息
#define URL_IM_GET_USERINFO		L"http://im-service.5fun.com/imkeeper/userinfo/getuserinfo"

//消息过滤
#define URL_IM_GET_CHECKMSG		L"http://im-service.5fun.com/imkeeper/chatroom/valid_message"

//获取更新日志 后续强更操作
//#define URL_GET_UPDATE_LOG		L"http://pcv1.ctapi.papa91.com/pc/ver/pc_chk_app_ver"

#define URL_GET_UPDATE_LOG		L"http://apipc.5fun.com/ver/version/index"

#define URL_GET_KEYBOARD		L"http://guorenres.5fun.com/keyMap/"
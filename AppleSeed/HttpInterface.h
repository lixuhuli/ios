#pragma once



//#define TEST
#ifdef TEST
//������Ϸ����
#define URL_REQUEST_GAME	L"http://testpcv1.ctapi.papa91.com/pc/game/gameinfov4"

//����ؼ�������
#define URL_SEARCH_AUTO		L"http://testpcv1.ctapi.papa91.com/pc/search/searchauto"//L"http://testanv1.ctapi.papa91.com/search/searchauto"

//�ؼ�������
#define URL_SEARCH_WORD		L"http://testpcv1.ctapi.papa91.com/pc/search/searchinfo"//L"http://testanv1.ctapi.papa91.com/search/searchinfo"

//��Ҷ�����
#define URL_RECOMMEND		L"http://testpcv1.ctapi.papa91.com/pc/game/randomgame"//L"http://testanv1.ctapi.papa91.com/game/randomgame"

//��ȡ������IP���˿ں�
#define URL_GETSERVER		L"http://testbattle.papa91.com/lobby/login?"

//�ο͵�¼
#define URL_GUEST_LOGIN		L"http://testbattle.papa91.com/user/tourist/login"//L"http://testanv2.cjapi.papa91.com/user/tourist/login"

//�û���Ϣ
#define URL_USER_INFO		L"http://testanv3frapi.papa91.com/battle_pc/info"//L"http://testanv3cjapi.papa91.com/user/info"//L"http://testanv2.cjapi.papa91.com/user/info"

//��ȡ��Ϸ�ϼ��б�
#define URL_GET_GAMELIST	L"http://testpcv1.ctapi.papa91.com/pc/battle/battle_room_game_list_cfg"//L"http://testanv9.ctapi.papa91.com/battle/battle_room_game_list_cfg"

//�������䣬��ȡ������Ϣ
#define URL_CREATE_ROOM		L"http://testbattle.papa91.com/lobby/create_room?"

//�ϴ�ͷ��
#define URL_UOLODA_AVATAR		L"http://testanv3frapi.papa91.com/battle_pc/upfile"

#else
//������Ϸ����
#define URL_REQUEST_GAME	L"http://testguoren.abcs8.com/v1/gameinfo/index"

// ��׿��Ϸ���� Ŀǰֻ֧�ֲ��Ի���
#define URL_REQUEST_ANDROID_GAME_TEST	L"http://testpcv1.ctapi.papa91.com/pc/game/gameinfov4"

//����ؼ�������
#define URL_SEARCH_AUTO		L"http://pcv1.ctapi.papa91.com/pc/search/searchauto"

//�ؼ�������
#define URL_SEARCH_WORD		L"http://pcv1.ctapi.papa91.com/pc/search/searchinfo"

//��Ҷ�����
#define URL_RECOMMEND		L"http://pcv1.ctapi.papa91.com/pc/game/randomgame"

//��ȡ������IP���˿ں�
#define URL_GETSERVER		L"http://anv3btapi.papa91.com/lobby/login?"

//�ο͵�¼
#define URL_GUEST_LOGIN		L"http://pcv3cjapi.5fun.cn/user/tourist/login"

//ע����֤���ȡ
#define URL_GET_VERIFICATION   L"http://testguoren.abcs8.com/v1/user/getMobileCode"

//�û���֤
#define URL_USER_CHECKCODE     L"http://testguoren.abcs8.com/v1/user/checkCode"

//������֤
#define URL_USER_CHECKMOBILECODE     L"http://testguoren.abcs8.com/v1/user/checkMobileCode"

//�û�ע��
#define URL_USER_REGISTER      L"http://testguoren.abcs8.com/v1/user/reg"

//�û���¼
#define URL_USER_LOGIN         L"http://testguoren.abcs8.com/v1/user/login"

//���ŵ�¼
#define URL_USER_CODE_LOGIN    L"http://testguoren.abcs8.com/v1/user/codeLogin"

//�޸�����
#define URL_USER_MODIFYPASSWORD      L"http://testguoren.abcs8.com/v1/user/modifyPassword"

//�û���Ϣ
#define URL_USER_INFO		L"http://testguoren.abcs8.com/v1/user/getUserData"

//��ѯ��Ϸ���½ӿ�
#define URL_CHECK_IOS_ENGINE_UPDATE		  L"http://testguoren.abcs8.com/v1/upgrade/index"

//��ѯ��Ϸһ���Ը��½ӿ�
#define URL_CHECK_IOS_ENGINE_ALL_UPDATE	  L"http://testguoren.abcs8.com/v1/upgrade/allUpgrade"



//��ȡ��Ϸ�ϼ��б�
#define URL_GET_GAMELIST	L"http://pcv1.ctapi.papa91.com/pc/battle/battle_room_game_list_cfg"

//�������䣬��ȡ������Ϣ
#define URL_CREATE_ROOM		L"http://anv3btapi.papa91.com/lobby/create_room?"

//�ϴ�ͷ��
#define URL_UOLODA_AVATAR	L"http://anv3frapi.papa91.com/battle_pc/upfile"
#endif

//��ȡ��λ�����䡢��ս������Ϸ�б�
#define URL_GET_RANKBATTLE_GAMES		L"http://anv3btapi.papa91.com/battle_pc/battle_support_game/index"


//��ѯ��Ϸ���½ӿ�
#define URL_CHECK_GAME_UPDATE			L"http://anv3btapi.papa91.com/battle_pc/check_version/index"


//��Ϸ����ർ�����ýӿ�
#define URL_GAME_TAB					L"http://anv3frapi.papa91.com/battle_pc/game_lib_menu/index_v3" //L"http://anv3frapi.papa91.com/battle_pc/game_lib_menu"

//////////////////////////////////////////////////////////////////////////////////////
//IM�ӿڶ���

//����TOKEN
#define URL_IM_CREATE_TOKEN		L"http://im-service.5fun.com/imkeeper/wyyxid/createwyyxid"

//����TOKEN
#define URL_IM_UPDATE_TOKEN		L"http://im-service.5fun.com/imkeeper/wyyxid/updatewyyxidandtoken"

//���������ͨ��ID
#define URL_IM_BLOCK_TOKEN		L"http://im-service.5fun.com/imkeeper/wyyxid/unbanwyyxid"

//�����û���Ϣ
#define URL_IM_UPDATE_USERINFO	L"http://im-service.5fun.com/imkeeper/userinfo/updateuserinfo"

//��ȡ�û���Ϣ
#define URL_IM_GET_USERINFO		L"http://im-service.5fun.com/imkeeper/userinfo/getuserinfo"

//��Ϣ����
#define URL_IM_GET_CHECKMSG		L"http://im-service.5fun.com/imkeeper/chatroom/valid_message"

//��ȡ������־ ����ǿ������
//#define URL_GET_UPDATE_LOG		L"http://pcv1.ctapi.papa91.com/pc/ver/pc_chk_app_ver"

#define URL_GET_UPDATE_LOG		L"http://apipc.5fun.com/ver/version/index"

#define URL_GET_KEYBOARD		L"http://guorenres.5fun.com/keyMap/"
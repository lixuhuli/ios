#pragma once
/*******************************************
*	����ͳ���¼����ֶζ���
*	2017��10��19��14:46:18
*	Yaojn
*/


////////////////////////////////////////////////////////////////////////////////////////////
//�����¼�����
#define EVENT_START_CLIENT			"startClient"
#define EVENT_GAME_DOWNLOAD			"gameDownload"
#define EVENT_START_GAME			"gameStart"
#define EVENT_INSTALL_GAME			"gameInstall"
#define EVENT_UNINSTALL				"clientRemove"
#define EVENT_INSTALL				"clientInstall"

#define EVENT_LOGIN					"userLogin"
#define EVENT_CREATE_ROOM			"createRoom"
#define EVENT_CLICK_LOBBY			"clickLobby"		//���������Ϸ
#define EVENT_CLICK_COLL_PAGE		"clickComPage"		//��������ϼ�
#define EVENT_ROOM_NUMBER			"lobbyRoomNumber"	//����������
#define EVENT_DUMP					"collDetails"		//�����ռ�
#define EVENT_GAME_UNZIP			"gameUnzip"			//��ѹ��Ϸ
#define EVENT_START_RANK			"clickVsStart"		//��ʼ��λ��


///////////////////////////////////////////////////////////////////////////////////////////
//���������ֶ�
#define KEY_DEVICE_ID			"deviceId"
#define KEY_AD_ID				"adId"
#define KEY_USER_ID				"uid"
#define KEY_APP_VER				"appVersion"
#define KEY_OS_VER				"osVersion"
#define KEY_OS_TYPE				"osType"
#define KEY_MAC					"mac"
#define KEY_TIME				"clickTime"				//����ʱ��
#define KEY_GAME_ID				"gameId"
#define KEY_GAME_TYPE			"gameType"				//�ֻ���FC��PSP��GBA
#define KEY_GAME_MODE			"gameMode"				//������Ϸ����ս
#define KEY_ROOM_ID				"roomId"
#define KEY_GAME_VER			"gameVersion"			//��Ϸ�汾
#define KEY_ROOM_NUMBER			"roomNumber"			//������Ŀ
#define KEY_DOWNLOAD_SPEED		"downSpeed"			    //�����ٶ�
#define KEY_DOWNLOAD_STATUS		"opStatus"				//����״̬��0.�������أ�1.����ʧ�ܣ�2.���سɹ���
#define KEY_UNZIP_SPEED			"unzipSpeed"			//��ѹ�ٶ�
#define KEY_UNZIP_STATUS		"opStatus"				//��ѹ״̬��0.�����ѹ��1����ѹ�ɹ���2����ѹʧ�ܣ�
#define KEY_STATUS				"status"				//״̬0����������1���ֹ�����
#define KEY_ROOM_TYPE			"matchLevel"			//���伶��0����ͨ����1����Ӣ����
		

//�������
#define KEY_DUMP_TYPE			"collType"				//�������ͣ�1��ƽ̨������2��pcģ����������"
#define KEY_DUMP_CODE			"collReason"			//����ԭ��


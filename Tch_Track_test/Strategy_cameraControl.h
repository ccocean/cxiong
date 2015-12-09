//#include "stdafx.h"

#ifndef CLIENT_CAMERACONTROL_H
#define CLIENT_CAMERACONTROL_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
	#include "winsock.h"
	#pragma comment(lib, "ws2_32.lib")
	#include "pthread.h"
#else
	#include <pthread.h>
#endif


///UDP���ƶ���
#define PANandTILT_CTRL_PTZ_STOP            0x1000
#define PANandTILT_CTRL_PTZ_UP              0x1001
#define PANandTILT_CTRL_PTZ_DOWN            0x1002
#define PANandTILT_CTRL_PTZ_LEFT            0x1003
#define PANandTILT_CTRL_PTZ_RIGHT           0x1004

#define PANandTILT_CTRL_PTZ_ZOOMIN          0x2001
#define PANandTILT_CTRL_PTZ_ZOOMOUT         0x2002
#define PANandTILT_CTRL_PTZ_ZOOMSTOP        0x2003
#define PANandTILT_CTRL_PTZ_FOCUSIN         0x3001
#define PANandTILT_CTRL_PTZ_FOCUSOUT        0x3002
#define PANandTILT_CTRL_PTZ_FOCUSSTOP       0x3003
#define PANandTILT_CTRL_PTZ_FOCUSAUTO		0x3044
#define PANandTILT_CTRL_PTZ_FOCUSMANUAL		0x3045

#define PANandTILT_CTRL_PTZ_GOTO_PRESET     0x4001
#define PANandTILT_CTRL_PTZ_SET_PRESET      0x4002
#define PANandTILT_CTRL_PTZ_CLE_PRESET      0x4003

#define PANandTILT_CTRL_PTZ_HOME            0x5001

#define PANandTILT_CTRL_PTZ_MASK			0XF000

#define HI_NET_DEV_CMD_REBOOT               0x1018

#define ACCEPT_BUFFER_SIZE 256

typedef struct CameraControl
{
	BOOL m_flag_start;

	struct sockaddr_in m_addr;
	char m_buffer[ACCEPT_BUFFER_SIZE];
	int m_addr_len;
	int m_send_socket;

	int move_speed_pan;		//ˮƽ��Χ0-18
	int move_speed_tilt;	//��б��Χ0-14

	BOOL m_thread_run_flag;
	pthread_t heart_tid;
	int m_posit_pan;
	int m_posit_tilt;
	int m_zoomValue;

	HANDLE m_hHandle1;
	HANDLE m_hHandle2;

} Strategy_CamControl_t;

int init_cam(Strategy_CamControl_t *cam);
int startControl(Strategy_CamControl_t *cam,const char addr[], const int port);
void stopControl(Strategy_CamControl_t *cam);
BOOL getStart_Status(Strategy_CamControl_t *cam);

void setMoveSpeed(Strategy_CamControl_t *cam,int speed_pan, int speed_tilt);
BOOL move(Strategy_CamControl_t *cam, int posittion_pan, int posittion_tilt, int flag);
BOOL setZoom(Strategy_CamControl_t *cam, int zoomPosition);
//BOOL reBoot(Strategy_CamControl_t *cam);

BOOL home(Strategy_CamControl_t *cam);
BOOL keepInstruct(Strategy_CamControl_t *cam,int type);
BOOL preset(Strategy_CamControl_t *cam ,int type, int id);

BOOL getPosit(Strategy_CamControl_t *cam ,int *posit_pan, int *posit_tilt, int waitMillisecond);
BOOL getZoom(Strategy_CamControl_t *cam ,int *zoomValue, int waitMillisecond);

BOOL getRun_Status(Strategy_CamControl_t *cam);
int recv_CameraInfo(Strategy_CamControl_t *cam, char* buffer);

void set_CameraInfo_panTilt(Strategy_CamControl_t *cam,int posit_pan, int posit_tilt);
void set_CameraInfo_zoom(Strategy_CamControl_t *cam,int zoomValue);

//class PanAndTiltCameraControl
//{
//public:
//	PanAndTiltCameraControl();
//	~PanAndTiltCameraControl();
//	int startControl(const char addr[], const int port);
//	void stopControl();
//	BOOL getStart_Status();
//
//	void setMoveSpeed(int speed_pan, int speed_tilt);
//	BOOL move(int posittion_pan, int posittion_tilt, int flag = TRUE);
//	BOOL setZoom(int zoomPosition);
//
//	BOOL home();
//	BOOL keepInstruct(int type, void* otherData = NULL);
//	BOOL preset(int type, int id);
//
//	BOOL getPosit(int *posit_pan, int *posit_tilt, int waitMillisecond);
//	BOOL getZoom(int *zoomValue, int waitMillisecond);
//
//	BOOL getRun_Status();
//	int recv_CameraInfo(char* buffer);
//	HANDLE m_hHandle1;
//	HANDLE m_hHandle2;
//	void set_CameraInfo_panTilt(int posit_pan, int posit_tilt);
//	void set_CameraInfo_zoom(int zoomValue);
//protected:
//
//private:
//	BOOL m_flag_start;
//
//	struct sockaddr_in m_addr;
//	char m_buffer[ACCEPT_BUFFER_SIZE];
//	int m_addr_len;
//	int m_send_socket;
//
//	int move_speed_pan;		//ˮƽ��Χ0-18
//	int move_speed_tilt;	//��б��Χ0-14
//	int send_net_cmd(char *cmd, int len);
//
//	BOOL keepMove(int type);
//	BOOL keepZoom(int type);
//	BOOL keepFocus(int type);
//
//	BOOL m_thread_run_flag;
//	pthread_t heart_tid;
//	int m_posit_pan;
//	int m_posit_tilt;
//	int m_zoomValue;
//};

#endif
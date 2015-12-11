#ifdef WIN32
#include "Strategy_cameraControl.h"
#include <windows.h>
#else
#include "../track_policy/Strategy_cameraControl.h"
#include <sys/time.h>
#endif

#ifdef WIN32
int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tp->tv_sec = (long)clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return 0;
}
#endif // WIN32

BOOL getStart_Status(Strategy_CamControl_t *cam)
{
	return cam->m_flag_start;
}

BOOL getRun_Status(Strategy_CamControl_t *cam)
{
	return cam->m_thread_run_flag;
}

static void* recv_thread(void *argv)
{
	Strategy_CamControl_t* p = (Strategy_CamControl_t*)argv;
	int len;
	char buffer[256];
	int posit_pan;
	int posit_tilt;
	int zoomValue;
	while (getRun_Status(p) == TRUE)
	{
		if (getStart_Status(p) == TRUE)
		{	
			len = recv_CameraInfo(p,buffer);
			if (len >= 7)
			{
				if (buffer[1] == 0x50)
				{
					if (len == 11)
					{
						posit_pan = ((buffer[2] & 0x0f) << 12) + ((buffer[3] & 0x0f) << 8) + ((buffer[4] & 0x0f) << 4) + (buffer[5] & 0x0f);
						posit_pan = ((posit_pan >> 15) & 1) ? posit_pan | ((-1 >> 16) << 16) : posit_pan & 0xffff;

						posit_tilt = ((buffer[6] & 0x0f) << 12) + ((buffer[7] & 0x0f) << 8) + ((buffer[8] & 0x0f) << 4) + (buffer[9] & 0x0f);
						posit_tilt = ((posit_tilt >> 15) & 1) ? posit_tilt | ((-1 >> 16) << 16) : posit_tilt & 0xffff;
						pthread_mutex_lock(&p->mutex1);
						set_CameraInfo_panTilt(p,posit_pan, posit_tilt);
						pthread_cond_signal(&p->cond1);
						pthread_mutex_unlock(&p->mutex1);
					}
					else if (len == 7)
					{
						zoomValue = ((buffer[2] & 0x0f) << 12) + ((buffer[3] & 0x0f) << 8) + ((buffer[4] & 0x0f) << 4) + (buffer[5] & 0x0f);
						zoomValue = ((zoomValue >> 15) & 1) ? zoomValue | ((-1 >> 16) << 16) : zoomValue & 0xffff;
						pthread_mutex_lock(&p->mutex2);
						set_CameraInfo_zoom(p,zoomValue);
						pthread_cond_signal(&p->cond2);
						pthread_mutex_unlock(&p->mutex2);
					}
				}
			}
		}
#ifdef WIN32
		Sleep(10);
#else
		usleep(10 * 1000);
#endif
	}
	return NULL;
}


static int send_net_cmd(Strategy_CamControl_t *cam,char *cmd, int len);

static BOOL keepMove(Strategy_CamControl_t *cam,int type);
static BOOL keepZoom(Strategy_CamControl_t *cam,int type);
static BOOL keepFocus(Strategy_CamControl_t *cam,int type);

#define CLIENT_CAMERA_SPEED_PAN_MIN 0
#define CLIENT_CAMERA_SPEED_PAN_MAX 24
#define CLIENT_CAMERA_SPEED_TILT_MIN 0
#define CLIENT_CAMERA_SPEED_TILT_MAX 20

int init_cam(Strategy_CamControl_t *cam)
{
    cam=(Strategy_CamControl_t*)malloc(sizeof(Strategy_CamControl_t));
	cam->m_flag_start = FALSE;
	memset(&cam->m_addr, 0, sizeof(cam->m_addr));
	memset(&cam->m_buffer, 0, sizeof(cam->m_buffer));
	cam->m_addr_len = sizeof(struct sockaddr_in);
	cam->m_send_socket = -1;

	cam->move_speed_pan = CLIENT_CAMERA_SPEED_PAN_MAX;
	cam->move_speed_tilt = CLIENT_CAMERA_SPEED_TILT_MAX;

	pthread_mutex_init(&(cam->mutex1),NULL);
	pthread_mutex_init(&(cam->mutex2), NULL);
	pthread_cond_init(&(cam->cond1),NULL);
	pthread_cond_init(&(cam->cond2),NULL);
	cam->m_thread_run_flag = TRUE;
	int ret = pthread_create(&cam->heart_tid, NULL, recv_thread, (void *)(cam));
	return ret;
}


int close_cam(Strategy_CamControl_t *cam)
{
	if (cam->m_flag_start == TRUE)
	{
		stopControl(cam);
	}
	cam->m_thread_run_flag = FALSE;
	pthread_join(cam->heart_tid, NULL);

	pthread_cond_destroy(&(cam->cond1));
	pthread_cond_destroy(&(cam->cond2));
	pthread_mutex_destroy(&(cam->mutex1));
	pthread_mutex_destroy(&(cam->mutex2));
    return 0;
}

int startControl(Strategy_CamControl_t *cam, const char addr[], const int port)
{
	if (cam->m_thread_run_flag == TRUE)
	{
		if (cam->m_flag_start == FALSE)
		{
			cam->m_addr.sin_family = AF_INET;
			cam->m_addr.sin_addr.s_addr = inet_addr(addr);
			cam->m_addr.sin_port = htons(port);
			cam->m_send_socket = socket(AF_INET, SOCK_DGRAM, 0);
			connect(cam->m_send_socket, &(cam->m_addr), sizeof(cam->m_addr));
			struct timeval Time;
			Time.tv_sec = 1;
			Time.tv_usec = 0;
			setsockopt(cam->m_send_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&Time, sizeof(int));
			if (3 > cam->m_send_socket)
			{
				printf("error!\n");
				return FALSE;
			}
			cam->m_flag_start = TRUE;
		}
		setMoveSpeed(cam, CLIENT_CAMERA_SPEED_PAN_MAX, CLIENT_CAMERA_SPEED_TILT_MAX);//max speed
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void stopControl(Strategy_CamControl_t *cam)
{
#ifdef WIN32
	closesocket(cam->m_send_socket);
#else
	close(cam->m_send_socket);
#endif
	cam->m_flag_start = FALSE;
}

int send_net_cmd(Strategy_CamControl_t *cam, char *cmd, int len)
{
	//return sendto(cam->m_send_socket, cmd, len, 0, (struct sockaddr *)&cam->m_addr, cam->m_addr_len);
	return send(cam->m_send_socket, cmd, len, 0);
}

void setMoveSpeed(Strategy_CamControl_t *cam, int speed_pan, int speed_tilt)
{
	if (speed_pan < CLIENT_CAMERA_SPEED_PAN_MIN)
		speed_pan = CLIENT_CAMERA_SPEED_PAN_MIN;
	if (speed_pan > CLIENT_CAMERA_SPEED_PAN_MAX)
		speed_pan = CLIENT_CAMERA_SPEED_PAN_MAX;
	if (speed_tilt < CLIENT_CAMERA_SPEED_TILT_MIN)
		speed_tilt = CLIENT_CAMERA_SPEED_TILT_MIN;
	if (speed_tilt > CLIENT_CAMERA_SPEED_TILT_MAX)
		speed_tilt = CLIENT_CAMERA_SPEED_TILT_MAX;

	cam->move_speed_pan = speed_pan;
	cam->move_speed_tilt = speed_tilt;
}



#define CLIENT_CAMERA_POSITTOIN_PAN_MIN -0x0990
#define CLIENT_CAMERA_POSITTOIN_PAN_MAX 0x0990
#define CLIENT_CAMERA_POSITTOIN_TILT_MIN -0x01B0
#define CLIENT_CAMERA_POSITTOIN_TILT_MAX 0x0510
#define POSITTION_TO_4BYTE(x) ((x >> 12) & 0x000f), ((x >> 8) & 0x000f), ((x >> 4) & 0x000f), (x & 0x000f)
BOOL move(Strategy_CamControl_t *cam, int posittion_pan, int posittion_tilt, int flag)
{
	if (posittion_pan < CLIENT_CAMERA_POSITTOIN_PAN_MIN)
		posittion_pan = CLIENT_CAMERA_POSITTOIN_PAN_MIN;
	if (posittion_pan > CLIENT_CAMERA_POSITTOIN_PAN_MAX)
		posittion_pan = CLIENT_CAMERA_POSITTOIN_PAN_MAX;

	if (posittion_tilt < CLIENT_CAMERA_POSITTOIN_TILT_MIN)
		posittion_tilt = CLIENT_CAMERA_POSITTOIN_TILT_MIN;
	if (posittion_tilt > CLIENT_CAMERA_POSITTOIN_TILT_MAX)
		posittion_tilt = CLIENT_CAMERA_POSITTOIN_TILT_MAX;

	char instruct[] = { 0x81, 0x01, 0x06, 0x02, cam->move_speed_pan, cam->move_speed_tilt, POSITTION_TO_4BYTE(posittion_pan), POSITTION_TO_4BYTE(posittion_tilt), 0xFF };
	if (flag == TRUE)
	{
		instruct[3] = 0x03;//相对坐标
	}
	else
	{
		instruct[3] = 0x02;//绝对坐标
	}
	if (send_net_cmd(cam, instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	else
		return TRUE;
}

#define CLIENT_CAMERA_ZOOM_MIN 0x0000
#define CLIENT_CAMERA_ZOOM_MAX 0x4000
#define ZOOMPOSITION_TO_4BYTE(x) ((x >> 12) & 0x000f), ((x >> 8) & 0x000f), ((x >> 4) & 0x000f), (x & 0x000f)
BOOL setZoom(Strategy_CamControl_t *cam, int zoomPosition)
{
	if (zoomPosition < CLIENT_CAMERA_ZOOM_MIN)
		zoomPosition = CLIENT_CAMERA_ZOOM_MIN;
	if (zoomPosition > CLIENT_CAMERA_ZOOM_MAX)
		zoomPosition = CLIENT_CAMERA_ZOOM_MAX;

	char instruct[] = { 0x81, 0x01, 0x04, 0x47, ZOOMPOSITION_TO_4BYTE(zoomPosition), 0xFF };
	if (send_net_cmd(cam, instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	else
		return TRUE;
}

BOOL home(Strategy_CamControl_t *cam)
//返回源点
{
	char instruct[] = { 0x81, 0x01, 0x06, 0x04, 0xFF };
	if (send_net_cmd(cam, instruct, sizeof(instruct)) != sizeof(instruct))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL keepInstruct(Strategy_CamControl_t *cam,int type)
{
	BOOL flag = FALSE;
	switch (type & PANandTILT_CTRL_PTZ_MASK){
	case 0x1000:
		flag = keepMove(cam,type);
		break;
	case 0x2000:
		flag = keepZoom(cam,type);
		break;
	case 0x3000:
		//对焦操作
		flag = keepFocus(cam,type);
		break;
	default:
		break;
	}
	return flag;
}

BOOL preset(Strategy_CamControl_t *cam,int type, int id)
{
	//预置位
	if (id<0 || id>254)
		return FALSE;

	char instruct[] = { 0x81, 0x01, 0x04, 0x3F, 0x00, id, 0xFF };
	switch (type){
	case PANandTILT_CTRL_PTZ_GOTO_PRESET://运行到预置位
		instruct[4] = 0x02;
		break;
	case PANandTILT_CTRL_PTZ_SET_PRESET://设置当前位置为预置位
		instruct[4] = 0x01;
		break;
	case PANandTILT_CTRL_PTZ_CLE_PRESET://删除当前预置位
		instruct[4] = 0x00;
		break;
	default:
		instruct[4] = 0x02;
		break;
	}
	if (send_net_cmd(cam,instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	else
		return TRUE;
}

BOOL keepFocus(Strategy_CamControl_t *cam,int type)
{
	char instruct[] = { 0x81, 0x01, 0x04, 0x08, 0x00, 0xFF };
	switch (type)
	{
	case PANandTILT_CTRL_PTZ_FOCUSMANUAL:
		instruct[3] = 0x38;
		instruct[4] = 0x03;
		break;
	case PANandTILT_CTRL_PTZ_FOCUSAUTO:
		instruct[3] = 0x38;
		instruct[4] = 0x02;
		break;
	case PANandTILT_CTRL_PTZ_FOCUSIN:
		instruct[4] = 0x02;
		break;
	case PANandTILT_CTRL_PTZ_FOCUSOUT:
		instruct[4] = 0x03;
		break;
	case PANandTILT_CTRL_PTZ_FOCUSSTOP:
		break;
	default:
		instruct[3] = 0x38;
		instruct[4] = 0x02;
		break;
	}
	if (send_net_cmd(cam, instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	else
		return TRUE;
}

BOOL keepMove(Strategy_CamControl_t *cam,int type)
{
	char instruct[] = { 0x81, 0x01, 0x06, 0x01, cam->move_speed_pan, cam->move_speed_tilt, 0x00, 0x00, 0xFF };
	switch (type){
	case PANandTILT_CTRL_PTZ_STOP:
		instruct[6] = 0x03;
		instruct[7] = 0x03;
		break;
	case PANandTILT_CTRL_PTZ_UP:
		instruct[6] = 0x03;
		instruct[7] = 0x01;
		break;
	case PANandTILT_CTRL_PTZ_DOWN:
		instruct[6] = 0x03;
		instruct[7] = 0x02;
		break;
	case PANandTILT_CTRL_PTZ_LEFT:
		instruct[6] = 0x01;
		instruct[7] = 0x03;
		break;
	case PANandTILT_CTRL_PTZ_RIGHT:
		instruct[6] = 0x02;
		instruct[7] = 0x03;
		break;
	default:
		instruct[6] = 0x03;
		instruct[7] = 0x03;
		break;
	}
	if (send_net_cmd(cam, instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	else
		return TRUE;
}

BOOL keepZoom(Strategy_CamControl_t *cam, int type)
{
	char instruct[] = { 0x81, 0x01, 0x04, 0x07, 0x00, 0xFF };
	switch (type){
	case PANandTILT_CTRL_PTZ_ZOOMIN:
		instruct[4] = 0x02;
		break;
	case PANandTILT_CTRL_PTZ_ZOOMOUT:
		instruct[4] = 0x03;
		break;
	case PANandTILT_CTRL_PTZ_ZOOMSTOP:
		instruct[4] = 0x00;
		break;
	default:
		instruct[4] = 0x03;
		break;
	}
	if (send_net_cmd(cam, instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	else
		return TRUE;
}

BOOL getPosit(Strategy_CamControl_t *cam, int *posit_pan, int *posit_tilt, int waitMillisecond)
{
	char instruct[] = { 0x81, 0x09, 0x06, 0x12, 0xFF };
	struct timeval tt;
	gettimeofday(&tt, NULL);
	struct timespec waitTime;
	waitTime.tv_sec = tt.tv_sec + waitMillisecond / 1000;
	waitTime.tv_nsec = tt.tv_usec * 1000 + (waitMillisecond % 1000) * 1000 * 1000;

	BOOL ret = FALSE;
	pthread_mutex_lock(&cam->mutex1);
	if (send_net_cmd(cam,instruct, sizeof(instruct)) == sizeof(instruct))
	{
		if (pthread_cond_timedwait(&cam->cond1, &cam->mutex1, &waitTime) != ETIMEDOUT)
		{
			*posit_pan = cam->m_posit_pan;
			*posit_tilt = cam->m_posit_tilt;
			ret = TRUE;
		}
	}
	pthread_mutex_unlock(&cam->mutex1);
	return ret;
}

BOOL getZoom(Strategy_CamControl_t *cam, int *zoomValue, int waitMillisecond)
{
	char instruct[] = { 0x81, 0x09, 0x04, 0x47, 0xFF };
	struct timeval tt;
	gettimeofday(&tt,NULL);
	struct timespec waitTime;
	waitTime.tv_sec = tt.tv_sec + waitMillisecond / 1000;
	waitTime.tv_nsec = tt.tv_usec * 1000 + (waitMillisecond % 1000) * 1000 * 1000;

	BOOL ret = FALSE;
	pthread_mutex_lock(&cam->mutex2);
	if (send_net_cmd(cam, instruct, sizeof(instruct)) == sizeof(instruct))
	{
		if (pthread_cond_timedwait(&cam->cond2, &cam->mutex2, &waitTime) != ETIMEDOUT)
		{
			*zoomValue = cam->m_zoomValue;
			ret = TRUE;
		}
	}
	pthread_mutex_unlock(&cam->mutex2);
	return ret;
}

//BOOL reBoot(Strategy_CamControl_t *cam)
//{
//	char instruct[] = {};
//}

int recv_CameraInfo(Strategy_CamControl_t *cam, char* buffer)
{
	//int len = recvfrom(cam->m_send_socket, cam->m_buffer, sizeof(cam->m_buffer), 0,
	//		(struct sockaddr*)&cam->m_addr, (socklen_t *)&cam->m_addr_len);
	int len = recv(cam->m_send_socket, cam->m_buffer, sizeof(cam->m_buffer),0);
	if (len > 0)
	{
		memcpy(buffer, cam->m_buffer, len);
	}
	return len;
}

void set_CameraInfo_panTilt(Strategy_CamControl_t *cam, int posit_pan, int posit_tilt)
{
	cam->m_posit_pan = posit_pan;
	cam->m_posit_tilt = posit_tilt;
}

void set_CameraInfo_zoom(Strategy_CamControl_t *cam , int zoomValue)
{
	cam->m_zoomValue = zoomValue;
}


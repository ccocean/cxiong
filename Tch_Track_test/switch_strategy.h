/*
* To change this license header, choose License Headers in Project Properties.
* To change this template file, choose Tools | Templates
* and open the template in the editor.
*/

/*
* File:   Switch_Strategy.h
* Author: Xiongcc
*
* Created on 2015��11��30��, ����2:37
*/

#ifndef SWITCH_STRATEGY_H
#define SWITCH_STRATEGY_H

#ifdef __cplusplus
extern "C" {
#endif


#include "Strategy_Queue.h"
#
#include "Strategy_cameraControl.h"

//

#ifndef WIN32
#include <mcfw/interfaces/ti_media_std.h>
#include <mcfw/interfaces/link_api/tch_params.h>
#include <mcfw/interfaces/link_api/stuTrack_settings_parameter.h>
#include <mcfw/interfaces/link_api/scdLink.h>
#include "mainEnc_communtication.h"
#include "public_mp.h"
    
#else
#include "stuTrack_settings_parameter.h"
#include "tch_params.h"
#include "scdLink.h"
#include "public_mp.h"
    
    //���õ�Ӱģʽ�׻���
	typedef struct _HD_FIRSTPICTURE_MODE_S
	{	
		int option;//���û��߻�ȡ
		int firstpicture;
		int display_time;

	}MainEnc_FirstPicMode_t;
    
#endif

#define FPS 25
#define SWITCH_TIME 2000

#define NULL_ACTIVITY 0
#define TCH_ACTIVITY 1
#define STU_ACTIVITY 2
#define VGA_ACTIVITY 3

//������ʦ����״̬
#define STRATEGY_TRACK_TCH_NULL 0
#define STRATEGY_TRACK_TCH_STAND 1			//��ʦ����վ��״̬
#define STRATEGY_TRACK_TCH_MOVEINVIEW 2		//��ʦ����д��ͷ��
#define STRATEGY_TRACK_TCH_MOVEOUTVIEW 3		//��ʦ�߳���д��ͷ
#define STRATEGY_TRACK_TCH_OUTSIDE 4			//��ʦ�뿪��������
#define STRATEGY_TRACK_TCH_BLACKBOARD 5		//��ʦ��д����״̬
#define STRATEGY_TRACK_TCH_MULITY 6			//��̨��Ŀ��״̬

#define STRATEGY_TRACK_STU_NULL 0			//û��ѧ���ź�
#define STRATEGY_TRACK_STU_STAND_SINGLE 1	//����ѧ��վ��
#define STRATEGY_TRACK_STU_STAND_MULITY 2	//���ѧ��վ��
#define STRATEGY_TRACK_STU_MOVE_SINGLE 3	//����ѧ�������ƶ�
#define STRATEGY_TRACK_STU_MOVE_MULITY 4	//���ѧ�������ƶ�

#define STRATEGY_TRACK_VGA_NULL 0	//	VGA���ź�
#define STRATEGY_TRACK_VGA_ACTIVITY 1	//VGA���ź�

//��ʱ��
typedef struct StrategyTimer
{
	unsigned long start;
	unsigned long finish;
	double deltaTime;
}Strategy_Timer_t;

typedef struct Strategy_Result
{
        int result_activity;
	int display_time;
        int firstpicture;
        
	int VGA_flag;//VGA�źű�־
	int STU_flag;//ѧ���źű�־
	int TCH_flag;//��ʦ�źű�־

	Strategy_Queue *tch_que;//��ʦ���з���
	int tch_posIndex;//��ʦλ��
	int tch_status;//��ʦ״̬

	TrackPrarms_Point_t stu_pos;//ѧ������

	int status;//����״̬��ָ��
	int last_status;//��һ�εĻ���״̬

	int posit_pan; //���ˮƽ����
	int posit_tilt; //�����ֱ����

	Strategy_Timer_t timer_stra;//��ʱ��

}Strategy_Result_t;

typedef struct Track_Strategy
{
        int isInit;
	char *tch_ip;
	char *stu_ip;
	int tch_port;
	int stu_port;
	AlgLink_ScdResult *vga;//vga�ṹ��
	Tch_Result_t *tch;//��ʦ����ṹ��
	StuITRACK_OutParams_t *stu;//ѧ������ṹ��
	Strategy_Result_t *stra;//���߽���ṹ��
	Strategy_CamControl_t *tch_cam;//��ʦ���
	Strategy_CamControl_t *stu_cam;//ѧ�����
}Track_Strategy_t;

/*
tch: Ϊ��ʦ���ٷ��صĽ����
stu: Ϊѧ�����ٷ��صĽ����
stra: Ϊ�Ѿ���ʼ���˵ľ�������
cam_tch: Ϊ���ƽ�ʦ��д��ͷ�Ŀ�������
cam_stu: Ϊ����ѧ����д��ͷ�Ŀ�������
*/
//int switch_strategy(Tch_Result_t* tch, StuITRACK_OutParams_t* stu, Strategy_Result_t* stra, Strategy_CamControl_t *cam_tch);

//��ʼ��������
//int init_strategy(Strategy_Result_t* stra);

int track_strategy(Track_Strategy_t *track_stra);
int first_PicMode(MainEnc_FirstPicMode_t *fpm, Track_Strategy_t *track_stra);
#ifdef __cplusplus
}
#endif

#endif /* SWITCH_STRATEGY_H */

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

#include "tch_params.h"
#include "Queue.h"
#include "stuTrack_settings_parameter.h"
#include "Strategy_cameraControl.h"
#include "scdLink.h"
//

#include <time.h>
#define CONVERSION_STUTRACK_UNITStoMS (1000)

#ifdef _WIN32
#include <windows.h>
#define  gettime GetTickCount
#else
#include <mcfw/src_bios6/utils/utils.h>
#define  gettime Utils_getCurTimeInMsec
#endif

#define FPS 25
#define SWITCH_TIME 2000

#define NULL_ACTIVITY 0
#define TCH_ACTIVITY 1
#define STU_ACTIVITY 2
#define VGA_ACTIVITY 3

//������ʦ����״̬
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

enum SWITCH_TABLE_T{
	/******************************************
	������
	*******************************************/
	SWITCH_TEA = 1,    //��ʦ����
	SWITCH_STU = 2,   //ѧ������
	SWITCH_VGA = 3,   //vga
	SWITCH_BLBO = 4,   // ����
	SWITCH_PSTU = 5,   //ѧ��ȫ��
	SWITCH_PTEA = 6,  //��ʦȫ��
	SWITCH_REMOTE = 7,  //Զ�ˣ�ֻ�л�������������

	/******************************************
	������
	*******************************************/
	SWITCH_2_VGA_TEA_PIP = 0X211,   //VGA��------ ��ʦ����------529
	SWITCH_2_VGA_TEA_PIP_1 = 0X1211,   //����

	SWITCH_2_TEA_VGA_PIP = 0x212,  //��ʦ������------VGA С------530
	SWITCH_2_TEA_STU_PIP = 0X213,  //��ʦ������------ѧ����������------  531
	SWITCH_2_STU_TEA_PIP = 0x214,	//ѧ����------ ��ʦС	------	532
	SWITCH_2_PSTU_TEA_PIP = 0x215,	//ѧ��ȫ����------ ��ʦС	------533
	SWITCH_2_VGA_PTEA_PIP = 0x216,	 //VGA�� ��ʦȫ��С ����	
	SWITCH_2_STU_EQU_PTEA = 0x217,	//ѧ����д����ʦȫ����


	SWITCH_2_TEA_EQU_VGA = 0x221,   //��ʦ��------VGA��------ 545->4
	SWITCH_2_TEA_EQU_VGA_1 = 0x1221,
	SWITCH_2_STU_EQU_TEA = 0x222,   //ѧ����------��ʦ��------    546
	SWITCH_2_PSTU_EQU_TEA = 0x223,	 //ѧ��ȫ��------��ʦ��------  547

	SWITCH_2_TEA_EQU_STU = 0x224,	 //��ʦ��------ѧ����------  548->5
	SWITCH_2_TEA_EQU_STU_1 = 0X1224,
	SWITCH_2_VGA_EQU_PTEA = 0x225,	 //VGA�� ��ʦȫ�� �ȷ�
	SWITCH_2_VGA_EQU_BLBO = 0x226,	//VGA�󣬰����ң��ȷ�

	SWITCH_2_VGA_TEA_1_PIP = 0X231, //VGA��------��ʦ����------561	->2
	SWITCH_2_VGA_TEA_1_PIP_1 = 0X1231,	 //����

	SWITCH_2_PSTU_TEA_1_PIP = 0x232,  //ѧ��ȫ����------��ʦ����------562
	SWITCH_2_STU_TEA_1_PIP = 0x233,    //ѧ���� ------ ��ʦС ���� ------549        ///
	SWITCH_2_VGA_PTEA_1_PIP = 0x234, //VGA�� ��ʦȫ�� ���� 


	SWICTH_2_VGA_TEA_2_PIP = 0x241,	//VGA��------��ʦ����------577->1
	SWICTH_2_VGA_TEA_2_PIP_1 = 0X1241,
	SWICTH_2_STU_TEA_2_PIP = 0x242,	//ѧ��	��------��ʦ����------578->3

	SWICTH_2_PSTU_TEA_2_PIP = 0x243,	//ѧ��ȫ��	��------��ʦ����------579
	SWITCH_2_VGA_PTEA_2_PIP = 0x244, //VGA�� ��ʦȫ�� ����	
	

	/******************************************
	������
	*******************************************/
	SWITCH_3_VGA_TEA_STU = 0X311,  //VGA��------��ʦ����------ѧ������------   785 -> 6
	SWITCH_3_TEA_VGA_STU = 0X1311, //��ʦ��------VGA ����------ѧ������	
	SWITCH_3_STU_VGA_TEA = 0X2311, //ѧ����------VGA����------��ʦ����

	SWITCH_3_VGA_PTEA_STU = 0X312, //VGA��------��ʦȫ������------ѧ������------ 786
	SWITCH_3_VGA_TEA_PSTU = 0X313,   //VGA ��------��ʦ����------ѧ��ȫ������------ 787
	SWITCH_3_VGA_PTEA_PSTU = 0X314,  //VGA ��------��ʦȫ��------ѧ��ȫ��------ 788

	SWITCH_3_BLBO_TEA_STU = 0X315,	//�����-----��ʦ����------ѧ������------  789
	SWITCH_3_BLBO_PTEA_STU = 0x316,	//�����-----��ʦȫ������------ѧ������------    790
	SWITCH_3_BLBO_TEA_PSTU = 0x317,	//�����------��ʦ����------ѧ��ȫ������------    791
	SWITCH_3_BLBO_PTEA_PSTU = 0X318,	//�����------��ʦȫ��------ѧ��ȫ��------  792

	SWITCH_3_PTEA_TEA_PSTU = 0X319,	//��ʦȫ����------��ʦ����------ѧ��ȫ������------  	793
	SWITCH_3_PTEA_TEA_STU = 0X31A,	//��ʦȫ����------��ʦ����------ѧ������------  		794
	SWITCH_3_TEA_PTEA_STU = 0x31b,//��ʦ�� ��ʦȫ�� ѧ������
	SWITCH_3_TEA_PSTU_STU = 0x31c,	//��ʦ�� -----ѧ��ȫ��------ ѧ������

	/******************************************
	�Ļ���
	*******************************************/
	SWITCH_4_VGA_TEA_STU_PTEA = 0x411,//VGA��------��ʦ����------ѧ������------��ʦȫ������------  1041
	SWITCH_4_TEA_VGA_STU_PTEA = 0x1411,	//��ʦ����------VGA��------ѧ������------��ʦȫ������------ 
	SWITCH_4_STU_TEA_VGA_PTEA = 0x2411,	//ѧ������------��ʦ����------VGA��------��ʦȫ������------ 
	SWITCH_4_PSTU_TEA_STU_VGA = 0x3411,	//��ʦȫ������------��ʦ����------ѧ������------VGA��------ 



	SWITCH_4_VGA_PTEA_STU_PTEA = 0X412,	//VGA��------��ʦȫ������------ѧ������------��ʦȫ������------ ��Ч1042
	SWITCH_4_VGA_TEA_PSTU_PTEA = 0x413,//VGA��------��ʦ����------ѧ��ȫ������------��ʦȫ������------    1043
	SWITCH_4_VGA_PTEA_PSTU_PTEA = 0x414,	//VGA��------��ʦȫ������------ѧ������------��ʦȫ������------��Ч	1044

	SWITCH_4_BLBO_TEA_STU_PTEA = 0x415,  //�����------��ʦ����------ѧ������------��ʦȫ������------     1045
	SWITCH_4_BLBO_TEA_PSTU_PTEA = 0X416, //�����------��ʦ����------ѧ��ȫ������------��ʦȫ������------      1046
	SWITCH_4_BLBO_PTEA_STU_PTEA = 0X417,//�����------��ʦȫ������------ѧ������------��ʦȫ������------   ��Ч
	SWITCH_4_BLBO_PTEA_PSTU_PTEA = 0x418,	//�����------��ʦȫ������------ѧ��ȫ������------��ʦȫ������------  ��Ч

	//NEW add
	SWITCH_4_VGA_TEA_STU_PSTU = 0x419,	//VGA��------��ʦ����------ѧ������------ѧ��ȫ������
	SWITCH_4_BLBO_TEA_STU_PSTU = 0x41a,	//�����------��ʦ����------ѧ������------ѧ��ȫ������

	SWITCH_4_PTEA_TEA_STU_BLBO = 0x41b,	//��ʦȫ����------��ʦ����------ѧ������------��������
	SWITCH_4_PTEA_TEA_PSTU_BLBO = 0x41c,	//��ʦȫ����------��ʦ����------ѧ��ȫ������------��������
	SWITCH_4_VGA_BLBO_TEA_STU = 0x41d,// VGA��-------------- ���� ��ʦ ѧ��

	// add by 2014-4-9
	SWITCH_4_VGA_TEA_STU_BLBO = 0x41e, // VGA��---��ʦ����---ѧ������----��������
	SWITCH_4_TEA_VGA_STU_BLBO = 0x41f,
	SWITCH_4_STU_TEA_VGA_BLBO = 0x421,
	SWITCH_4_BLBO_TEA_STU_VGA = 0x422,

	SWITCH_4_PTEA_TEA_STU_VGA = 0x423,
	SWITCH_4_TEA_VGA_STU_PSTU = 0x424,
	SWITCH_4_STU_TEA_VGA_PSTU = 0x425,
	SWITCH_4_PTEA_TEA_STU_PSTU = 0x426,


	/******************************************
	�廭��
	*******************************************/
	SWITCH_5 = 0X511, //1297


	/******************************************
	������
	*******************************************/
	SWITCH_6_VGA_1 = 0X611,//VGA��---��ʦ����---ѧ������---��ʦȫ������---ѧ��ȫ������---��������
	SWITCH_6_TEA = 0X1611,//��ʦ��
	SWITCH_6_STU = 0X2611,//ѧ����
	SWITCH_6_PTEA = 0X3611,//��ʦȫ����
	SWITCH_6_PSTU = 0X4611,//ѧ��ȫ����
	SWITCH_6_BLBO = 0X5611,//�����

	SWITCH_6_2 = 0X621,//VGA---��ʦ---ѧ��---��ʦȫ��---ѧ��ȫ��---����


	SWITCH_MAX_LAYOUT = 0XFFFF,//end
};

//��ʱ��
typedef struct StrategyTimer
{
	DWORD start;
	DWORD finish;
	double deltaTime;
}Strategy_Timer_t;

typedef struct Strategy_Result
{
	int VGA_flag;//VGA�źű�־
	int STU_flag;//ѧ���źű�־
	int TCH_flag;//��ʦ�źű�־

	Queue *tch_que;//��ʦ���з���
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

#ifdef __cplusplus
}
#endif

#endif /* SWITCH_STRATEGY_H */
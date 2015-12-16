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
#include "Strategy_cameraControl.h"
#include <stdlib.h>
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

#define IP_ADDR_LEN 16
#define FPS 25
#define SWITCH_TIME 2000
#define SWITCH_VGA_TIME 10000

#define NULL_ACTIVITY 0
#define TCH_ACTIVITY 1
#define STU_ACTIVITY 2
#define VGA_ACTIVITY 3

#define VGA_PARAMS 0
#define TCH_PARAMS 1
#define STU_PARAMS 2

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

	//typedef struct Strategy_Params
	//{
	//	AlgLink_ScdResult vga;//vga�ṹ��
	//	Tch_Result_t tch;//��ʦ����ṹ��
	//	StuITRACK_OutParams_t stu;//ѧ������ṹ��
	//}Strategy_Params_t;

	typedef struct IP_PORT
	{
		char tch_ip[IP_ADDR_LEN];//��ʦ�����ip
		char stu_ip[IP_ADDR_LEN];//ѧ�������ip
		int tch_port;//��ʦ����Ķ˿�
		int stu_port;//ѧ������Ķ˿�
	}Strategy_IpPort_t;

	typedef struct Strategy_sysData
	{
		int result_activity;//¼�Ƴ�ʼ�����ʶ��
		int display_time;//��ʼ�������ʱ��
		int firstpicture;//��ʼ����״̬

		int status;//���εĻ���״̬
		int last_status;//��һ�λ���״̬

		int VGA_flag;//VGA�źű�־
		int STU_flag;//ѧ���źű�־
		int TCH_flag;//��ʦ�źű�־

		//Strategy_Queue *tch_que;//��ʦ���з���
		int tch_posIndex;//��ʦλ��
		int tch_status;//��ʦ״̬

		TrackPrarms_Point_t stu_pos;//ѧ������

		int posit_pan; //���ˮƽ����
		int posit_tilt; //�����ֱ����

		Strategy_Timer_t timer_stra;//��ʱ��
		Strategy_Timer_t timer_vga;//vga��ʱ��

		Strategy_CamControl_t tch_cam;//��ʦ���
		Strategy_CamControl_t stu_cam;//ѧ�����

		char tch_ip[IP_ADDR_LEN];//��ʦ�����ip
		char stu_ip[IP_ADDR_LEN];//ѧ�������ip
		int tch_port;//��ʦ����Ķ˿�
		int stu_port;//ѧ������Ķ˿�

		//Strategy_Params_t strat_info;//�ڲ�����Ĳ���״̬

	}Strategy_sysData_t;

	/*==============================================================================
		����: <switch_strategy_vga>
		����: ����VGA״̬���жϲ���
		����:track_stra :�Ѿ���ʼ���ľ�������vga��vga����
		����ֵ:��0-���ߴ���   �����޾���
	==============================================================================*/
	int switch_strategy_vga(Strategy_sysData_t *sysData, AlgLink_ScdResult *vga);

	/*==============================================================================
	����: <switch_strategy_tch>
	����: ����TCH״̬���жϲ���
	����:track_stra :�Ѿ���ʼ���ľ�������tch����ʦ����
	����ֵ:��0-���ߴ���   �����޾���
	==============================================================================*/
	int switch_strategy_tch(Strategy_sysData_t *sysData, Tch_Result_t *tch);

	/*==============================================================================
	����: <switch_strategy_stu>
	����: ����STU״̬���жϲ���
	����:track_stra :�Ѿ���ʼ���ľ�������stu��ѧ������
	����ֵ:��0-���ߴ���   �����޾���
	==============================================================================*/
	int switch_strategy_stu(Strategy_sysData_t *sysData, StuITRACK_OutParams_t *stu);

	/*==============================================================================
		����: <switch_strategy>
		����: ��ȡ���߽��
		����:track_stra :�Ѿ���ʼ���ľ�����
		����ֵ:��0-���ߴ���   �����޾���
		==============================================================================*/
	//int switch_strategy(Strategy_sysData_t *track_stra, Strategy_Params_t *stra_params ,int flag);

	/*==============================================================================
		����: <init_track_strategy>
		����: ��ʼ����������
		����:track_stra :
		����ֵ:0-�ɹ�   ����ʧ��
		==============================================================================*/
	Strategy_sysData_t*  init_track_strategy(Strategy_IpPort_t ip_port);

	/*==============================================================================
		����: <destroy_track_strategy>
		����: �ݻپ�����
		����:track_stra :
		����ֵ:0-�ɹ�   ����ʧ��
		==============================================================================*/
	int destroy_track_strategy(Strategy_sysData_t *track_stra);

	/*==============================================================================
		����: <first_PicMode>
		����: ����¼�Ƴ�ʼ����
		����:fpm :
		int option;//���û��߻�ȡ
		int firstpicture;//�׻�������
		int display_time;//����ʱ��
		����ֵ:0-�ɹ�   ����ʧ��
		==============================================================================*/
	int first_PicMode(MainEnc_FirstPicMode_t *fpm, Strategy_sysData_t *track_stra);
#ifdef __cplusplus
}
#endif

#endif /* SWITCH_STRATEGY_H */

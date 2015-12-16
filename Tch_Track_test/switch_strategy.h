/*
* To change this license header, choose License Headers in Project Properties.
* To change this template file, choose Tools | Templates
* and open the template in the editor.
*/

/*
* File:   Switch_Strategy.h
* Author: Xiongcc
*
* Created on 2015年11月30日, 下午2:37
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

	//设置电影模式首画面
	typedef struct _HD_FIRSTPICTURE_MODE_S
	{
		int option;//设置或者获取
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

	//定义老师跟踪状态
#define STRATEGY_TRACK_TCH_NULL 0
#define STRATEGY_TRACK_TCH_STAND 1			//老师处在站立状态
#define STRATEGY_TRACK_TCH_MOVEINVIEW 2		//老师在特写镜头内
#define STRATEGY_TRACK_TCH_MOVEOUTVIEW 3		//老师走出特写镜头
#define STRATEGY_TRACK_TCH_OUTSIDE 4			//老师离开跟踪区域
#define STRATEGY_TRACK_TCH_BLACKBOARD 5		//老师书写板书状态
#define STRATEGY_TRACK_TCH_MULITY 6			//讲台多目标状态

#define STRATEGY_TRACK_STU_NULL 0			//没有学生信号
#define STRATEGY_TRACK_STU_STAND_SINGLE 1	//单个学生站立
#define STRATEGY_TRACK_STU_STAND_MULITY 2	//多个学生站立
#define STRATEGY_TRACK_STU_MOVE_SINGLE 3	//单个学生区域移动
#define STRATEGY_TRACK_STU_MOVE_MULITY 4	//多个学生区域移动

#define STRATEGY_TRACK_VGA_NULL 0	//	VGA无信号
#define STRATEGY_TRACK_VGA_ACTIVITY 1	//VGA有信号

	//计时器
	typedef struct StrategyTimer
	{
		unsigned long start;
		unsigned long finish;
		double deltaTime;
	}Strategy_Timer_t;

	//typedef struct Strategy_Params
	//{
	//	AlgLink_ScdResult vga;//vga结构体
	//	Tch_Result_t tch;//教师结果结构体
	//	StuITRACK_OutParams_t stu;//学生结果结构体
	//}Strategy_Params_t;

	typedef struct IP_PORT
	{
		char tch_ip[IP_ADDR_LEN];//教师相机的ip
		char stu_ip[IP_ADDR_LEN];//学生相机的ip
		int tch_port;//教师相机的端口
		int stu_port;//学生相机的端口
	}Strategy_IpPort_t;

	typedef struct Strategy_sysData
	{
		int result_activity;//录制初始画面标识符
		int display_time;//初始画面持续时间
		int firstpicture;//初始画面状态

		int status;//本次的画面状态
		int last_status;//上一次画面状态

		int VGA_flag;//VGA信号标志
		int STU_flag;//学生信号标志
		int TCH_flag;//教师信号标志

		//Strategy_Queue *tch_que;//教师序列分析
		int tch_posIndex;//教师位置
		int tch_status;//教师状态

		TrackPrarms_Point_t stu_pos;//学生坐标

		int posit_pan; //相机水平坐标
		int posit_tilt; //相机垂直坐标

		Strategy_Timer_t timer_stra;//计时器
		Strategy_Timer_t timer_vga;//vga计时器

		Strategy_CamControl_t tch_cam;//教师相机
		Strategy_CamControl_t stu_cam;//学生相机

		char tch_ip[IP_ADDR_LEN];//教师相机的ip
		char stu_ip[IP_ADDR_LEN];//学生相机的ip
		int tch_port;//教师相机的端口
		int stu_port;//学生相机的端口

		//Strategy_Params_t strat_info;//内部保存的参数状态

	}Strategy_sysData_t;

	/*==============================================================================
		函数: <switch_strategy_vga>
		功能: 更新VGA状态，判断策略
		参数:track_stra :已经初始化的决策器，vga：vga参数
		返回值:非0-决策代码   否则无决策
	==============================================================================*/
	int switch_strategy_vga(Strategy_sysData_t *sysData, AlgLink_ScdResult *vga);

	/*==============================================================================
	函数: <switch_strategy_tch>
	功能: 更新TCH状态，判断策略
	参数:track_stra :已经初始化的决策器，tch：教师参数
	返回值:非0-决策代码   否则无决策
	==============================================================================*/
	int switch_strategy_tch(Strategy_sysData_t *sysData, Tch_Result_t *tch);

	/*==============================================================================
	函数: <switch_strategy_stu>
	功能: 更新STU状态，判断策略
	参数:track_stra :已经初始化的决策器，stu：学生参数
	返回值:非0-决策代码   否则无决策
	==============================================================================*/
	int switch_strategy_stu(Strategy_sysData_t *sysData, StuITRACK_OutParams_t *stu);

	/*==============================================================================
		函数: <switch_strategy>
		功能: 获取决策结果
		参数:track_stra :已经初始化的决策器
		返回值:非0-决策代码   否则无决策
		==============================================================================*/
	//int switch_strategy(Strategy_sysData_t *track_stra, Strategy_Params_t *stra_params ,int flag);

	/*==============================================================================
		函数: <init_track_strategy>
		功能: 初始化决策器及
		参数:track_stra :
		返回值:0-成功   否则失败
		==============================================================================*/
	Strategy_sysData_t*  init_track_strategy(Strategy_IpPort_t ip_port);

	/*==============================================================================
		函数: <destroy_track_strategy>
		功能: 摧毁决策器
		参数:track_stra :
		返回值:0-成功   否则失败
		==============================================================================*/
	int destroy_track_strategy(Strategy_sysData_t *track_stra);

	/*==============================================================================
		函数: <first_PicMode>
		功能: 设置录制初始画面
		参数:fpm :
		int option;//设置或者获取
		int firstpicture;//首画面类型
		int display_time;//持续时间
		返回值:0-成功   否则失败
		==============================================================================*/
	int first_PicMode(MainEnc_FirstPicMode_t *fpm, Strategy_sysData_t *track_stra);
#ifdef __cplusplus
}
#endif

#endif /* SWITCH_STRATEGY_H */

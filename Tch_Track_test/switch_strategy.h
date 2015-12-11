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
    
    //设置电影模式首画面
	typedef struct _HD_FIRSTPICTURE_MODE_S
	{	
		int option;//设置或者获取
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

typedef struct Strategy_Result
{
        int result_activity;
	int display_time;
        int firstpicture;
        
	int VGA_flag;//VGA信号标志
	int STU_flag;//学生信号标志
	int TCH_flag;//教师信号标志

	Strategy_Queue *tch_que;//教师序列分析
	int tch_posIndex;//教师位置
	int tch_status;//教师状态

	TrackPrarms_Point_t stu_pos;//学生坐标

	int status;//画面状态的指令
	int last_status;//上一次的画面状态

	int posit_pan; //相机水平坐标
	int posit_tilt; //相机垂直坐标

	Strategy_Timer_t timer_stra;//计时器

}Strategy_Result_t;

typedef struct Track_Strategy
{
        int isInit;
	char *tch_ip;
	char *stu_ip;
	int tch_port;
	int stu_port;
	AlgLink_ScdResult *vga;//vga结构体
	Tch_Result_t *tch;//教师结果结构体
	StuITRACK_OutParams_t *stu;//学生结果结构体
	Strategy_Result_t *stra;//决策结果结构体
	Strategy_CamControl_t *tch_cam;//教师相机
	Strategy_CamControl_t *stu_cam;//学生相机
}Track_Strategy_t;

/*
tch: 为教师跟踪返回的结果；
stu: 为学生跟踪返回的结果；
stra: 为已经初始化了的决策器；
cam_tch: 为控制教师特写镜头的控制器；
cam_stu: 为控制学生特写镜头的控制器。
*/
//int switch_strategy(Tch_Result_t* tch, StuITRACK_OutParams_t* stu, Strategy_Result_t* stra, Strategy_CamControl_t *cam_tch);

//初始化决策器
//int init_strategy(Strategy_Result_t* stra);

int track_strategy(Track_Strategy_t *track_stra);
int first_PicMode(MainEnc_FirstPicMode_t *fpm, Track_Strategy_t *track_stra);
#ifdef __cplusplus
}
#endif

#endif /* SWITCH_STRATEGY_H */

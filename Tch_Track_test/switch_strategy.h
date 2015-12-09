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

//定义老师跟踪状态
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

enum SWITCH_TABLE_T{
	/******************************************
	单画面
	*******************************************/
	SWITCH_TEA = 1,    //老师近景
	SWITCH_STU = 2,   //学生近景
	SWITCH_VGA = 3,   //vga
	SWITCH_BLBO = 4,   // 板书
	SWITCH_PSTU = 5,   //学生全景
	SWITCH_PTEA = 6,  //老师全景
	SWITCH_REMOTE = 7,  //远端，只有互动的主讲才有

	/******************************************
	两画面
	*******************************************/
	SWITCH_2_VGA_TEA_PIP = 0X211,   //VGA大------ 老师左上------529
	SWITCH_2_VGA_TEA_PIP_1 = 0X1211,   //交换

	SWITCH_2_TEA_VGA_PIP = 0x212,  //老师近景大------VGA 小------530
	SWITCH_2_TEA_STU_PIP = 0X213,  //老师近景大------学生近景左上------  531
	SWITCH_2_STU_TEA_PIP = 0x214,	//学生大------ 老师小	------	532
	SWITCH_2_PSTU_TEA_PIP = 0x215,	//学生全景大------ 老师小	------533
	SWITCH_2_VGA_PTEA_PIP = 0x216,	 //VGA大 老师全景小 左上	
	SWITCH_2_STU_EQU_PTEA = 0x217,	//学生特写左，老师全景右


	SWITCH_2_TEA_EQU_VGA = 0x221,   //老师左------VGA右------ 545->4
	SWITCH_2_TEA_EQU_VGA_1 = 0x1221,
	SWITCH_2_STU_EQU_TEA = 0x222,   //学生左------老师右------    546
	SWITCH_2_PSTU_EQU_TEA = 0x223,	 //学生全景------老师右------  547

	SWITCH_2_TEA_EQU_STU = 0x224,	 //老师左------学生右------  548->5
	SWITCH_2_TEA_EQU_STU_1 = 0X1224,
	SWITCH_2_VGA_EQU_PTEA = 0x225,	 //VGA大 老师全景 等分
	SWITCH_2_VGA_EQU_BLBO = 0x226,	//VGA左，板书右，等分

	SWITCH_2_VGA_TEA_1_PIP = 0X231, //VGA大------老师右下------561	->2
	SWITCH_2_VGA_TEA_1_PIP_1 = 0X1231,	 //交换

	SWITCH_2_PSTU_TEA_1_PIP = 0x232,  //学生全景大------老师右下------562
	SWITCH_2_STU_TEA_1_PIP = 0x233,    //学生大 ------ 老师小 右下 ------549        ///
	SWITCH_2_VGA_PTEA_1_PIP = 0x234, //VGA大 老师全景 右下 


	SWICTH_2_VGA_TEA_2_PIP = 0x241,	//VGA大------老师左上------577->1
	SWICTH_2_VGA_TEA_2_PIP_1 = 0X1241,
	SWICTH_2_STU_TEA_2_PIP = 0x242,	//学生	大------老师右上------578->3

	SWICTH_2_PSTU_TEA_2_PIP = 0x243,	//学生全景	大------老师右上------579
	SWITCH_2_VGA_PTEA_2_PIP = 0x244, //VGA大 老师全景 右上	
	

	/******************************************
	三画面
	*******************************************/
	SWITCH_3_VGA_TEA_STU = 0X311,  //VGA大------老师左上------学生左下------   785 -> 6
	SWITCH_3_TEA_VGA_STU = 0X1311, //教师大------VGA 左上------学生左下	
	SWITCH_3_STU_VGA_TEA = 0X2311, //学生大------VGA左上------教师左下

	SWITCH_3_VGA_PTEA_STU = 0X312, //VGA大------老师全景左上------学生左下------ 786
	SWITCH_3_VGA_TEA_PSTU = 0X313,   //VGA 大------老师左上------学生全景左下------ 787
	SWITCH_3_VGA_PTEA_PSTU = 0X314,  //VGA 大------老师全景------学生全景------ 788

	SWITCH_3_BLBO_TEA_STU = 0X315,	//板书大-----老师左上------学生左下------  789
	SWITCH_3_BLBO_PTEA_STU = 0x316,	//板书大-----老师全景左上------学生左下------    790
	SWITCH_3_BLBO_TEA_PSTU = 0x317,	//板书大------老师左上------学生全景左下------    791
	SWITCH_3_BLBO_PTEA_PSTU = 0X318,	//板书大------老师全景------学生全景------  792

	SWITCH_3_PTEA_TEA_PSTU = 0X319,	//老师全景大------老师左上------学生全景左下------  	793
	SWITCH_3_PTEA_TEA_STU = 0X31A,	//老师全景大------老师左上------学生左下------  		794
	SWITCH_3_TEA_PTEA_STU = 0x31b,//老师大 老师全景 学生近景
	SWITCH_3_TEA_PSTU_STU = 0x31c,	//老师大 -----学生全景------ 学生近景

	/******************************************
	四画面
	*******************************************/
	SWITCH_4_VGA_TEA_STU_PTEA = 0x411,//VGA大------老师左上------学生左中------老师全景左下------  1041
	SWITCH_4_TEA_VGA_STU_PTEA = 0x1411,	//老师左上------VGA大------学生左中------老师全景左下------ 
	SWITCH_4_STU_TEA_VGA_PTEA = 0x2411,	//学生左中------老师左上------VGA大------老师全景左下------ 
	SWITCH_4_PSTU_TEA_STU_VGA = 0x3411,	//老师全景左下------老师左上------学生左中------VGA大------ 



	SWITCH_4_VGA_PTEA_STU_PTEA = 0X412,	//VGA大------老师全景左上------学生左中------老师全景左下------ 无效1042
	SWITCH_4_VGA_TEA_PSTU_PTEA = 0x413,//VGA大------老师左上------学生全景左中------老师全景左下------    1043
	SWITCH_4_VGA_PTEA_PSTU_PTEA = 0x414,	//VGA大------老师全景左上------学生左中------老师全景左下------无效	1044

	SWITCH_4_BLBO_TEA_STU_PTEA = 0x415,  //板书大------老师左上------学生左中------老师全景左下------     1045
	SWITCH_4_BLBO_TEA_PSTU_PTEA = 0X416, //板书大------老师左上------学生全景左中------老师全景左下------      1046
	SWITCH_4_BLBO_PTEA_STU_PTEA = 0X417,//板书大------老师全景左上------学生左中------老师全景左下------   无效
	SWITCH_4_BLBO_PTEA_PSTU_PTEA = 0x418,	//板书大------老师全景左上------学生全景左中------老师全景左下------  无效

	//NEW add
	SWITCH_4_VGA_TEA_STU_PSTU = 0x419,	//VGA大------教师左上------学生左中------学生全景左下
	SWITCH_4_BLBO_TEA_STU_PSTU = 0x41a,	//板书大------教师左上------学生左中------学生全景左下

	SWITCH_4_PTEA_TEA_STU_BLBO = 0x41b,	//教师全景大------教师左上------学生左中------板书左下
	SWITCH_4_PTEA_TEA_PSTU_BLBO = 0x41c,	//教师全景大------教师左上------学生全景左中------板书左下
	SWITCH_4_VGA_BLBO_TEA_STU = 0x41d,// VGA大-------------- 板书 老师 学生

	// add by 2014-4-9
	SWITCH_4_VGA_TEA_STU_BLBO = 0x41e, // VGA大---老师左上---学生左中----板书左下
	SWITCH_4_TEA_VGA_STU_BLBO = 0x41f,
	SWITCH_4_STU_TEA_VGA_BLBO = 0x421,
	SWITCH_4_BLBO_TEA_STU_VGA = 0x422,

	SWITCH_4_PTEA_TEA_STU_VGA = 0x423,
	SWITCH_4_TEA_VGA_STU_PSTU = 0x424,
	SWITCH_4_STU_TEA_VGA_PSTU = 0x425,
	SWITCH_4_PTEA_TEA_STU_PSTU = 0x426,


	/******************************************
	五画面
	*******************************************/
	SWITCH_5 = 0X511, //1297


	/******************************************
	六画面
	*******************************************/
	SWITCH_6_VGA_1 = 0X611,//VGA大---老师左中---学生左上---老师全景左下---学生全景中下---板书右下
	SWITCH_6_TEA = 0X1611,//老师大
	SWITCH_6_STU = 0X2611,//学生大
	SWITCH_6_PTEA = 0X3611,//老师全景大
	SWITCH_6_PSTU = 0X4611,//学生全景大
	SWITCH_6_BLBO = 0X5611,//板书大

	SWITCH_6_2 = 0X621,//VGA---老师---学生---老师全景---学生全景---板书


	SWITCH_MAX_LAYOUT = 0XFFFF,//end
};

//计时器
typedef struct StrategyTimer
{
	DWORD start;
	DWORD finish;
	double deltaTime;
}Strategy_Timer_t;

typedef struct Strategy_Result
{
	int VGA_flag;//VGA信号标志
	int STU_flag;//学生信号标志
	int TCH_flag;//教师信号标志

	Queue *tch_que;//教师序列分析
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

#ifdef __cplusplus
}
#endif

#endif /* SWITCH_STRATEGY_H */
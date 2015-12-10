#include "Switch_Strategy.h"

#ifndef WIN32
#include <sys/time.h>
unsigned long gettime()
{
	struct timeval Getime;
	unsigned long millisecond = 0;

	gettimeofday (&Getime, NULL);
	millisecond = Getime.tv_sec * 1000 + Getime.tv_usec / 1000;
	return millisecond;
}
#endif
static void clear_timer(Strategy_Timer_t *time)
{
	time->start = 0;
	time->finish = 0;
	time->deltaTime = 0;
}

static int strategy_frame_analyze(Strategy_sysData_t* stra)
{
	int i = 0;
	int statis[7] = { 0 };
	int err = 0;
	PNode pnode = stra->tch_que->front;
	for (i = 0; i < stra->tch_que->size; i++)
	{
		if (STRATEGY_TRACK_TCH_STAND <= pnode->data && pnode->data <= STRATEGY_TRACK_TCH_MULITY)
		{
			statis[pnode->data]++;
			pnode = pnode->next;
		}
		else
		{
			err++;
		}
	}
	int num = statis[0];
	int res = 0;
	for (i = 1; i < 7;i++)
	{
		if (statis[i]>num)
		{
			num = statis[i];
			res = i;
		}
	}
	stra->tch_status = (statis[res] > err) ? res : -1;
	return stra->tch_status;
}

/*
分析vga是何种信号
*/
static int vga_strategy_analyze(Strategy_sysData_t* stra, AlgLink_ScdResult *vga)
{
	stra->VGA_flag = STRATEGY_TRACK_VGA_ACTIVITY;
	return VGA_ACTIVITY;
}

/*
分析学生是何种信号
*/
static int stu_strategy_analyze(Strategy_sysData_t* stra, StuITRACK_OutParams_t* stu)
{
	if (RESULT_STUTRACK_IF_NEWCHANGE(stu->result_flag))//判断当前帧是否有变化
	{
		if (RESULT_STUTRACK_IF_STANDUP(stu->result_flag))//判断当前帧是否有站立
		{
			if (stu->count_trackObj_stand == 1)
			{
				stra->STU_flag = STRATEGY_TRACK_STU_STAND_SINGLE;//标识学生区域有1个响应
				//移动相机到该坐标，计时切特写
				stra->stu_pos = stu->stand_position;
				move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标
				setZoom(stra->stu_cam, stu->stretchingCoefficient_stand);
				//stra->status = SWITCH_PSTU;
				//打开计时器
				stra->timer_stra.start = gettime();
				return STU_ACTIVITY;
			}
			else if (stu->count_trackObj_stand > 1)
			{
				stra->STU_flag = STRATEGY_TRACK_STU_STAND_MULITY;//标识学生区域有多个响应
				//移动相机到该坐标，切全景
				stra->stu_pos = stu->stand_position;
				move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标,移动到最后一个运动目标处
				setZoom(stra->stu_cam, stu->stretchingCoefficient_stand);
				//stra->status = SWITCH_PSTU;
				//计时器清零
				clear_timer(&stra->timer_stra);
				return STU_ACTIVITY;
			}
			else
				return NULL_ACTIVITY;
		}
		if (RESULT_STUTRACK_IF_SITDOWN(stu->result_flag))
		{
			if (stu->count_trackObj_stand == 0)
			{
				stra->STU_flag = STRATEGY_TRACK_STU_NULL;//标识学生区域有0个响应
				//stra->status = SWITCH_PSTU;
				//计时器清零
				clear_timer(&stra->timer_stra);
				return STU_ACTIVITY;
			}
			else if (stu->count_trackObj_stand == 1)
			{
				stra->STU_flag = STRATEGY_TRACK_STU_STAND_SINGLE;//标识学生区域有1个响应
				//移动相机到该坐标，计时切特写
				stra->stu_pos = stu->stand_position;
				move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标,移动到最后一个运动目标处；
				setZoom(stra->stu_cam, stu->stretchingCoefficient_stand);
				//stra->status = SWITCH_PSTU;
				//打开计时器
				stra->timer_stra.start = gettime();
				return STU_ACTIVITY;
			}
			else
			{
				stra->STU_flag = STRATEGY_TRACK_STU_STAND_MULITY;//标识学生区域有多个响应
				//移动相机到该坐标，切全景
				stra->stu_pos = stu->stand_position;
				move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标,移动到最后一个运动目标处；
				setZoom(stra->stu_cam, stu->stretchingCoefficient_stand);
				//stra->status = SWITCH_PSTU;
				//计时器清零
				clear_timer(&stra->timer_stra);
				return STU_ACTIVITY;
			}
		}
		if (RESULT_STUTRACK_IF_MOVE(stu->result_flag))
		{
			if (stu->count_trackObj_bigMove == 1)
			{
				stra->STU_flag = STRATEGY_TRACK_STU_MOVE_SINGLE;
				//移动相机跟随坐标，计时切特写
				stra->stu_pos = stu->move_position;
				move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随移动坐标
				setZoom(stra->stu_cam, stu->stretchingCoefficient_bigMove);
				//stra->status = SWITCH_PSTU;
				//计时器清零
				clear_timer(&stra->timer_stra);
				return STU_ACTIVITY;
			}
			else
			{
				stra->STU_flag = STRATEGY_TRACK_STU_MOVE_MULITY;
				//移动相机到该坐标，切全景
				stra->stu_pos = stu->move_position;
				move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随最后一个移动坐标
				setZoom(stra->stu_cam, stu->stretchingCoefficient_bigMove);
				//stra->status = SWITCH_PSTU;
				//计时器清零
				clear_timer(&stra->timer_stra);
				return STU_ACTIVITY;
			}
		}
		if (RESULT_STUTRACK_IF_STOPMOVE(stu->result_flag))
		{
			if (stu->count_trackObj_bigMove == 0)
			{
				if (stra->tch_status == STRATEGY_TRACK_TCH_OUTSIDE)
				{
					stra->STU_flag = STRATEGY_TRACK_STU_STAND_SINGLE;//教师在学生区域停下
					stra->stu_pos = stu->move_position;
					move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随最后一个移动坐标
					setZoom(stra->stu_cam, stu->stretchingCoefficient_bigMove);
					//打开计时器
					//stra->timer_stra.start = gettime();
					return STU_ACTIVITY;
				}
				else
				{
					stra->STU_flag = STRATEGY_TRACK_STU_NULL;//标识学生区域有0个响应
					stra->stu_pos = stu->move_position;
					move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随最后一个移动坐标
					setZoom(stra->stu_cam, stu->stretchingCoefficient_bigMove);
					//stra->status = SWITCH_PSTU;
					//计时器清零
					clear_timer(&stra->timer_stra);
					return NULL_ACTIVITY;
				}
				
			}
			else if (stu->count_trackObj_bigMove == 1)
			{
				stra->STU_flag = STRATEGY_TRACK_STU_MOVE_SINGLE;
				//移动相机跟随坐标，计时切全景
				stra->stu_pos = stu->move_position;
				move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随移动坐标
				setZoom(stra->stu_cam, stu->stretchingCoefficient_bigMove);
				//stra->status = SWITCH_PSTU;
				//计时器清零
				clear_timer(&stra->timer_stra);
				return STU_ACTIVITY;
			}
			else
			{
				stra->STU_flag = STRATEGY_TRACK_STU_MOVE_MULITY;
				//移动相机到该坐标，切全景
				stra->stu_pos = stu->move_position;
				move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随最后一个移动坐标
				setZoom(stra->stu_cam, stu->stretchingCoefficient_bigMove);
				//stra->status = SWITCH_PSTU;
				//计时器清零
				clear_timer(&stra->timer_stra);
				return STU_ACTIVITY;
			}
		}
	}
	else
	{
		if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE)
		{
			
			//if (stra->timer_stra.deltaTime > SWITCH_TIME)
			//{
			//	//判断相机是否到位

			//	//判断教师状态是否为特写，切双画面
			//	if (stra->tch_status == STRATEGY_TRACK_TCH_MOVEINVIEW)
			//	{
			//		stra->status = SWITCH_2_STU_EQU_TEA;
			//	}
			//	else
			//	{
			//		stra->status = SWITCH_STU;
			//	}
			//}
			//else
			//{
			//	//移动相机
			//}
			return STU_ACTIVITY;
		}
		if (stra->STU_flag == STRATEGY_TRACK_STU_MOVE_SINGLE)
		{
			//移动相机，保持全景
			return STU_ACTIVITY;
		}
		if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE || stra->STU_flag == STRATEGY_TRACK_STU_MOVE_SINGLE)
		{
			//移动相机，保持全景
			return STU_ACTIVITY;
		}
		if (stra->STU_flag == STRATEGY_TRACK_STU_NULL)
		{
			//学生区域没有消息，等待其他信号
			return NULL_ACTIVITY;
		}
		return NULL_ACTIVITY;
	}
	return NULL_ACTIVITY;
}

/*
分析教师是何种信号
*/
static int tch_strategy_analyze(Strategy_sysData_t* stra, Tch_Result_t* tch)
{
	if (stra->tch_status == STRATEGY_TRACK_TCH_BLACKBOARD)
	{
		//切板书镜头
		//始终移动摄像头跟踪教师
		stra->tch_posIndex = tch->pos;
		stra->TCH_flag = stra->tch_status;
		preset(stra->tch_cam, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
		return TCH_ACTIVITY;
		//if (stra->STU_flag == NULL_ACTIVITY && stra->VGA_flag == NULL_ACTIVITY)
		//{
		//	stra->status = SWITCH_BLBO;//发板书指令
		//	return TCH_ACTIVITY;
		//}
	}
	else if (stra->tch_status == STRATEGY_TRACK_TCH_MOVEOUTVIEW || stra->tch_status == STRATEGY_TRACK_TCH_MULITY)
	{
		//切全景
		//始终移动摄像头跟踪教师
		stra->TCH_flag = stra->tch_status;
		stra->tch_posIndex = tch->pos;
		preset(stra->tch_cam, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
		return TCH_ACTIVITY;
		//if (stra->STU_flag == NULL_ACTIVITY && stra->VGA_flag == NULL_ACTIVITY)
		//{
		//	stra->status = SWITCH_PTEA;//发教师全景指令
		//	return TCH_ACTIVITY;
		//}
	}
	else if (stra->tch_status == STRATEGY_TRACK_TCH_MOVEINVIEW)
	{
		//切特写
		//判断摄像头是否到位并且够时间就切特写
		stra->tch_posIndex = tch->pos;
		stra->TCH_flag = stra->tch_status;
		preset(stra->tch_cam, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
		return TCH_ACTIVITY;
		//if (stra->STU_flag == NULL_ACTIVITY && stra->VGA_flag == NULL_ACTIVITY)
		//{
		//	stra->status = SWITCH_TEA;//发教师特写指令
		//	return TCH_ACTIVITY;
		//}
	}
	else if (stra->tch_status == STRATEGY_TRACK_TCH_OUTSIDE)
	{
		//切学生
		stra->tch_posIndex = 0;
		stra->TCH_flag = stra->tch_status;
		home(stra->tch_cam);
		return NULL_ACTIVITY;
		/*if (stra->STU_flag == NULL_ACTIVITY && stra->VGA_flag == NULL_ACTIVITY)
		{
			stra->status = SWITCH_PSTU;
			return NULL_ACTIVITY;
		}*/
	}
	else
	{
		//无状态则返回教师全景
		home(stra->tch_cam);
		stra->TCH_flag = stra->tch_status;
                stra->TCH_flag = STRATEGY_TRACK_TCH_NULL;
		return NULL_ACTIVITY;
		/*if (stra->STU_flag == NULL_ACTIVITY && stra->VGA_flag == NULL_ACTIVITY)
		{
			stra->status = SWITCH_PTEA;
			return NULL_ACTIVITY;
		}*/
	}
	return NULL_ACTIVITY;
}

/*
综合三种信号来判断该切什么画面
*/
int strategy_status_analyze(Strategy_sysData_t* stra,Strategy_Result_t *stra_res)//根据三个不同的信号分析当前的画面决策
{
	if (stra->VGA_flag == STRATEGY_TRACK_VGA_ACTIVITY)//VGA信号
	{
		if (stra->TCH_flag == STRATEGY_TRACK_TCH_BLACKBOARD)
		{
			//切板书与VGA双画面
			stra_res->status = SWITCH_2_VGA_EQU_BLBO;
			return stra_res->status;
		}
		else if (stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW&&stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE)
		{
			//判断学生站立时间及相机是否到位
			stra->timer_stra.finish = gettime();
			stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
			if (stra->timer_stra.deltaTime>SWITCH_TIME)
			{
				if (getPosit(stra->stu_cam, &stra->posit_pan, &stra->posit_tilt, 500))
				{
					if (stra->posit_pan == stra->stu_pos.x&&stra->posit_tilt == stra->stu_pos.y)
					{
						//切大VGA，小学生以及小教师的三画面
						stra_res->status = SWITCH_3_VGA_TEA_STU;
						return stra_res->status;
					}
					else
					{
						move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
						stra_res->status = SWITCH_VGA;
						return stra_res->status;
					}
				}
				else
				{
					move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
					stra_res->status = SWITCH_VGA;
					return stra_res->status;
				}
			}
			else
			{
				move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
				stra_res->status = SWITCH_VGA;
				return stra_res->status;
			}
		}
		else
		{
			move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
			stra_res->status = SWITCH_VGA;
			return stra_res->status;
		}
	}
	else//无VGA信号
	{
		if (stra->STU_flag != STRATEGY_TRACK_STU_NULL)//学生区域有运动
		{
			if (stra->TCH_flag != STRATEGY_TRACK_TCH_OUTSIDE)//同时教师区域内有信号
			{
				if (stra->TCH_flag == STRATEGY_TRACK_TCH_BLACKBOARD)//教师信号为板书
				{
					move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
					stra_res->status = SWITCH_BLBO;
					return stra_res->status;
				}
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE&&stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW)//教师信号为特写并且学生信号也为特写
				{
					//计时切特写
					stra->timer_stra.finish = gettime();
					stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
					if (stra->timer_stra.deltaTime>SWITCH_TIME)
					{
						if (getPosit(stra->stu_cam, &stra->posit_pan, &stra->posit_tilt, 500))
						{
							if (stra->posit_pan == stra->stu_pos.x&&stra->posit_tilt == stra->stu_pos.y)
							{
								stra_res->status = SWITCH_2_STU_EQU_TEA;
								return stra_res->status;
							}
							else
							{
								move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
								stra_res->status = SWITCH_PSTU;
								return stra_res->status;
							}
						}
						else
						{
							move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
							stra_res->status = SWITCH_PSTU;
							return stra_res->status;
						}
						/*stra->status = SWITCH_2_STU_EQU_TEA;
						return stra->status;*/
					}
					else
					{
						move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
						stra_res->status = SWITCH_PSTU;
						return stra_res->status;
					}
				}
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_MULITY&&stra->TCH_flag  == STRATEGY_TRACK_TCH_MOVEINVIEW)//教师信号为特写，学生信号为多人站立
				{
					stra_res->status = SWITCH_2_PSTU_EQU_TEA;
					return stra_res->status;
				}
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE&&stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEOUTVIEW)
				{
					//学生特写等分教师全景
					stra->timer_stra.finish = gettime();
					stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
					if (stra->timer_stra.deltaTime>SWITCH_TIME)
					{
						if (getPosit(stra->stu_cam, &stra->posit_pan, &stra->posit_tilt, 500))
						{
							if (stra->posit_pan == stra->stu_pos.x&&stra->posit_tilt == stra->stu_pos.y)
							{
								stra_res->status = SWITCH_2_STU_EQU_PTEA;
								return stra_res->status;
							}
							else
							{
								move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
								stra_res->status = SWITCH_PSTU;
								return stra_res->status;
							}
						}
						else
						{
							move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
							stra_res->status = SWITCH_PSTU;
							return stra_res->status;
						}
						/*stra->status = SWITCH_2_STU_EQU_TEA;
						return stra->status;*/
					}
					else
					{
						move(stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
						stra_res->status = SWITCH_PSTU;
						return stra_res->status;
					}
					/*stra->status = SWITCH_2_STU_EQU_PTEA;
					return stra->status;*/
				}
				else if ((stra->STU_flag == STRATEGY_TRACK_STU_MOVE_SINGLE || stra->STU_flag == STRATEGY_TRACK_STU_MOVE_MULITY) && stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW)
				{
					stra_res->status = SWITCH_2_PSTU_EQU_TEA;
					return stra_res->status;
				}
				else
				{
					stra_res->status = SWITCH_PSTU;
					return stra_res->status;
				}
			}
			else//教师区域无信号
			{
				if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE)
				{
					//判断是否到时间和相机是否到位
					stra->timer_stra.finish = gettime();
					stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
					if (stra->timer_stra.deltaTime>SWITCH_TIME)
					{
					//切学生区域的教师特写
						stra_res->status = SWITCH_STU;
						return stra_res->status;
					}
					else
					{
						stra_res->status = SWITCH_PSTU;
						return stra_res->status;
					}
				}
				else
				{
					stra_res->status = SWITCH_PSTU;
					return stra_res->status;
				}
					
			}
		}
		else//学生区域无运动
		{
			if (stra->TCH_flag == STRATEGY_TRACK_TCH_BLACKBOARD)
			{
				stra_res->status = SWITCH_BLBO;
				stra_res->last_status = SWITCH_BLBO;
				return stra_res->status;
			}
			else if (stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW)
			{
				if (stra_res->last_status == SWITCH_BLBO)
				{
					if (stra->timer_stra.start==0)
					{
						stra->timer_stra.start = gettime();
						stra_res->status = SWITCH_BLBO;
						stra_res->last_status = SWITCH_BLBO;
						return stra_res->status;
					}
					else
					{
						stra->timer_stra.finish = gettime();
						stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
						if (stra->timer_stra.deltaTime>SWITCH_TIME)
						{
							stra_res->status = SWITCH_TEA;
							stra_res->last_status = SWITCH_TEA;
                                                        clear_timer(&stra->timer_stra);
							return stra_res->status;
						}
						else
						{
							stra_res->status = SWITCH_BLBO;
							stra_res->last_status = SWITCH_BLBO;
							return stra_res->status;
						}
					}
				}
				else
				{
					stra_res->status = SWITCH_TEA;
					stra_res->last_status = SWITCH_TEA;
                                        clear_timer(&stra->timer_stra);
					return stra_res->status;
				}
			}
			else if (stra->TCH_flag == STRATEGY_TRACK_TCH_OUTSIDE)
			{
				if (stra_res->last_status == STRATEGY_TRACK_STU_STAND_MULITY)//如果上一状态是多目标则切教师全景
				{
					stra_res->status = SWITCH_PTEA;
                                        stra_res->last_status = SWITCH_PTEA;
					return stra_res->status;
				}
				else//否则为教师走下讲台，切学生。
				{
					stra_res->status = SWITCH_PTEA;
                                        stra_res->last_status = SWITCH_PTEA;
					return stra_res->status;
					//return 
				}
			}
			else
			{
				if (stra_res->last_status == SWITCH_BLBO)
				{
					if (stra->timer_stra.start == 0)
					{
						stra->timer_stra.start = gettime();
						stra_res->status = SWITCH_BLBO;
						stra_res->last_status = SWITCH_BLBO;
						return stra_res->status;
					}
					else
					{
						stra->timer_stra.finish = gettime();
						stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
						if (stra->timer_stra.deltaTime > SWITCH_TIME)
						{
							stra_res->status = SWITCH_PTEA;
							stra_res->last_status = SWITCH_PTEA;
                                                        clear_timer(&stra->timer_stra);
							return stra_res->status;
						}
						else
						{
							stra_res->status = SWITCH_BLBO;
							stra_res->last_status = SWITCH_BLBO;
							return stra_res->status;
						}
					}
				}
				else
				{
					stra_res->status = SWITCH_PTEA;
					stra_res->last_status = SWITCH_PTEA;
                                        clear_timer(&stra->timer_stra);
					return stra_res->status;
				}
			}
		}
	}
}

/*
tch: 为教师跟踪返回的结果；
stu: 为学生跟踪返回的结果；
stra: 为已经初始化了的决策器；
cam_tch: 为控制教师特写镜头的控制器；
cam_stu: 为控制学生特写镜头的控制器。
*/
int switch_strategy(Track_Strategy_t *track_stra, Strategy_Result_t *stra_res)//还需要控制摄像机的结构体和VGA信号的结构体
{
        Strategy_sysData_t *sysData=(Strategy_sysData_t *)(track_stra->sysData);
	if (sysData->result_activity == 1)
	{
		if (sysData->tch_que->size<FPS - 1)
		{
			EnQueue(sysData->tch_que, track_stra->tch->status);//持续向队列加入教师状态去统计

			//首先判断VGA信号
			vga_strategy_analyze(sysData, track_stra->vga);
			//其次响应学生的信号
			stu_strategy_analyze(sysData, track_stra->stu);
                        int res=-1;
			//组合VGA信号和学生信号以及上一次教师的信号来判断该切怎样的画面
			if (sysData->TCH_flag>0)
			{
				res = strategy_status_analyze(sysData,stra_res);
			}
			else
			{
				stra_res->status = SWITCH_PTEA;//刚启动时保持教师全景
				return stra_res->status ;
			}
			return (res > NULL_ACTIVITY) ? res : NULL_ACTIVITY;
		}
		else
		{
			//Item *pitem;
			//int res_stu = -1;
			//int res_tch = -1;
                        int res=-1;
			EnQueue(sysData->tch_que, track_stra->tch->status);
			if (-1 < strategy_frame_analyze(sysData))
			{
				//先判断VGA信号
				vga_strategy_analyze(sysData, track_stra->vga);
				//判断学生的信号类型
				stu_strategy_analyze(sysData, track_stra->stu);
                                //判断教师的信号类型
				tch_strategy_analyze(sysData, track_stra->tch);
			}
			//DeQueue(stra->tch_que, &pitem);

			//组合VGA信号和学生信号以及这一次教师的信号来判断该切怎样的画面
			res=strategy_status_analyze(sysData,stra_res);

			ClearQueue(sysData->tch_que);
			return (res > NULL_ACTIVITY) ? res : NULL_ACTIVITY;
		}
	}
	else
	{
		sysData->timer_stra.finish = gettime();
		sysData->timer_stra.deltaTime = sysData->timer_stra.finish - sysData->timer_stra.start;
		if (sysData->timer_stra.deltaTime<sysData->display_time)
		{
			stra_res->status = SWITCH_VGA;
			return stra_res->status;
		}
		else
		{
			sysData->result_activity = 1;
			clear_timer(&sysData->timer_stra);
			return NULL_ACTIVITY;
		}
	}
        return 0;
}

static int init_strategy(Strategy_sysData_t* stra)
{
    stra->tch_que = InitQueue();
    stra->STU_flag = 0;
    stra->VGA_flag = 0;
    stra->TCH_flag = 0;
    stra->timer_stra.start = 0;
    stra->timer_stra.finish = 0;
    stra->timer_stra.deltaTime = 0;
	/*stra->tch_que->size = 0;
	stra->status = 0;
	stra->stu_pos.x = -1;
	stra->stu_pos.y = -1;
	stra->tch_posIndex = -1;
	stra->STU_flag = 0;
	stra->VGA_flag = 0;
	stra->TCH
	stra->timer_stra.start = 0;
	stra->timer_stra.finish = 0;
	stra->timer_stra.deltaTime = 0;*/
    return 0;
}

int first_PicMode(MainEnc_FirstPicMode_t *fpm, Track_Strategy_t *track_stra)
{
        Strategy_sysData_t *sysData=(Strategy_sysData_t *)(track_stra->sysData);
	if (fpm->firstpicture != 0 && fpm->display_time > 0)
	{
		sysData->result_activity = 0;
		sysData->timer_stra.start = gettime();
		sysData->display_time = fpm->display_time;
		return 0;
	}
	else
		return -1;
}

int init_track_strategy(Track_Strategy_t *track_stra)
{
    Strategy_sysData_t *sysData=(Strategy_sysData_t *)(track_stra->sysData);

    init_cam(sysData->tch_cam);
    init_cam(sysData->stu_cam);

	
#ifdef WIN32
	int iResult = 0;
	WSADATA wsaData = { 0 };
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif // WIN32

	startControl(sysData->tch_cam, track_stra->tch_ip, track_stra->tch_port);
	startControl(sysData->stu_cam, track_stra->stu_ip, track_stra->stu_port);

	init_strategy(sysData);
	//return switch_strategy(track_stra,stra_res);
        return 0;
}

int destroy_track_strategy(Track_Strategy_t *track_stra)
{
    if(track_stra==NULL||stra_res==NULL)
    {
        return -1;
    }
    Strategy_sysData_t *sysData=(Strategy_sysData_t *)(track_stra->sysData);
    close_cam(sysData->tch_cam);
    close_cam(sysData->stu_cam);
    DestroyQueue(sysData->tch_que);
    return 0;
}

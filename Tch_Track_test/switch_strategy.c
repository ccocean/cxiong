#include "switch_strategy.h"

static void clear_timer(Strategy_Timer_t *time)
{
	time->start = 0;
	time->finish = 0;
	time->deltaTime = 0;
}

static int strategy_frame_analyze(Strategy_Result_t* stra)
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
static int vga_strategy_analyze(Strategy_Result_t* stra, AlgLink_ScdResult *vga)
{
	stra->VGA_flag = STRATEGY_TRACK_VGA_ACTIVITY;
	return VGA_ACTIVITY;
}

/*
分析学生是何种信号
*/
static int stu_strategy_analyze(Strategy_Result_t* stra, StuITRACK_OutParams_t* stu, Strategy_CamControl_t *stu_cam)
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标
				setZoom(stu_cam, stu->stretchingCoefficient_stand);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标,移动到最后一个运动目标处
				setZoom(stu_cam, stu->stretchingCoefficient_stand);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标,移动到最后一个运动目标处；
				setZoom(stu_cam, stu->stretchingCoefficient_stand);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标,移动到最后一个运动目标处；
				setZoom(stu_cam, stu->stretchingCoefficient_stand);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随移动坐标
				setZoom(stu_cam, stu->stretchingCoefficient_bigMove);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随最后一个移动坐标
				setZoom(stu_cam, stu->stretchingCoefficient_bigMove);
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
					move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随最后一个移动坐标
					setZoom(stu_cam, stu->stretchingCoefficient_bigMove);
					//打开计时器
					//stra->timer_stra.start = gettime();
					return STU_ACTIVITY;
				}
				else
				{
					stra->STU_flag = STRATEGY_TRACK_STU_NULL;//标识学生区域有0个响应
					stra->stu_pos = stu->move_position;
					move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随最后一个移动坐标
					setZoom(stu_cam, stu->stretchingCoefficient_bigMove);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随移动坐标
				setZoom(stu_cam, stu->stretchingCoefficient_bigMove);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随最后一个移动坐标
				setZoom(stu_cam, stu->stretchingCoefficient_bigMove);
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
}

/*
分析教师是何种信号
*/
static int tch_strategy_analyze(Strategy_Result_t* stra, Tch_Result_t* tch, Strategy_CamControl_t *cam_tch)
{
	if (stra->tch_status == STRATEGY_TRACK_TCH_BLACKBOARD)
	{
		//切板书镜头
		//始终移动摄像头跟踪教师
		stra->tch_posIndex = tch->pos;
		stra->TCH_flag = stra->tch_status;
		preset(cam_tch, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
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
		preset(cam_tch, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
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
		preset(cam_tch, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
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
		home(cam_tch);
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
		home(cam_tch);
		stra->TCH_flag = stra->tch_status;
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
static int strategy_status_analyze(Strategy_Result_t* stra, Strategy_CamControl_t *tch_cam, Strategy_CamControl_t *stu_cam)//根据三个不同的信号分析当前的画面决策
{
	if (stra->VGA_flag == STRATEGY_TRACK_VGA_ACTIVITY)//VGA信号
	{
		if (stra->TCH_flag == STRATEGY_TRACK_TCH_BLACKBOARD)
		{
			//切板书与VGA双画面
			stra->status = SWITCH_2_VGA_EQU_BLBO;
			return stra->status;
		}
		else if (stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW&&stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE)
		{
			//判断学生站立时间及相机是否到位
			stra->timer_stra.finish = gettime();
			stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
			if (stra->timer_stra.deltaTime>SWITCH_TIME)
			{
				if (getPosit(stu_cam, &stra->posit_pan, &stra->posit_tilt, 500))
				{
					if (stra->posit_pan == stra->stu_pos.x&&stra->posit_tilt == stra->stu_pos.y)
					{
						//切大VGA，小学生以及小教师的三画面
						stra->status = SWITCH_3_VGA_TEA_STU;
						return stra->status;
					}
					else
					{
						move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
						stra->status = SWITCH_VGA;
						return stra->status;
					}
				}
				else
				{
					move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
					stra->status = SWITCH_VGA;
					return stra->status;
				}
			}
			else
			{
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
				stra->status = SWITCH_VGA;
				return stra->status;
			}
		}
		else
		{
			move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
			stra->status = SWITCH_VGA;
			return stra->status;
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
					move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
					stra->status = SWITCH_BLBO;
					return stra->status;
				}
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE&&stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW)//教师信号为特写并且学生信号也为特写
				{
					//计时切特写
					stra->timer_stra.finish = gettime();
					stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
					if (stra->timer_stra.deltaTime>SWITCH_TIME)
					{
						if (getPosit(stu_cam, &stra->posit_pan, &stra->posit_tilt, 500))
						{
							if (stra->posit_pan == stra->stu_pos.x&&stra->posit_tilt == stra->stu_pos.y)
							{
								stra->status = SWITCH_2_STU_EQU_TEA;
								return stra->status;
							}
							else
							{
								move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
								stra->status = SWITCH_PSTU;
								return stra->status;
							}
						}
						else
						{
							move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
							stra->status = SWITCH_PSTU;
							return stra->status;
						}
						/*stra->status = SWITCH_2_STU_EQU_TEA;
						return stra->status;*/
					}
					else
					{
						move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
						stra->status = SWITCH_PSTU;
						return stra->status;
					}
				}
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_MULITY&&stra->TCH_flag  == STRATEGY_TRACK_TCH_MOVEINVIEW)//教师信号为特写，学生信号为多人站立
				{
					stra->status = SWITCH_2_PSTU_EQU_TEA;
					return stra->status;
				}
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE&&stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEOUTVIEW)
				{
					//学生特写等分教师全景
					stra->timer_stra.finish = gettime();
					stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
					if (stra->timer_stra.deltaTime>SWITCH_TIME)
					{
						if (getPosit(stu_cam, &stra->posit_pan, &stra->posit_tilt, 500))
						{
							if (stra->posit_pan == stra->stu_pos.x&&stra->posit_tilt == stra->stu_pos.y)
							{
								stra->status = SWITCH_2_STU_EQU_PTEA;
								return stra->status;
							}
							else
							{
								move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
								stra->status = SWITCH_PSTU;
								return stra->status;
							}
						}
						else
						{
							move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
							stra->status = SWITCH_PSTU;
							return stra->status;
						}
						/*stra->status = SWITCH_2_STU_EQU_TEA;
						return stra->status;*/
					}
					else
					{
						move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
						stra->status = SWITCH_PSTU;
						return stra->status;
					}
					/*stra->status = SWITCH_2_STU_EQU_PTEA;
					return stra->status;*/
				}
				else if ((stra->STU_flag == STRATEGY_TRACK_STU_MOVE_SINGLE || stra->STU_flag == STRATEGY_TRACK_STU_MOVE_MULITY) && stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW)
				{
					stra->status = SWITCH_2_PSTU_EQU_TEA;
					return stra->status;
				}
				else
				{
					stra->status = SWITCH_PSTU;
					return stra->status;
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
						stra->status = SWITCH_STU;
						return stra->status;
					}
					else
					{
						stra->status = SWITCH_PSTU;
						return stra->status;
					}
				}
				else
				{
					stra->status = SWITCH_PSTU;
					return stra->status;
				}
					
			}
		}
		else//学生区域无运动
		{
			if (stra->TCH_flag == STRATEGY_TRACK_TCH_BLACKBOARD)
			{
				stra->status = SWITCH_BLBO;
				stra->last_status = SWITCH_BLBO;
				return stra->status;
			}
			else if (stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW)
			{
				if (stra->last_status == SWITCH_BLBO)
				{
					if (stra->timer_stra.start==0)
					{
						stra->timer_stra.start = gettime();
						stra->status = SWITCH_BLBO;
						stra->last_status = SWITCH_BLBO;
						return stra->status;
					}
					else
					{
						stra->timer_stra.finish = gettime();
						stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
						if (stra->timer_stra.deltaTime>SWITCH_TIME)
						{
							stra->status = SWITCH_TEA;
							stra->last_status = SWITCH_TEA;
							return stra->status;
						}
						else
						{
							stra->status = SWITCH_BLBO;
							stra->last_status = SWITCH_BLBO;
							return stra->status;
						}
					}
				}
				else
				{
					stra->status = SWITCH_TEA;
					stra->last_status = SWITCH_TEA;
					return stra->status;
				}
			}
			else if (stra->TCH_flag == STRATEGY_TRACK_TCH_OUTSIDE)
			{
				if (stra->last_status == STRATEGY_TRACK_STU_STAND_MULITY)//如果上一状态是多目标则切教师全景
				{
					stra->status = SWITCH_PTEA;
					return stra->status;
				}
				else//否则为教师走下讲台，切学生。
				{
					stra->status = SWITCH_PSTU;
					return stra->status;
					//return 
				}
			}
			else
			{
				if (stra->last_status == SWITCH_BLBO)
				{
					if (stra->timer_stra.start == 0)
					{
						stra->timer_stra.start = gettime();
						stra->status = SWITCH_BLBO;
						stra->last_status = SWITCH_BLBO;
						return stra->status;
					}
					else
					{
						stra->timer_stra.finish = gettime();
						stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
						if (stra->timer_stra.deltaTime > SWITCH_TIME)
						{
							stra->status = SWITCH_PTEA;
							stra->last_status = SWITCH_PTEA;
							return stra->status;
						}
						else
						{
							stra->status = SWITCH_BLBO;
							stra->last_status = SWITCH_BLBO;
							return stra->status;
						}
					}
				}
				else
				{
					stra->status = SWITCH_PTEA;
					stra->last_status = SWITCH_PTEA;
					return stra->status;
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
static int switch_strategy(AlgLink_ScdResult *vga,Tch_Result_t* tch, StuITRACK_OutParams_t* stu, Strategy_Result_t* stra, Strategy_CamControl_t *cam_tch, Strategy_CamControl_t *cam_stu)//还需要控制摄像机的结构体和VGA信号的结构体
{
	if (stra->tch_que->size<FPS-1)
	{
		EnQueue(stra->tch_que, tch->status);//持续向队列加入教师状态去统计

		//首先判断VGA信号
		vga_strategy_analyze(stra,vga);
		//其次响应学生的信号
		int res = stu_strategy_analyze(stra, stu, cam_stu);

		//组合VGA信号和学生信号以及上一次教师的信号来判断该切怎样的画面
		if (stra->TCH_flag>0)
		{
			//strategy_status_analyze(stra);
		}
		else
		{
			stra->status = SWITCH_PTEA;//刚启动时保持教师全景
		}
		return (res>NULL_ACTIVITY) ? res : NULL_ACTIVITY;
	}
	else
	{
		//Item *pitem;
		int res_stu=-1;
		int res_tch=-1;
		EnQueue(stra->tch_que, tch->status);
		if (-1 < strategy_frame_analyze(stra))
		{
			//先判断VGA信号
			vga_strategy_analyze(stra,vga);
			//判断学生是否有响应，但不打断教师跟踪
			res_stu = stu_strategy_analyze(stra, stu, cam_stu);
			res_tch = tch_strategy_analyze(stra, tch, cam_tch);
		}
		//DeQueue(stra->tch_que, &pitem);

		//组合VGA信号和学生信号以及这一次教师的信号来判断该切怎样的画面
		//strategy_status_analyze(stra);

		ClearQueue(stra->tch_que);
		return (res_stu>res_tch) ? res_stu : res_tch;
	}
}

static int init_strategy(Strategy_Result_t* stra)
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

int track_strategy(Track_Strategy_t *track_stra)
{
	init_cam(track_stra->tch_cam);
	init_cam(track_stra->stu_cam);

#ifdef WIN32
	int iResult = 0;
	WSADATA wsaData = { 0 };
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif // WIN32

	startControl(track_stra->tch_cam, track_stra->tch_ip, track_stra->tch_port);
	startControl(track_stra->stu_cam, track_stra->stu_ip, track_stra->stu_port);

	init_strategy(track_stra->stra);
	return switch_strategy(track_stra->vga,track_stra->tch, track_stra->stu, track_stra->stra, track_stra->tch_cam, track_stra->stu_cam);
}
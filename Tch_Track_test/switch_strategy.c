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
	for (i = 1; i < 7; i++)
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
	if (NULL==stra||vga==NULL)
	{
		return -1;
	}
	stra->VGA_flag = STRATEGY_TRACK_VGA_ACTIVITY;
	return VGA_ACTIVITY;
}

/*
分析学生是何种信号
*/
static int stu_strategy_analyze(Strategy_sysData_t* stra, StuITRACK_OutParams_t* stu)
{
	if (NULL == stra || stu == NULL)
	{
		return -1;
	}
	if (RESULT_STUTRACK_IF_NEWCHANGE(stu->result_flag))//判断当前帧是否有变化
	{
		if (RESULT_STUTRACK_IF_STANDUP(stu->result_flag))//判断当前帧是否有站立
		{
			if (stu->count_trackObj_stand == 1)
			{
				stra->STU_flag = STRATEGY_TRACK_STU_STAND_SINGLE;//标识学生区域有1个响应
				//移动相机到该坐标，计时切特写
				stra->stu_pos = stu->stand_position;
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_stand);
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
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标,移动到最后一个运动目标处
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_stand);
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
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标,移动到最后一个运动目标处；
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_stand);
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
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标,移动到最后一个运动目标处；
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_stand);
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
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随移动坐标
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_bigMove);
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
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随最后一个移动坐标
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_bigMove);
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
					move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随最后一个移动坐标
					setZoom(&stra->stu_cam, stu->stretchingCoefficient_bigMove);
					//打开计时器
					//stra->timer_stra.start = gettime();
					return STU_ACTIVITY;
				}
				else
				{
					stra->STU_flag = STRATEGY_TRACK_STU_NULL;//标识学生区域有0个响应
					stra->stu_pos = stu->move_position;
					move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随最后一个移动坐标
					setZoom(&stra->stu_cam, stu->stretchingCoefficient_bigMove);
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
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随移动坐标
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_bigMove);
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
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//绝对坐标，跟随最后一个移动坐标
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_bigMove);
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
			/*stra->timer_stra.finish = gettime();
			stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;*/
			return STU_ACTIVITY;
		}
		if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_MULITY)
		{
			//clear_timer(&stra->timer_stra);
			return STU_ACTIVITY;
		}
		if (stra->STU_flag == STRATEGY_TRACK_STU_MOVE_SINGLE || stra->STU_flag == STRATEGY_TRACK_STU_MOVE_MULITY)
		{
			//移动相机，保持全景
			//clear_timer(&stra->timer_stra);
			return STU_ACTIVITY;
		}
		if (stra->STU_flag == STRATEGY_TRACK_STU_NULL)
		{
			//学生区域没有消息，等待其他信号
			//clear_timer(&stra->timer_stra);
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
	if (NULL == stra || tch == NULL)
	{
		return -1;
	}
	if (stra->tch_status == STRATEGY_TRACK_TCH_BLACKBOARD)
	{
		//切板书镜头
		//始终移动摄像头跟踪教师
		stra->tch_posIndex = tch->pos;
		stra->TCH_flag = stra->tch_status;
		preset(&stra->tch_cam, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
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
		preset(&stra->tch_cam, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
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
		preset(&stra->tch_cam, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
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
		home(&stra->tch_cam);
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
		home(&stra->tch_cam);
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
int strategy_status_analyze(Strategy_sysData_t* stra)//根据三个不同的信号分析当前的画面决策
{
	if (NULL == stra)
	{
		return -1;
	}
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
			if (stra->timer_stra.deltaTime > SWITCH_TIME)
			{
				if (getPosit(&stra->stu_cam, &stra->posit_pan, &stra->posit_tilt, 500))
				{
					if (stra->posit_pan == stra->stu_pos.x&&stra->posit_tilt == stra->stu_pos.y)
					{
						//切大VGA，小学生以及小教师的三画面
						stra->status = SWITCH_3_VGA_TEA_STU;
						return stra->status;
					}
					else
					{
						move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
						stra->status = SWITCH_VGA;
						return stra->status;
					}
				}
				else
				{
					startControl(&stra->stu_cam, stra->stu_ip, stra->stu_port);//重连
					move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
					stra->status = SWITCH_VGA;
					return stra->status;
				}
			}
			else
			{
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
				stra->status = SWITCH_VGA;
				return stra->status;
			}
		}
		else
		{
			move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
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
					move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
					stra->status = SWITCH_BLBO;
					return stra->status;
				}
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE&&stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW)//教师信号为特写并且学生信号也为特写
				{
					//计时切特写
					stra->timer_stra.finish = gettime();
					stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
					if (stra->timer_stra.deltaTime > SWITCH_TIME)
					{
						if (getPosit(&stra->stu_cam, &stra->posit_pan, &stra->posit_tilt, 500))
						{
							if (stra->posit_pan == stra->stu_pos.x&&stra->posit_tilt == stra->stu_pos.y)
							{
								stra->status = SWITCH_2_STU_EQU_TEA;
								return stra->status;
							}
							else
							{
								move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
								stra->status = SWITCH_PSTU;
								return stra->status;
							}
						}
						else
						{
							move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
							stra->status = SWITCH_PSTU;
							return stra->status;
						}
						/*stra->status = SWITCH_2_STU_EQU_TEA;
						return stra->status;*/
					}
					else
					{
						move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
						stra->status = SWITCH_PSTU;
						return stra->status;
					}
				}
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_MULITY&&stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW)//教师信号为特写，学生信号为多人站立
				{
					stra->status = SWITCH_2_PSTU_EQU_TEA;
					return stra->status;
				}
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE&&stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEOUTVIEW)
				{
					//学生特写等分教师全景
					stra->timer_stra.finish = gettime();
					stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
					if (stra->timer_stra.deltaTime > SWITCH_TIME)
					{
						if (getPosit(&stra->stu_cam, &stra->posit_pan, &stra->posit_tilt, 500))
						{
							if (stra->posit_pan == stra->stu_pos.x&&stra->posit_tilt == stra->stu_pos.y)
							{
								stra->status = SWITCH_2_STU_EQU_PTEA;
								return stra->status;
							}
							else
							{
								move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
								stra->status = SWITCH_PSTU;
								return stra->status;
							}
						}
						else
						{
							move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
							stra->status = SWITCH_PSTU;
							return stra->status;
						}
						/*stra->status = SWITCH_2_STU_EQU_TEA;
						return stra->status;*/
					}
					else
					{
						move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
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
					if (stra->timer_stra.deltaTime > SWITCH_TIME)
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
							stra->status = SWITCH_TEA;
							stra->last_status = SWITCH_TEA;
							clear_timer(&stra->timer_stra);
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
					clear_timer(&stra->timer_stra);
					return stra->status;
				}
			}
			else if (stra->TCH_flag == STRATEGY_TRACK_TCH_OUTSIDE)
			{
				if (stra->last_status == STRATEGY_TRACK_STU_STAND_MULITY)//如果上一状态是多目标则切教师全景
				{
					stra->status = SWITCH_PTEA;
					stra->last_status = SWITCH_PTEA;
					return stra->status;
				}
				else//否则为教师走下讲台，切学生。
				{
					stra->status = SWITCH_PTEA;
					stra->last_status = SWITCH_PTEA;
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
							clear_timer(&stra->timer_stra);
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
					clear_timer(&stra->timer_stra);
					return stra->status;
				}
			}
		}
	}
}



int switch_strategy_tch(Strategy_sysData_t *sysData, Tch_Result_t *tch)
{
	if (NULL == sysData || tch == NULL)
	{
            printf("parameters errors\r\n");    
            return -1;
	}
	if (sysData->result_activity == 1)
	{
            if (sysData->timer_vga.start>0)
            {
                sysData->timer_vga.finish = gettime();
                sysData->timer_vga.deltaTime = sysData->timer_vga.finish - sysData->timer_vga.start;
                if (sysData->timer_vga.deltaTime>SWITCH_VGA_TIME)
                {
                        sysData->VGA_flag = STRATEGY_TRACK_VGA_NULL;
                        clear_timer(&sysData->timer_vga);
                }
            }
            if (sysData->tch_que->size < FPS - 1)
            {
                EnQueue(sysData->tch_que, tch->status);//持续向队列加入教师状态去统计
                if (sysData->TCH_flag>0)
                {
                        int res = strategy_status_analyze(sysData);
                        return (res > NULL_ACTIVITY) ? res : NULL_ACTIVITY;
                }
                else
                {
                        sysData->status = SWITCH_PTEA;//刚启动时保持教师全景
                        return sysData->status;
                }
            }
            else
            {
                int res = -1;
                EnQueue(sysData->tch_que, tch->status);
                if (-1 < strategy_frame_analyze(sysData))
                {
                        tch_strategy_analyze(sysData, tch);
                }

                //组合VGA信号和学生信号以及这一次教师的信号来判断该切怎样的画面
                res = strategy_status_analyze(sysData);

                ClearQueue(sysData->tch_que);
                return (res > NULL_ACTIVITY) ? res : NULL_ACTIVITY;
            }
	}
	else
	{
            sysData->timer_stra.finish = gettime();
            sysData->timer_stra.deltaTime = sysData->timer_stra.finish - sysData->timer_stra.start;
            if (sysData->timer_stra.deltaTime < sysData->display_time)
            {
                sysData->status = sysData->firstpicture;
                return sysData->status;
            }
            else
            {
                sysData->result_activity = 1;
                sysData->VGA_flag = NULL_ACTIVITY;
                sysData->TCH_flag = NULL_ACTIVITY;
                sysData->STU_flag = NULL_ACTIVITY;
                clear_timer(&sysData->timer_stra);
                return SWITCH_PTEA;//初始画面结束，先切教师全景
            }
	}
	return 0;
}

int switch_strategy_stu(Strategy_sysData_t *sysData, StuITRACK_OutParams_t *stu)
{
	if (NULL == sysData || stu == NULL)
	{
            printf("parameters errors\r\n");  
            return -1;
	}
	//分析学生的信号
	int res = 0;
	if (sysData->result_activity == 1)
	{
            if (sysData->timer_vga.start > 0)
            {
                sysData->timer_vga.finish = gettime();
                sysData->timer_vga.deltaTime = sysData->timer_vga.finish - sysData->timer_vga.start;
                if (sysData->timer_vga.deltaTime > SWITCH_VGA_TIME)
                {
                    sysData->VGA_flag = STRATEGY_TRACK_VGA_NULL;
                    clear_timer(&sysData->timer_vga);
                }
            }
            stu_strategy_analyze(sysData, stu);
            res = strategy_status_analyze(sysData);
            return (res > NULL_ACTIVITY) ? res : NULL_ACTIVITY;
	}
	else
	{
            sysData->timer_stra.finish = gettime();
            sysData->timer_stra.deltaTime = sysData->timer_stra.finish - sysData->timer_stra.start;
            if (sysData->timer_stra.deltaTime < sysData->display_time)
            {
                sysData->status = sysData->firstpicture;
                return sysData->status;
            }
            else
            {
                sysData->result_activity = 1;
                sysData->VGA_flag = NULL_ACTIVITY;
                sysData->TCH_flag = NULL_ACTIVITY;
                sysData->STU_flag = NULL_ACTIVITY;
                clear_timer(&sysData->timer_stra);
                return SWITCH_PTEA;//初始画面结束，先切教师全景
            }
	}
	return 0;
}

int switch_strategy_vga(Strategy_sysData_t *sysData, AlgLink_ScdResult *vga)
{
	if (NULL == sysData || vga == NULL)
	{
            printf("parameters errors\r\n");  
            return -1;
	}
	int res = 0;
	if (sysData->result_activity == 1)
	{
            sysData->timer_vga.start = gettime();
            vga_strategy_analyze(sysData, vga);
            res = strategy_status_analyze(sysData);
            return (res > NULL_ACTIVITY) ? res : NULL_ACTIVITY;
	}
	else
	{
            sysData->timer_stra.finish = gettime();
            sysData->timer_stra.deltaTime = sysData->timer_stra.finish - sysData->timer_stra.start;
            if (sysData->timer_stra.deltaTime < sysData->display_time)
            {
                    sysData->status = sysData->firstpicture;
                    return sysData->status;
            }
            else
            {
                    sysData->result_activity = 1;
                    sysData->VGA_flag = NULL_ACTIVITY;
                    sysData->TCH_flag = NULL_ACTIVITY;
                    sysData->STU_flag = NULL_ACTIVITY;
                    clear_timer(&sysData->timer_stra);
                    return SWITCH_PTEA;//初始画面结束，先切教师全景
            }
	}
	return 0;
}

static int init_strategy(Strategy_sysData_t* stra)
{
    if(NULL==stra)
    {
        printf("parameters is null!");
        return -1;
    }
	stra->tch_que = InitQueue();
	stra->STU_flag = NULL_ACTIVITY;
	stra->VGA_flag = NULL_ACTIVITY;
	stra->TCH_flag = NULL_ACTIVITY;
	/*stra->timer_stra.start = 0;
	stra->timer_stra.finish = 0;
	stra->timer_stra.deltaTime = 0;*/
	clear_timer(&stra->timer_stra);
	clear_timer(&stra->timer_vga);
	return 0;
}

int first_PicMode(MainEnc_FirstPicMode_t *fpm, Strategy_sysData_t *track_stra)
{
	if (fpm == NULL || track_stra == NULL)
		return -1;
	// *sysData=(Strategy_sysData_t *)(track_stra->sysData);
	if (fpm->firstpicture != 0 && fpm->display_time > 0)
	{
		track_stra->result_activity = 0;
		track_stra->timer_stra.start = gettime();
		track_stra->display_time = fpm->display_time;
		return 0;
	}
	else
		return -2;
}

Strategy_sysData_t* init_track_strategy(Strategy_IpPort_t ip_port)
{
    if (ip_port.tch_ip == NULL || ip_port.stu_ip == NULL)
        return NULL;

    printf("Strategy is starting...-----------------------------------");
    
    Strategy_sysData_t *sysData = (Strategy_sysData_t*)malloc(sizeof(Strategy_sysData_t));
    
    if(NULL==sysData)
    {
        printf("Strategy init failed! malloc failed!\n");
        return NULL;
    }
    
    memset(sysData, 0, sizeof(Strategy_sysData_t));

    if(0!=init_cam(&sysData->tch_cam)||0!=init_cam(&sysData->stu_cam))
    {
        printf("cams init failed! Thread create failed!\n");
        return NULL;
    }

    if(FALSE==startControl(&sysData->tch_cam, ip_port.tch_ip, ip_port.tch_port))
    {
        printf("tch cam start failed!\r\n");
        return NULL;
    }
    if(FALSE==startControl(&sysData->stu_cam, ip_port.stu_ip, ip_port.stu_port))
    {
        printf("stu cam start failed!\r\n");
        return NULL;
    }
        
#ifdef WIN32
	int iResult = 0;
	WSADATA wsaData = { 0 };
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        strcpy(sysData->tch_ip, ip_port.tch_ip);
        strcpy(sysData->stu_ip, ip_port.stu_ip);
#else
        snprintf(sysData->tch_ip,IP_ADDR_LEN,"%s",ip_port.tch_ip);
        snprintf(sysData->stu_ip,IP_ADDR_LEN,"%s",ip_port.stu_ip);
#endif // WIN32
        sysData->tch_port = ip_port.tch_port;
	sysData->stu_port = ip_port.stu_port;
	
	if(init_strategy(sysData)<0)
        {
            printf("Strategy_sysData_t init failed!\r\n");
            return NULL;
        }
	//return switch_strategy(track_stra,stra_res);
        printf("Strategy init success!====================================");
	return sysData;
}

int destroy_track_strategy(Strategy_sysData_t *track_stra)
{
	if (track_stra == NULL)
	{
		return -1;
	}
	//Strategy_sysData_t *sysData = (Strategy_sysData_t *)(track_stra->sysData);
	close_cam(&track_stra->tch_cam);
	close_cam(&track_stra->stu_cam);
	DestroyQueue(track_stra->tch_que);
	free(track_stra);
	return 0;
}

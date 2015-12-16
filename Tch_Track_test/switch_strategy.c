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
����vga�Ǻ����ź�
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
����ѧ���Ǻ����ź�
*/
static int stu_strategy_analyze(Strategy_sysData_t* stra, StuITRACK_OutParams_t* stu)
{
	if (NULL == stra || stu == NULL)
	{
		return -1;
	}
	if (RESULT_STUTRACK_IF_NEWCHANGE(stu->result_flag))//�жϵ�ǰ֡�Ƿ��б仯
	{
		if (RESULT_STUTRACK_IF_STANDUP(stu->result_flag))//�жϵ�ǰ֡�Ƿ���վ��
		{
			if (stu->count_trackObj_stand == 1)
			{
				stra->STU_flag = STRATEGY_TRACK_STU_STAND_SINGLE;//��ʶѧ��������1����Ӧ
				//�ƶ�����������꣬��ʱ����д
				stra->stu_pos = stu->stand_position;
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//��������
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_stand);
				//stra->status = SWITCH_PSTU;
				//�򿪼�ʱ��
				stra->timer_stra.start = gettime();
				return STU_ACTIVITY;
			}
			else if (stu->count_trackObj_stand > 1)
			{
				stra->STU_flag = STRATEGY_TRACK_STU_STAND_MULITY;//��ʶѧ�������ж����Ӧ
				//�ƶ�����������꣬��ȫ��
				stra->stu_pos = stu->stand_position;
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//��������,�ƶ������һ���˶�Ŀ�괦
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_stand);
				//stra->status = SWITCH_PSTU;
				//��ʱ������
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
				stra->STU_flag = STRATEGY_TRACK_STU_NULL;//��ʶѧ��������0����Ӧ
				//stra->status = SWITCH_PSTU;
				//��ʱ������
				clear_timer(&stra->timer_stra);
				return STU_ACTIVITY;
			}
			else if (stu->count_trackObj_stand == 1)
			{
				stra->STU_flag = STRATEGY_TRACK_STU_STAND_SINGLE;//��ʶѧ��������1����Ӧ
				//�ƶ�����������꣬��ʱ����д
				stra->stu_pos = stu->stand_position;
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//��������,�ƶ������һ���˶�Ŀ�괦��
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_stand);
				//stra->status = SWITCH_PSTU;
				//�򿪼�ʱ��
				stra->timer_stra.start = gettime();
				return STU_ACTIVITY;
			}
			else
			{
				stra->STU_flag = STRATEGY_TRACK_STU_STAND_MULITY;//��ʶѧ�������ж����Ӧ
				//�ƶ�����������꣬��ȫ��
				stra->stu_pos = stu->stand_position;
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//��������,�ƶ������һ���˶�Ŀ�괦��
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_stand);
				//stra->status = SWITCH_PSTU;
				//��ʱ������
				clear_timer(&stra->timer_stra);
				return STU_ACTIVITY;
			}
		}
		if (RESULT_STUTRACK_IF_MOVE(stu->result_flag))
		{
			if (stu->count_trackObj_bigMove == 1)
			{
				stra->STU_flag = STRATEGY_TRACK_STU_MOVE_SINGLE;
				//�ƶ�����������꣬��ʱ����д
				stra->stu_pos = stu->move_position;
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//�������꣬�����ƶ�����
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_bigMove);
				//stra->status = SWITCH_PSTU;
				//��ʱ������
				clear_timer(&stra->timer_stra);
				return STU_ACTIVITY;
			}
			else
			{
				stra->STU_flag = STRATEGY_TRACK_STU_MOVE_MULITY;
				//�ƶ�����������꣬��ȫ��
				stra->stu_pos = stu->move_position;
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//�������꣬�������һ���ƶ�����
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_bigMove);
				//stra->status = SWITCH_PSTU;
				//��ʱ������
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
					stra->STU_flag = STRATEGY_TRACK_STU_STAND_SINGLE;//��ʦ��ѧ������ͣ��
					stra->stu_pos = stu->move_position;
					move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//�������꣬�������һ���ƶ�����
					setZoom(&stra->stu_cam, stu->stretchingCoefficient_bigMove);
					//�򿪼�ʱ��
					//stra->timer_stra.start = gettime();
					return STU_ACTIVITY;
				}
				else
				{
					stra->STU_flag = STRATEGY_TRACK_STU_NULL;//��ʶѧ��������0����Ӧ
					stra->stu_pos = stu->move_position;
					move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//�������꣬�������һ���ƶ�����
					setZoom(&stra->stu_cam, stu->stretchingCoefficient_bigMove);
					//stra->status = SWITCH_PSTU;
					//��ʱ������
					clear_timer(&stra->timer_stra);
					return NULL_ACTIVITY;
				}

			}
			else if (stu->count_trackObj_bigMove == 1)
			{
				stra->STU_flag = STRATEGY_TRACK_STU_MOVE_SINGLE;
				//�ƶ�����������꣬��ʱ��ȫ��
				stra->stu_pos = stu->move_position;
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//�������꣬�����ƶ�����
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_bigMove);
				//stra->status = SWITCH_PSTU;
				//��ʱ������
				clear_timer(&stra->timer_stra);
				return STU_ACTIVITY;
			}
			else
			{
				stra->STU_flag = STRATEGY_TRACK_STU_MOVE_MULITY;
				//�ƶ�����������꣬��ȫ��
				stra->stu_pos = stu->move_position;
				move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//�������꣬�������һ���ƶ�����
				setZoom(&stra->stu_cam, stu->stretchingCoefficient_bigMove);
				//stra->status = SWITCH_PSTU;
				//��ʱ������
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
			//�ƶ����������ȫ��
			//clear_timer(&stra->timer_stra);
			return STU_ACTIVITY;
		}
		if (stra->STU_flag == STRATEGY_TRACK_STU_NULL)
		{
			//ѧ������û����Ϣ���ȴ������ź�
			//clear_timer(&stra->timer_stra);
			return NULL_ACTIVITY;
		}
		return NULL_ACTIVITY;
	}
	return NULL_ACTIVITY;
}

/*
������ʦ�Ǻ����ź�
*/
static int tch_strategy_analyze(Strategy_sysData_t* stra, Tch_Result_t* tch)
{
	if (NULL == stra || tch == NULL)
	{
		return -1;
	}
	if (stra->tch_status == STRATEGY_TRACK_TCH_BLACKBOARD)
	{
		//�а��龵ͷ
		//ʼ���ƶ�����ͷ���ٽ�ʦ
		stra->tch_posIndex = tch->pos;
		stra->TCH_flag = stra->tch_status;
		preset(&stra->tch_cam, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
		return TCH_ACTIVITY;
		//if (stra->STU_flag == NULL_ACTIVITY && stra->VGA_flag == NULL_ACTIVITY)
		//{
		//	stra->status = SWITCH_BLBO;//������ָ��
		//	return TCH_ACTIVITY;
		//}
	}
	else if (stra->tch_status == STRATEGY_TRACK_TCH_MOVEOUTVIEW || stra->tch_status == STRATEGY_TRACK_TCH_MULITY)
	{
		//��ȫ��
		//ʼ���ƶ�����ͷ���ٽ�ʦ
		stra->TCH_flag = stra->tch_status;
		stra->tch_posIndex = tch->pos;
		preset(&stra->tch_cam, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
		return TCH_ACTIVITY;
		//if (stra->STU_flag == NULL_ACTIVITY && stra->VGA_flag == NULL_ACTIVITY)
		//{
		//	stra->status = SWITCH_PTEA;//����ʦȫ��ָ��
		//	return TCH_ACTIVITY;
		//}
	}
	else if (stra->tch_status == STRATEGY_TRACK_TCH_MOVEINVIEW)
	{
		//����д
		//�ж�����ͷ�Ƿ�λ���ҹ�ʱ�������д
		stra->tch_posIndex = tch->pos;
		stra->TCH_flag = stra->tch_status;
		preset(&stra->tch_cam, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
		return TCH_ACTIVITY;
		//if (stra->STU_flag == NULL_ACTIVITY && stra->VGA_flag == NULL_ACTIVITY)
		//{
		//	stra->status = SWITCH_TEA;//����ʦ��дָ��
		//	return TCH_ACTIVITY;
		//}
	}
	else if (stra->tch_status == STRATEGY_TRACK_TCH_OUTSIDE)
	{
		//��ѧ��
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
		//��״̬�򷵻ؽ�ʦȫ��
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
�ۺ������ź����жϸ���ʲô����
*/
int strategy_status_analyze(Strategy_sysData_t* stra)//����������ͬ���źŷ�����ǰ�Ļ������
{
	if (NULL == stra)
	{
		return -1;
	}
	if (stra->VGA_flag == STRATEGY_TRACK_VGA_ACTIVITY)//VGA�ź�
	{
		if (stra->TCH_flag == STRATEGY_TRACK_TCH_BLACKBOARD)
		{
			//�а�����VGA˫����
			stra->status = SWITCH_2_VGA_EQU_BLBO;
			return stra->status;
		}
		else if (stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW&&stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE)
		{
			//�ж�ѧ��վ��ʱ�估����Ƿ�λ
			stra->timer_stra.finish = gettime();
			stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
			if (stra->timer_stra.deltaTime > SWITCH_TIME)
			{
				if (getPosit(&stra->stu_cam, &stra->posit_pan, &stra->posit_tilt, 500))
				{
					if (stra->posit_pan == stra->stu_pos.x&&stra->posit_tilt == stra->stu_pos.y)
					{
						//�д�VGA��Сѧ���Լ�С��ʦ��������
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
					startControl(&stra->stu_cam, stra->stu_ip, stra->stu_port);//����
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
	else//��VGA�ź�
	{
		if (stra->STU_flag != STRATEGY_TRACK_STU_NULL)//ѧ���������˶�
		{
			if (stra->TCH_flag != STRATEGY_TRACK_TCH_OUTSIDE)//ͬʱ��ʦ���������ź�
			{
				if (stra->TCH_flag == STRATEGY_TRACK_TCH_BLACKBOARD)//��ʦ�ź�Ϊ����
				{
					move(&stra->stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
					stra->status = SWITCH_BLBO;
					return stra->status;
				}
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE&&stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW)//��ʦ�ź�Ϊ��д����ѧ���ź�ҲΪ��д
				{
					//��ʱ����д
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
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_MULITY&&stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW)//��ʦ�ź�Ϊ��д��ѧ���ź�Ϊ����վ��
				{
					stra->status = SWITCH_2_PSTU_EQU_TEA;
					return stra->status;
				}
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE&&stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEOUTVIEW)
				{
					//ѧ����д�ȷֽ�ʦȫ��
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
			else//��ʦ�������ź�
			{
				if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE)
				{
					//�ж��Ƿ�ʱ�������Ƿ�λ
					stra->timer_stra.finish = gettime();
					stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
					if (stra->timer_stra.deltaTime > SWITCH_TIME)
					{
						//��ѧ������Ľ�ʦ��д
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
		else//ѧ���������˶�
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
				if (stra->last_status == STRATEGY_TRACK_STU_STAND_MULITY)//�����һ״̬�Ƕ�Ŀ�����н�ʦȫ��
				{
					stra->status = SWITCH_PTEA;
					stra->last_status = SWITCH_PTEA;
					return stra->status;
				}
				else//����Ϊ��ʦ���½�̨����ѧ����
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
                EnQueue(sysData->tch_que, tch->status);//��������м����ʦ״̬ȥͳ��
                if (sysData->TCH_flag>0)
                {
                        int res = strategy_status_analyze(sysData);
                        return (res > NULL_ACTIVITY) ? res : NULL_ACTIVITY;
                }
                else
                {
                        sysData->status = SWITCH_PTEA;//������ʱ���ֽ�ʦȫ��
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

                //���VGA�źź�ѧ���ź��Լ���һ�ν�ʦ���ź����жϸ��������Ļ���
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
                return SWITCH_PTEA;//��ʼ������������н�ʦȫ��
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
	//����ѧ�����ź�
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
                return SWITCH_PTEA;//��ʼ������������н�ʦȫ��
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
                    return SWITCH_PTEA;//��ʼ������������н�ʦȫ��
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

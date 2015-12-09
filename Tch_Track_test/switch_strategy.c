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
����vga�Ǻ����ź�
*/
static int vga_strategy_analyze(Strategy_Result_t* stra, AlgLink_ScdResult *vga)
{
	stra->VGA_flag = STRATEGY_TRACK_VGA_ACTIVITY;
	return VGA_ACTIVITY;
}

/*
����ѧ���Ǻ����ź�
*/
static int stu_strategy_analyze(Strategy_Result_t* stra, StuITRACK_OutParams_t* stu, Strategy_CamControl_t *stu_cam)
{
	if (RESULT_STUTRACK_IF_NEWCHANGE(stu->result_flag))//�жϵ�ǰ֡�Ƿ��б仯
	{
		if (RESULT_STUTRACK_IF_STANDUP(stu->result_flag))//�жϵ�ǰ֡�Ƿ���վ��
		{
			if (stu->count_trackObj_stand == 1)
			{
				stra->STU_flag = STRATEGY_TRACK_STU_STAND_SINGLE;//��ʶѧ��������1����Ӧ
				//�ƶ�����������꣬��ʱ����д
				stra->stu_pos = stu->stand_position;
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//��������
				setZoom(stu_cam, stu->stretchingCoefficient_stand);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//��������,�ƶ������һ���˶�Ŀ�괦
				setZoom(stu_cam, stu->stretchingCoefficient_stand);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//��������,�ƶ������һ���˶�Ŀ�괦��
				setZoom(stu_cam, stu->stretchingCoefficient_stand);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//��������,�ƶ������һ���˶�Ŀ�괦��
				setZoom(stu_cam, stu->stretchingCoefficient_stand);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//�������꣬�����ƶ�����
				setZoom(stu_cam, stu->stretchingCoefficient_bigMove);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//�������꣬�������һ���ƶ�����
				setZoom(stu_cam, stu->stretchingCoefficient_bigMove);
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
					move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//�������꣬�������һ���ƶ�����
					setZoom(stu_cam, stu->stretchingCoefficient_bigMove);
					//�򿪼�ʱ��
					//stra->timer_stra.start = gettime();
					return STU_ACTIVITY;
				}
				else
				{
					stra->STU_flag = STRATEGY_TRACK_STU_NULL;//��ʶѧ��������0����Ӧ
					stra->stu_pos = stu->move_position;
					move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//�������꣬�������һ���ƶ�����
					setZoom(stu_cam, stu->stretchingCoefficient_bigMove);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//�������꣬�����ƶ�����
				setZoom(stu_cam, stu->stretchingCoefficient_bigMove);
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
				move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);//�������꣬�������һ���ƶ�����
				setZoom(stu_cam, stu->stretchingCoefficient_bigMove);
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
			
			//if (stra->timer_stra.deltaTime > SWITCH_TIME)
			//{
			//	//�ж�����Ƿ�λ

			//	//�жϽ�ʦ״̬�Ƿ�Ϊ��д����˫����
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
			//	//�ƶ����
			//}
			return STU_ACTIVITY;
		}
		if (stra->STU_flag == STRATEGY_TRACK_STU_MOVE_SINGLE)
		{
			//�ƶ����������ȫ��
			return STU_ACTIVITY;
		}
		if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE || stra->STU_flag == STRATEGY_TRACK_STU_MOVE_SINGLE)
		{
			//�ƶ����������ȫ��
			return STU_ACTIVITY;
		}
		if (stra->STU_flag == STRATEGY_TRACK_STU_NULL)
		{
			//ѧ������û����Ϣ���ȴ������ź�
			return NULL_ACTIVITY;
		}
		return NULL_ACTIVITY;
	}
}

/*
������ʦ�Ǻ����ź�
*/
static int tch_strategy_analyze(Strategy_Result_t* stra, Tch_Result_t* tch, Strategy_CamControl_t *cam_tch)
{
	if (stra->tch_status == STRATEGY_TRACK_TCH_BLACKBOARD)
	{
		//�а��龵ͷ
		//ʼ���ƶ�����ͷ���ٽ�ʦ
		stra->tch_posIndex = tch->pos;
		stra->TCH_flag = stra->tch_status;
		preset(cam_tch, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
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
		preset(cam_tch, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
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
		preset(cam_tch, PANandTILT_CTRL_PTZ_GOTO_PRESET, stra->tch_posIndex);
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
		//��״̬�򷵻ؽ�ʦȫ��
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
�ۺ������ź����жϸ���ʲô����
*/
static int strategy_status_analyze(Strategy_Result_t* stra, Strategy_CamControl_t *tch_cam, Strategy_CamControl_t *stu_cam)//����������ͬ���źŷ�����ǰ�Ļ������
{
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
			if (stra->timer_stra.deltaTime>SWITCH_TIME)
			{
				if (getPosit(stu_cam, &stra->posit_pan, &stra->posit_tilt, 500))
				{
					if (stra->posit_pan == stra->stu_pos.x&&stra->posit_tilt == stra->stu_pos.y)
					{
						//�д�VGA��Сѧ���Լ�С��ʦ��������
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
	else//��VGA�ź�
	{
		if (stra->STU_flag != STRATEGY_TRACK_STU_NULL)//ѧ���������˶�
		{
			if (stra->TCH_flag != STRATEGY_TRACK_TCH_OUTSIDE)//ͬʱ��ʦ���������ź�
			{
				if (stra->TCH_flag == STRATEGY_TRACK_TCH_BLACKBOARD)//��ʦ�ź�Ϊ����
				{
					move(stu_cam, stra->stu_pos.x, stra->stu_pos.y, FALSE);
					stra->status = SWITCH_BLBO;
					return stra->status;
				}
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE&&stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEINVIEW)//��ʦ�ź�Ϊ��д����ѧ���ź�ҲΪ��д
				{
					//��ʱ����д
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
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_MULITY&&stra->TCH_flag  == STRATEGY_TRACK_TCH_MOVEINVIEW)//��ʦ�ź�Ϊ��д��ѧ���ź�Ϊ����վ��
				{
					stra->status = SWITCH_2_PSTU_EQU_TEA;
					return stra->status;
				}
				else if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE&&stra->TCH_flag == STRATEGY_TRACK_TCH_MOVEOUTVIEW)
				{
					//ѧ����д�ȷֽ�ʦȫ��
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
			else//��ʦ�������ź�
			{
				if (stra->STU_flag == STRATEGY_TRACK_STU_STAND_SINGLE)
				{
					//�ж��Ƿ�ʱ�������Ƿ�λ
					stra->timer_stra.finish = gettime();
					stra->timer_stra.deltaTime = stra->timer_stra.finish - stra->timer_stra.start;
					if (stra->timer_stra.deltaTime>SWITCH_TIME)
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
				if (stra->last_status == STRATEGY_TRACK_STU_STAND_MULITY)//�����һ״̬�Ƕ�Ŀ�����н�ʦȫ��
				{
					stra->status = SWITCH_PTEA;
					return stra->status;
				}
				else//����Ϊ��ʦ���½�̨����ѧ����
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
tch: Ϊ��ʦ���ٷ��صĽ����
stu: Ϊѧ�����ٷ��صĽ����
stra: Ϊ�Ѿ���ʼ���˵ľ�������
cam_tch: Ϊ���ƽ�ʦ��д��ͷ�Ŀ�������
cam_stu: Ϊ����ѧ����д��ͷ�Ŀ�������
*/
static int switch_strategy(AlgLink_ScdResult *vga,Tch_Result_t* tch, StuITRACK_OutParams_t* stu, Strategy_Result_t* stra, Strategy_CamControl_t *cam_tch, Strategy_CamControl_t *cam_stu)//����Ҫ����������Ľṹ���VGA�źŵĽṹ��
{
	if (stra->tch_que->size<FPS-1)
	{
		EnQueue(stra->tch_que, tch->status);//��������м����ʦ״̬ȥͳ��

		//�����ж�VGA�ź�
		vga_strategy_analyze(stra,vga);
		//�����Ӧѧ�����ź�
		int res = stu_strategy_analyze(stra, stu, cam_stu);

		//���VGA�źź�ѧ���ź��Լ���һ�ν�ʦ���ź����жϸ��������Ļ���
		if (stra->TCH_flag>0)
		{
			//strategy_status_analyze(stra);
		}
		else
		{
			stra->status = SWITCH_PTEA;//������ʱ���ֽ�ʦȫ��
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
			//���ж�VGA�ź�
			vga_strategy_analyze(stra,vga);
			//�ж�ѧ���Ƿ�����Ӧ��������Ͻ�ʦ����
			res_stu = stu_strategy_analyze(stra, stu, cam_stu);
			res_tch = tch_strategy_analyze(stra, tch, cam_tch);
		}
		//DeQueue(stra->tch_que, &pitem);

		//���VGA�źź�ѧ���ź��Լ���һ�ν�ʦ���ź����жϸ��������Ļ���
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
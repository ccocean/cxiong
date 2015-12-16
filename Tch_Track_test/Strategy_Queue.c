#include "../track_policy/Strategy_Queue.h"


/*����һ���ն���*/
Strategy_Queue *InitQueue()
{
	Strategy_Queue *pqueue = (Strategy_Queue *)malloc(sizeof(Strategy_Queue));
	if (pqueue != NULL)
	{
		pqueue->front = NULL;
		pqueue->rear = NULL;
		pqueue->size = 0;
	}
	return pqueue;
}

/*����һ������*/
void DestroyQueue(Strategy_Queue *pqueue)
{
	if (IsEmpty(pqueue) != 1)
		ClearQueue(pqueue);
	free(pqueue);
}

/*���һ������*/
void ClearQueue(Strategy_Queue *pqueue)
{
	while (IsEmpty(pqueue) != 1)
	{
		DeQueue(pqueue, NULL);
	}
}

/*�ж϶����Ƿ�Ϊ��*/
int IsEmpty(Strategy_Queue *pqueue)
{
	if (pqueue->front == NULL&&pqueue->rear == NULL&&pqueue->size == 0)
		return 1;
	else
		return 0;
}

/*���ض��д�С*/
int GetSize(Strategy_Queue *pqueue)
{
	return pqueue->size;
}

/*���ض�ͷԪ��*/
PNode GetFront(Strategy_Queue *pqueue, Item *pitem)
{
	if (IsEmpty(pqueue) != 1 && pitem != NULL)
	{
		*pitem = pqueue->front->data;
	}
	return pqueue->front;
}

/*���ض�βԪ��*/

PNode GetRear(Strategy_Queue *pqueue, Item *pitem)
{
	if (IsEmpty(pqueue) != 1 && pitem != NULL)
	{
		*pitem = pqueue->rear->data;
	}
	return pqueue->rear;
}

/*����Ԫ�����*/
PNode EnQueue(Strategy_Queue *pqueue, Item item)
{
	PNode pnode = (PNode)malloc(sizeof(Node));
	if (pnode != NULL)
	{
		pnode->data = item;
		pnode->next = NULL;

		if (IsEmpty(pqueue))
		{
			pqueue->front = pnode;
		}
		else
		{
			pqueue->rear->next = pnode;
		}
		pqueue->rear = pnode;
		pqueue->size++;
	}
	return pnode;
}

/*��ͷԪ�س���*/
PNode DeQueue(Strategy_Queue *pqueue, Item *pitem)
{
	PNode pnode = pqueue->front;
	if (IsEmpty(pqueue) != 1 && pnode != NULL)
	{
		if (pitem != NULL)
			*pitem = pnode->data;
		pqueue->size--;
		pqueue->front = pnode->next;
		free(pnode);
		if (pqueue->size == 0)
			pqueue->rear = NULL;
	}
	return pqueue->front;
}

/*�������в��Ը����������visit����*/
void QueueTraverse(Strategy_Queue *pqueue)
{
	PNode pnode = pqueue->front;
	int i = pqueue->size;
	while (i--)
	{
		//visit(pnode->data);
		//printf(pnode->data);
		pnode = pnode->next;
	}

}
#ifndef Queue_H  
#define Queue_H  

#ifndef WIN32

    #include<stdlib.h>
#else
    #include<malloc.h>  
    #include<stdio.h>  
#endif


typedef int Item;
typedef struct node * PNode;
typedef struct node
{
	Item data;
	PNode next;
}Node;

typedef struct
{
	PNode front;
	PNode rear;
	int size;
}Strategy_Queue;

/*����һ���ն���*/
Strategy_Queue *InitQueue();

/*����һ������*/
void DestroyQueue(Strategy_Queue *pqueue);

/*���һ������*/
void ClearQueue(Strategy_Queue *pqueue);

/*�ж϶����Ƿ�Ϊ��*/
int IsEmpty(Strategy_Queue *pqueue);

/*���ض��д�С*/
int GetSize(Strategy_Queue *pqueue);

/*���ض�ͷԪ��*/
PNode GetFront(Strategy_Queue *pqueue, Item *pitem);

/*���ض�βԪ��*/
PNode GetRear(Strategy_Queue *pqueue, Item *pitem);

/*����Ԫ�����*/
PNode EnQueue(Strategy_Queue *pqueue, Item item);

/*��ͷԪ�س���*/
PNode DeQueue(Strategy_Queue *pqueue, Item *pitem);

/*�������в��Ը����������visit����*/
void QueueTraverse(Strategy_Queue *pqueue);

#endif
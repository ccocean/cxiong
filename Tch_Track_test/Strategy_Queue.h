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

/*构造一个空队列*/
Strategy_Queue *InitQueue();

/*销毁一个队列*/
void DestroyQueue(Strategy_Queue *pqueue);

/*清空一个队列*/
void ClearQueue(Strategy_Queue *pqueue);

/*判断队列是否为空*/
int IsEmpty(Strategy_Queue *pqueue);

/*返回队列大小*/
int GetSize(Strategy_Queue *pqueue);

/*返回队头元素*/
PNode GetFront(Strategy_Queue *pqueue, Item *pitem);

/*返回队尾元素*/
PNode GetRear(Strategy_Queue *pqueue, Item *pitem);

/*将新元素入队*/
PNode EnQueue(Strategy_Queue *pqueue, Item item);

/*队头元素出队*/
PNode DeQueue(Strategy_Queue *pqueue, Item *pitem);

/*遍历队列并对各数据项调用visit函数*/
void QueueTraverse(Strategy_Queue *pqueue);

#endif
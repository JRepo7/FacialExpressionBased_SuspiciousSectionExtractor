#include "CircularQueue.h"

void CircularQueue::enqueue(bool data)
{
	int pos;

	if (rear == capacity - 1)
	{
		pos = rear;
		rear = 0;
		init = false;
	}
	else
	{
		pos = rear++;
		if (init == false)
		{
			if (front == capacity - 1)
			{
				front = 0;
			}
			front++;
		}
	}

	node[pos].data = data;
}

//bool CircularQueue::dequeue()
//{
//	int pos = front;
//	if (front == capacity - 1)
//	{
//		front = 0;
//	}
//	else
//	{
//		front++;
//	}
//
//	return node[pos].data;
//}
bool CircularQueue::getState(int pos)
{
	return node[pos].data;
}
int CircularQueue::getSize()
{
	if (front <= rear)
	{
		return rear - front;
	}
	else
	{
		return capacity - front + rear;
	}
}
bool CircularQueue::isEmpty()
{
	return front == rear;
}
bool CircularQueue::isFull()
{
	if (front <= rear)
	{
		return (rear - front) == (capacity - 1);
	}
	else
	{
		return (rear + 1) == front;
	}
}
int CircularQueue::getRear()
{
	return rear;
}
int CircularQueue::getFront()
{
	return front;
}
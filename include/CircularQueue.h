#include <iostream>
using namespace std;

typedef struct Node
{
	bool data;
}Node;

class CircularQueue
{
private:
	Node* node;
	int capacity;
	int front;
	int rear;

public:
	CircularQueue(int capacity)
	{
		this->capacity = capacity + 1;
		node = new Node[this->capacity];
		front = rear = 0;
		init = true;
	}
	~CircularQueue()
	{
		delete[]node;
	}
	void enqueue(bool data);
	//bool dequeue(void);
	int getSize(void);
	bool isEmpty(void);
	bool isFull(void);
	int getRear();
	int getFront();
	bool getState(int pos);
	bool init;
};
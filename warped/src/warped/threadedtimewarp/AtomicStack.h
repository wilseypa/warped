#ifndef ATOMICSTACK_H_
#define ATOMICSTACK_H_

// See copyright notice in file Copyright in the root directory of this archive.

//******************************THERE IS A KNOWN BUG IN THIS CODE*********************************
//This is a naive implementation of a stack, its errors rarely occur but they do exist.
//http://en.wikipedia.org/wiki/ABA_problem
//************************************************************************************************

template<class element> class node_t
{
public:
	node_t(element *data) : data(data), prev(0) {}
	element *data;
	node_t<element> *prev;
};

template<class element> class AtomicStack
{
private:
	node_t<element> *top;

public:
	 //prevHead will start with a new blank node
	 //so we can just blindly delete it each time
	 //a new head is set and not have to check for NULL
	AtomicStack() : top(0) {}
	~AtomicStack()
	{
		while(pop()!=0);
	};

	void push(element *data)
	{
		ASSERT(data!=0);
		//A local copy of the top of the stack
		node_t<element> *topNode = 0;
		//Create a node to hold the new element
		node_t<element> *newNode = new node_t<element>(data);
		// Loop until successful
		while(true)
		{
			//Make a local copy of top
			topNode = top;
			//Have this newNodes prev pointer point to what
			//we believe to be the top of the stack
			//(This could be wrong if another thread already updated top,
			// this is why it may take a couple trys)
			newNode->prev = topNode;
			//Attempt to update the top to include our new node,
			//assuming our local copy is still the top
			if (__sync_bool_compare_and_swap(&top, topNode,  newNode))
			{
				//We successfully updated the top of the stack to be our new node
				//because our new node->prev is already configured we are done
				break;
			}
			//We failed so lets try again
		}//End While
	}

	element *pop()
	{
		//Our return data
		element *returnVal = 0;
		//A local copy of the top of the stack
		node_t<element> *topNode = 0;
		while(true)
		{
			//Get a local copy of the top of the stack
			topNode = top;
			//if the stack is not empty
			if (topNode!=0)
			{
				//Potential Error here though unlikely:
				//A segfault will happen if some other thread has already deleted the node pointed to by topNode
				if (__sync_bool_compare_and_swap(&top, topNode, topNode->prev))
				{
					//We own this local copy so grab the data and delete the node
					returnVal = topNode->data;
					delete topNode;
					break;
				}
				//Else someone modified top, we should try again
			}
			else
			{
				//Stack is empty
				break;
			}
		}
		return returnVal;
	}

	element *peekNext()
	{
		//The head node is a dummy node so we want the node following the head node
		if (top!=0)
			//Possible problem here if some other thread has already deleted top
			//between the time we checked and now
			return top->data;
		else
			return 0;
	}

};

#endif /* ATOMICSTACK_H_ */

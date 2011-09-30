#ifndef ATOMICQUEUE_H_
#define ATOMICQUEUE_H_

// See copyright notice in file Copyright in the root directory of this archive.

//******************************THERE IS A KNOWN BUG IN THIS CODE*********************************
// It is possible for 2 dequeues to happen simultaneously causing headNext->data to segfault
// because of the "delete queueHead" line happened before headNext was updated
// It is rather rare because it is a matter of a couple assembly instructions being executed out of order
//************************************************************************************************

//Atomic Non Blocking Queue data structure found described in the following paper
//M. M. Michael and M. L. Scott. Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue
//Algorithms. In Proceedings of the Fifteenth ACM Symposium on Principles of Distributed Computing, pages
//267–275, Philadelphia, PA, May 1996.

template<class element> class q_node_t
{
public:
	q_node_t(element data) : data(data), next(0) {}
	element data;
	q_node_t<element> *next;
};

template<class element> class AtomicQueue
{
private:
	q_node_t<element> *head;
	q_node_t<element> *tail;
public:
	 //prevHead will start with a new blank node
	 //so we can just blindly delete it each time
	 //a new head is set and not have to check for NULL
	AtomicQueue() : head(new q_node_t<element>(0)), tail(head) {}
	~AtomicQueue()
	{
		while(dequeue()!=0);
		delete head;
	};

	void enqueue(element e)
	{
		ASSERT(e!=0);
		// Loop until successful
		q_node_t<element> *queueTail = 0;
		q_node_t<element> *tailNext = 0;
		while(true)
		{
			queueTail = tail;
			tailNext = tail->next;
			//Create a node to hold the new element
			//	Auto sets its next pointer to NULL and count=0
			q_node_t<element> *newNode = new q_node_t<element>(e);
			if (__sync_bool_compare_and_swap(&tail->next, 0,  newNode))
			{
				//Be a good citizen and update the tail from the second to last node to the last
				__sync_bool_compare_and_swap(&tail, queueTail,  tail->next);
				break;
			}
			else
			{
				//Help some other thread to put the tail in the right spot
				__sync_bool_compare_and_swap(&tail, queueTail,  tail->next);
			}
		}//End While
	}

	element dequeue()
	{
		element returnVal = 0;
		q_node_t<element> *queueHead;
		q_node_t<element> *queueTail;
		q_node_t<element> *headNext;
		//q_node_t<element> *
		while (true)
		{
			queueHead = head;
			queueTail = tail;
			headNext = head->next;
			if (queueHead == head)
			{
				if (queueHead == queueTail)
				{
					if (headNext==0)
					{
						//Queue must be empty so return NULL
						returnVal = 0;
						break;
					}
					else
					{
						//Help some other thread to put the tail in the right spot
						__sync_bool_compare_and_swap(&tail, queueTail,  headNext);
					}
				}
				else
				{
					//Get our data which is in the new dummy node's data field
					returnVal = headNext->data;
					//Now try to remove the node so no one else gets it
					if (__sync_bool_compare_and_swap(&head, queueHead,  headNext) )
					{
						//Wait here until all threads messing with the tail are finished
						break;
					}
					//If your here you failed to get the node so you need to try again
				}//End else *queueHead != *queueTail
			}//End if head is really the head
		}// End While
		if (returnVal!=0) {
			//Delete the old dummy node
			delete queueHead;
		}
		return returnVal;
	}

	element peekNext()
	{
		//The head node is a dummy node so we want the node following the head node
		if (head->next!=0)
			return head->next->data;
		else
			return 0;
	}

};

#endif /* ATOMICQUEUE_H_ */

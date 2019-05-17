/*********A very basic Work Queue class***************/

#ifndef __WORKQUEUE_H__
#define __WORKQUEUE_H__

#include <queue>
#include <mutex>


template<typename T>
class CWorkQueue
{
public:
	CWorkQueue() {}

	//Insert an item at the back of the queue and signal any thread that might be waiting for the q to be populated
	void push(const T& item)
	{
		std::lock_guard<std::mutex> _lock(m_WorkQMutex);
		workQ.push(item);

	}

	//Attempt to get a workitem from the queue
	//If the Q is empty just return false; this is important for the way in which we have written the DoWork() function in the ThreadPool class
	//Because of the lock on the mutex the operation of retrieving an item from the front of the queue and popping it off becomes one atomic operation.
	bool pop(T& _workItem)
	{
		//If the queue is empty return false
		std::lock_guard<std::mutex> _lock(m_WorkQMutex);
		if (workQ.empty())
		{
			return false;
		}
		_workItem = workQ.front();
		workQ.pop();
		return true;
	}

	//Checking if the queue is empty or not
	bool empty() const
	{
		std::lock_guard<std::mutex> _lock(m_WorkQMutex);
		return workQ.empty();
	}

private:
	std::queue<T> workQ;
	mutable std::mutex m_WorkQMutex;

};
#endif
#ifndef LOCKEDQUEUE_H_
#define LOCKEDQUEUE_H_


#include <pthread.h>
#include <deque>
#include <string>

using std::deque;

template<class element> class LockedQueue {
private:
    deque<element> myQueue;

    pthread_spinlock_t spinlock;
    pthread_mutex_t mutex;

public:
    LockedQueue() {
        pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
        pthread_mutex_init(&mutex, NULL);
    }
    ~LockedQueue() {};

    void enqueue(element e, const std::string syncMech) {
        const std::string syncMechanism = "ATOMICLOCK";  /* MUTEX code section is not needed currently.
                                 * Code section not deleted keeping future use
                                 * in mind */

        if (syncMechanism == "ATOMICLOCK") {
            pthread_spin_lock(&spinlock);
        } else {
            pthread_mutex_lock(&mutex);
        }

        myQueue.push_back(e);

        if (syncMechanism == "ATOMICLOCK") {
            pthread_spin_unlock(&spinlock);
        } else {
            pthread_mutex_unlock(&mutex);
        }
    }

    element dequeue(const std::string syncMech) {
        const std::string syncMechanism = "ATOMICLOCK";  /* MUTEX code section is not needed currently.
                                 * Code section not deleted keeping future use
                                 * in mind */

        element returnVal = 0;
        if (!myQueue.empty()) {
            if (syncMechanism == "ATOMICLOCK") {
                pthread_spin_lock(&spinlock);
            } else {
                pthread_mutex_lock(&mutex);
            }

            if (!myQueue.empty()) {
                returnVal = myQueue.front();
                myQueue.pop_front();
            }

            if (syncMechanism == "ATOMICLOCK") {
                pthread_spin_unlock(&spinlock);
            } else {
                pthread_mutex_unlock(&mutex);
            }
        }
        return returnVal;
    }

    element peekNext(const std::string syncMech) {
        const std::string syncMechanism = "ATOMICLOCK";  /* MUTEX code section is not needed currently.
                                 * Code section not deleted keeping future use
                                 * in mind */

        element returnVal = 0;

        if (syncMechanism == "ATOMICLOCK") {
            pthread_spin_lock(&spinlock);
        } else {
            pthread_mutex_lock(&mutex);
        }

        if (!myQueue.empty()) {
            returnVal = myQueue.front();
        }

        if (syncMechanism == "ATOMICLOCK") {
            pthread_spin_unlock(&spinlock);
        } else {
            pthread_mutex_unlock(&mutex);
        }

        return returnVal;
    }
};

#endif

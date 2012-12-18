// See copyright notice in file Copyright in the root directory of this archive.

#ifndef LockState_H_
#define LockState_H_

#include <iostream>
#include <pthread.h>
using namespace std;

static const int NOONE = -1;
class LockState {
public:
	LockState() {
		lockOwner = NOONE;
		pthread_mutex_init(&mutexLock, NULL);
	}
	~LockState() {
	}
	bool releaseLock(const unsigned int &threadNumber, const string syncMechanism) {
		if(syncMechanism == "ATOMICLOCK") {
			//If Currently Working and we can set it to Available then return true else return false;
			return __sync_bool_compare_and_swap(&lockOwner, threadNumber, NOONE);
		}
		else if(syncMechanism == "MUTEX") {
			if( !pthread_mutex_lock(&mutexLock) ) {
				if(threadNumber == lockOwner) {
					lockOwner = NOONE;
					pthread_mutex_unlock(&mutexLock);
					return true;
				} else {
					pthread_mutex_unlock(&mutexLock);
					return false;
				}
			} else {
				cout << "mutex_err_releaseLock" << endl;
				return false;
			}
		}
		else {
			cout << "Invalid sync mechanism" << endl;
			return false;
		}
	}
	bool setLock(const unsigned int &threadNumber, const string syncMechanism) {
		if(syncMechanism == "ATOMICLOCK") {
			//If Available and we can set it to Working then return true else return false;
			return __sync_bool_compare_and_swap(&lockOwner, NOONE, threadNumber);
		}
		else if(syncMechanism == "MUTEX") {
			if( !pthread_mutex_lock(&mutexLock) ) {
				if( lockOwner == NOONE ) {
					lockOwner = threadNumber;
					pthread_mutex_unlock(&mutexLock);
					return true;
				} else {
					pthread_mutex_unlock(&mutexLock);
					return false;
				}
			} else {
				cout << "mutex_err_setLock" << endl;
				return false;
			}
		}
		else {
			cout << "Invalid sync mechanism" << endl;
			return false;
		}
	}
	const bool hasLock(const unsigned int &threadNumber, const string syncMechanism) const {
		if(syncMechanism == "ATOMICLOCK") {
			return (threadNumber == lockOwner);
		}
		else if(syncMechanism == "MUTEX") {
			return (threadNumber == lockOwner);
		}
		else {
			cout << "Invalid sync mechanism" << endl;
			return false;
		}
	}
	const void showStatus() {
		cout << "Locked By: " << lockOwner << endl;
	}
	bool isLocked() {
		if (this->lockOwner == NOONE)
			return false;
		else
			return true;
	}
	int whoHasLock() {
		return lockOwner;
	}
private:
	int lockOwner;
	pthread_mutex_t mutexLock;
};

#endif /* LockState_H_ */

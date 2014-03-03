
#ifndef LockState_H_
#define LockState_H_

#include <iostream>
#include <string>
#include <pthread.h>

static const int NOONE = -1;
class LockState {
public:
    LockState() {
        lockOwner = NOONE;
        pthread_mutex_init(&mutexLock, NULL);
    }
    ~LockState() {
        pthread_mutex_destroy(&mutexLock);
    }
    bool releaseLock(const unsigned int& threadNumber, const std::string syncMechanism) {
        if (syncMechanism == "AtomicLock") {
            //If Currently Working and we can set it to Available then return true else return false;
            return __sync_bool_compare_and_swap(&lockOwner, threadNumber, NOONE);
        } else if (syncMechanism == "Mutex") {
            if (!pthread_mutex_lock(&mutexLock)) {
                if (threadNumber == (unsigned int) lockOwner) {
                    lockOwner = NOONE;
                    pthread_mutex_unlock(&mutexLock);
                    return true;
                } else {
                    pthread_mutex_unlock(&mutexLock);
                    return false;
                }
            } else {
                std::cout << "mutex_err_releaseLock" << std::endl;
                return false;
            }
        } else {
            std::cout << "Invalid sync mechanism" << std::endl;
            return false;
        }
    }
    void setLock(const unsigned int& threadNumber, const std::string syncMechanism) {
        if (syncMechanism == "AtomicLock") {
            //If Available and we can set it to Working then return true else return false;
            while (!__sync_bool_compare_and_swap(&lockOwner, NOONE, threadNumber));
        } else if (syncMechanism == "Mutex") {
            bool locked = false;
            while (!locked) {
                if (!pthread_mutex_lock(&mutexLock)) {
                    if (lockOwner == NOONE) {
                        lockOwner = threadNumber;
                        pthread_mutex_unlock(&mutexLock);
                        locked = true;
                    } else if(lockOwner == (int) threadNumber) {
                        pthread_mutex_unlock(&mutexLock);
                        locked = true;
                    } else {
                        pthread_mutex_unlock(&mutexLock);
                    }
                } else {
                    std::cout << "mutex_err_setLock" << std::endl;
                    return;
                }
            }
        } else {
            std::cerr << "Invalid sync mechanism" << std::endl;
            exit(-1);
        }
    }
    bool hasLock(const unsigned int& threadNumber, const std::string syncMechanism) const {
        if (syncMechanism == "AtomicLock") {
            return (threadNumber == (unsigned int) lockOwner);
        } else if (syncMechanism == "Mutex") {
            return (threadNumber == (unsigned int) lockOwner);
        } else {
            std::cout << "Invalid sync mechanism" << std::endl;
            return false;
        }
    }
    void showStatus() {
        std::cout << "Locked By: " << lockOwner << std::endl;
    }
    bool isLocked() {
        if (this->lockOwner == NOONE)
        { return false; }
        else
        { return true; }
    }
    int whoHasLock() {
        return lockOwner;
    }
private:
    int lockOwner;
    pthread_mutex_t mutexLock;
};

#endif /* LockState_H_ */

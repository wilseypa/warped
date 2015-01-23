
#ifndef LockState_H_
#define LockState_H_

#include <iostream>
#include <string>
#include <pthread.h>
#include "tsx.h"

#define NOONE 0xFFFF

class LockState {
public:
    LockState() {
        paddedLockOwner.lockOwner = NOONE;
        pthread_mutex_init(&mutexLock, NULL);
    }
    ~LockState() {
        pthread_mutex_destroy(&mutexLock);
    }
    bool releaseLock(const unsigned int& threadNumber, const std::string syncMechanism) {
        if (syncMechanism == "AtomicLock" || syncMechanism == "HleAtomicLock") {
            //If Currently Working and we can set it to Available then return true else return false;
            return __sync_bool_compare_and_swap(&paddedLockOwner.lockOwner, threadNumber, NOONE);
        } else if (syncMechanism == "Mutex") {
            if (!pthread_mutex_lock(&mutexLock)) {
                if (threadNumber == (unsigned int) paddedLockOwner.lockOwner) {
                    paddedLockOwner.lockOwner = NOONE;
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
        if (syncMechanism == "AtomicLock" || syncMechanism == "HleAtomicLock") {
            //If Available and we can set it to Working then return true else return false;
            if( paddedLockOwner.lockOwner == threadNumber ) return;
            while (!__sync_bool_compare_and_swap(&paddedLockOwner.lockOwner, NOONE, threadNumber));
        } else if (syncMechanism == "Mutex") {
            bool locked = false;
            while (!locked) {
                if (!pthread_mutex_lock(&mutexLock)) {
                    if (paddedLockOwner.lockOwner == NOONE) {
                        paddedLockOwner.lockOwner = threadNumber;
                        pthread_mutex_unlock(&mutexLock);
                        locked = true;
                    } else if(paddedLockOwner.lockOwner == (int) threadNumber) {
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
    void releaseHleLock(const unsigned int& threadNumber) {
        while (!_xrelease(&paddedLockOwner.lockOwner, &threadNumber));
    }
    void setHleLock(const unsigned int& threadNumber) {
        if( paddedLockOwner.lockOwner == threadNumber ) return;
        while(!_xacquire(&paddedLockOwner.lockOwner, &threadNumber));
    }
    bool hasLock(const unsigned int& threadNumber, const std::string syncMechanism) const {
        if (syncMechanism == "HleAtomicLock") {
            return (threadNumber == (unsigned int) paddedLockOwner.lockOwner);
        } else if (syncMechanism == "AtomicLock") {
            return (threadNumber == (unsigned int) paddedLockOwner.lockOwner);
        } else if (syncMechanism == "Mutex") {
            return (threadNumber == (unsigned int) paddedLockOwner.lockOwner);
        } else {
            std::cout << "Invalid sync mechanism" << std::endl;
            return false;
        }
    }
    void showStatus() {
        std::cout << "Locked By: " << paddedLockOwner.lockOwner << std::endl;
    }
    bool isLocked() {
        if (this->paddedLockOwner.lockOwner == NOONE)
        { return false; }
        else
        { return true; }
    }
    int whoHasLock() {
        return paddedLockOwner.lockOwner;
    }
private:
    struct {
        int lockOwner;
        char padding[60];
    } paddedLockOwner __attribute__((aligned(L1DSZ)));
    pthread_mutex_t mutexLock __attribute__((aligned(L1DSZ)));

} __attribute__((aligned(L1DSZ)));

#endif /* LockState_H_ */

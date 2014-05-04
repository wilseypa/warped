
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

        tsxRtmRetries = TSXRTM_RETRIES;

        //rtm stats
        tsxCommits = 0;
        tsxAborts = 0;
        for (int i = 0; i < 4; i++) {
            tsxAbrtType[i] = 0;
        }
    }
    ~LockState() {
        pthread_mutex_destroy(&mutexLock);
    }
    bool releaseLock(const unsigned int& threadNumber, const std::string syncMechanism) {
#if USETSX_RTM
        if (paddedLockOwner.lockOwner == NOONE) {
            _xend();
            tsxCommits++;
            return true;
        }
#endif
        if (syncMechanism == "HleAtomicLock") {
            if (_xtest()) { tsxCommits++; }
            else { tsxAborts++; }
            while (!_xrelease(&paddedLockOwner.lockOwner, &threadNumber));
            return true;
        } else if (syncMechanism == "AtomicLock") {
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
#if USETSX_RTM
        unsigned status;
        int retries = 0;

        if (tsxCommits == 0 && tsxAborts > TSXRTM_RETRIES * 100) { }
        else if (tsxRtmRetries > 1 && tsxAborts > tsxCommits << 1) {
            tsxRtmRetries--;
        } else {
            do {
                status = _xbegin();
                if (status == _XBEGIN_STARTED) {
                    if (paddedLockOwner.lockOwner == NOONE) {
                       return;
                    }
                    _xabort(_ABORT_LOCK_BUSY);
                }
                ABORT_COUNT(_XA_RETRY, status);
                ABORT_COUNT(_XA_EXPLICIT, status);
                ABORT_COUNT(_XA_CONFLICT, status);
                ABORT_COUNT(_XA_CAPACITY, status);
                if (!(status & _XABORT_RETRY) ||
                    ((status & _XABORT_EXPLICIT) && _XABORT_CODE(status) != _ABORT_LOCK_BUSY))
                {
                    break;
                } else if ((status & _XABORT_EXPLICIT) && _XABORT_CODE(status) == _ABORT_LOCK_BUSY) {
                    while (isLocked());
                } else if (status & _XABORT_CONFLICT) {
                    _mm_pause();
                }
            } while (retries++ < tsxRtmRetries);
            tsxAborts++;
        }
#endif
        if (syncMechanism == "HleAtomicLock") {
            if( paddedLockOwner.lockOwner == threadNumber ) return;
            while(!_xacquire(&paddedLockOwner.lockOwner, &threadNumber));
        } else if (syncMechanism == "AtomicLock") {
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
    void reportTSXstats() {
        std::cout << "Total commits: " << tsxCommits << std::endl;
        std::cout << "Total aborts: " << tsxAborts << std::endl;
        std::cout << "\t_XA_RETRY: " << tsxAbrtType[_XA_RETRY] << std::endl;
        std::cout << "\t_XA_EXPLICIT: " << tsxAbrtType[_XA_EXPLICIT] << std::endl;
        std::cout << "\t_XA_CONFLICT: " << tsxAbrtType[_XA_CONFLICT] << std::endl;
        std::cout << "\t_XA_CAPACITY: " << tsxAbrtType[_XA_CAPACITY] << std::endl;
    }
private:
    struct {
        int lockOwner;
        char padding[60];
    } paddedLockOwner __attribute__((aligned(L1DSZ)));
    pthread_mutex_t mutexLock;

    //rtm retries
    int tsxRtmRetries;

    //rtm stats
    long tsxCommits;
    long tsxAborts;
    unsigned tsxAbrtType[4];
} __attribute__((aligned(L1DSZ)));

#endif /* LockState_H_ */

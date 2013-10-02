#ifndef SCHEDULING_DATA_H
#define SCHEDULING_DATA_H


#include "warped.h"
#include "ReceiverQueueContainer.h"

class SchedulingData {
public:

    SchedulingData() : scheduleList(new vector<ScheduleListContainer*>),
        popHeapFlag(false),
        makeHeapFlag(false) {
    };

    ~SchedulingData() {
        vector<ScheduleListContainer*>::iterator begin_iter = scheduleList->begin();
        vector<ScheduleListContainer*>::iterator end_iter = scheduleList->end();
        while (begin_iter != end_iter) {
            ScheduleListContainer* ptr = *(begin_iter);
            begin_iter++;
            delete ptr;
        }
        delete scheduleList;
    }

    void PopHeapFlag(bool value) { popHeapFlag = value;}
    bool PopHeapFlag() { return popHeapFlag;}

    void MakeHeapFlag(bool value) { makeHeapFlag = value;}
    bool MakeHeapFlag() { return makeHeapFlag;}

    vector<ScheduleListContainer*>* scheduleList;

private:

    bool popHeapFlag;
    bool makeHeapFlag;
};

#endif


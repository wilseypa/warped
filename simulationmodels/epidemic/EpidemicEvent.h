#ifndef EPIDEMIC_EVENT_H
#define EPIDEMIC_EVENT_H

#include "DefaultEvent.h"
#include "IntVTime.h"
#include "SerializedInstance.h"
#include "Person.h"

#define DISEASE "disease"
#define DIFFUSION "diffusion"

class EpidemicEvent: public DefaultEvent {

public:

    /* Constructor */
    EpidemicEvent(  const VTime &initSendTime,
                    const VTime &initRecvTime,
                    SimulationObject *initSender,
                    SimulationObject *initReceiver,
                    Person *person,
                    string diseaseOrDiffusion ) : 
            DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver ),
            pid(0), susceptibility(0.0), vaccinationStatus(""), 
            infectionState(""), timeSpentInCurrState(0),
            diseaseOrDiffusion(diseaseOrDiffusion) {

        if( person ) {
            pid = person->pid;
            susceptibility = person->susceptibility;
            vaccinationStatus = person->vaccinationStatus;
            infectionState = person->infectionState;
            timeSpentInCurrState = initRecvTime.getApproximateIntTime() - person->lastStateChangeTime;
        }
    }

    /* Destructor */
    ~EpidemicEvent() {}

    static Serializable* deserialize( SerializedInstance *instance ) {

        VTime *sendTime = dynamic_cast<VTime *>(instance->getSerializable());
        VTime *receiveTime = dynamic_cast<VTime *>(instance->getSerializable());
        unsigned int senderSimManID = instance->getUnsigned();
        unsigned int senderSimObjID = instance->getUnsigned();
        unsigned int receiverSimManID = instance->getUnsigned();
        unsigned int receiverSimObjID = instance->getUnsigned();
        unsigned int eventId          = instance->getUnsigned();

        ObjectID sender(senderSimObjID, senderSimManID);
        ObjectID receiver(receiverSimObjID, receiverSimManID);

        EpidemicEvent *event = new EpidemicEvent(*sendTime, *receiveTime, sender, receiver, eventId);   

        event->setPID ( instance->getUnsigned() );
        event->setSusceptibility ( instance->getDouble() );
        event->setVaccinationStatus (instance->getString() );
        event->setInfectionState ( instance->getString() );
        event->setTimeSpentInCurrState ( instance->getInt() );
        event->setDiseaseOrDiffusion ( instance->getString() );

        delete sendTime;
        delete receiveTime;

        return event;
    }

    void serialize( SerializedInstance *addTo ) const {
        Event::serialize(addTo);
        addTo->addUnsigned(pid);
        addTo->addDouble(susceptibility);
        addTo->addString(vaccinationStatus);
        addTo->addString(infectionState);
        addTo->addInt( timeSpentInCurrState );
        addTo->addString(diseaseOrDiffusion);
    }

    bool eventCompare( const Event* event ) {

        EpidemicEvent *thisEvent = (EpidemicEvent *) event;

        return (    compareEvents ( this, event ) && 
                    ( pid == thisEvent->getPID() ) &&
                    ( susceptibility == thisEvent->getSusceptibility() ) &&
                    ( vaccinationStatus == thisEvent->getVaccinationStatus() ) &&
                    ( infectionState == thisEvent->getInfectionState() ) &&
                    ( timeSpentInCurrState == thisEvent->getTimeSpentInCurrState() ) &&
                    ( diseaseOrDiffusion == thisEvent->getDiseaseOrDiffusion() )   );
    }

    static const string &getEpidemicEventDataType(){
        static string epidemicEventDataType = "EpidemicEvent";
        return epidemicEventDataType;
    }

    const string &getDataType() const { return getEpidemicEventDataType(); }

    unsigned int getEventSize() const { return sizeof(EpidemicEvent); }

    void setPID( unsigned int personId ) { pid = personId; }

    unsigned int getPID() { return pid; }

    void setSusceptibility( double susValue ) { susceptibility = susValue; }

    double getSusceptibility() { return susceptibility; }

    void setVaccinationStatus( const string statusValue ) { vaccinationStatus = statusValue; }

    const string &getVaccinationStatus() { return vaccinationStatus; }

    void setInfectionState( const string stateValue ) { infectionState = stateValue; }

    const string &getInfectionState() { return infectionState; }

    void setTimeSpentInCurrState( int timeSpent ) { 
        timeSpentInCurrState = timeSpent;
    }

    int getTimeSpentInCurrState() { return timeSpentInCurrState; }

    void setDiseaseOrDiffusion( const string eventTypeValue ) { diseaseOrDiffusion = eventTypeValue; }

    const string &getDiseaseOrDiffusion() { return diseaseOrDiffusion; }

private:

    EpidemicEvent(  const VTime &initSendTime,
                    const VTime &initRecvTime,
                    const ObjectID &initSender,
                    const ObjectID &initReceiver,
                    const unsigned int eventId  ) :
            DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver, eventId ),
            pid(0), susceptibility(0), vaccinationStatus(""), 
            infectionState(""), timeSpentInCurrState(0), 
            diseaseOrDiffusion("") {
    }

    /* Person ID */
    unsigned int pid;

    /* Person's susceptibility */
    double susceptibility;

    /* Vaccination status */
    string vaccinationStatus;

    /* Person's infection state */
    string infectionState;

    /* Time spent in the current state */
    int timeSpentInCurrState;

    /* Distinguish between "disease" and "diffusion" event */
    string diseaseOrDiffusion;
};

#endif


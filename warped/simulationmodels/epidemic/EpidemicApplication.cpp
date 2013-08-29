#include "EpidemicApplication.h"
#include "LocationObject.h"
#include "EpidemicPartitioner.h"
#include "Person.h"
#include "tinyxml2.h"
#include "warped/PartitionInfo.h"
#include "warped/RoundRobinPartitioner.h"
#include "warped/DeserializerManager.h"

#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#define INIT_VTIME 0

using namespace std;
using namespace tinyxml2;


EpidemicApplication::EpidemicApplication(string inputFileName, int numObjects):
    inputFileName(inputFileName),
    numObjects(numObjects) { }

int EpidemicApplication::getNumberOfSimulationObjects(int mgrId) const {
    return numObjects;
}

const PartitionInfo *EpidemicApplication::getPartitionInfo(
                    unsigned int numberOfProcessorsAvailable ) {

    EpidemicPartitioner *myPartitioner = new EpidemicPartitioner();
    int numRegions = 0, numLocations = 0, numPersons = 0;
    unsigned int pid = 0, travelTimeToHub = 0, latentDwellTime = 0, 
            incubatingDwellTime = 0, infectiousDwellTime = 0, 
            asymptDwellTime = 0, locStateRefreshInterval = 0,
            locDiffusionTrigInterval = 0;
    double susceptibility = 0.0;
    float transmissibility = 0.0, probULU = 0.0, probULV = 0.0, 
            probURV = 0.0, probUIV = 0.0, probUIU = 0.0, 
            latentInfectivity = 0.0, incubatingInfectivity = 0.0, 
            infectiousInfectivity = 0.0, asymptInfectivity = 0.0;
    string locationName = "", infectionState = "", vaccinationStatus = "", 
            regionName = "";

    Person *person;
    vector <SimulationObject*> *locObjs;
    vector <Person *> *personVec;

    vector <SimulationObject *> simulationObjVec;
    map <string, unsigned int> travelMap;
    
    /* open the XML file */
    XMLDocument EpidemicConfig;
    int errorID = EpidemicConfig.LoadFile(inputFileName.c_str());
    if(errorID){
        cerr<<"ERROR : return code from XMLDocument::LoadFile() is "<<errorID<<"."<<endl;
        abort();
    }

    XMLElement* diseaseElement=NULL,*numOfRegions=NULL,*region=NULL,*location=NULL,*people=NULL;

    diseaseElement =EpidemicConfig.FirstChildElement()->FirstChildElement("disease");
    diseaseElement->FirstChildElement("transmissibility")->QueryFloatText(&transmissibility);
        
    /* Refer to README for more details */
    diseaseElement->FirstChildElement("latent_dwell_time")->QueryUnsignedText(&latentDwellTime);
    diseaseElement->FirstChildElement("latent_infectivity")->QueryFloatText(&latentInfectivity);
    diseaseElement->FirstChildElement("incubating_dwell_time")->QueryUnsignedText(&incubatingDwellTime );
    diseaseElement->FirstChildElement("incubating_infectivity")->QueryFloatText(&incubatingInfectivity);
    diseaseElement->FirstChildElement("infectious_dwell_time")->QueryUnsignedText(&infectiousDwellTime);
    diseaseElement->FirstChildElement("infectious_infectivity")->QueryFloatText(&infectiousInfectivity);
    diseaseElement->FirstChildElement("asympt_dwell_time")->QueryUnsignedText(&asymptDwellTime);
    diseaseElement->FirstChildElement("asympt_infectivity")->QueryFloatText(&asymptInfectivity);
    diseaseElement->FirstChildElement("prob_ul_u")->QueryFloatText(&probULU);
    diseaseElement->FirstChildElement("prob_ul_v")->QueryFloatText(&probULV);
    diseaseElement->FirstChildElement("prob_ur_v")->QueryFloatText(&probURV);
    diseaseElement->FirstChildElement("prob_ui_v")->QueryFloatText(&probUIV);
    diseaseElement->FirstChildElement("prob_ui_u")->QueryFloatText(&probUIU);
    diseaseElement->FirstChildElement("location_state_refresh_interval")->QueryUnsignedText(&locStateRefreshInterval);
    

    /* Refer to README for more details */
    numOfRegions=EpidemicConfig.FirstChildElement()->FirstChildElement("number_of_regions");
    numOfRegions->QueryIntText(&numRegions);
    region=numOfRegions;

    /* For each region in the simulation, initialize the locations */
    for( int regIndex = 0; regIndex < numRegions; regIndex++ ) {
        region =region->NextSiblingElement();
        const char*rName = region->FirstChildElement("region_name")->GetText();
        string rname(rName);
        regionName = rname;

        numLocations = 0;
        region->FirstChildElement("number_of_locations")->QueryIntText(&numLocations);
        location = region->FirstChildElement("number_of_locations");
        locObjs = new vector<SimulationObject*>;  
        for( int locIndex = 0; locIndex < numLocations; locIndex++ ) {
            personVec = new vector <Person *>;

            location = location->NextSiblingElement();
            const char*lName = location->FirstChildElement("location_name")->GetText();          
            string lname(lName);
            locationName = lname;
            location->FirstChildElement("number_of_persons")->QueryIntText(&numPersons);            
            location->FirstChildElement("travel_time_to_central_hub")->QueryUnsignedText(&travelTimeToHub);         
            location->FirstChildElement("diffusion_trigger_interval")->QueryUnsignedText(&locDiffusionTrigInterval);            
            people = location->FirstChildElement("diffusion_trigger_interval");

            locationName += ",";
            locationName += regionName;

            travelMap.insert( pair <string, unsigned int>(locationName, travelTimeToHub) );

            /* Read each person's details */
            for(int perIndex = 0; perIndex < numPersons; perIndex++) {
                people= people->NextSiblingElement();
                people->FirstChildElement("pid")->QueryUnsignedText(&pid);
                people->FirstChildElement("susceptibility")->QueryDoubleText(&susceptibility);
                const char *vaccFlag=people->FirstChildElement("is_vaccinated")->GetText();
                string vaccflag(vaccFlag);
                vaccinationStatus=vaccflag;
                const char *infectState=people->FirstChildElement("infection_state")->GetText();
                string infectstate(infectState);
                infectionState = infectstate;   
             
                person = new Person( pid, susceptibility, vaccinationStatus, infectionState, INIT_VTIME, INIT_VTIME );
                personVec->push_back( person );
            }

            LocationObject *locObject = new LocationObject( locationName, transmissibility,
                                                            latentDwellTime, incubatingDwellTime,
                                                            infectiousDwellTime, asymptDwellTime,
                                                            latentInfectivity, incubatingInfectivity,
                                                            infectiousInfectivity, asymptInfectivity,
                                                            probULU, probULV, probURV, probUIV, probUIU,
                                                            locStateRefreshInterval, locDiffusionTrigInterval, 
                                                            personVec, travelTimeToHub);
            locObjs->push_back(locObject);
            simulationObjVec.push_back(locObject);
        }
        numObjects += numLocations;

        /* Add the group of objects to the partition information */
        myPartitioner->addObjectGroup(locObjs);
    }

    /* Send travel map to all the objects once all the objects have been created */
    for( vector<SimulationObject*>::iterator vecIter = simulationObjVec.begin();
                                    vecIter != simulationObjVec.end(); vecIter++ ) {
        LocationObject *locObj = static_cast <LocationObject *> (*vecIter);
        locObj->populateTravelMap(&travelMap);
    }

    /* Perform the actual partitioning of groups */
    const PartitionInfo *retval = myPartitioner->partition( NULL, numberOfProcessorsAvailable );

    EpidemicConfig.SaveFile(inputFileName.c_str());
    return retval;
}

int EpidemicApplication::finalize() {
    return 0;
}

void EpidemicApplication::registerDeserializers() {
    DeserializerManager::instance()->registerDeserializer(  EpidemicEvent::getEpidemicEventDataType(), 
                                                            &EpidemicEvent::deserialize );
}
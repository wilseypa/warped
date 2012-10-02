# Process this file with automake to produce Makefile.in

# See copyright notice in file Copyright in the root directory of this archive.

include $(top_srcdir)/Makefile.am.common

include warped/eclmpl/Makefile.am.stub


lib_LTLIBRARIES = libwarped.la 

if USE_TIMEWARP

if HAVE_MPICH
libwarped_la_LIBADD = -lutils -lmpich -lmpl -lpthread
else 
if HAVE_MPI
libwarped_la_LIBADD = -lutils -lmpi -lpthread
else
libwarped_la_LIBADD = -lutils -lpthread
endif
endif

else
libwarped_la_LIBADD = -lutils -lpthread
endif

WARPED_H_FILES = \
	$(ECLMPL_H_FILES) \
	warped/AdaptiveOutputManager.h \
	warped/AdaptiveStateManagerBase.h \
	warped/AggressiveOutputManager.h \
	warped/Application.h \
	warped/SharedDVFSManager.h \
	warped/CentralizedEventSetSchedulingManager.h \
	warped/ChebyFossilCollManager.h \
	warped/CirculateInitializationMessage.h \
	warped/DVFSManager.h \
	warped/DVFSManagerFactory.h \
	warped/DVFSManagerImplementationBase.h \
	warped/CommunicatingEntity.h \
	warped/CommunicationManager.h \
	warped/CommunicationManagerFactory.h \
	warped/CommunicationManagerImplementationBase.h \
	warped/Configurer.h \
	warped/Configurable.h\
	warped/ConfigurationManager.h \
	warped/ConfigurationManagerFactory.h \
	warped/ControlManager.h \
	warped/CostAdaptiveStateManager.h \
	warped/DecentralizedEventSetSchedulingManager.h \
	warped/DefaultCommunicationManager.h \
	warped/DefaultEvent.h \
	warped/DefaultObjectID.h \
	warped/DefaultPartitioner.h \
	warped/DefaultPhysicalCommunicationLayer.h \
	warped/DefaultSchedulingManager.h \
	warped/DefaultTimeWarpEventSet.h \
	warped/DefaultTimeWarpEventContainer.h \
	warped/DefaultTimeWarpEventContOneAntiMsg.h \
	warped/IntVTime.h \
	warped/DeserializerManager.h \
	warped/DistributedDVFSManager.h \
	warped/Event.h \
	warped/EventFunctors.h \
	warped/EventId.h \
	warped/EventMessage.h \
	warped/EventMessageBase.h \
	warped/EventSet.h \
	warped/EventSetFactory.h \
	warped/FileData.h \
	warped/FileManager.h \
	warped/FileQueue.h \
	warped/FlatState.h \
	warped/GVTManager.h \
	warped/GVTManagerFactory.h \
	warped/GVTManagerImplementationBase.h \
	warped/GVTUpdateMessage.h \
	warped/InFileData.h \
	warped/InFileQueue.h \
	warped/InitializationMessage.h \
	warped/InputSetObject.h \
	warped/KernelMessage.h \
	warped/LazyOutputManager.h \
	warped/LocalEventMessage.h \
	warped/LocalFossilCollectMessage.h \
	warped/LocalKernelMessage.h \
	warped/LocalNegativeEventMessage.h \
	warped/MatternGVTManager.h \
	warped/MatternGVTMessage.h \
	warped/MatternObjectRecord.h \
	warped/MessageManager.h \
	warped/MsgAggregatingCommunicationManager.h \
	warped/NegativeEvent.h \
	warped/NegativeEventMessage.h \
	warped/ObjectID.h \
	warped/ObjectEventPair.h \
	warped/OptFossilCollManager.h \
	warped/OptFossilCollManagerFactory.h \
	warped/OutputEvents.h \
	warped/OutputManager.h \
	warped/OutputManagerFactory.h \
	warped/OutputManagerImplementationBase.h \
	warped/Partitioner.h \
	warped/PartitionInfo.h \
	warped/PeriodicStateManager.h \
	warped/ReceiverQueueContainer.h \
	warped/RestoreCkptMessage.h \
	warped/RoundRobinPartitioner.h \
	warped/SchedulingData.h \
	warped/SchedulingManager.h \
	warped/SchedulingManagerFactory.h \
	warped/SenderQueueContainer.h \
	warped/SequentialConfigurationManager.h \
	warped/SequentialSimulationManager.h \
	warped/SequentialSimulationStream.h \
	warped/Serializable.h \
	warped/Serializable.cpp \
	warped/SerializedInstance.h \
	warped/SetObject.h \
	warped/Simulation.h \
	warped/SimulationConfiguration.h \
	warped/SimulationManager.h \
	warped/SimulationManagerImplementationBase.h \
	warped/SimulationObject.h \
	warped/SimulationObjectProxy.h \
	warped/SimulationStream.h \
	warped/SingleLinkedList.h \
	warped/SingleTerminationManager.h \
	warped/SinkObject.h \
	warped/SourceObject.h \
	warped/Spinner.h \
	warped/SplayTree.h \
	warped/StackOfPtr.h \
	warped/StartMessage.h \
	warped/State.h \
	warped/StateManager.h \
	warped/StateManagerFactory.h \
	warped/StateManagerImplementationBase.h \
	warped/StateSetObject.h \
	warped/TerminationManager.h \
	warped/TerminateToken.h \
	warped/TokenPassingTerminationManager.h \
	warped/TimeWarpCentralizedEventSet.h \
	warped/TimeWarpCentralizedEventSetFactory.h \
	warped/TimeWarpConfigurationManager.h \
	warped/TimeWarpDecentralizedEventSet.h \
	warped/TimeWarpDecentralizedEventSetFactory.h \
	warped/TimeWarpEventSet.h \
	warped/TimeWarpEventSetFactory.h \
	warped/TimeWarpMultiSet.h \
	warped/TimeWarpMultiSetOneAntiMsg.h \
	warped/TimeWarpMultiSetSchedulingManager.h \
	warped/TimeWarpSenderQueue.h \
	warped/TimeWarpSimulationManager.h \
	warped/TimeWarpSimulationManagerFactory.h \
	warped/TimeWarpSimulationObjectQueue.h \
	warped/TimeWarpSimulationObjectQueueFactory.h \
	warped/TimeWarpSimulationStream.h \
	warped/Transceiver.h \
	warped/TreeElement.h \
	warped/UsefulWorkMessage.h \
	warped/VTime.h \
	warped/version.h \
	warped/warped.h \
	warped/FactoryImplementationBase.cpp \
	warped/FactoryManager.cpp \
	warped/FactorySymbolTable.cpp \
	warped/ObjectStub.cpp \
	warped/Factory.h \
	warped/FactoryImplementationBase.h \
	warped/FactoryManager.h \
	warped/FactorySymbolTable.h \
	warped/ObjectStub.h \
	warped/Types.h \
	warped/ConsoleIO.cpp \
	warped/Socket.cpp \
	warped/StopWatch.cpp \
	warped/ConsoleIO.h \
	warped/Socket.h \
	warped/StopWatch.h \
	warped/WarpedMain.h \
	warped/controlkit/Actuator.h \
	warped/controlkit/AsyncSensor.h \
	warped/controlkit/IIRFilter.h \
	warped/controlkit/ActuatorImplementationBase.h \
	warped/controlkit/AverageFilter.h \
	warped/controlkit/MedianFilter.h \
	warped/controlkit/AggregateAsyncFilteredSensor.h \
	warped/controlkit/Sensor.h \
	warped/controlkit/AggregateSyncSensor.h \
	warped/controlkit/FIRFilter.h \
	warped/controlkit/SensorImplementationBase.h \
	warped/controlkit/AsyncFilteredSensor.h \
	warped/controlkit/Filter.h \
	warped/controlkit/SyncSensor.h\
	warped/ThreadedOutputEvents.h \
	warped/ThreadedOutputManagerImplementationBase.h \
	warped/ThreadedAggressiveOutputManager.h \
	warped/ThreadedTimeWarpMultiSet.h \
	warped/ThreadedTimeWarpMultiSetLTSF.h \
	warped/ThreadedStateManagerImplementationBase.h \
	warped/ThreadedPeriodicStateManager.h \
	warped/ThreadedOutputManager.h \
	warped/ThreadedStateManager.h \
	warped/ThreadedTimeWarpEventSet.h \
	warped/ThreadedTimeWarpMultiSetSchedulingManager.h \
	warped/ThreadedTimeWarpSimulationManager.h\
	warped/StragglerEvent.h\
	warped/ThreadedOptFossilCollManager.h\
	warped/ThreadedChebyFossilCollManager.h\
	warped/ThreadedLazyOutputManager.h\
	warped/ThreadedMatternGVTManager.h\
	warped/ThreadedDynamicOutputManager.h\
	warped/ThreadedCostAdaptiveStateManager.h\
	warped/LockedQueue.h\
	warped/WorkerInformation.h

library_includedir=$(includedir)/warped
library_include_HEADERS = $(WARPED_H_FILES) \
	$(top_builddir)/src/warped/WarpedConfig.h

libwarped_la_SOURCES = \
	$(WARPED_H_FILES) \
	$(ECLMPL_SRC) \
	warped/AdaptiveOutputManager.cpp \
	warped/AdaptiveStateManagerBase.cpp \
	warped/AggressiveOutputManager.cpp \
	warped/SharedDVFSManager.cpp \
	warped/CentralizedEventSetSchedulingManager.cpp \
	warped/ChebyFossilCollManager.cpp \
	warped/CirculateInitializationMessage.cpp \
	warped/DVFSManagerFactory.cpp \
	warped/DVFSManagerImplementationBase.cpp \
	warped/CommunicationManager.cpp \
	warped/CommunicationManagerImplementationBase.cpp \
	warped/CommunicationManagerFactory.cpp \
	warped/Configurable.cpp \
	warped/ConfigurationManagerFactory.cpp \
	warped/Configurer.cpp \
	warped/ControlManager.cpp \
	warped/CostAdaptiveStateManager.cpp \
	warped/DecentralizedEventSetSchedulingManager.cpp \
	warped/DefaultCommunicationManager.cpp \
	warped/DefaultEvent.cpp \
	warped/DefaultPartitioner.cpp \
	warped/DefaultPhysicalCommunicationLayer.cpp \
	warped/DefaultSchedulingManager.cpp \
	warped/DefaultTimeWarpEventSet.cpp \
	warped/DefaultTimeWarpEventContainer.cpp \
	warped/DefaultTimeWarpEventContOneAntiMsg.cpp \
	warped/DeserializerManager.cpp \
	warped/DistributedDVFSManager.cpp \
	warped/Event.cpp \
	warped/EventMessage.cpp \
	warped/EventMessageBase.cpp \
	warped/EventSetFactory.cpp \
	warped/FileManager.cpp \
	warped/FileQueue.cpp \
	warped/FlatState.cpp \
	warped/GVTManagerFactory.cpp \
	warped/GVTManagerImplementationBase.cpp \
	warped/GVTUpdateMessage.cpp \
	warped/InFileQueue.cpp \
	warped/InitializationMessage.cpp \
	warped/IntVTime.cpp \
	warped/KernelMessage.cpp \
	warped/LazyOutputManager.cpp \
	warped/MatternGVTManager.cpp \
	warped/MatternGVTMessage.cpp \
	warped/MessageManager.cpp \
	warped/MsgAggregatingCommunicationManager.cpp \
	warped/NegativeEvent.cpp \
	warped/NegativeEventMessage.cpp \
	warped/OptFossilCollManager.cpp \
	warped/OptFossilCollManagerFactory.cpp \
	warped/OutputEvents.cpp \
	warped/OutputManagerFactory.cpp \
	warped/OutputManagerImplementationBase.cpp \
	warped/Partitioner.cpp \
	warped/PartitionInfo.cpp \
	warped/PeriodicStateManager.cpp \
	warped/RestoreCkptMessage.cpp \
	warped/RoundRobinPartitioner.cpp \
	warped/SchedulingManagerFactory.cpp \
	warped/SequentialConfigurationManager.cpp \
	warped/SequentialFileManager.cpp \
	warped/SequentialSimulationManager.cpp \
	warped/SequentialSimulationStream.cpp \
	warped/SerializedInstance.cpp \
	warped/Simulation.cpp \
	warped/SimulationConfiguration.cpp \
	warped/SimulationManager.cpp \
	warped/SimulationManagerImplementationBase.cpp \
	warped/SimulationObject.cpp \
	warped/SimulationObjectProxy.cpp \
	warped/SimulationStream.cpp \
	warped/SingleLinkedList.cpp \
	warped/SinkObject.cpp \
	warped/SourceObject.cpp \
	warped/Spinner.cpp \
	warped/SplayTree.cpp \
	warped/State.cpp \
	warped/StartMessage.cpp \
	warped/StateManagerFactory.cpp \
	warped/StateManagerImplementationBase.cpp \
	warped/TerminateToken.cpp \
	warped/TimeWarpCentralizedEventSetFactory.cpp \
	warped/TimeWarpDecentralizedEventSetFactory.cpp \
	warped/TimeWarpEventSet.cpp \
	warped/TimeWarpEventSetFactory.cpp \
	warped/TimeWarpConfigurationManager.cpp \
	warped/TimeWarpMultiSet.cpp \
	warped/TimeWarpMultiSetOneAntiMsg.cpp \
	warped/TimeWarpMultiSetSchedulingManager.cpp \
	warped/TimeWarpSenderQueue.cpp \
	warped/TimeWarpSimulationManager.cpp \
	warped/TimeWarpSimulationManagerFactory.cpp \
	warped/TimeWarpSimulationObjectQueue.cpp \
	warped/TimeWarpSimulationObjectQueueFactory.cpp \
	warped/TimeWarpSimulationStream.cpp \
	warped/TokenPassingTerminationManager.cpp \
	warped/Transceiver.cpp \
	warped/UsefulWorkMessage.cpp \
	warped/VTime.cpp \
	warped/WarpedMain.cpp \
	warped/warped.cpp\
	warped/ThreadedOutputEvents.cpp \
    warped/ThreadedOutputManagerImplementationBase.cpp \
    warped/ThreadedAggressiveOutputManager.cpp \
    warped/ThreadedTimeWarpMultiSet.cpp \
    warped/ThreadedTimeWarpMultiSetLTSF.cpp \
    warped/ThreadedStateManagerImplementationBase.cpp \
    warped/ThreadedPeriodicStateManager.cpp \
    warped/ThreadedTimeWarpEventSet.cpp \
    warped/ThreadedTimeWarpMultiSetSchedulingManager.cpp \
    warped/ThreadedTimeWarpSimulationManager.cpp\
    warped/StragglerEvent.cpp\
    warped/ThreadedOptFossilCollManager.cpp\
    warped/ThreadedChebyFossilCollManager.cpp\
	warped/ThreadedLazyOutputManager.cpp\
	warped/ThreadedDynamicOutputManager.cpp\
	warped/ThreadedMatternGVTManager.cpp\
	warped/ThreadedCostAdaptiveStateManager.cpp

CLEANFILES=$(BUILT_SOURCES)


include eclmpl/Makefile.am.stub

lib_LTLIBRARIES = libwarped.la 

if HAVE_MPICH
libwarped_la_LIBADD = -lmpich -lmpl -lpthread 
else 
if HAVE_MPI
libwarped_la_LIBADD = -lmpi -lpthread 
else
libwarped_la_LIBADD = -lpthread
endif
endif

libwarped_la_LIBADD += $(top_builddir)/thirdparty/json/libjsoncpp.la
libwarped_la_LIBADD += $(top_builddir)/thirdparty/metis/libmetis/libmetis.la

libwarped_la_CPPFLAGS=-I$(top_srcdir)/thirdparty -I$(top_srcdir)/thirdparty/metis/include

CONTROLKIT_H_FILES = \
	controlkit/Actuator.h \
	controlkit/ActuatorImplementationBase.h \
	controlkit/AggregateAsyncFilteredSensor.h \
	controlkit/AggregateSyncSensor.h \
	controlkit/AsyncFilteredSensor.h \
	controlkit/AsyncSensor.h \
	controlkit/AverageFilter.h \
	controlkit/Filter.h \
	controlkit/FIRFilter.h \
	controlkit/IIRFilter.h \
	controlkit/MedianFilter.h \
	controlkit/Sensor.h \
	controlkit/SensorImplementationBase.h \
	controlkit/SyncSensor.h

WARPED_H_FILES = \
	$(ECLMPL_H_FILES) \
	AdaptiveOutputManager.h \
	AdaptiveStateManagerBase.h \
	AggressiveOutputManager.h \
	Application.h \
	AtomicState.h \
	ChebyFossilCollManager.h \
	CirculateInitializationMessage.h \
	CommunicatingEntity.h \
	CommunicationManager.h \
	CommunicationManagerFactory.h \
	CommunicationManagerImplementationBase.h \
	Configurable.h \
	ConfigurationManager.h \
	Configurer.h \
	ControlManager.h \
	CostAdaptiveStateManager.h \
	DefaultCommunicationManager.h \
	DefaultEvent.h \
	DefaultObjectID.h \
	DefaultPhysicalCommunicationLayer.h \
	DefaultTimeWarpEventContainer.h \
	DefaultTimeWarpEventContOneAntiMsg.h \
	DefaultTimeWarpEventSet.h \
	DeserializerManager.h \
	DistributedDVFSManager.h \
	DVFSManager.h \
	DVFSManagerFactory.h \
	DVFSManagerImplementationBase.h \
	Event.h \
	EventFunctors.h \
	EventId.h \
	EventMessage.h \
	EventMessageBase.h \
	EventSet.h \
	EventSetFactory.h \
	Factory.h \
	FactoryImplementationBase.h \
	FactoryManager.h \
	FactorySymbolTable.h \
	FileData.h \
	FileQueue.h \
	FlatState.h \
	GraphStatistics.h \
	GreedyPartitioner.h \
	GVTManager.h \
	GVTManagerFactory.h \
	GVTManagerImplementationBase.h \
	GVTUpdateMessage.h \
	InFileData.h \
	InFileQueue.h \
	InitializationMessage.h \
	InputSetObject.h \
	IntVTime.h \
	KernelMessage.h \
	LadderQStrict.h \
	ListNode.h \
	LockFreeList.h \
	LadderQRelaxed.h \
	LazyOutputManager.h \
	LockedQueue.h \
	LockState.h \
	MatternGVTManager.h \
	MatternGVTMessage.h \
	MatternObjectRecord.h \
	MessageManager.h \
	MsgAggregatingCommunicationManager.h \
	NegativeEvent.h \
	NegativeEventMessage.h \
	ObjectID.h \
	ObjectStub.h \
	OptFossilCollManager.h \
	OptFossilCollManagerFactory.h \
	OutputEvents.h \
	OutputManager.h \
	OutputManagerFactory.h \
	OutputManagerImplementationBase.h \
	Partitioner.h \
	PartitionInfo.h \
	PartitionManager.h \
	PeriodicStateManager.h \
	ProfileGuidedPartitioner.h \
	ReceiverQueueContainer.h \
	RestoreCkptMessage.h \
	RoundRobinPartitioner.h \
	SchedulingData.h \
	SchedulingManager.h \
	SchedulingManagerFactory.h \
	SenderQueueContainer.h \
	SequentialConfigurationManager.h \
	SequentialSimulationManager.h \
	SequentialSimulationStream.h \
	Serializable.h \
	SerializedInstance.h \
	SetObject.h \
	SharedDVFSManager.h \
	Simulation.h \
	SimulationConfiguration.h \
	SimulationManager.h \
	SimulationManagerImplementationBase.h \
	SimulationObject.h \
	SimulationObjectProxy.h \
	SimulationStream.h \
	SingleLinkedList.h \
	SingleTerminationManager.h \
	Socket.h \
	Spinner.h \
	SplayTree.h \
	StackOfPtr.h \
	StartMessage.h \
	State.h \
	StateManager.h \
	StateManagerFactory.h \
	StateManagerImplementationBase.h \
	StateSetObject.h \
	StopWatch.h \
	StragglerEvent.h \
	TerminateToken.h \
	TerminationManager.h \
	ThreadedAggressiveOutputManager.h \
	ThreadedChebyFossilCollManager.h \
	ThreadedCostAdaptiveStateManager.h \
	ThreadedDynamicOutputManager.h \
	ThreadedLazyOutputManager.h \
	ThreadedMatternGVTManager.h \
	ThreadedOptFossilCollManager.h \
	ThreadedOutputEvents.h \
	ThreadedOutputManager.h \
	ThreadedOutputManagerImplementationBase.h \
	ThreadedPeriodicStateManager.h \
	ThreadedStateManager.h \
	ThreadedStateManagerImplementationBase.h \
	ThreadedTimeWarpEventSet.h \
	ThreadedTimeWarpMultiSet.h \
	ThreadedTimeWarpMultiSetLTSF.h \
	ThreadedTimeWarpMultiSetSchedulingManager.h \
	ThreadedTimeWarpSimulationManager.h \
	TimeWarpCentralizedEventSet.h \
	TimeWarpConfigurationManager.h \
	TimeWarpEventSet.h \
	TimeWarpEventSetFactory.h \
	TimeWarpMultiSet.h \
	TimeWarpMultiSetOneAntiMsg.h \
	TimeWarpMultiSetSchedulingManager.h \
	TimeWarpSimulationManager.h \
	TimeWarpSimulationManagerFactory.h \
	TimeWarpSimulationStream.h \
	TokenPassingTerminationManager.h \
	Transceiver.h \
	TreeElement.h \
	Types.h \
	UsefulWorkMessage.h \
	VTime.h \
	warped.h \
	WarpedDebug.h \
	WarpedMain.h \
	WorkerInformation.h

library_includedir=$(includedir)
library_include_HEADERS = $(WARPED_H_FILES) \
	$(top_builddir)/src/WarpedConfig.h

controlkit_includedir=$(includedir)/controlkit
controlkit_include_HEADERS= $(CONTROLKIT_H_FILES)

libwarped_la_SOURCES = \
	$(CONTROLKIT_H_FILES) \
	$(WARPED_H_FILES) \
	$(ECLMPL_SRC) \
	AdaptiveOutputManager.cpp \
	AdaptiveStateManagerBase.cpp \
	AggressiveOutputManager.cpp \
	Application.cpp \
	ChebyFossilCollManager.cpp \
	CirculateInitializationMessage.cpp \
	CommunicationManager.cpp \
	CommunicationManagerFactory.cpp \
	CommunicationManagerImplementationBase.cpp \
	Configurable.cpp \
	Configurer.cpp \
	ControlManager.cpp \
	CostAdaptiveStateManager.cpp \
	DefaultCommunicationManager.cpp \
	DefaultEvent.cpp \
	DefaultPhysicalCommunicationLayer.cpp \
	DefaultTimeWarpEventContainer.cpp \
	DefaultTimeWarpEventContOneAntiMsg.cpp \
	DefaultTimeWarpEventSet.cpp \
	DeserializerManager.cpp \
	DistributedDVFSManager.cpp \
	DVFSManagerFactory.cpp \
	DVFSManagerImplementationBase.cpp \
	Event.cpp \
	EventMessage.cpp \
	EventMessageBase.cpp \
	EventSetFactory.cpp \
	FactoryImplementationBase.cpp \
	FactoryManager.cpp \
	FactorySymbolTable.cpp \
	FileQueue.cpp \
	FlatState.cpp \
	GraphStatistics.cpp \
	GreedyPartitioner.cpp \
	GVTManagerFactory.cpp \
	GVTManagerImplementationBase.cpp \
	GVTUpdateMessage.cpp \
	InFileQueue.cpp \
	InitializationMessage.cpp \
	IntVTime.cpp \
	KernelMessage.cpp \
	LazyOutputManager.cpp \
	MatternGVTManager.cpp \
	MatternGVTMessage.cpp \
	MessageManager.cpp \
	MsgAggregatingCommunicationManager.cpp \
	NegativeEvent.cpp \
	NegativeEventMessage.cpp \
	ObjectStub.cpp \
	OptFossilCollManager.cpp \
	OptFossilCollManagerFactory.cpp \
	OutputEvents.cpp \
	OutputManagerFactory.cpp \
	OutputManagerImplementationBase.cpp \
	Partitioner.cpp \
	PartitionInfo.cpp \
	PartitionManager.cpp \
	PeriodicStateManager.cpp \
	ProfileGuidedPartitioner.cpp \
	RestoreCkptMessage.cpp \
	RoundRobinPartitioner.cpp \
	SchedulingManagerFactory.cpp \
	SequentialConfigurationManager.cpp \
	SequentialSimulationManager.cpp \
	SequentialSimulationStream.cpp \
	Serializable.cpp \
	SerializedInstance.cpp \
	SharedDVFSManager.cpp \
	Simulation.cpp \
	SimulationConfiguration.cpp \
	SimulationManager.cpp \
	SimulationManagerImplementationBase.cpp \
	SimulationObject.cpp \
	SimulationObjectProxy.cpp \
	SimulationStream.cpp \
	SingleLinkedList.cpp \
	Socket.cpp \
	Spinner.cpp \
	SplayTree.cpp \
	StartMessage.cpp \
	State.cpp \
	StateManagerFactory.cpp \
	StateManagerImplementationBase.cpp \
	StopWatch.cpp \
	TerminateToken.cpp \
	ThreadedCostAdaptiveStateManager.cpp \
	ThreadedDynamicOutputManager.cpp \
	ThreadedLazyOutputManager.cpp \
	ThreadedMatternGVTManager.cpp \
	ThreadedOutputEvents.cpp \
	TimeWarpConfigurationManager.cpp \
	TimeWarpEventSet.cpp \
	TimeWarpEventSetFactory.cpp \
	TimeWarpMultiSet.cpp \
	TimeWarpMultiSetOneAntiMsg.cpp \
	TimeWarpMultiSetSchedulingManager.cpp \
	TimeWarpSimulationManager.cpp \
	TimeWarpSimulationManagerFactory.cpp \
	TimeWarpSimulationStream.cpp \
	TokenPassingTerminationManager.cpp \
	Transceiver.cpp \
	UsefulWorkMessage.cpp \
	VTime.cpp \
	warped.cpp \
	WarpedDebug.cpp \
	WarpedMain.cpp \
    StragglerEvent.cpp \
    ThreadedAggressiveOutputManager.cpp \
    ThreadedChebyFossilCollManager.cpp \
    ThreadedOptFossilCollManager.cpp \
    ThreadedOutputManagerImplementationBase.cpp \
    ThreadedPeriodicStateManager.cpp \
    ThreadedStateManagerImplementationBase.cpp \
    ThreadedTimeWarpEventSet.cpp \
    ThreadedTimeWarpMultiSet.cpp \
    ThreadedTimeWarpMultiSetLTSF.cpp \
    ThreadedTimeWarpMultiSetSchedulingManager.cpp \
    ThreadedTimeWarpSimulationManager.cpp

libwarped_la_CXXFLAGS = -Wall -Wextra -pedantic

EXTRA_DIST = \
	eclmpl/Makefile.am.stub \
	WarpedConfig.h.in

CLEANFILES=$(BUILT_SOURCES)


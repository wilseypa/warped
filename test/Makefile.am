if USE_CPPUNIT
UNIT_TEST_PROGRAM=warpedUnitTests

warpedUnitTests_SOURCES = \
	warpedUnitTestsMain.cpp	\
	AdaptiveOutputManagerTest.cpp \
	AdaptiveOutputManagerTest.h \
	AdaptiveStateManagerBaseTest.cpp \
	AdaptiveStateManagerBaseTest.h \
	AggressiveOutputManagerTest.cpp \
	AggressiveOutputManagerTest.h \
	CirculateInitializationMessageTest.cpp \
	CirculateInitializationMessageTest.h \
	CostAdaptiveStateManagerTest.cpp \
	CostAdaptiveStateManagerTest.h \
	DefaultSchedulingManagerTest.cpp \
	DefaultSchedulingManagerTest.h \
	DefaultTimeWarpEventSetTest.cpp \
	DefaultTimeWarpEventSetTest.h \
	DefaultTimeWarpEventSetOneAntiMsgTest.cpp \
	DefaultTimeWarpEventSetOneAntiMsgTest.h \
	EventMessageBaseTest.cpp \
	EventMessageBaseTest.h \
	EventMessageTest.cpp \
	EventMessageTest.h \
	EventTest.cpp \
	EventTest.h \
	GVTUpdateMessageTest.cpp \
	GVTUpdateMessageTest.h \
	InitializationMessageTest.cpp \
	InitializationMessageTest.h \
	KernelMessageTest.cpp \
	KernelMessageTest.h \
	LazyOutputManagerTest.cpp \
	LazyOutputManagerTest.h \
	MatternGVTMessageTest.cpp \
	MatternGVTMessageTest.h \
	NegativeEventMessageTest.cpp \
	NegativeEventMessageTest.h \
	OutputManagerTest.cpp \
	OutputManagerTest.h \
	RestoreCkptMessageTest.cpp \
	RestoreCkptMessageTest.h \
	SchedulingManagerTest.cpp \
	SchedulingManagerTest.h \
	SerializableTest.cpp \
	SerializableTest.h \
	SerializedInstanceTest.cpp \
	SerializedInstanceTest.h \
	StartMessageTest.cpp \
	StartMessageTest.h \
	TerminateTokenTest.cpp \
	TerminateTokenTest.h \
	TimeWarpEventSetTest.h \
	TimeWarpEventSetTest.cpp \
	TimeWarpMultiSetTest.h \
	TimeWarpMultiSetTest.cpp \
	TimeWarpMultiSetOneAntiMsgTest.h \
	TimeWarpMultiSetOneAntiMsgTest.cpp \
	TimeWarpSimulationManagerAdapter.cpp \
	WarpedUnitTest.cpp \
	WarpedUnitTest.h

warpedUnitTests_LDFLAGS=-static
warpedUnitTests_LDADD=$(top_builddir)/src/libwarped.la

# Add the executable to the TESTS variable so we can run it with 'make check'
TESTS=warpedUnitTests

endif

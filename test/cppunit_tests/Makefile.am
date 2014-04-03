if USE_CPPUNIT
noinst_PROGRAMS=warpedUnitTests

warpedUnitTests_SOURCES = \
	AdaptiveOutputManagerTest.cpp \
	AdaptiveOutputManagerTest.h \
	AdaptiveStateManagerBaseTest.cpp \
	AdaptiveStateManagerBaseTest.h \
	AggressiveOutputManagerTest.cpp \
	AggressiveOutputManagerTest.h \
	ApplicationAdapter.h \
	CirculateInitializationMessageTest.cpp \
	CirculateInitializationMessageTest.h \
	CostAdaptiveStateManagerTest.cpp \
	CostAdaptiveStateManagerTest.h \
	DefaultTimeWarpEventSetOneAntiMsgTest.cpp \
	DefaultTimeWarpEventSetOneAntiMsgTest.h \
	DefaultTimeWarpEventSetTest.cpp \
	DefaultTimeWarpEventSetTest.h \
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
	SimulationObjectAdapter.h \
	StartMessageTest.cpp \
	StartMessageTest.h \
	TerminateTokenTest.cpp \
	TerminateTokenTest.h \
	TimeWarpEventSetTest.cpp \
	TimeWarpEventSetTest.h \
	TimeWarpMultiSetOneAntiMsgTest.cpp \
	TimeWarpMultiSetOneAntiMsgTest.h \
	TimeWarpMultiSetTest.cpp \
	TimeWarpMultiSetTest.h \
	TimeWarpSimulationManagerAdapter.h \
	TimeWarpSimulationManagerAdapter.cpp \
	WarpedUnitTest.cpp \
	WarpedUnitTest.h \
	warpedUnitTestsMain.cpp

warpedUnitTests_LDFLAGS=-static
warpedUnitTests_LDADD=$(top_builddir)/src/libwarped.la
warpedUnitTests_CPPFLAGS=-I$(top_srcdir)/thirdparty -I$(top_srcdir)/src

# Add the executable to the TESTS variable so we can run it with 'make check'
TESTS=warpedUnitTests

endif

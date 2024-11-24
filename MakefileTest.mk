CFLAGS = -DTEST -DUNITY_INCLUDE_CONFIG_H
TESTS_DIR = Tests
MOCKS_DIR = $(TESTS_DIR)/Mocks
FAKES_DIR = $(TESTS_DIR)/Fakes
UNITY_DIR = $(MODULES_DIR)/Unity-2.6.0

FOR_TARGET = Y

TARGET_LY68_DRIVER_DIR = $(TESTS_DIR)/Target_Tests/LY68_Driver

HOST_LY68_DRIVER_DIR = $(TESTS_DIR)/Host_Tests/LY68_Driver

C_INCLUDES += \
-I$(UNITY_DIR)/src \
-I$(UNITY_DIR)/extras/fixture/src \
-I$(UNITY_DIR)/extras/memory/src \
-I$(TESTS_DIR) \
-I$(MOCKS_DIR)/Inc \
-I$(FAKES_DIR)/Inc

C_SOURCES += \
$(UNITY_DIR)/src/unity.c \
$(UNITY_DIR)/extras/fixture/src/unity_fixture.c \
$(UNITY_DIR)/extras/memory/src/unity_memory.c

ifeq ($(FOR_TARGET), Y)
C_SOURCES += \
$(TESTS_DIR)/target_tests.c \
$(TARGET_LY68_DRIVER_DIR)/ly68_driver_test_runner.c \
$(TARGET_LY68_DRIVER_DIR)/ly68_driver_test.c

include MakefileTestTarget.mk
else
C_SOURCES += \
$(TESTS_DIR)/host_tests.c \
$(HOST_LY68_DRIVER_DIR)/ly68_driver_test_runner.c \
$(HOST_LY68_DRIVER_DIR)/ly68_driver_test.c \
$(MOCKS_DIR)/Src/mock.c \
$(MOCKS_DIR)/Src/mock_spi.c \
$(MOCKS_DIR)/Src/mock_cs_pin.c

include MakefileTestHost.mk
endif

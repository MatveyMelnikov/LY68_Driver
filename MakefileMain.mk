MODULES_DIR = Modules

LY68_DRIVER_DIR = $(MODULES_DIR)/LY68_Driver

TESTS = Y

C_INCLUDES = \
-I$(LY68_DRIVER_DIR)/Inc

C_SOURCES = \
$(LY68_DRIVER_DIR)/Src/ly68_driver.c

ifeq ($(TESTS), Y)
include MakefileTest.mk
else
include MakefileTarget.mk                                       
endif
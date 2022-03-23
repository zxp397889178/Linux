PROJECT_ROOT= $(shell pwd)
PROJECT_NAME= PayFrame
SOLUTION_ROOT= $(shell cd $(PROJECT_ROOT)/../../.. && pwd && cd $(PROJECT_ROOT))
TEMP_ROOT= $(SOLUTION_ROOT)/temp/$(PROJECT_NAME)
SERVER_BASE=$(SOLUTION_ROOT)/../serverbase
INCLUDE_ROOT=$(SERVER_BASE)/include
LIB_DIR= $(SERVER_BASE)/lib
BIN_DIR= $(SERVER_BASE)/bin
PAYLIB_DIR= $(SOLUTION_ROOT)/lib
MYSQL_INC = /usr/include/mysql
MYSQL_LIB = /usr/lib64/mysql

TARGETLIBS = lib$(PROJECT_NAME).$(KERNEL_VER).$(PROCESS_VER).a
TARGETSLIBS= lib$(PROJECT_NAME).so
EXECUTABLE= $(PROJECT_NAME)

## 以下为获取版本号信息 ##
VERSION_FILE = ./version.txt
MAJOR_VER = $(shell awk -F '.' '{print $$1}' $(VERSION_FILE) )
MINOR_VER = $(shell awk -F '.' '{print $$2}' $(VERSION_FILE) )
BUILD_VER = $(shell awk -F '.' '{print $$3}' $(VERSION_FILE) )
GIT_VER = $(shell git show -s --pretty=format:%h)
SVN_VER = $(shell svn info | grep "^版本:" | awk '{print $$2}' )
KERNEL_VER = $(shell uname -r | awk -F '.' '{print $$(NF-1)}')
PROCESS_VER = $(shell uname -p)
NEXT_BUILD_VER = $(shell let v1=$(BUILD_VER); let v2=$$v1+1; echo $$v2)
SO_NXT_VERSION = $(MAJOR_VER).$(MINOR_VER).$(NEXT_BUILD_VER)
SO_CUR_VERSION = $(MAJOR_VER).$(MINOR_VER).$(BUILD_VER)
# SO_BUILD_VERSION = $(SO_VERSION)-$(GIT_VER).$(KERNEL_VER).$(PROCESS_VER)
SO_BUILD_VERSION = $(SO_NXT_VERSION).$(KERNEL_VER).$(PROCESS_VER)
SO_INST_VERSION = $(SO_CUR_VERSION).$(KERNEL_VER).$(PROCESS_VER)

## 判断是否有代码增加
CHANGE_SOURCE := $(shell git status -s ./ | grep -E "\.cpp|\.C\.cc\.h" | grep -E "^ M|^ A|^ R" | awk '{print $$2}')
CHANGE_SOURCE_COUNT := $(shell git status -s ./ | grep -E "\.cpp|\.C\.cc\.h" | grep -E "^ M|^ A|^ R" | wc -l )

## 判断GCC版本是否支持C11
GCC_VERSION5 := $(shell expr `gcc -dumpversion | awk -F '.' '{print $$1}'` \> 4)
GCC_VERSION4 := $(shell expr `gcc -dumpversion | awk -F '.' '{print $$1}'` \= 4)
GCC_SUBVERSION := $(shell expr `gcc -dumpversion | awk -F '.' '{print $$2}'` \>= 7)
ifeq ($(GCC_VERSION5), 1)
C11_SUPPORT := 1
else ifeq ($(GCC_VERSION4)_$(GCC_SUBVERSION), 1_1)
C11_SUPPORT := 1
endif

### C compiler flags
CC=gcc 
EXTRA_FLAGS = -fPIC -ggdb3 
ifdef C11_SUPPORT
CFLAGS=-std=gnu++11
endif
CFLAGS+=-Wall -O2 $(EXTRA_FLAGS) -I$(INCLUDE_ROOT) -I$(MYSQL_INC) -I$(PCRE_HOME)/include 

### C++ compiler flags
CXX=g++ 
CPPFLAGS = 
CXXFLAGS = $(CPPFLAGS) $(CFLAGS)

### link flags
LINKCC=$(CC)
ARFLAGS=rc
## LDFLAGS  =-lstdc++ -ldl -lutil -lpthread -export-dynamic -ggdb3 -L$(SOLUTION_ROOT)/lib -lDatabase -lNetWork -lProtocol -lBaseCode -lm
## XLDFLAGS =-Xliner "-(" $(LDFLAGS) -Xlinker "-)"
##XLDFLAGS=-lstdc++ -ldl -lutil -lpthread -L$(SOLUTION_ROOT)/lib -liconv -lDatabase -lNetWork -lProtocol -lBaseCode -lm ../../lib/libhiredis.a -lSecurity -lDbAdapter -lmysqlclient
LDFLAGS=-lstdc++ -lUtility -lDatabase -lNetWork -lSecurity -lBaseCode
LDFLAGS+=-lmysqlclient -lhiredis
LDFLAGS+=-ldl -lutil -lpthread -lpcrecpp -liconv -lcurl -lcrypt -lssl -lz -lm -lrt
## LDLIBS += -L $(LIB_DIR) -L .
LDLIBS += -L . -L$(BIN_DIR) -L$(MYSQL_LIB)
XLDFLAGS=$(LDLIBS) $(LDFLAGS)

PLUGIN_LDFLAGS = $(XLDFLAGS)
DBPLUGIN_LDFLAGS=$(XLDFLAGS)

# SOFLAGS  =-lstdc++ -shared -fPIC

### 编译环境变量定义
CROSS_COMPILE=
AS     = $(CROSS_COMPILE)as
LD     = $(CROSS_COMPILE)ld
CC     = $(CROSS_COMPILE)gcc
CPP    = $(CC) -E
AR     = $(CROSS_COMPILE)ar
NM     = $(CROSS_COMPILE)nm
STRIP  = $(CROSS_COMPILE)strip
RANLIB = $(CROSS_COMPILE)ranlib

### 本机相关命令
CP     := cp
RM     := rm
MKDIR  := mkdir
SED    := sed
FIND   := find
XARGS  := xargs


########################################################################################################################
### source files define
### you can changed <DIRS_1>'s value

DIRS_1 := . 
DIRS_2 := $(foreach dir, $(DIRS_1), $(PROJECT_ROOT)/$(dir))
DIRS   := $(subst /.,,$(DIRS_2))
SRCS := $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c)) \
		$(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc)) \
		$(foreach dir, $(DIRS), $(wildcard $(dir)/*.C)) \
		$(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp)) 
OBJS := $(patsubst %.c,   %.o, $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))) \
		$(patsubst %.cc,  %.o, $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc))) \
		$(patsubst %.C,   %.o, $(foreach dir, $(DIRS), $(wildcard $(dir)/*.C))) \
		$(patsubst %.cpp, %.o, $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp)))
OBJS_1    := $(subst $(PROJECT_ROOT)/,,$(OBJS))
OBJS_2    := $(foreach obj, $(OBJS_1), $(TEMP_ROOT)/$(obj))
LIB_OBJS  := $(subst ./,,$(OBJS_2))
DEPS      := $(patsubst %.o, %.d, $(LIB_OBJS))


ALL_DIRS := $(DIRS)
ALL_SRCS := $(SRCS)
ALL_OBJS := $(LIB_OBJS)
ALL_DEPS := $(DEPS)


########################################################################################################################
### dependencys file build rules

$(TEMP_ROOT)/%.d: %.c
	@[ ! -d $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@)) ] & $(MKDIR) -p $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@))
	$(CC) -MM $(CFLAGS) $< > $@
	$(CC) -MM $(CFLAGS) $< | sed s/\\.o/.d/ > $@

$(TEMP_ROOT)/%.d: %.cc
	@[ ! -d $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@)) ] & $(MKDIR) -p $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@))
	$(CXX) -MM $(CXXFLAGS) $< > $@
	$(CXX) -MM $(CXXFLAGS) $< | sed s/\\.o/.d/ > $@

$(TEMP_ROOT)/%.d: %.cpp
	@[ ! -d $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@)) ] & $(MKDIR) -p $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@))
	$(CXX) -MM $(CXXFLAGS) $< > $@
	$(CXX) -MM $(CXXFLAGS) $< | sed s/\\.o/.d/ > $@

$(TEMP_ROOT)/%.d: %.C
	@[ ! -d $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@)) ] & $(MKDIR) -p $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@))
	$(CXX) -MM $(CXXFLAGS) $< > $@
	$(CXX) -MM $(CXXFLAGS) $< | sed s/\\.o/.d/ > $@ 


########################################################################################################################
### object files build rules

$(TEMP_ROOT)/%.o: %.c
	@[ ! -d $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@)) ] & $(MKDIR) -p $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@))
	$(CC) $(CFLAGS) -o $@ -c $<

$(TEMP_ROOT)/%.o: %.cc
	@[ ! -d $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@)) ] & $(MKDIR) -p $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@))
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(TEMP_ROOT)/%.o: %.cpp
	@[ ! -d $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@)) ] & $(MKDIR) -p $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@))
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(TEMP_ROOT)/%.o: %.C
	@[ ! -d $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@)) ] & $(MKDIR) -p $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@))
	$(CXX) $(CXXFLAGS) -o $@ -c $<


########################################################################################################################
### user defines

all: buildver $(TARGETLIBS) $(TARGETSLIBS)
buildver:
ifneq ($(CHANGE_SOURCE_COUNT), 0)
	@echo "$(PROJECT_NAME) source has channged, increase version number."
	@echo $(SO_NXT_VERSION) > $(VERSION_FILE)
	@git add $(VERSION_FILE) Makefile
#	@git commit -m "build $(PROJECT_NAME), version $(SO_NXT_VERSION), .so target is $(EXECUTABLE)"
else
SO_BUILD_VERSION := $(SO_INST_VERSION)
endif

$(TARGETLIBS): $(LIB_OBJS)
	@$(AR) $(ARFLAGS) $@ $(LIB_OBJS)
	@$(RANLIB) $@

$(TARGETSLIBS): $(LIB_OBJS)
	@$(RM) -f $@.*
	@$(LINKCC) -shared $(subst $(DIRS), $(TEMP_ROOT), $^) $(CXXFLAGS) -o $@.$(SO_BUILD_VERSION) $(XLDFLAGS)

$(EXECUTABLE): $(LIB_OBJS)
	-@yes|$(RM) $@.*
	@$(LINKCC) $(subst $(DIRS), $(TEMP_ROOT), $^) $(CXXFLAGS) -o$@.$(SO_BUILD_VERSION) $(XLDFLAGS) -rdynamic
	# @$(CP) -f $(EXECUTABLE) $(EXECUTABLE)_`date +%Y%m%d`
	# @rm -f webproxy
	# @ln -s $(EXECUTABLE)_`date +%Y%m%d` webproxy
##	$(STRIP) --strip-unneeded $@

devel_headers := BillpointDataList.h BillPointList.h ChannelList.h \
	Config.h Dict_ClientType.h GameList.h IpList.h MoneyRange.h \
	SdkChannelList.h SdkList.h VipList.h TimeRange.h Common.h \
	RangeBase.h StringRange.h Dict_PayStyle.h pay_error.h IClient.h \
	WithdrawLimitData.h UserCreditList.h ChannelGroup.h
DEVEL_INCLUDE=$(SERVER_BASE)/include/PayFrame
DEVEL_LIB=$(SERVER_BASE)/lib
DEVEL_BIN=$(SERVER_BASE)/bin
install:
	@[ ! -d $(DEVEL_INCLUDE)  ] & $(MKDIR) -p $(DEVEL_INCLUDE)
	@[ ! -d $(DEVEL_LIB)  ] & $(MKDIR) -p $(DEVEL_LIB)
	@$(RM) -f $(DEVEL_LIB)/$(TARGETLIBS).*
	@$(RM) -f $(DEVEL_BIN)/$(TARGETSLIBS).*
	-@yes|$(CP) $(devel_headers) $(DEVEL_INCLUDE)/
	-@yes|$(CP) $(TARGETLIBS) $(DEVEL_LIB)
	-@yes|$(CP) $(TARGETSLIBS).* $(DEVEL_BIN)
	cd $(DEVEL_LIB)
	@ln -s -f $(DEVEL_LIB)/$(TARGETLIBS) $(DEVEL_LIB)/libPayFrame.a
	cd $(DEVEL_BIN)
	@ln -s -f $(DEVEL_BIN)/$(TARGETSLIBS).$(SO_BUILD_VERSION) $(DEVEL_BIN)/libPayFrame.so
	@echo "install $(TARGETLIBS) to  $(DEVEL_LIB) succeed."
	@echo "install $(TARGETSLIBS).$(SO_BUILD_VERSION) to  $(DEVEL_BIN) succeed."

DEBUG_ROOT=/data/work_svn/ngwork/bin/paycenter
debug:
	@rm -f $(DEBUG_ROOT)/$(PROJECT_NAME).*
	-@yes|$(CP) $(PROJECT_NAME).* $(DEBUG_ROOT)
	-@ln -s -f $(DEBUG_ROOT)/$(PROJECT_NAME).$(SO_BUILD_VERSION) $(DEBUG_ROOT)/paysvr
	@echo "install $(PROJECT_NAME).$(SO_BUILD_VERSION) to  $(DEBUG_ROOT) succeed."


clean:
	-$(RM) $(ALL_OBJS) $(ALL_DEPS) $(TARGETLIBS) $(TARGETSLIBS) $(PROJECT_NAME).* *~
	-$(RM) -f lib$(PROJECT_NAME).so.*
	-$(RM) -f lib$(PROJECT_NAME).a
	-$(RM) -rf $(TEMP_ROOT)

explain:
	@echo "The following information repressents you program:"
	@echo "Solution Dir          : $(SOLUTION_ROOT)"
	@echo "Project Dir           : $(PROJECT_ROOT)" 
	@echo "Temp dir              : $(TEMP_ROOT)"
	@echo "INCLUDE_ROOT dir      : $(INCLUDE_ROOT)"
	@echo "LIB_DIR dir           : $(LIB_DIR)"
	@echo "BIN_DIR dir           : $(BIN_DIR)"
	@echo "Final libs name       : $(TARGETLIBS) $(TARGETSLIBS)"
	@echo "Source dir            : $(DIRS)"
	@echo "Source files          : $(SRCS)" 
	@echo "Object files          : $(LIB_OBJS)"
	@echo "Dependency files      : $(DEPS)"
	@echo "Final executable name : $(EXECUTABLE)"
	@echo "Plugin source dir       : $(PLUGIN_DIRS)"
	@echo "Plugin Source files     : $(PLUGIN_SRCS)"
	@echo "Plugin Dependency files : $(PLUGIN_DEPS)"
	@echo ".so next version:    $(SO_NXT_VERSION)"
	@echo ".so build version:   $(SO_BUILD_VERSION)"
	@echo ".so install version: $(SO_INST_VERSION)"

depend: $(ALL_DEPS)
	@echo "Dependencies are now up-to-date."

-include $(ALL_DEPS)



PROJECT_ROOT= $(shell pwd)
PROJECT_NAME= payframe
SOLUTION_ROOT= $(shell cd $(PROJECT_ROOT)/../../.. && pwd && cd $(PROJECT_ROOT))
TEMP_ROOT= $(SOLUTION_ROOT)/temp/$(PROJECT_NAME)
INCLUDE_ROOT=$(SOLUTION_ROOT)/../serverbase/include
LIB_DIR=$(SOLUTION_ROOT)/../serverbase/lib
BIN_DIR= $(SOLUTION_ROOT)/../serverbase/bin
PAYFRAME_LIB=$(LIB_DIR)
PAYFRAME_INC=$(INCLUDE_ROOT)/PayFrame
DEVEL_DIR = $(PAYFRAME_INC)
MYSQL_INC = /usr/include/mysql
MYSQL_LIB = /usr/lib64/mysql

EXECUTABLE= $(PROJECT_NAME)
ALL_PLUGIN=
# ALL_PLUGIN= plugin_demo.so
#
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
CFLAGS+=-Wall -O2 $(EXTRA_FLAGS) -I$(INCLUDE_ROOT) -I$(SOLUTION_ROOT)/BaseCode  -I$(MYSQL_INC) -I$(PAYFRAME_INC)

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

# LDFLAGS=-lstdc++ -lPayFrame -lUtility -lNetWork  -lDatabase -lSecurity -lBaseCode
LDFLAGS=-lstdc++ -lPayFrame -lUtility -lNetWork -lDatabase -lSecurity -lBaseCode
LDFLAGS+=-lmysqlclient -lhiredis -liconv -lpcrecpp -lcurl -lz
LDFLAGS+=-lpthread -ldl -lutil -lcrypto -lssl -lm -lrt
LDLIBS += -L . -L$(BIN_DIR) -L$(PAYFRAME_LIB) -L$(MYSQL_LIB)
# STATIC_LIBS= $(PAYFRAME_LIB)/libPayFrame.a 
# STATIC_LIBS+=$(LIB_DIR)/libUtility.a $(LIB_DIR)/libNetWork.a 
# STATIC_LIBS+=$(LIB_DIR)/libDatabase.a 
# STATIC_LIBS+=$(LIB_DIR)/libSecurity.a $(LIB_DIR)/libBaseCode.a

XLDFLAGS=$(LDLIBS) $(LDFLAGS) 

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


########################################################################################################################
### test files define
### you can changed <PLUGIN_DIRS_1>'s value

PLUGIN_DIRS_1 := plugin 
PLUGIN_DIRS_2 := $(foreach dir, $(PLUGIN_DIRS_1), $(PROJECT_ROOT)/$(dir))
PLUGIN_DIRS   := $(subst /.,,$(PLUGIN_DIRS_2))
PLUGIN_SRCS := $(foreach dir, $(PLUGIN_DIRS), $(wildcard $(dir)/*.c)) \
	 		 $(foreach dir, $(PLUGIN_DIRS), $(wildcard $(dir)/*.cc)) \
			 $(foreach dir, $(PLUGIN_DIRS), $(wildcard $(dir)/*.C)) \
			 $(foreach dir, $(PLUGIN_DIRS), $(wildcard $(dir)/*.cpp)) 
PLUGIN_OBJS := $(patsubst %.c,   %.o, $(foreach dir, $(PLUGIN_DIRS), $(wildcard $(dir)/*.c))) \
			 $(patsubst %.cc,  %.o, $(foreach dir, $(PLUGIN_DIRS), $(wildcard $(dir)/*.cc))) \
	 		 $(patsubst %.C,   %.o, $(foreach dir, $(PLUGIN_DIRS), $(wildcard $(dir)/*.C))) \
			 $(patsubst %.cpp, %.o, $(foreach dir, $(PLUGIN_DIRS), $(wildcard $(dir)/*.cpp)))
PLUGIN_OBJS_1   := $(subst $(PROJECT_ROOT)/,,$(PLUGIN_OBJS))
PLUGIN_OBJS_2   := $(foreach obj, $(PLUGIN_OBJS_1), $(TEMP_ROOT)/$(obj))
ALL_PLUGIN_OBJS := $(subst ./,,$(PLUGIN_OBJS_2))
PLUGIN_DEPS     := $(patsubst %.o,%.d,$(ALL_PLUGIN_OBJS))

# ALL_DIRS := $(DIRS) $(PLUGIN_DIRS)
# ALL_SRCS := $(SRCS) $(PLUGIN_SRCS)
# ALL_OBJS := $(LIB_OBJS) $(ALL_PLUGIN_OBJS)
# ALL_DEPS := $(DEPS) $(PLUGIN_DEPS)

ALL_DIRS := $(DIRS)
ALL_SRCS := $(SRCS)
ALL_OBJS := $(LIB_OBJS)
ALL_DEPS := $(DEPS)
PLUGIN_COMM_OBJS :=  

DEMO1_OBJS := $(TEMP_ROOT)/plugin/demo.o $(TEMP_ROOT)/plugin/sysinfo.o \
					$(PLUGIN_COMM_OBJS)

DEMO2_OBJS := $(TEMP_ROOT)/plugin/oradata.o $(PLUGIN_COMM_OBJS)

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

all: buildver $(EXECUTABLE) $(ALL_PLUGIN)
buildver:
	@echo "$(PROJECT_NAME) source has channged, increase version number."
	@echo $(SO_NXT_VERSION) > $(VERSION_FILE)
	@git add $(VERSION_FILE) Makefile
#	@git commit -m "build $(PROJECT_NAME), version $(SO_NXT_VERSION), .so target is $(EXECUTABLE)"

plugin: $(ALL_PLUGIN) 

$(TARGETLIBS): $(LIB_OBJS)
	@$(AR) $(ARFLAGS) $@ $(LIB_OBJS)
	@$(RANLIB) $@

$(TARGETSLIBS): $(LIB_OBJS)
	@$(LINKCC) -shared $(subst $(DIRS), $(TEMP_ROOT), $^) $(CXXFLAGS) -o $@ $(XLDFLAGS)

$(EXECUTABLE): $(LIB_OBJS)
	-@yes|$(RM) $@.*
	$(LINKCC) $(subst $(DIRS), $(TEMP_ROOT), $^) $(CXXFLAGS) -o$@.$(SO_BUILD_VERSION) $(XLDFLAGS) 
	# @$(CP) -f $(EXECUTABLE) $(EXECUTABLE)_`date +%Y%m%d`
	# @rm -f webproxy
	# @ln -s $(EXECUTABLE)_`date +%Y%m%d` webproxy
##	$(STRIP) --strip-unneeded $@

plugin_demo.so : $(DEMO1_OBJS)
	-@yes|$(RM) $@.*
	$(LINKCC) -shared -fPIC $^ $(CXXFLAGS) -o $@.$(SO_BUILD_VERSION) $(LDLIBS)

plugin_oradata.so : $(DEMO2_OBJS)
	$(LINKCC) -shared -fPIC $^ $(CXXFLAGS) $(DBPLUGIN_LDFLAGS) -o $@ $(LDLIBS)

INCLUDE_DIRS := $(subst $(PROJECT_ROOT), $(INCLUDE_ROOT), $(DIRS))
devel_headers := IClient.h IWebClientMgr.h IWebPlugin.h pay_error.h
DEVEL_HOME=$(SOLUTION_ROOT)
DEVEL_INC=$(DEVEL_HOME)/include/PayFrame
DEVEL_BIN=$(SOLUTION_ROOT)/bin
install:
	@[ ! -d $(DEVEL_INC)  ] & $(MKDIR) -p $(DEVEL_INC)
	@$(RM) -f $(DEVEL_BIN)/$(PROJECT_NAME).*
	-@yes|$(CP) $(devel_headers) $(DEVEL_INC)
	-@yes|$(CP) $(PROJECT_NAME).* $(DEVEL_BIN)
	-@cd $(DEVEL_BIN)
	-@ln -s -f $(DEVEL_BIN)/$(PROJECT_NAME).$(SO_BUILD_VERSION) $(DEVEL_BIN)/paysvr
	@echo "dist to devel header to $(DEVEL_INC) succeed"
	@echo "dist to binary to $(DEVEL_BIN) succeed"

DEBUG_ROOT=$(SOLUTION_ROOT)/debug/paycenter
debug:
	-@yes|$(CP) $(PROJECT_NAME).* $(DEBUG_ROOT)
	@echo "install $(PROJECT_NAME).$(SO_BUILD_VERSION) to  $(DEBUG_ROOT) succeed."


clean:
	$(RM) -r -f $(TEMP_ROOT)
	-$(RM) $(ALL_OBJS) $(ALL_DEPS) $(TARGETLIBS) $(TARGETSLIBS) $(PROJECT_NAME).* $(ALL_PLUGIN) *~
	@rm -rf $(TEMP_ROOT)

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
	@echo "Plugin Object files     : $(ALL_PLUGIN_OBJS)"
	@echo "Plugin Dependency files : $(PLUGIN_DEPS)"
	@echo ".so next version:    $(SO_NXT_VERSION)"
	@echo ".so build version:   $(SO_BUILD_VERSION)"
	@echo ".so install version: $(SO_INST_VERSION)"

depend: $(ALL_DEPS)
	@echo "Dependencies are now up-to-date."

-include $(ALL_DEPS)



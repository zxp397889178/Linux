PROJECT_ROOT= $(shell pwd)
PROJECT_NAME= paycenter
SOLUTION_ROOT= $(shell cd $(PROJECT_ROOT)/../../.. && pwd && cd $(PROJECT_ROOT))
TEMP_ROOT= $(SOLUTION_ROOT)/temp/$(PROJECT_NAME)
SERVER_BASE=$(SOLUTION_ROOT)/../serverbase
INCLUDE_ROOT=$(SERVER_BASE)/include
FRAME_INC=$(INCLUDE_ROOT)/PayFrame
LIB_DIR= $(SERVER_BASE)/lib
BIN_DIR= $(SERVER_BASE)/bin
MYSQL_LIB=/usr/lib64/mysql
MYSQL_INC=/usr/include/mysql
ORACLE_LIB=/usr/lib/oracle/11.2/client64/lib
ORACLE_INC=/usr/include/oracle/11.2/client64
MAGICK_INC=/usr/include/ImageMagick-6

export SOLUTION_ROOT SERVER_BASE INCLUDE_ROOT FRAME_INC LIB_DIR BIN_DIR MYSQL_LIB MYSQL_INC ORACLE_LIB ORACLE_INC MAGICK_INC

## 以下为构建需要编译的子目录
# EXCLUDE_DIRS:=docs demo bak vnpay
EXCLUDE_DIRS:=botai docs hongxin lion-104 phoenix tianxiahui vnpay xinfutong asia66 demo guanglian razorpay svip tianyi waiwai youle bak chaofan fulian haojie jiuwang monkey v8pay zhongbi baoli fun hongteng junpay pay58 richcw tangren xhpay zhpay sanan
PLUGIN_DIRS :=$(shell find . -maxdepth 1 -type d | sort -u)
PLUGIN_DIRS :=$(basename $(patsubst ./%,%,$(PLUGIN_DIRS)))
PLUGIN_DIRS :=$(filter-out $(EXCLUDE_DIRS), $(PLUGIN_DIRS))
ALL_PLUGIN  =$(PLUGIN_DIRS)
# ALL_PLUGIN= pay_zhpay.so pay_zhongbi.so pay_chaofan.so pay_haojie.so \
			pay_monkey.so pay_hongxin.so pay_tangren.so

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
SO_BUILD_VERSION = $(SO_NXT_VERSION)-$(SVN_VER).$(KERNEL_VER).$(PROCESS_VER)
SO_INST_VERSION = $(SO_CUR_VERSION)-*.$(KERNEL_VER).$(PROCESS_VER)

## 判断代码是否有修改或新增
CHANGE_SOURCE := $(shell git status -s ./ | grep -E "\.cpp|\.C\.cc\.h" | grep -E "^ M|^ A|^ R" | awk '{print $$2}')
CHANGE_SOURCE_COUNT := $(shell git status -s ./ | grep -E "\.cpp|\.C\.cc\.h" | grep -E "^ M|^ A|^ R" | wc -l )
ifneq ($(SOURCE_CHANGE_COUNT), 0)
SOURCE_CHANGED := 1
endif

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
CC=gcc -fdiagnostics-color=auto
EXTRA_FLAGS = -fPIC -ggdb3 
ifdef C11_SUPPORT
CFLAGS=-std=gnu++11
endif
CFLAGS += -Wall -O2 $(EXTRA_FLAGS) -I$(INCLUDE_ROOT) -I$(FRAME_INC) -I$(INCLUDE_ROOT)/BaseCode -I$(MYSQL_INC) 

### C++ compiler flags
CXX=g++ -fdiagnostics-color=auto
CPPFLAGS = -Wl,--as-needed -shared -fPIC 
CXXFLAGS = $(CPPFLAGS) $(CFLAGS)

### link flags
LINKCC=$(CC)
ARFLAGS=rc
LDFLAGS=-lstdc++ -ggdb3 -lPayFrame -lUtility -lDatabase -lNetWork -lSecurity -lBaseCode
LDFLAGS+=-lmysqlclient -lhiredis -ldl -lutil -lpthread -liconv -lcurl -lpcrecpp -lrt -lz -lm
LDLIBS += -L . -L$(BIN_DIR) -L$(MYSQL_LIB) -L$(REDIS_HOME)/lib
XLDFLAGS=$(LDLIBS) $(LDFLAGS) 

PLUGIN_LDFLAGS = $(XLDFLAGS)
DBPLUGIN_LDFLAGS=$(XLDFLAGS) 

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

export CC CFLAGS CXX CPPFLAGS MAGICKFLAGS CXXFLAGS LINKCC ARFLAGS XLDFLAGS LDLIBS PLUGIN_LDFLAGS DBPLUGIN_LDFLAGS CROSS_COMPILE AS LD CPP AR NM STRIP RANLIB CP RM MKDIR SED FIND XARGS

########################################################################################################################
# 构建公共的obj
COMMON_SRC_DIR :=$(PROJECT_ROOT)
COMMON_SRC :=$(foreach dir, $(COMMON_SRC_DIR), $(wildcard $(dir)/*.c)) \
			 $(foreach dir, $(COMMON_SRC_DIR), $(wildcard $(dir)/*.cc)) \
			 $(foreach dir, $(COMMON_SRC_DIR), $(wildcard $(dir)/*.C)) \
			 $(foreach dir, $(COMMON_SRC_DIR), $(wildcard $(dir)/*.cpp))
COMMON_OBJ_1 := $(patsubst %.c,   %.o, $(foreach dir, $(COMMON_SRC_DIR), $(wildcard $(dir)/*.c)))  \
				$(patsubst %.cc,  %.o, $(foreach dir, $(COMMON_SRC_DIR), $(wildcard $(dir)/*.cc))) \
				$(patsubst %.C,   %.o, $(foreach dir, $(COMMON_SRC_DIR), $(wildcard $(dir)/*.C)))  \
				$(patsubst %.cpp, %.o, $(foreach dir, $(COMMON_SRC_DIR), $(wildcard $(dir)/*.cpp))) 
COMMON_OBJS := $(subst $(PROJECT_ROOT)/,$(TEMP_ROOT)/,$(COMMON_OBJ_1))

export COMMON_OBJS

########################################################################################################################
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

all: buildcommon buildver $(ALL_PLUGIN)
buildver:
ifdef SOURCE_CHANGED
	@echo "======================================================================="
	@echo "<$(PROJECT_NAME)> source has changed, increase version number."
	@echo "======================================================================="
	@echo $(SO_NXT_VERSION) > $(VERSION_FILE)
#	# # @svn commit -m "build $(PROJECT_NAME), version $(SO_NXT_VERSION)"
#	# # @svn update
endif

buildcommon: $(ALL_OBJS)

$(ALL_PLUGIN): ECHO
	@make -C $@

ECHO:
	@echo "begin compile project $(ALL_PLUGIN)..."

PLUGIN_BIN=$(SOLUTION_ROOT)/bin
install:
	@[ ! -d $(PLUGIN_BIN) ] & $(MKDIR) -p $(PLUGIN_BIN)
	rm -f $(PLUGIN_BIN)/plugin_pay_*.*
	rm -f $(PLUGIN_BIN)/plugin_withdraw*
	-@yes|$(CP) plugin_* $(PLUGIN_BIN)
	@echo "copy *.so to $(PLUGIN_BIN) ok"

clean:
	-$(RM) -rf $(TEMP_ROOT)
	-$(RM) plugin_pay_* plugin_withdraw* *~

explain:
	@echo "The following information repressents you program:"
	@echo "Solution Dir : $(SOLUTION_ROOT)"
	@echo "Project Dir  : $(PROJECT_ROOT)" 
	@echo "Temp dir     : $(TEMP_ROOT)"
	@echo "INCLUDE_ROOT dir    : $(INCLUDE_ROOT)"
	@echo "LIB_DIR dir         : $(LIB_DIR)"
	@echo "BIN_DIR dir         : $(BIN_DIR)"
	@echo "Plugin source dir   : $(PLUGIN_DIRS)"
	@echo "Plugin Source files : $(PLUGIN_SRCS)"
	@echo "Plugin Object files : $(ALL_PLUGIN_OBJS)"
	@echo "Common objects: $(COMMON_OBJS)"
	@echo "Plugin Dependency files : $(PLUGIN_DEPS)"

depend: $(ALL_DEPS)
	@echo "Dependencies are now up-to-date."

-include $(ALL_DEPS)



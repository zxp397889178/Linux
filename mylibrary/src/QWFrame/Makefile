PROJECT_ROOT  = $(shell pwd)
PROJECT_NAME  = QWFrame
SOLUTION_ROOT = $(shell cd $(PROJECT_ROOT)/../.. && pwd && cd $(PROJECT_ROOT))
TEMP_ROOT     = $(SOLUTION_ROOT)/temp/$(PROJECT_NAME)
INCLUDE_ROOT  = $(SOLUTION_ROOT)/include/$(PROJECT_NAME)
LIB_DIR		  = $(SOLUTION_ROOT)/lib
BIN_DIR		  = $(SOLUTION_ROOT)/bin

TARGETLIBS    = lib$(PROJECT_NAME).$(KERNEL_VER).$(PROCESS_VER).a 
TARGETSLIBS   = lib$(PROJECT_NAME).so
EXECUTABLE    = test_frame

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
## CFLAGS = -Wall -O2 -fPIC -MD $(foreach dir, $(INCLUDE_DIR), -I$(dir))
CFLAGS+=-Wall -O2 $(EXTRA_FLAGS) -I$(SOLUTION_ROOT)/include -I$(SOLUTION_ROOT)/include/Database -I$(SOLUTION_ROOT)/include/NetWork -I$(SOLUTION_ROOT)/include/ProtoBase -I/usr/include/mysql

### C++ compiler flags
CXX=g++
CPPFLAGS = 
CXXFLAGS = $(CPPFLAGS) $(CFLAGS)

### link flags
LINKCC=$(CC)
ARFLAGS=rc
LDFLAGS  =-L$(SOLUTION_ROOT)/lib -lstdc++ -ldl -lutil -lpthread -export-dynamic -ggdb3 
## XLDFLAGS =-Xliner "-(" $(LDFLAGS) -Xlinker "-)"
XLDFLAGS=-L$(SOLUTION_ROOT)/lib -lstdc++ -ldl -lutil -lpthread  -lmysqlclient -lhiredis
## LDLIBS += -L $(LIB_DIR) -L .
LDLIBS += -L. -L.. -L /usr/lib64/mysql 
TEST_LDFLAGS = -l$(PROJECT_NAME) -lBaseCode -lDatabase -lbeecrypt -lNetWork -lProtoBase -lSecurity -lmysqlclient -lhiredis -lssl
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

DIRS_1 := . Common Protocol Subscribe
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
### you can changed <TEST_DIRS_1>'s value

TEST_DIRS_1 := test
TEST_DIRS_2 := $(foreach dir, $(TEST_DIRS_1), $(PROJECT_ROOT)/$(dir))
TEST_DIRS   := $(subst /.,,$(TEST_DIRS_2))
TEST_SRCS := $(foreach dir, $(TEST_DIRS), $(wildcard $(dir)/*.c)) \
	 		 $(foreach dir, $(TEST_DIRS), $(wildcard $(dir)/*.cc)) \
			 $(foreach dir, $(TEST_DIRS), $(wildcard $(dir)/*.C)) \
			 $(foreach dir, $(TEST_DIRS), $(wildcard $(dir)/*.cpp)) 
TEST_OBJS := $(patsubst %.c,   %.o, $(foreach dir, $(TEST_DIRS), $(wildcard $(dir)/*.c))) \
			 $(patsubst %.cc,  %.o, $(foreach dir, $(TEST_DIRS), $(wildcard $(dir)/*.cc))) \
	 		 $(patsubst %.C,   %.o, $(foreach dir, $(TEST_DIRS), $(wildcard $(dir)/*.C))) \
			 $(patsubst %.cpp, %.o, $(foreach dir, $(TEST_DIRS), $(wildcard $(dir)/*.cpp)))
TEST_OBJS_1   := $(subst $(PROJECT_ROOT)/,,$(TEST_OBJS))
TEST_OBJS_2   := $(foreach obj, $(TEST_OBJS_1), $(TEMP_ROOT)/$(obj))
TEST_EXE_OBJS := $(subst ./,,$(TEST_OBJS_2))
# TEST_EXE_OBJS := $(subst ./,,$(foreach obj, $(TEST_OBJS), $(TEMP_ROOT)/$(obj)))
TEST_DEPS     := $(patsubst %.o,%.d,$(TEST_EXE_OBJS))

ALL_DIRS := $(DIRS) $(TEST_DIRS)
ALL_SRCS := $(SRCS) $(TEST_SRCS)
ALL_OBJS := $(LIB_OBJS) $(TEST_EXE_OBJS)
ALL_DEPS := $(DEPS) $(TEST_DEPS)

########################################################################################################################
### user defines

all: buildver $(TARGETLIBS) $(TARGETSLIBS) 
buildver:
ifneq ($(CHANGE_SOURCE_COUNT), 0)
	@echo "$(PROJECT_NAME) source has channged, increase version number."
	@echo $(SO_NXT_VERSION) > $(VERSION_FILE)
	@git add $(VERSION_FILE) Makefile
	$(foreach v, $(CHANGE_SOURCE), $(shell git add $(v)) )
	@git commit -m "build $(PROJECT_NAME), version $(SO_NXT_VERSION), .so target is $(TARGETSLIBS).$(SO_BUILD_VERSION)"
else
SO_BUILD_VERSION := $(SO_INST_VERSION)
endif

lib: $(TARGETLIBS) $(TARGETSLIBS)

test: $(EXECUTABLE) 

$(TARGETLIBS): $(LIB_OBJS)
	@$(AR) $(ARFLAGS) $@ $(LIB_OBJS)
	@$(RANLIB) $@

$(TARGETSLIBS): $(LIB_OBJS)
	@$(LINKCC) -shared $(subst $(DIRS), $(TEMP_ROOT), $^) $(CXXFLAGS) $(XLDFLAGS) -o $@.$(SO_BUILD_VERSION) $(LDLIBS)

$(EXECUTABLE): $(TEST_EXE_OBJS)
	$(LINKCC) $(CXXFLAGS) $(XLDFLAGS) $(LDLIBS) $(TEST_LDFLAGS) -o $@ $(TEMP_ROOT)/test/$@.o 
##	$(STRIP) --strip-unneeded $@

## libBaseCode.a : $(DEPS) $(LIB_OBJS)
## 	ar cr libBaseCode.a $(LIB_OBJS)
##
## libBaseCode.so: $(DEPS) $(LIB_OBJS)
##	$(LINKCC) $(SOFLAGS) -o libBaseCode.so $(LIB_OBJS)


########################################################################################################################
### dependencys file build rules

$(TEMP_ROOT)/%.d: %.c
	@[ ! -d $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@)) ] & $(MKDIR) -p $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@))
	rm -rf $@;\
	$(CXX) -MM $(CXXFLAGS) $< > $@.1;\
	sed 's,\($(notdir $*)\).o[:],$(patsubst %.d,%.o,$@) $@ : ,g' < $@.1 > $@;\
	rm -f $@.1

$(TEMP_ROOT)/%.d: %.cc
	@[ ! -d $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@)) ] & $(MKDIR) -p $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@))
	rm -rf $@;\
	$(CXX) -MM $(CXXFLAGS) $< > $@.1;\
	sed 's,\($(notdir $*)\).o[:],$(patsubst %.d,%.o,$@) $@ : ,g' < $@.1 > $@;\
	rm -f $@.1

$(TEMP_ROOT)/%.d: %.cpp
	@[ ! -d $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@)) ] & $(MKDIR) -p $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@))
	rm -rf $@;\
	$(CXX) -MM $(CXXFLAGS) $< > $@.1;\
	sed 's,\($(notdir $*)\).o[:],$(patsubst %.d,%.o,$@) $@ : ,g' < $@.1 > $@;\
	rm -f $@.1

$(TEMP_ROOT)/%.d: %.C
	@[ ! -d $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@)) ] & $(MKDIR) -p $(dir $(subst $(ALL_DIRS), $(TEMP_ROOT), $@))
	rm -rf $@;\
	$(CXX) -MM $(CXXFLAGS) $< > $@.1;\
	sed 's,\($(notdir $*)\).o[:],$(patsubst %.d,%.o,$@) $@ : ,g' < $@.1 > $@;\
	rm -f $@.1

-include $(ALL_DEPS)

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




INCLUDE_DIRS := $(subst $(PROJECT_ROOT), $(INCLUDE_ROOT), $(DIRS))
install:
	@$(foreach dir, $(INCLUDE_DIRS), $(shell $(MKDIR) -p $(dir)))
	@[ ! -d $(LIB_DIR) ] & $(MKDIR) -p $(LIB_DIR)
	@[ ! -d $(BIN_DIR) ] & $(MKDIR) -p $(BIN_DIR)
	-@yes|$(CP) $(TARGETLIBS) $(LIB_DIR)
	@$(RM) -rf $(INCLUDE_DIR)
	@$(foreach dir, $(DIRS_1), $(shell yes|$(CP) $(dir)/*.h $(INCLUDE_ROOT)/$(dir)) )
	@$(foreach dir, $(DIRS_1), $(shell yes|$(CP) $(dir)/*.hpp $(INCLUDE_ROOT)/$(dir)) )
#	-@yes|$(CP) BaseCode.h $(SOLUTION_ROOT)/include
	@$(RM) -f $(BIN_DIR)/lib$(PROJECT_NAME).so.*
	-@yes|$(CP) lib$(PROJECT_NAME).so.* $(BIN_DIR)
	-@cd $(BIN_DIR)
	-@yes|ln -s -f ./lib$(PROJECT_NAME).so.$(SO_INST_VERSION) $(BIN_DIR)/lib$(PROJECT_NAME).so
	-@cd $(LIB_DIR)
	-@yes|ln -s -f ./$(TARGETLIBS) $(LIB_DIR)/lib$(PROJECT_NAME).a
##	-$(CP) -f *.h $(INCLUDE_ROOT)
	
clean:
	-$(RM) $(ALL_OBJS) $(ALL_DEPS) $(TARGETLIBS) $(TARGETSLIBS) $(EXECUTABLE) *~ $(LIB_DIR)/$(TARGETLIBS) $(BIN_DIR)/$(TARGETSLIBS)

explain:
	@echo "The following information repressents you program:"
	@echo "Solution Dir          : $(SOLUTION_ROOT)"
	@echo "Project Dir           : $(PROJECT_ROOT)" 
	@echo "Temp dir              : $(TEMP_ROOT)"
	@echo "Final libs name       : $(TARGETLIBS) $(TARGETSLIBS)"
	@echo "Source dir            : $(DIRS)"
	@echo "Source files          : $(SRCS)" 
	@echo "Object files          : $(LIB_OBJS)"
	@echo "Dependency files      : $(DEPS)"
	@echo "Final executable name : $(EXECUTABLE)"
	@echo "Test source dir       : $(TEST_DIRS)"
	@echo "Test Source files     : $(TEST_SRCS)"
	@echo "Test Object files     : $(TEST_EXE_OBJS)"
	@echo "Test Dependency files : $(TEST_DEPS)"

depend: $(ALL_DEPS)
	@echo "Dependencies are now up-to-date."

-include $(ALL_DEPS)



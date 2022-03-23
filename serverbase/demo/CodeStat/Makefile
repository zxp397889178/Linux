PROJECT_ROOT= $(shell pwd)
PROJECT_NAME=codestat
SOLUTION_ROOT= $(shell cd $(PROJECT_ROOT)/../.. && pwd && cd $(PROJECT_ROOT))
TEMP_ROOT= $(SOLUTION_ROOT)/temp/$(PROJECT_NAME)
INCLUDE_ROOT=  $(SOLUTION_ROOT)/include
LIB_DIR= $(SOLUTION_ROOT)/lib
BIN_DIR= $(SOLUTION_ROOT)/bin
MYSQL_INC=/usr/include/mysql
MYSQL_LIB=/usr/lib64/mysql

EXECUTABLE=codestat

### C compiler flags
CC=gcc
CFLAGS=-Wall -O2 -fPIC -ggdb3 -I$(INCLUDE_ROOT) -I$(MYSQL_INC) 

### C++ compiler flags
CXX=g++
CPPFLAGS = 
CXXFLAGS = $(CPPFLAGS) $(CFLAGS)

### link flags
LINKCC=$(CC)
ARFLAGS=rc
XLDFLAGS=-lstdc++ -ldl -lutil -lpthread -liconv -lpcrecpp -lm -lz -lssl -lcrypto -lcurl
LDLIBS += -L . -L$(LIB_DIR) -lNetWork -lSecurity -lDatabase -lBaseCode -L$(MYSQL_LIB) -lmysqlclient -lhiredis -lrt

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
EXE_OBJS  := $(subst ./,,$(OBJS_2))
DEPS      := $(patsubst %.o, %.d, $(EXE_OBJS))

ALL_DIRS := $(DIRS)
ALL_SRCS := $(SRCS)
ALL_OBJS := $(EXE_OBJS)
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

all: $(EXECUTABLE) 

$(EXECUTABLE): $(ALL_OBJS)
#	$(LINKCC) $(CXXFLAGS) $(XLDFLAGS) -o $@ $(TEMP_ROOT)/$@.o $(LDLIBS)
	@$(LINKCC) $(subst $(DIRS), $(TEMP_ROOT), $^) $(CXXFLAGS) $(XLDFLAGS) -o $@ $(LDLIBS)

INCLUDE_DIRS := $(subst $(PROJECT_ROOT), $(INCLUDE_ROOT), $(DIRS))
install: install1 
install1:
	-@yes|$(CP) $(EXECUTABLE) $(BIN_DIR)
	@echo "install $(EXECUTABLE) to $(BIN_DIR) succeed."
	
clean:
	-$(RM) $(ALL_OBJS) $(ALL_DEPS) $(EXECUTABLE) *~

explain:
	@echo "The following information repressents you program:"
	@echo "Solution Dir          : $(SOLUTION_ROOT)"
	@echo "Project Dir           : $(PROJECT_ROOT)" 
	@echo "Temp dir              : $(TEMP_ROOT)"
	@echo "Source dir            : $(DIRS)"
	@echo "Include path          : $(INCLUDE_ROOT)" 
	@echo "Lib path              : $(LIB_DIR)" 
	@echo "Source files          : $(SRCS)" 
	@echo "Dependency files      : $(DEPS)"
	@echo "Final executable name : $(EXECUTABLE)"

depend: $(ALL_DEPS)
	@echo "Dependencies are now up-to-date."

-include $(ALL_DEPS)



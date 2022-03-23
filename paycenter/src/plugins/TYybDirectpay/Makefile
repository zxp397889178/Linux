PROJECT_ROOT=$(shell pwd)
PROJECT_NAME=plugin_pay_$(shell pwd | awk -F '/' '{print $$NF}')
TEMP_ROOT=$(SOLUTION_ROOT)/temp/paycenter/$(PROJECT_NAME)

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
SO_NEXT_VERSION = $(MAJOR_VER).$(MINOR_VER).$(NEXT_BUILD_VER)
SO_CUR_VERSION = $(MAJOR_VER).$(MINOR_VER).$(BUILD_VER)
SO_BUILD_VERSION = $(SO_NEXT_VERSION)-$(SVN_VER).$(KERNEL_VER).$(PROCESS_VER)
SO_INST_VERSION = $(SO_CUR_VERSION)-$(SVN_VER).$(KERNEL_VER).$(PROCESS_VER)

## 判断代码是否有修改或新增
SOURCE_CHANGE_COUNT = $(shell svn st | grep -E '\.cpp|\.C|\.h|\.cc' | grep -E '^M|^A|^R' | wc -l )
ifneq ($(SOURCE_CHANGE_COUNT), 0)
SOURCE_CHANGED := 1
endif
#SOURCE_CHANGE_COUNT = $(shell let v1=$(SOURCE_CHANGE_1); echo $$v1)


########################################################################################################################
### test files define
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
OBJS_1 := $(subst $(PROJECT_ROOT)/,,$(OBJS))
OBJS_2 := $(foreach obj, $(OBJS_1), $(TEMP_ROOT)/$(obj))

ALL_DIRS := $(DIRS)
ALL_OBJS := $(subst ./,,$(OBJS_2)) 
ALL_DEPS := $(patsubst %.o, %.d, $(ALL_OBJS))

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
all: buildver plugin
buildver:
	@echo "================================================================="
	@echo "<< $(PROJECT_NAME) >> source has changed, increase version number."
	@echo "================================================================="
	@echo "source file has changed."
	@echo $(SO_NEXT_VERSION) > $(VERSION_FILE)
	# @svn commit -m "build $(PROJECT_NAME), version $(SO_NEXT_VERSION)"
	# @svn update
	@$(RM) -f ../$(PROJECT_NAME).so.*

plugin: $(ALL_OBJS) $(COMMON_OBJS)
	@echo ">>>>>> begin to compile $(PROJECT_NAME)..."
	$(foreach f, $(wildcard ../$(PROJECT_NAME).so.*), $(shell $(RM) $f)) 
	$(LINKCC) $^ $(CXXFLAGS) $(DBPLUGIN_LDFLAGS) -o ../$(PROJECT_NAME).so.$(SO_INST_VERSION) $(LDLIBS) -lmysqlclient -lhiredis -lssl -lcrypto -lcurl

clean:
	-$(RM) $(ALL_OBJS) $(ALL_DEPS) ../$(PROJECT_NAME).so.* *~

explain:
	@echo "The following information repressents you program:"
	@echo "Solution Dir : $(SOLUTION_ROOT)"
	@echo "Project Dir  : $(PROJECT_ROOT)" 
	@echo "Temp dir     : $(TEMP_ROOT)"
	@echo "Source dir   : $(DIRS)"
	@echo "Source files : $(SRCS)"
	@echo "Object files : $(ALL_OBJS)"
	@echo "Dependency files : $(ALL_DEPS)"
	@echo ".so current ver  : $(SO_CUR_VERSION)"
	@echo ".so next ver     : $(SO_NEXT_VERSION)"
	@echo ".so build ver    : $(SO_BUILD_VERSION)"
	@echo ".so install ver  : $(SO_INST_VERSION)"
	@echo "svn ver          : $(SVN_VER)"

depend: $(ALL_DEPS)
	@echo "Dependencies are now up-to-date."

-include $(ALL_DEPS)

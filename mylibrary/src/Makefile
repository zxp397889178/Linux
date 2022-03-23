## 以下为构建需要编译的子目录

# EXCLUDE_DIRS:=QWFrame
# PROJECT_DIRS :=$(shell find . -maxdepth 1 -type d)
# PROJECT_DIRS :=$(basename $(patsubst ./%,%,$(PROJECT_DIRS)))
# PROJECT_DIRS :=$(filter-out $(EXCLUDE_DIRS), $(PROJECT_DIRS))
# ALL_PROJECT  =$(PROJECT_DIRS)
ALL_PROJECT = BaseCode Security Network ProtoBase Database DbAdapter 

CHANGE_SOURCE := $(shell git status -s ./ | grep -E "^ M|^ A|^ R" | awk '{print $$2}')
CHANGE_SOURCE_COUNT := $(shell git status -s ./ | grep -E "^ M|^ A|^ R" | wc -l )

all: $(ALL_PROJECT) 

$(ALL_PROJECT): ECHO
	@make -C $@

ECHO:
	@echo "begin compile project $(ALL_PROJECT)..."

clean:
	$(foreach p, $(ALL_PROJECT), $(shell make -C $(p) clean) )

explain:
	@echo "all project : $(ALL_PROJECT)"

install:
	$(foreach p, $(ALL_PROJECT), $(shell make -C $(p) install) )

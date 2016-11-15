#
# Misc init stuff..
#
d := $(shell pwd)
BUILD_DIR := builds
BIN_DIR := $(BUILD_DIR)/bin
VENV_DIR := $(BUILD_DIR)/venv
VENV_REQUIREMENTS := $(d)/tools/requirements.txt

#
# Project settings & targets
#
INCLUDES := $(d)/include
SRCS := $(wildcard src/*.cpp)
LIBS := -lc++ -lm -lSystem
TARGET := $(BIN_DIR)/lockfree
CLEAN := $(d)/logs/*

#
# Tool settings
#
CXX := clang -x c++
LD := clang++
LDFLAGS :=  -arch x86_64
CXXFLAGS := -x c++ -O3 -g -Wall -arch x86_64 -fmessage-length=0 -std=c++14 -stdlib=libc++ -c


#
# Continue on with the build rules
#
OBJS := $(addprefix $(BUILD_DIR)/,$(SRCS:.cpp=.o))
CXXFLAGS += $(foreach inc,$(INCLUDES),$(addprefix -I,$(inc)))

define \n


endef

# colours
t_black := $(shell tput setaf 0)
t_red := $(shell tput setaf 1)
t_green := $(shell tput setaf 2)
t_yellow := $(shell tput setaf 3)
t_blue := $(shell tput setaf 4)
t_megenta := $(shell tput setaf 5)
t_cyan := $(shell tput setaf 6)
t_white := $(shell tput setaf 7)

t_normal := $(shell tput sgr0)
t_bold := $(shell tput bold)
t_ul := $(shell tput smul)

t_error := ${t_red}
t_warn := ${t_yellow}
t_info := ${t_green}

t_CPP := ${t_blue}
t_LINK := ${t_red}
t_RUN := ${t_green}
t_CLEAN := ${t_yellow}
t_DEBUG := ${t_red}

# Fancy echo command
VERBOSE ?= 0
ifeq ("$(VERBOSE)","0")
  echo_cmd_no_quiet = echo "${t_bold}${t_$(firstword $(1))}$(firstword $(1))${t_normal} $(wordlist 2,$(words $1),$(1))";
  echo_cmd = @$(call echo_cmd_no_quiet,$1)
  _@ := @
else # Verbose output
  echo_cmd_no_quiet =
  echo_cmd =
  _@ :=
endif

# create a .o : .cpp rule
define CREATE_RULE_CPP
$(1) : $(2)
	@mkdir -p $(dir $1)
	$(call echo_cmd,CPP $(2)) $(CXX) -MD -MF $(patsubst %.o,%.d,$(1)) $(CXXFLAGS) $(2) -o $(1)
	${\n}
-include $(patsubst %.o,%.d,$(1))
endef

$(foreach obj,$(OBJS),$(eval $(call CREATE_RULE_CPP,$(strip $(obj)),$(subst $(BUILD_DIR)/,,$(patsubst %.o,%.cpp,$(obj))))))

# out basic target : object(s) rule
$(TARGET) : $(OBJS)
	@mkdir -p $(dir $@)
	$(call echo_cmd,LINK $(TARGET)) $(LD) $(LDFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

#
# defaul goals
#
all : $(TARGET)
.DEFAULT_GOAL := all

ALL_CLEAN := $(TARGET) $(OBJS) $(CLEAN)
clean:
	$(call echo_cmd,CLEAN $(ALL_CLEAN)) rm -f $(ALL_CLEAN)


run: $(TARGET)
	$(call echo_cmd,RUN $(TARGET)) $(TARGET) | tee $(d)/logs/output.txt

debug: $(TARGET)
	$(call echo_cmd,DEBUG $(TARGET)) lldb $(TARGET)

$(VENV_DIR) : $(VENV_REQUIREMENTS)
	@mkdir -p $(VENV_DIR)
	$(call echo_cmd,VIRTUALENV Creating $(VENV_DIR)...) virtualenv -p python3 $(VENV_DIR)
	$(call echo_cmd,VIRTUALENV Installing $(VENV_REQUIREMENTS)...) source $(VENV_DIR)/bin/activate; pip install -r $(VENV_REQUIREMENTS)

venv : ($VENV_DIR)

graph: $(VENV_DIR)
	$(call echo_cmd,GRAPH Generating from $(d)/logs/output.txt) \
		source $(VENV_DIR)/bin/activate; \
		python3 $(d)/tools/graph.py --input $(d)/logs/output.txt --output $(BUILD_DIR)/output.html

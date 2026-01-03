# ==================================
# Project Root & Common
# ==================================
PROJECT_ROOT := ../..
include $(PROJECT_ROOT)/common.mk

# ==================================
# Output
# ==================================
TARGET_DIR := $(PROJECT_ROOT)/lib/common
TARGET     := $(TARGET_DIR)/libcommon.a

# ==================================
# Directories
# ==================================
SRC_DIR := .
OBJ_DIR := $(PROJECT_ROOT)/obj/common
INC_DIR := $(PROJECT_ROOT)/include
LIB_DIR := $(PROJECT_ROOT)/lib

# ==================================
# Compiler / Archiver
# ==================================
CXX 			:= $(COMMON_CXX)
AR  			:= ar

CXXFLAGS 		:= $(COMMON_CXXFLAGS)
ARFLAGS  		:= rcs

# LFLAGS		 	:= $(COMMON_LFLAGS)
# LDFLAGS      	:= -pthread \
#              	   -Wl,-rpath=$(LIB_DIR)/slog \
#              	   -Wl,-rpath=$(LIB_DIR)/mysql \
#              	   -Wl,-rpath=$(LIB_DIR)/hiredis

# ==================================
# Includes
# ==================================
INCLUDES := \
	-I$(INC_DIR) \
	-I$(INC_DIR)/COMMON \
	-I$(INC_DIR)/COMMON/packet \
	-I$(INC_DIR)/COMMON/slog

# ==================================
# Sources
# ==================================
SRCS := \
	slog/slog.cpp \
	slog/K_slog.cpp \
	packet/PacketParser.cpp \
	packet/PacketFactory.cpp

# ==================================
# Objects / Deps
# ==================================
OBJS := $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# ==================================
# Rules
# ==================================
.PHONY: all clean fclean re

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo " [AR] $@"
	@mkdir -p $(TARGET_DIR)
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ 
#$(LIB_PATH) $(LFLAGS) $(LDFLAGS)

# ==================================
# Clean
# ==================================
clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(TARGET)

re: fclean all

# ==================================
# Dependency
# ==================================
-include $(DEPS)

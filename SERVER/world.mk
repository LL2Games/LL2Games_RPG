# ==================================
# Build Config
# ==================================
CXX					= g++
CXXFLAGS			= -std=c++17 -O2 -Wall -Wextra -Werror -MMD -MP -g3
INC_DIR				= include
INCLUDES_COMMON		= -I$(INC_DIR)/COMMON -I$(INC_DIR)/COMMON/packet
INCLUDES_WORLD 		= -I$(INC_DIR)/WORLD -I$(INC_DIR)/WORLD/core -I$(INC_DIR)/WORLD/db -I$(INC_DIR)/WORLD/packet 

PROJECT_ROOT		= $(shell pwd)
LFALGS				= -lmysqlclient -lslog
LDFLAGS				= -Wl,-rpath=$(PROJECT_ROOT)/lib/slog -Wl,-rpath=$(PROJECT_ROOT)/lib/mysql -pthread

LIB_INCLUDE			= -I./include/slog

LIB_SLOG_PATH		= ./lib/slog
LIB_MYSQL_PATH		= ./lib/mysql
LIB_PATH			= -L$(LIB_SLOG_PATH) -L$(LIB_MYSQL_PATH)

BIN_DIR				= bin

LOG_DIR				= logs/world

WORLDD		= $(BIN_DIR)/worldD

TARGETS		= $(WORLDD) 
# ==================================
# Library Structure
# ==================================
LIB_SLOG_DIR		= ./lib/slog
OBJ_DIR_SLOG		= obj/slog

LIB_SLOG			= $(LIB_SLOG_DIR)/libslog.so
SRCS_SLOG			= $(LIB_SLOG_DIR)/slog.cpp

OBJS_SLOG			= $(SRCS_SLOG:$(LIB_SLOG_DIR)/%.cpp=$(OBJ_DIR_SLOG)/%.o)

# ==================================
# Directory Structure
# ==================================
SRC_DIR				= src

OBJ_DIR_COMMON		= obj/common
OBJ_DIR_WORLD		= obj/world

# subdirectories
COMMON_DIR = $(SRC_DIR)/COMMON
COMMON_PACKET_DIR = $(COMMON_DIR)/packet

WORLD_DIR = $(SRC_DIR)/WORLD
WORLD_CORE_DIR = $(WORLD_DIR)/core
WORLD_DB_DIR = $(WORLD_DIR)/db
WORLD_PACKET_DIR = $(WORLD_DIR)/packet

# ==================================
# Source Files
# ==================================
SRCS_COMMON =\
	$(COMMON_DIR)/K_slog.cpp \
	\
	$(COMMON_PACKET_DIR)/PacketParser.cpp\
	$(COMMON_PACKET_DIR)/PacketFactory.cpp

SRCS_WORLD =\
	$(WORLD_CORE_DIR)/WorldServer.cpp \
	\
	$(WORLD_PACKET_DIR)/CharacterHandler.cpp \
	\
	$(WORLD_DIR)/main.cpp


# ==================================
# Object / Dep Files
# ==================================
OBJS_COMMON = $(SRCS_COMMON:%.cpp=$(OBJ_DIR_COMMON)/%.o)
DEPS_COMMON = $(OBJS_COMMON:.o=.d)

OBJS_WORLD = $(SRCS_WORLD:%.cpp=$(OBJ_DIR_WORLD)/%.o)
DEPS_WORLD = $(OBJS_WORLD:.o=.d)

# ==================================
# Build Rules
# ==================================

all: $(TARGETS)

$(BIN_DIR):
	@echo " [MKDIR] $@"
	@mkdir -p $@

$(LOG_DIR):
	@echo " [MKDIR] $@"
	@mkdir -p $@

$(LIB_SLOG): $(OBJS_SLOG)
	@echo " [LD] $@"
	$(CXX) -shared -o $@ $^

$(WORLDD) : $(OBJS_COMMON) $(OBJS_WORLD) | $(LIB_SLOG) $(BIN_DIR) $(LOG_DIR)
	@echo " [LD] $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES_COMMON) $(INCLUDES_WORLD) -o $@ $^ $(LIB_INCLUDE) $(LIB_PATH) $(LFALGS) $(LDFLAGS)

# 각각의 .cpp -> obj/*.o 규칙
$(OBJ_DIR_SLOG)/%.o: $(LIB_SLOG_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) -fPIC $(LIB_INCLUDE) -c $< -o $@

$(OBJ_DIR_COMMON)/%.o:%.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES_COMMON) $(LIB_INCLUDE) $(LIB_PATH) $(LFALGS) $(LDFLAGS)

$(OBJ_DIR_WORLD)/%.o:%.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) $(INCLUDES_COMMON) $(INCLUDES_WORLD) -c $< -o $@ $(LIB_INCLUDE) $(LIB_PATH) $(LFALGS) $(LDFLAGS)

# ==================================
# Utility
# ==================================
lib : $(LIB_SLOG)

world : $(WORLDD)

clean:
	rm -rf $(OBJ_DIR_WORLD) $(OBJ_DIR_COMMON) $(OBJ_DIR_SLOG)

fclean: clean
	rm -rf $(TARGETS)

re : fclean all

# 자동 의존성 include
 -include $(DEPS)

 .PHONY: all clean fclean re lib world

#-MMD  → .d 의존성 생성
#-MP   → 의존성 타겟 더미 생성해 빌드 오류 방지
#-include $(DEPS) → 생성된 .d 불러오기
#-O2   → 최적화(성능)
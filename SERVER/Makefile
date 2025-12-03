# ==================================
# Build Config
# ==================================
CXX					= g++
CXXFLAGS			= -std=c++17 -O2 -Wall -Wextra -Werror -MMD -MP
INC_DIR				= include
INCLUDES_COMMON		= -I$(INC_DIR)/COMMON -I$(INC_DIR)/COMMON/packet
INCLUDES_MAIN		= -I$(INC_DIR)/MAIN/daemon -I$(INC_DIR)/MAIN/manager -I$(INC_DIR)/MAIN/monitor
INCLUDES_LOGIN 		= -I$(INC_DIR)/LOGIN
INCLUDES_CHAT 		= -I$(INC_DIR)/CHAT

PROJECT_ROOT		= $(shell pwd)
LFALGS				= -lmysqlclient -lslog
LDFLAGS				= -Wl,-rpath=$(PROJECT_ROOT)/lib/slog -Wl,-rpath=$(PROJECT_ROOT)/lib/mysql -pthread

LIB_INCLUDE			= -I./include/slog

LIB_SLOG_PATH		= ./lib/slog
LIB_MYSQL_PATH		= ./lib/mysql
LIB_PATH			= -L$(LIB_SLOG_PATH) -L$(LIB_MYSQL_PATH)

BIN_DIR				= bin

LOG_DIR				= logs/main logs/login logs/chat

MAIND		= $(BIN_DIR)/mainD
LOGIND		= $(BIN_DIR)/loginD
CHATD		= $(BIN_DIR)/chatD

TARGETS		= $(MAIND) $(LOGIND) $(CHATD)
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
SRC_DIR_MAIN		= src/MAIN
SRC_DIR_LOGIN 		= src/LOGIN
SRC_DIR_CHAT 		= src/CHAT

OBJ_DIR_COMMON		= obj/common
OBJ_DIR_MAIN		= obj/main
OBJ_DIR_LOGIN		= obj/login
OBJ_DIR_CHAT		= obj/chat

# subdirectories
COMMON_DIR = $(SRC_DIR)/COMMON

DAEMON_DIR	= $(SRC_DIR_MAIN)/daemon
MANAGER_DIR = $(SRC_DIR_MAIN)/manager
MONITOR_DIR = $(SRC_DIR_MAIN)/monitor

LOGIN_CORE_DIR = $(SRC_DIR_LOGIN)/core
LOGIN_DB_DIR = $(SRC_DIR_LOGIN)/db
LOGIN_PACKET_DIR = $(SRC_DIR_LOGIN)/packet
LOGIN_UTIL_DIR = $(SRC_DIR_LOGIN)/util

CHAT_CORE_DIR = $(SRC_DIR_CHAT)/core
CHAT_DB_DIR = $(SRC_DIR_CHAT)/db
CHAT_PACKET_DIR = $(SRC_DIR_CHAT)/packet
CHAT_UTIL_DIR = $(SRC_DIR_CHAT)/util

# ==================================
# Source Files
# ==================================
SRCS_COMMON =\
	$(COMMON_DIR)/K_slog.cpp

SRCS_MAIN =\
	$(DAEMON_DIR)/BaseDaemon.cpp \
	$(DAEMON_DIR)/ChatDaemon.cpp \
	$(DAEMON_DIR)/LoginDaemon.cpp \
	$(DAEMON_DIR)/DaemonFactory.cpp \
	\
	$(MANAGER_DIR)/ProcessManager.cpp \
	\
	$(MONITOR_DIR)/ProcessMonitor.cpp \
	\
	$(SRC_DIR_MAIN)/main.cpp

SRCS_LOGIN =\
	$(LOGIN_CORE_DIR)/Server.cpp \
	\
	$(LOGIN_DB_DIR)/MySQLManager.cpp \
	\
	$(LOGIN_PACKET_DIR)/LoginHandler.cpp \
	$(LOGIN_PACKET_DIR)/PacketFactory.cpp \
	$(LOGIN_PACKET_DIR)/PacketParser.cpp \
	\
	$(LOGIN_UTIL_DIR)/StringUtil.cpp\
	\
	$(SRC_DIR_LOGIN)/main.cpp

SRCS_CHAT =\
	$(CHAT_CORE_DIR)/Server.cpp \
	$(CHAT_CORE_DIR)/Client.cpp \
	\
	$(CHAT_DB_DIR)/MySQLManager.cpp \
	\
	$(CHAT_PACKET_DIR)/ChatInitHandler.cpp \
	$(CHAT_PACKET_DIR)/ChatMsgHandler.cpp \
	$(CHAT_PACKET_DIR)/PacketFactory.cpp \
	$(CHAT_PACKET_DIR)/PacketParser.cpp \
	\
	$(CHAT_UTIL_DIR)/StringUtil.cpp\
	\
	$(SRC_DIR_CHAT)/main.cpp


# ==================================
# Object / Dep Files
# ==================================
OBJS_COMMON = $(SRCS_COMMON:%.cpp=$(OBJ_DIR_COMMON)/%.o)
DEPS_COMMON = $(OBJS_COMMON:.o=.d)

OBJS_MAIN = $(SRCS_MAIN:%.cpp=$(OBJ_DIR_MAIN)/%.o)
DEPS_MAIN = $(OBJS_MAIN:.o=.d)

OBJS_LOGIN = $(SRCS_LOGIN:%.cpp=$(OBJ_DIR_LOGIN)/%.o)
DEPS_LOGIN = $(OBJS_LOGIN:.o=.d)

OBJS_CHAT = $(SRCS_CHAT:%.cpp=$(OBJ_DIR_CHAT)/%.o)
DEPS_CHAT = $(OBJS_CHAT:.o=.d)

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

$(MAIND): $(OBJS_COMMON) $(OBJS_MAIN) | $(LIB_SLOG) $(BIN_DIR) $(LOG_DIR)
	@echo " [LD] $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES_COMMON) $(INCLUDES_MAIN) -o $@ $^ $(LIB_INCLUDE) $(LIB_PATH) $(LFALGS) $(LDFLAGS)

$(LOGIND) : $(OBJS_COMMON) $(OBJS_LOGIN) | $(LIB_SLOG) $(BIN_DIR) $(LOG_DIR)
	@echo " [LD] $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES_COMMON) $(INCLUDES_LOGIN) -o $@ $^ $(LIB_INCLUDE) $(LIB_PATH) $(LFALGS) $(LDFLAGS)

$(CHATD) : $(OBJS_COMMON) $(OBJS_CHAT) | $(LIB_SLOG) $(BIN_DIR) $(LOG_DIR)
	@echo " [LD] $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES_COMMON) $(INCLUDES_CHAT) -o $@ $^ $(LIB_INCLUDE) $(LIB_PATH) $(LFALGS) $(LDFLAGS)

# 각각의 .cpp -> obj/*.o 규칙
$(OBJ_DIR_SLOG)/%.o: $(LIB_SLOG_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) -fPIC $(LIB_INCLUDE) -c $< -o $@

$(OBJ_DIR_COMMON)/%.o:%.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES_COMMON) $(LIB_INCLUDE) $(LIB_PATH) $(LFALGS) $(LDFLAGS)

$(OBJ_DIR_MAIN)/%.o:%.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES_COMMON) $(INCLUDES_MAIN) -c $< -o $@ $(LIB_INCLUDE) $(LIB_PATH) $(LFALGS) $(LDFLAGS)

$(OBJ_DIR_LOGIN)/%.o:%.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES_COMMON) $(INCLUDES_LOGIN) -c $< -o $@ $(LIB_INCLUDE) $(LIB_PATH) $(LFALGS) $(LDFLAGS)

$(OBJ_DIR_CHAT)/%.o:%.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) $(INCLUDES_COMMON) $(INCLUDES_CHAT) -c $< -o $@ $(LIB_INCLUDE) $(LIB_PATH) $(LFALGS) $(LDFLAGS)

# ==================================
# Utility
# ==================================
lib : $(LIB_SLOG)

main : $(MAIND)

login : $(LOGIND)

chat : $(CHATD)

clean:
	rm -rf $(OBJ_DIR_MAIN) $(OBJ_DIR_COMMON) $(OBJ_DIR_LOGIN) $(OBJ_DIR_CHAT) $(OBJ_DIR_SLOG)

fclean: clean
	rm -rf $(TARGETS)

re : fclean all

# 자동 의존성 include
 -include $(DEPS)

 .PHONY: all clean fclean re lib main login chat 

#-MMD  → .d 의존성 생성
#-MP   → 의존성 타겟 더미 생성해 빌드 오류 방지
#-include $(DEPS) → 생성된 .d 불러오기
#-O2   → 최적화(성능)
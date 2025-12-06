CXX					= g++
CXXFLAGS			= -std=c++17 -O2 -Wall -Wextra -Werror -MMD -MP
INC_DIR				= ../../include

INCLUDES_COMMON		= -I$(INC_DIR)/COMMON -I$(INC_DIR)/COMMON/packet
INCLUDES_CHANNEL 	= -I$(INC_DIR)/CHANNEL

#실행 파일 
BIN_DIR				= bin

#라이브러리
PROJECT_ROOT		= $(shell pwd)
LFALGS				= -lmysqlclient #-lslog
LDFLAGS				= -Wl,-rpath=$(PROJECT_ROOT)/lib/slog -Wl,-rpath=$(PROJECT_ROOT)/lib/mysql -pthread

#LIB_SLOG_PATH		= ./lib/slog
LIB_MYSQL_PATH		= ./lib/mysql
#LIB_PATH			= -L$(LIB_SLOG_PATH) -L$(LIB_MYSQL_PATH)
LIB_PATH			= -L$(LIB_MYSQL_PATH)


CHANNELD			=$(BIN_DIR)/channelD


SRC_DIR				= src
SRC_DIR_CHANNEL		= src/db


LOGIN_DB_DIR = $(SRC_DIR_CHANNEL)/db

SRCS_CHANNEL =\
	ChannelServer.cpp \
	ChannelSession.cpp \
	MapManager.cpp \
	$(LOGIN_DB_DIR)/MySQLManager.cpp \
	$(LOGIN_DB_DIR)/RedisClient.cpp \
	PlayerManager.cpp \
	../main.cpp


OBJS_CHANNEL = $(SRCS_CHANNEL:%.cpp=$(OBJ_DIR_CHANNEL)/%.o)
DEPS_LOGIN = $(OBJS_CHANNEL:.o=.d)


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

$(CHANNELD) : $(OBJS_CHANNEL) | $(LIB_SLOG) $(BIN_DIR) $(LOG_DIR)
	@echo " [LD] $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES_COMMON) $(INCLUDES_CHAT) -o $@ $^ $(LIB_INCLUDE) $(LIB_PATH) $(LFALGS) $(LDFLAGS)

$(OBJ_DIR_CHANNEL)/%.o:%.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) $(INCLUDES_COMMON) $(INCLUDES_CHANNEL) -c $< -o $@ $(LIB_INCLUDE) $(LIB_PATH) $(LFALGS) $(LDFLAGS)


channel : $(CHANNELD)

clean:
	rm -rf $(CHANNELD) $(OBJ_DIR_CHANNEL)

fclean: clean
	rm -rf $(TARGETS)

re : fclean all

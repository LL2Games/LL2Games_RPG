CXX					= g++
CXXFLAGS			= -std=c++17 -O2 -Wall -Wextra -Werror -MMD -MP
INC_DIR				= ../../include
OBJ_DIR				= ../../obj/channel
BIN_DIR				= ../../bin
LIB_DIR				= ../../lib

INCLUDES			= -I$(INC_DIR)

# 라이브러리
PROJECT_ROOT		= $(shell cd ../.. && pwd)
LIB_MYSQL_PATH		= $(PROJECT_ROOT)/lib/mysql

LDFLAGS				= -L$(LIB_MYSQL_PATH) -lmysqlclient -Wl,-rpath=$(LIB_MYSQL_PATH) -pthread

# 실행 파일
CHANNELD			= $(BIN_DIR)/channelD

# 소스 파일
CORE_SRCS =\
	core/ChannelServer.cpp \
	core/ChannelSession.cpp \
	core/MapManager.cpp \
	core/PlayerManager.cpp \
	core/Player.cpp

DB_SRCS =\
	db/MySqlConnectionPool.cpp \
	db/RedisClient.cpp

MAIN_SRC = main.cpp

ALL_SRCS = $(CORE_SRCS) $(DB_SRCS) $(MAIN_SRC)

# 오브젝트 파일
OBJS = $(ALL_SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)


.PHONY: all clean

all: $(CHANNELD)

$(CHANNELD): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@echo " [LD] $@"
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(CHANNELD)

-include $(DEPS)

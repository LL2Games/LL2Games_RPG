CXX					= g++
CXXFLAGS			= -std=c++17 -O2 -Wall -Wextra -Werror -MMD -MP
INC_DIR				= ../../include
OBJ_DIR				= ../../obj/channel
BIN_DIR				= ../../bin
LIB_DIR				= ../../lib

INCLUDES			= -I$(INC_DIR) -I$(INC_DIR)/slog

# 라이브러리
PROJECT_ROOT		= $(shell cd ../.. && pwd)
LIB_MYSQL_PATH		= $(PROJECT_ROOT)/lib/mysql
LIB_SLOG_PATH		= $(PROJECT_ROOT)/lib/slog

LIB_INCLUDE			= -I../../include/slog
LIB_SLOG_DIR		= ../../lib/slog
OBJ_DIR_SLOG		= ../../obj/slog

LIB_SLOG			= $(LIB_SLOG_DIR)/libslog.so
SRCS_SLOG			= $(LIB_SLOG_DIR)/slog.cpp

OBJS_SLOG			= $(SRCS_SLOG:$(LIB_SLOG_DIR)/%.cpp=$(OBJ_DIR_SLOG)/%.o)


LDFLAGS				= -Wl,-rpath=$(PROJECT_ROOT)/lib/slog -L$(LIB_SLOG_PATH) -lslog -Wl,-rpath=$(PROJECT_ROOT)/lib/mysql -L$(LIB_MYSQL_PATH) -lmysqlclient -pthread

# 실행 파일
CHANNELD			= $(BIN_DIR)/channelD

# 소스 파일
CORE_SRCS =\
	core/ChannelServer.cpp \
	core/ChannelSession.cpp \
	core/MapManager.cpp \
	core/MapInstance.cpp \
	core/PlayerManager.cpp \
	core/Player.cpp 

DB_SRCS =\
	db/MySqlConnectionPool.cpp \
	db/PlayerService.cpp \
	db/RedisClient.cpp \
	db/RRedisUtility.cpp

PACKET_SRCS =\
	../COMMON/packet/PlayerHandler.cpp \
	../COMMON/packet/PacketFactory.cpp \
	../COMMON/packet/PacketParse.cpp

COMMON_SRCS =\
	../COMMON/K_slog.cpp

MAIN_SRC = main.cpp

ALL_SRCS = $(CORE_SRCS) $(DB_SRCS) $(PACKET_SRCS) $(COMMON_SRCS) $(MAIN_SRC)

# 오브젝트 파일
OBJS = $(ALL_SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)


.PHONY: all clean

all: $(CHANNELD)

$(CHANNELD): $(OBJS) $(LIB_SLOG)
	@mkdir -p $(BIN_DIR)
	@echo " [LD] $@"
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIB_INCLUDE) -c $< -o $@

$(LIB_SLOG): $(OBJS_SLOG)
	@echo " [LD] $@"
	$(CXX) -shared -o $@ $^

$(OBJ_DIR_SLOG)/%.o: $(LIB_SLOG_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) -fPIC $(LIB_INCLUDE) -c $< -o $@


clean:
	rm -rf $(OBJ_DIR) $(CHANNELD)

-include $(DEPS)

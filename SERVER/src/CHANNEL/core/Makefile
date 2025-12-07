CXX					= g++
CXXFLAGS			= -std=c++17 -O2 -Wall -Wextra -Werror -MMD -MP
INC_DIR				= ../../../include
OBJ_DIR				= ../../../obj/channel
BIN_DIR				= ../../../bin
LIB_DIR				= ../../../lib

INCLUDES			= -I$(INC_DIR)

# 라이브러리
PROJECT_ROOT		= $(shell cd ../../.. && pwd)
LIB_MYSQL_PATH		= $(PROJECT_ROOT)/lib/mysql
LIB_CHANNEL_DB		= $(LIB_DIR)/channel/db/libchannel_db.a

LDFLAGS				= -L$(LIB_MYSQL_PATH) -lmysqlclient -Wl,-rpath=$(LIB_MYSQL_PATH) -pthread

# 실행 파일
CHANNELD			= $(BIN_DIR)/channelD

# 소스 파일
SRCS =\
	ChannelServer.cpp \
	ChannelSession.cpp \
	MapManager.cpp \
	PlayerManager.cpp

# 오브젝트 파일
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/core/%.o)
DEPS = $(OBJS:.o=.d)


.PHONY: all clean

all: $(CHANNELD)

$(CHANNELD): $(OBJS) $(LIB_CHANNEL_DB)
	@mkdir -p $(BIN_DIR)
	@echo " [LD] $@"
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIB_CHANNEL_DB) $(LDFLAGS)

$(OBJ_DIR)/core/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)/core $(CHANNELD)

-include $(DEPS)

# ==================================
# Build Config
# ==================================
CXX			= g++
CXXFLAGS	= -std=c++17 -O2 -Wall -Wextra -Werror -MMD -MP
INCLUDES	= -Iinclude/daemon -Iinclude/manager -Iinclude/monitor

TARGET		= mainD

# ==================================
# Directory Structure
# ==================================
SRC_DIR		= src
INC_DIR		= include
OBJ_DIR		= obj

# subdirectories
DAEMON_DIR	= $(SRC_DIR)/daemon
MANAGER_DIR = $(SRC_DIR)/manager
MONITOR_DIR = $(SRC_DIR)/monitor

# ==================================
# Source Files
# ==================================
SRCS =\
	$(DAEMON_DIR)/BaseDaemon.cpp \
	$(DAEMON_DIR)/ChatDaemon.cpp \
	$(DAEMON_DIR)/LoginDaemon.cpp \
	$(DAEMON_DIR)/DaemonFactory.cpp \
	\
	$(MANAGER_DIR)/ProcessManager.cpp \
	\
	$(MONITOR_DIR)/ProcessMonitor.cpp \
	\
	$(SRC_DIR)/main.cpp

# ==================================
# Object / Dep Files
# ==================================
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

# ==================================
# Build Rules
# ==================================

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo " [LD] $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# 각각의 .cpp -> obj/*.o 규칙
$(OBJ_DIR)/%.o:%.cpp
	@mkdir -p $(dir $@)
	@echo " [CXX] $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ==================================
# Utility
# ==================================
clean:
	rm -rf $(OBJ_DIR) 

fclean: clean
	rm -rf $(TARGET)

re : clean all

# 자동 의존성 include
 -include $(DEPS)

 .PHONY: all clean fclean re

#-MMD  → .d 의존성 생성
#-MP   → 의존성 타겟 더미 생성해 빌드 오류 방지
#-include $(DEPS) → 생성된 .d 불러오기
#-O2   → 최적화(성능)
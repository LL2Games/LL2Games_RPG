CXX					= g++
CXXFLAGS			= -std=c++17 -O2 -Wall -Wextra -Werror -MMD -MP
AR					= ar
ARFLAGS				= rcs


SRC_DIR				= .
OBJ_DIR				= ../../../obj/channel/db
LIB_DIR				= ../../../lib/channel/db
INC_DIR				= ../../../include


TARGET				= $(LIB_DIR)/libchannel_db.a


SRCS				= $(wildcard $(SRC_DIR)/*.cpp)
OBJS				= $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS				= $(OBJS:.o=.d)

# Include paths
INCLUDES			= -I$(INC_DIR)/CHANNEL/db -I$(INC_DIR)/COMMON

# Build rules
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^
	@echo "Created static library: $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

-include $(DEPS)




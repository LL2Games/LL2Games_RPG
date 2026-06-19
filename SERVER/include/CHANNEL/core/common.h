#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <thread>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h> // TCP_NODELAY는 이 헤더가 필수입니다!
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <vector>
#include <deque>
#include "K_slog.h"

#include "COMMON/packet/Packet.h"

// DB 헤더들
#include <mysql/mysql.h>
#include <hiredis/hiredis.h>


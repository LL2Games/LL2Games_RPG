#pragma once

// ============================================================
// C 표준 라이브러리
// ============================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// ============================================================
// C++ 표준 라이브러리
// ============================================================
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdint>
#include <vector>
#include <deque>
#include <map>
#include <unordered_map>

// ============================================================
// Linux / POSIX 시스템 헤더
// ============================================================
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

// ============================================================
// Linux 네트워크 / 소켓 헤더
// ============================================================
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/epoll.h>

// ============================================================
// 외부 라이브러리 헤더
// ============================================================
// MySQL C API
#include <mysql/mysql.h>

// Redis C Client
#include <hiredis/hiredis.h>

// ============================================================
// 프로젝트 공통 헤더
// ============================================================
// Log
#include "slog/K_slog.h"
// Packet
#include "COMMON/packet/Packet.h"

// ============================================================
// 공통 상수 정의
// ============================================================
#define MAX_BUF 1024
#define MAX_CLIENTS FD_SETSIZE
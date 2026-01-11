#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <vector>
#include <iostream>
#include <fcntl.h>
#include "slog/K_slog.h"


#define MAX_BUF 1024
#define MAX_CLIENTS FD_SETSIZE

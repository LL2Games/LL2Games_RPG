#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "slog/K_slog.h"

#include <vector>


#define MAX_BUF 1024
#define MAX_CLIENTS FD_SETSIZE

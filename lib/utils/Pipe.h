//
// Created by yanci on 14/04/23.
//

#ifndef HW1_PIPE_H
#define HW1_PIPE_H

#include <unistd.h>
#include <vector>
#include <sys/socket.h>
#include "../Map.h"

#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)

std::vector<int> createPipes(Map* map);

#endif //HW1_PIPE_H

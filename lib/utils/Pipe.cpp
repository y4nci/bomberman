//
// Created by yanci on 14/04/23.
//

#include "Pipe.h"

std::vector<int> createPipes(Map* map) {
    std::vector<int> fds;

    for (int i = 0; i < map->getBomberCount(); i++) {
        int fd[2];
        PIPE(fd);
        fds.push_back(fd[0]);
        map->getBombers()[i].setFd(fd[1]);
    }

    return fds;
}

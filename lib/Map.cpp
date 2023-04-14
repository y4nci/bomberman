/**
 * @file Map.cpp
 * @author Baran Yancı (e2449015)
 * @brief This file includes function definitions for the Map class.
 * @version 0.0.0
 * @date 2023-04-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Map.h"

Map::Map(int width, int height, std::vector<ObstacleInput> obstacleInputs, std::vector<BomberInput> bomberInputs) {
    int bomberId = 0;

    this->width = width;
    this->height = height;

    for (size_t i = 0; i < obstacleInputs.size(); i++) {
        Obstacle obstacle(obstacleInputs[i].x, obstacleInputs[i].y, obstacleInputs[i].durability);

        this->obstacles.push_back(obstacle);
    }

    for (size_t i = 0; i < bomberInputs.size(); i++) {
        Bomber bomber(bomberInputs[i].x, bomberInputs[i].y, bomberId, bomberInputs[i].argv);

        this->bombers.push_back(bomber);

        bomberId++;
    }
}

Map::~Map() {
    this->obstacles.clear();
    this->bombers.clear();
    this->bombs.clear();
}

int Map::getWidth() {
    return this->width;
}

int Map::getHeight() {
    return this->height;
}

int Map::getObstacleCount() {
    return this->obstacles.size();
}

int Map::getBomberCount() {
    return this->bombers.size();
}

std::vector<Obstacle> Map::getObstacles() {
    return this->obstacles;
}

std::vector<Bomber> Map::getBombers() {
    return this->bombers;
}

std::vector<Bomb> Map::getBombs() {
    return this->bombs;
}

void Map::setWidth(int width) {
    this->width = width;
}

void Map::setHeight(int height) {
    this->height = height;
}

void Map::setObstacles(std::vector<Obstacle> obstacles) {
    this->obstacles = (obstacles);
}

void Map::setBombers(std::vector<Bomber> bombers) {
    this->bombers = (bombers);
}

void Map::moveBomber(int id, int targetX, int targetY) {
    Bomber *bomber = NULL;

    for (size_t i = 0; i < this->bombers.size(); i++) {
        if (this->bombers[i].getId() == id) {
            bomber = &this->bombers[i];
        }
    }

    if (bomber == NULL) {
        return;
    }

    for (size_t i = 0; i < this->obstacles.size(); i++) {
        if (this->obstacles[i].getX() == targetX && this->obstacles[i].getY() == targetY) {
            return;
        }
    }

    for (size_t i = 0; i < this->bombers.size(); i++) {
        if (this->bombers[i].getX() == targetX && this->bombers[i].getY() == targetY && this->bombers[i].getId() != id) {
            return;
        }
    }

    if (targetX < 0 || targetX >= this->width || targetY < 0 || targetY >= this->height) {
        return;
    }

    if (targetX - bomber->getX() > 1 || targetX - bomber->getX() < -1 || targetY - bomber->getY() > 1 || targetY - bomber->getY() < -1) {
        return;
    }

    bomber->setX(targetX);
    bomber->setY(targetY);
}

void Map::setBombs(std::vector<Bomb> bombs) {
    this->bombs = (bombs);
}

int Map::plantBomb(int bomberId, int radius, int durability) {
    Bomber *bomber = NULL;
    Bomb bomb (0, 0,radius,durability);
    int fd;

    for (size_t i = 0; i < this->bombers.size(); i++) {
        if (this->bombers[i].getId() == bomberId) {
            bomber = &this->bombers[i];
        }
    }

    bomb.setX(bomber->getX());
    bomb.setY(bomber->getY());

    fd = createBombPipe(&bomb);

    this->bombs.push_back(bomb);

    return fd;
}

// OTHER FUNCTIONS

std::vector<int> createBomberPipes(Map* map) {
    std::vector<int> fds;

    for (int i = 0; i < map->getBomberCount(); i++) {
        int fd[2];
        PIPE(fd);
        fds.push_back(fd[0]);
        map->getBombers()[i].setFd(fd[1]);
    }

    return fds;
}

int createBombPipe(Bomb* bomb) {
    int fd[2];
    PIPE(fd);
    bomb->setFd(fd[1]);

    return fd[0];
}

void forkProcesses(Map* map, std::vector<int>* fds) {
    for (int i = 0; i < map->getBomberCount(); i++) {
        int pid = fork();
        if (pid == 0) { // child
            char** argv = new char*[map->getBombers()[i].getArgv().size() + 1];
            for (size_t j = 0; j < map->getBombers()[i].getArgv().size(); j++) {
                argv[j] = (char*) map->getBombers()[i].getArgv()[j].c_str();
            }
            dup2(map->getBombers()[i].getFd(), STDIN_FILENO);
            dup2(map->getBombers()[i].getFd(), STDOUT_FILENO);
            argv[map->getBombers()[i].getArgv().size()] = NULL;
            close((*fds)[i]); // CLOSE PARENT'S CHANNEL
            execv(argv[0], argv);
            send_message(map->getBombers()[i].getFd(), (om*) BOMBER_START);

            delete [] argv;
        } else {
            close(map->getBombers()[i].getFd()); // CLOSE CHILD'S CHANNEL
        }
    }
}

// forkbomb

bool isGameFinished(Map map) {
    return (map.getBomberCount() == 1);
}

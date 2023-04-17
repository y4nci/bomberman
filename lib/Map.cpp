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
#include "../logging.h"

// DEBUG
#include <iostream>
// DEBUG

/**
 * @brief Construct a new Map:: Map object
 *
 * @param width width of the map
 * @param height height of the map
 * @param obstacleInputs the vector of ObstacleInput objects
 * @param bomberInputs the vector of BomberInput objects
 */
Map::Map(int width, int height, std::vector<ObstacleInput> obstacleInputs, std::vector<BomberInput> bomberInputs) {
    int bomberId = 0;

    this->width = width;
    this->height = height;
    this->luckyWinnerId = -1;

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
    this->luckyWinnerId = -1;
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

int Map::getBombCount() {
    return this->bombs.size();
}

int Map::getLuckyWinnerId() {
    return this->luckyWinnerId;
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

void Map::setLuckyWinnerId(int luckyWinnerId) {
    this->luckyWinnerId = luckyWinnerId;
}

void Map::setObstacles(std::vector<Obstacle> obstacles) {
    this->obstacles = (obstacles);
}

void Map::setBombers(std::vector<Bomber> bombers) {
    this->bombers = (bombers);
}

/**
 * @brief this function returns an std::pair<int, std::vector<od>> object.\n
 * the pair includes: the number of objects (bombers, bombs, obstacles) that the bomber can see and a vector of od objects.\n
 * the bomber can see 3 block up, down, left and right of him. no diagonals\n
 * @param id the id of the bomber
 * @return std::pair<int, std::vector<od>> the pair includes: the number of objects (bombers, bombs, obstacles) that the bomber can see and a vector of od objects.
 */
std::pair<int, std::vector<od> > Map::seeBomber(int id) {
    Bomber *bomber = NULL;
    std::vector<od> objects;

    for (size_t i = 0; i < this->bombers.size(); i++) {
        if (this->bombers[i].getId() == id) {
            bomber = &this->bombers[i];
        }
    }

    if (bomber == NULL) {
        return std::make_pair(0, objects);
    }

    for (size_t i = 0; i < this->obstacles.size(); i++) {
        bool bomberCanSee = (
            (this->obstacles[i].getX() >= bomber->getX() - 3 && this->obstacles[i].getX() <= bomber->getX() + 3 && bomber->getY() == this->obstacles[i].getY()) ||
            (this->obstacles[i].getY() >= bomber->getY() - 3 && this->obstacles[i].getY() <= bomber->getY() + 3 && bomber->getX() == this->obstacles[i].getX())
        );

        if (bomberCanSee) {
            od obstacleData;
            coordinate coor;
            coor.x = this->obstacles[i].getX();
            coor.y = this->obstacles[i].getY();
            obstacleData.type = OBSTACLE;
            obstacleData.position = coor;

            objects.push_back(obstacleData);
        }
    }

    for (size_t i = 0; i < this->bombers.size(); i++) {
        bool bomberCanSee;

        if (!this->bombers[i].getIsAlive() || this->bombers[i].getId() == id) continue;

        bomberCanSee = (
                (this->bombers[i].getX() >= bomber->getX() - 3 && this->bombers[i].getX() <= bomber->getX() + 3 && bomber->getY() == this->bombers[i].getY()) ||
                (this->bombers[i].getY() >= bomber->getY() - 3 && this->bombers[i].getY() <= bomber->getY() + 3 && bomber->getX() == this->bombers[i].getX())
        );

        if (bomberCanSee) {
            od obstacleData;
            coordinate coor;
            coor.x = this->bombers[i].getX();
            coor.y = this->bombers[i].getY();
            obstacleData.type = BOMBER;
            obstacleData.position = coor;

            objects.push_back(obstacleData);
        }
    }

    for (size_t i = 0; i < this->bombs.size(); i++) {
        bool bomberCanSee;

        if (this->bombs[i].getIsExploded()) continue;

        bomberCanSee = (
            (this->bombs[i].getX() >= bomber->getX() - 3 && this->bombs[i].getX() <= bomber->getX() + 3 && bomber->getY() == this->bombs[i].getY()) ||
            (this->bombs[i].getY() >= bomber->getY() - 3 && this->bombs[i].getY() <= bomber->getY() + 3 && bomber->getX() == this->bombs[i].getX())
        );

        if (bomberCanSee) {
            od obstacleData;
            coordinate coor;
            coor.x = this->bombs[i].getX();
            coor.y = this->bombs[i].getY();
            obstacleData.type = BOMB;
            obstacleData.position = coor;

            objects.push_back(obstacleData);
        }
    }

    return std::make_pair(objects.size(), objects);
}

/**
 * @brief this function returns an std::pair<int, int> object.\n
 * the pair includes: the x and y coordinates of the bomber after the move.\n
 * if the move is not possible, the function returns the current position of the bomber.\n
 * @param id the id of the bomber
 * @param targetX the x coordinate of the target
 * @param targetY the y coordinate of the target
 * @return std::pair<int, int> the pair includes: the x and y coordinates of the bomber after the move.
 */
std::pair<int, int> Map::moveBomber(int id, int targetX, int targetY) {
    for (size_t i = 0; i < this->obstacles.size(); i++) {
        if (this->obstacles[i].getX() == targetX && this->obstacles[i].getY() == targetY) {
            return std::make_pair(this->bombers[id].getX(), this->bombers[id].getY());
        }
    }

    for (size_t i = 0; i < this->bombers.size(); i++) {
        if (!this->bombers[i].getIsAlive()) continue;
        if (this->bombers[i].getX() == targetX && this->bombers[i].getY() == targetY && this->bombers[i].getId() != id) {
            return std::make_pair(this->bombers[id].getX(), this->bombers[id].getY());
        }
    }

    if (targetX < 0 || targetX >= this->width || targetY < 0 || targetY >= this->height) {
        return std::make_pair(this->bombers[id].getX(), this->bombers[id].getY());
    }

    if (targetX - this->bombers[id].getX() > 1 || targetX - this->bombers[id].getX() < -1 || targetY - this->bombers[id].getY() > 1 || targetY - this->bombers[id].getY() < -1) {
        return std::make_pair(this->bombers[id].getX(), this->bombers[id].getY());
    }

    if (targetX != this->bombers[id].getX() && targetY != this->bombers[id].getY()) {
        return std::make_pair(this->bombers[id].getX(), this->bombers[id].getY());
    }

    this->bombers[id].setX(targetX);
    this->bombers[id].setY(targetY);

    return std::make_pair(targetX, targetY);
}

/**
 * @brief this function kills a bomber.\n
 * the bomber is NOT removed from the map and the fd is closed.\n
 * they are only marked dead. the bomber is removed from the\n
 * map by the controller when necessary.
 * @param id id of the bomber
 */
void Map::killBomber(int id) {
    this->bombers[id].setIsAlive(false);
}

void Map::setBombs(std::vector<Bomb> bombs) {
    this->bombs = (bombs);
}

/**
 * creates pipes, forks and starts bomb process
 * @param bomberId id of bomber who planted the bomb
 * @param radius
 * @param durability
 * @return a pair of fd and pid of bomb process, respectively
 */
std::pair<int, int> Map::plantBomb(int bomberId, int radius, int interval) {
    Bomber bomber = this->bombers[bomberId];
    Bomb bomb (0, 0,radius, interval);
    int bombPID;

    // check if there is already a bomb at the bomber's position
    for (size_t i = 0; i < this->bombs.size(); i++) {
        if (this->bombs[i].getIsExploded()) continue;
        if (this->bombs[i].getX() == bomber.getX() && this->bombs[i].getY() == bomber.getY()) {
            return std::make_pair(-1, -1);
        }
    }

    bomb.setX(bomber.getX());
    bomb.setY(bomber.getY());

    int pid = forkBombProcess(this, &bomb);

    bomb.setPID(pid);

    this->bombs.push_back(bomb);

    return std::make_pair(bomb.getFd(), bombPID);
}

/**
 * @brief explodes the bomb at the given coordinates. returns the ids of the bombers that were killed\n
 * also reduces a point from the obstacle if it is on the bomb's path. removes the obstacle if it has no more points
 * @param bombX x coordinate of the bomb
 * @param bombY y coordinate of the bomb
 * @return the ids of the bombers that were killed
 */
std::vector<int> Map::explodeBomb(int bombX, int bombY) {
    /**
     * if all the bombers die with the same bomb, the farthest bomber from the bomb is going to win the game.\n
     * if there are multiple bombers that are the same distance from the bomb, the bomber with the highest id wins.\n
     * this is only because the bomber with the highest id is the last bomber in the vector, so is actually random.\n
     * in order to keep the information about who won the game, we hold the distance and the id in variables.\n
     *
     * NOTE: their fd should also not be closed. that's because the bgame main function will close the fd of the bomber that won the game.
     */

    int lowestDistance = std::max(this->height, this->width), lowestDistanceBomberId = -1;
    int bombRadius;
    int initialBomberCount = this->bombers.size();
    std::vector<int> killedBombers;
    std::vector<std::pair<int, int> > obstacleCoords;

    for (size_t i = 0; i < this->bombs.size(); i++) {
        if (this->bombs[i].getIsExploded()) continue;
        if (this->bombs[i].getX() == bombX && this->bombs[i].getY() == bombY) {
            bombRadius = this->bombs[i].getRadius();
            this->bombs[i].setIsExploded(true);

            break;
        }
    }

    for (size_t i = 0; i < this->bombers.size(); i++) {
        bool mightDie;

        if (!this->bombers[i].getIsAlive()) continue;

        mightDie = (
                (this->bombers[i].getX() <= bombX + bombRadius && this->bombers[i].getX() >= bombX - bombRadius && bombY == this->bombers[i].getY()) ||
                (this->bombers[i].getY() <= bombY + bombRadius && this->bombers[i].getY() >= bombY - bombRadius && bombX == this->bombers[i].getX())
        );

        if (mightDie) {
            bool thereIsAnObstacleInWay = false;

            for (size_t j = 0; j < this->obstacles.size(); j++) {
                bool thisObstacleIsInTheWay = (
                    (this->obstacles[j].getX() < bombX && this->obstacles[j].getX() > this->bombers[i].getX() && bombY == this->obstacles[j].getY()) ||
                    (this->obstacles[j].getX() > bombX && this->obstacles[j].getX() < this->bombers[i].getX() && bombY == this->obstacles[j].getY()) ||
                    (this->obstacles[j].getY() < bombY && this->obstacles[j].getY() > this->bombers[i].getY() && bombX == this->obstacles[j].getX()) ||
                    (this->obstacles[j].getY() > bombY && this->obstacles[j].getY() < this->bombers[i].getY() && bombY == this->obstacles[j].getX())
                );

                thereIsAnObstacleInWay = thereIsAnObstacleInWay || thisObstacleIsInTheWay;

                if (thisObstacleIsInTheWay) {
                    obstacleCoords.push_back(std::make_pair(this->obstacles[j].getX(), this->obstacles[j].getY()));
                    break;
                }
            }

            if (!thereIsAnObstacleInWay) {
                if (this->bombers[i].getX() == bombX) {
                    if (std::abs(this->bombers[i].getY() - bombY) <= lowestDistance) {
                        lowestDistance = std::abs(this->bombers[i].getY() - bombY);
                        lowestDistanceBomberId = this->bombers[i].getId();
                    }
                } else {
                    if (std::abs(this->bombers[i].getX() - bombX) <= lowestDistance) {
                        lowestDistance = std::abs(this->bombers[i].getX() - bombX);
                        lowestDistanceBomberId = this->bombers[i].getId();
                    }
                }

                killedBombers.push_back(this->bombers[i].getId());
            }
        }
    }

    // at this point, there might be multiple occurrences of the same obstacle in the vector. we need to remove them
    std::sort(obstacleCoords.begin(), obstacleCoords.end());
    obstacleCoords.erase(std::unique(obstacleCoords.begin(), obstacleCoords.end()), obstacleCoords.end());

    for (size_t i = 0; i < obstacleCoords.size(); i++) {
        for (size_t j = 0; j < this->obstacles.size(); j++) {
            if (this->obstacles[j].getX() == obstacleCoords[i].first && this->obstacles[j].getY() == obstacleCoords[i].second) {
                obsd* obstacleData = new obsd;
                obstacleData->position.x = obstacleCoords[i].first;
                obstacleData->position.y = obstacleCoords[i].second;

                if (this->obstacles[j].getDurability() != -1) {
                    this->obstacles[j].decreaseDurability(1);
                }

                obstacleData->remaining_durability = this->obstacles[j].getDurability();
                print_output(NULL, NULL, obstacleData, NULL);

                if (this->obstacles[j].getDurability() == 0) {
                    this->obstacles.erase(this->obstacles.begin() + j);
                }
                break;
            }
        }
    }

    // set the lucky winner id if all the bombers are killed
    if (killedBombers.size() == initialBomberCount) {
        this->setLuckyWinnerId(lowestDistanceBomberId);
    } else {
        this->setLuckyWinnerId(-1);
    }

    return killedBombers;
}



//
//
// ---------------------------------------- OTHER FUNCTIONS ----------------------------------------
//
//




/**
 * Forks the bomber processes, creates pipes, and returns their PIDs
 * @param map the map instance
 * @param fds the fds the parent process (controller) holds. comes from main in bgame
 * @return PIDs of the bomber processes
 */
std::vector<int> forkBomberProcesses(Map* map, std::vector<int>* fds) {
    std::vector<int> PIDs;

    for (int i = 0; i < map->getBomberCount(); i++) {
        int fd[2];
        PIPE(fd);
        fds->push_back(fd[0]);
        map->getBombers()[i].setFd(fd[0]);

        int pid = fork();
        PIDs.push_back(pid);
        map->getBombers()[i].setPID(pid);

        if (pid == 0) { // child
            close(fd[0]); // CLOSE PARENT'S CHANNEL
            char** argv = new char*[map->getBombers()[i].getArgv().size() + 1];
            for (size_t j = 0; j < map->getBombers()[i].getArgv().size(); j++) {
                argv[j] = (char*) map->getBombers()[i].getArgv()[j].c_str();
            }
            dup2(fd[1], STDIN_FILENO);
            dup2(fd[1], STDOUT_FILENO);
            argv[map->getBombers()[i].getArgv().size()] = NULL;
            execv(argv[0], argv);
        } else {
            close(fd[1]); // CLOSE CHILD'S CHANNEL
        }
    }

    return PIDs;
}

/**
 * Forks the bomb process, creates pipe, and returns its PID
 * @param map the map instance
 * @param bomb the bomb instance
 * @param bomberId the id of the bomber who dropped the bomb
 * @return PID of the bomb process
 */
int forkBombProcess(Map* map, Bomb* bomb) {
    int fd[2];
    PIPE(fd);

    bomb->setFd(fd[0]);

    int pid = fork();

    if (pid == 0) { // child
        char** argv = new char*[3];

        argv[0] = new char [8];
        argv[1] = new char [16];
        argv[2] = NULL;
        
        dup2(fd[1], STDIN_FILENO);
        dup2(fd[1], STDOUT_FILENO);

        sprintf(argv[0], "./bomb");
        sprintf(argv[1], "%d", bomb->getInterval());
        argv[2] = NULL;
        
        close(fd[0]); // CLOSE PARENT'S CHANNEL
        execv(argv[0], argv);
    } else {
        close(fd[1]); // CLOSE CHILD'S CHANNEL
    }

    return pid;
}

int remainingAliveBomberCount(Map* map) {
    int remainingBomberCount = 0;

    for (size_t i = 0; i < map->getBombers().size(); i++) {
        if (map->getBombers()[i].getIsAlive()) {
            remainingBomberCount++;
        }
    }

    return remainingBomberCount;
}

bool isGameFinished(Map *map) {
    return (remainingAliveBomberCount(map) <= 1);
}

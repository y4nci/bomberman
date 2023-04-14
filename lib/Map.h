/**
 * @file Map.h
 * @author Baran Yancı (e2449015)
 * @brief This file includes class definitions about map for bgame.
 * @version 0.0.0
 * @date 2023-04-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Bomb.h"
#include "Obstacle.h"
#include <unistd.h>
#include <vector>
#include <sys/socket.h>
#include "../message.h"

#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)

class Map {
private:
    int width;
    int height;
    std::vector<Obstacle> obstacles;
    std::vector<Bomber> bombers;
    std::vector<Bomb> bombs;
public:
    Map(int width, int height, std::vector<ObstacleInput> obstacleInputs, std::vector<BomberInput> bomberInputs);
    ~Map();
    int getWidth();
    int getHeight();
    int getObstacleCount();
    int getBomberCount();
    std::vector<Obstacle> getObstacles();
    std::vector<Bomber> getBombers();
    std::vector<Bomb> getBombs();

    void setWidth(int width);
    void setHeight(int height);
    void setObstacles(std::vector<Obstacle> obstacles);
    void setBombers(std::vector<Bomber> bombers);
    void setBombs(std::vector<Bomb> bombs);

    void moveBomber(int id, int targetX, int targetY);
    int plantBomb(int bomberId, int radius, int durability);
};

// OTHER FUNCTIONS

std::vector<int> createBomberPipes(Map* map);
int createBombPipe(Bomb* bomb);

void forkProcesses(Map* map, std::vector<int>* fds);
void forkBomb();
bool isGameFinished(Map map);

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

#include <algorithm>
#include <unistd.h>
#include <vector>
#include <sys/socket.h>
#include "../message.h"

#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)
#define MAX_OBJECT_COUNT 24

class Map {
private:
    int width;
    int height;
    // holds the id of the winner bomber who won in a round where everyone died
    int luckyWinnerId;
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
    int getBombCount();
    int getLuckyWinnerId();
    std::vector<Obstacle> getObstacles();
    std::vector<Bomber> getBombers();
    std::vector<Bomb> getBombs();

    void setWidth(int width);
    void setHeight(int height);
    void setLuckyWinnerId(int luckyWinnerId);
    void setObstacles(std::vector<Obstacle> obstacles);
    void setBombers(std::vector<Bomber> bombers);
    void setBombs(std::vector<Bomb> bombs);

    std::pair<int, std::vector<od> > seeBomber(int id);
    std::pair<int, int> moveBomber(int id, int targetX, int targetY);
    void killBomber(int id);
    std::pair<int, int> plantBomb(int bomberId, int radius, int durability);
    std::vector<int> explodeBomb(int bombX, int bombY);
};



//
//
// ---------------------------------------- OTHER FUNCTIONS ----------------------------------------
//
//



std::vector<int> forkBomberProcesses(Map* map, std::vector<int>* fds);
int forkBombProcess(Map* map, Bomb* bomb, int bomberId);
bool isGameFinished(Map* map);

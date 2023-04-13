/**
 * @file Inputs.h
 * @author Baran Yancı (e2449015)
 * @brief This file includes class definitions for inputs for bgame.
 * @version 0.0.0
 * @date 2023-04-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef INPUTS_H
#define INPUTS_H

#include <string>
#include <vector>

#include "Obstacle.h"
#include "Bomber.h"

typedef struct BomberInput {
    int x;
    int y;
    int argc;
    std::vector<std::string> argv;
} BomberInput;

typedef struct ObstacleInput {
    int x;
    int y;
    int durability;
} ObstacleInput;

class Inputs {
private:
    std::string rawInput;
    int mapWidth;
    int mapHeight;
    int obstacleCount;
    int bomberCount;

    std::vector<ObstacleInput> obstacleInputs;
    std::vector<BomberInput> bomberInputs;
public:
    Inputs();
    ~Inputs();

    int getMapWidth() const;
    int getMapHeight() const;
    int getObstacleCount() const;
    int getBomberCount() const;

    std::vector<ObstacleInput> getObstacleInputs() const;
    std::vector<BomberInput> getBomberInputs() const;

    void setMapWidth(int mapWidth);
    void setMapHeight(int mapHeight);
    void setObstacleCount(int obstacleCount);
    void setBomberCount(int bomberCount);

    void setObstacleInputs(std::vector<ObstacleInput> obstacleInputs);
    void setBomberInputs(std::vector<BomberInput> bomberInputs);
};

#endif

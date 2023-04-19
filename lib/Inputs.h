/**
 * @file Inputs.h
 * @author Baran Yancı (e2449015)
 * @brief This file includes class and struct definitions for inputs for bgame.
 * @version 0.0.1
 * @date 2023-04-19
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


/**
 * @brief The struct for bomber inputs.
 * the struct contains x and y coordinates of the bomber, including the argument count and argument vector.
 */
typedef struct BomberInput {
    int x;
    int y;
    int argc;
    std::vector<std::string> argv;
} BomberInput;

/**
 * @brief The struct for obstacle inputs.
 * the struct contains x and y coordinates of the obstacle, including the durability.
 */
typedef struct ObstacleInput {
    int x;
    int y;
    int durability;
} ObstacleInput;

class Inputs {
private:
    /**
     * @brief the raw input string.
     */
    std::string rawInput;

    /**
     * @brief the width of the map.
     */
    int mapWidth;

    /**
     * @brief the height of the map.
     */
    int mapHeight;

    /**
     * @brief the number of obstacles.
     */
    int obstacleCount;

    /**
     * @brief the number of bombers.
     */
    int bomberCount;

    /**
     * @brief the vector of obstacle inputs.
     */
    std::vector<ObstacleInput> obstacleInputs;

    /**
     * @brief the vector of bomber inputs.
     */
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

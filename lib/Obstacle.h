/**
 * @file Obstacles.h
 * @author Baran Yancı (e2449015)
 * @brief This file includes class definitions for obstacles for bgame.
 * @version 0.0.1
 * @date 2023-04-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef HW1_OBSTACLE_H
#define HW1_OBSTACLE_H

#include "Inputs.h"

class Obstacle {
private:
    /**
     * @brief the x coordinate of the obstacle.
     */
    int x;

    /**
     * @brief the y coordinate of the obstacle.
     */
    int y;

    /**
     * @brief the durability of the obstacle.
     */
    int durability;
public:
    Obstacle(int x, int y, int durability);
    ~Obstacle();
    int getX() const;
    int getY() const;
    int getDurability() const;
    void setX(int x);
    void setY(int y);
    void setDurability(int durability);
    void decreaseDurability(int amount);
};

#endif //HW1_OBSTACLE_H

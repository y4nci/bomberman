/**
 * @file Obstacle.cpp
 * @author Baran Yancı (e2449015)
 * @brief This file includes function definitions for obstacles for bgame.
 * @version 0.0.1
 * @date 2023-04-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Obstacle.h"

Obstacle::Obstacle(int x, int y, int durability) {
    this->x = x;
    this->y = y;
    this->durability = durability;
}

Obstacle::~Obstacle() {
}

int Obstacle::getX() const {
    return this->x;
}

int Obstacle::getY() const {
    return this->y;
}

int Obstacle::getDurability() const {
    return this->durability;
}

void Obstacle::setX(int x) {
    this->x = x;
}

void Obstacle::setY(int y) {
    this->y = y;
}

void Obstacle::setDurability(int durability) {
    this->durability = durability;
}

void Obstacle::decreaseDurability(int amount) {
    this->durability -= amount;
}

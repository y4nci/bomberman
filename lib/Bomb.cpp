/**
 * @file Bomb.cpp
 * @author Baran Yancı (e2449015)
 * @brief This file includes function definitions about Bomb class.
 * @version 0.0.0
 * @date 2023-04-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Bomb.h"

Bomb::Bomb(int x, int y, int radius, int durability) {
    this->x = x;
    this->y = y;
    this->radius = radius;
    this->remainingTime = durability;
}

Bomb::~Bomb() = default;

int Bomb::getX() const {
    return this->x;
}

int Bomb::getY() const {
    return this->y;
}

int Bomb::getRadius() const {
    return this->radius;
}

int Bomb::getRemainingTime() const {
    return this->remainingTime;
}

void Bomb::setX(int x) {
    this->x = x;
}

void Bomb::setY(int y) {
    this->y = y;
}

void Bomb::setRadius(int radius) {
    this->radius = radius;
}

void Bomb::setDurability(int durability) {
    this->remainingTime = durability;
}

void Bomb::decrementTime() {
    this->remainingTime--;
}
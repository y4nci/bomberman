/**
 * @file Bomber.cpp
 * @author Baran Yancı (e2449015)
 * @brief This file includes function definitions for bomber for bgame.
 * @version 0.0.0
 * @date 2023-04-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Bomber.h"
#include <utility>

Bomber::Bomber(int x, int y, int id, std::vector<std::string> argv) {
    this->x = x;
    this->y = y;
    this->id = id;
    this->argv = (argv);
}

Bomber::~Bomber() {
    this->argv.clear();
}

int Bomber::getX() const {
    return this->x;
}

int Bomber::getY() const {
    return this->y;
}

int Bomber::getId() const {
    return this->id;
}

std::vector<std::string> Bomber::getArgv() {
    return this->argv;
}

void Bomber::setX(int x) {
    this->x = x;
}

void Bomber::setY(int y) {
    this->y = y;
}

void Bomber::setId(int id) {
    this->id = id;
}

void Bomber::setArgv(std::vector<std::string> argv) {
    this->argv = (argv);
}

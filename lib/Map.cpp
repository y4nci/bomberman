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

void Map::moveBomber(int targetX, int targetY) {
    Bomber *bomber = NULL;

    for (size_t i = 0; i < this->bombers.size(); i++) {
        if (this->bombers[i].getX() == targetX && this->bombers[i].getY() == targetY) {
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
        if (this->bombers[i].getX() == targetX && this->bombers[i].getY() == targetY) {
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

void Map::plantBomb(int bomberId, int radius, int durability) {
    Bomber *bomber = NULL;
    Bomb bomb (0, 0,radius,durability);

    for (size_t i = 0; i < this->bombers.size(); i++) {
        if (this->bombers[i].getId() == bomberId) {
            bomber = &this->bombers[i];
        }
    }

    bomb.setX(bomber->getX());
    bomb.setY(bomber->getY());

    this->bombs.push_back(bomb);
}

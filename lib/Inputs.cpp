/**
 * @file Inputs.cpp
 * @author Baran Yancı (e2449015)
 * @brief This file includes function definitions about inputs for bgame.
 * @version 0.0.0
 * @date 2023-04-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <utility>
#include <iostream>
#include "Inputs.h"

/**
 * @brief This function gets input from stdin, parses and stores it.
 */

Inputs::Inputs() {
    std::cin >> this->mapWidth >> this->mapHeight >> this->obstacleCount >> this->bomberCount;

    for (int i = 0; i < this->obstacleCount; i++) {
        ObstacleInput obstacleInput;
        std::cin >> obstacleInput.x >> obstacleInput.y >> obstacleInput.durability;
        this->obstacleInputs.push_back(obstacleInput);
    }

    for (int i = 0; i < this->bomberCount; i++) {
        BomberInput bomberInput;
        std::cin >> bomberInput.x >> bomberInput.y >> bomberInput.argc;
        for (int j = 0; j < bomberInput.argc; j++) {
            std::string arg;
            std::cin >> arg;
            bomberInput.argv.push_back(arg);
        }
        this->bomberInputs.push_back(bomberInput);
    }
}

Inputs::~Inputs() {
    this->rawInput.clear();
    this->obstacleInputs.clear();
    this->bomberInputs.clear();
}

int Inputs::getMapWidth() const {
    return this->mapWidth;
}

int Inputs::getMapHeight() const {
    return this->mapHeight;
}

int Inputs::getObstacleCount() const {
    return this->obstacleCount;
}

int Inputs::getBomberCount() const {
    return this->bomberCount;
}

std::vector<ObstacleInput> Inputs::getObstacleInputs() const {
    return this->obstacleInputs;
}

std::vector<BomberInput> Inputs::getBomberInputs() const {
    return this->bomberInputs;
}

void Inputs::setMapWidth(int mapWidth) {
    this->mapWidth = mapWidth;
}

void Inputs::setMapHeight(int mapHeight) {
    this->mapHeight = mapHeight;
}

void Inputs::setObstacleCount(int obstacleCount) {
    this->obstacleCount = obstacleCount;
}

void Inputs::setBomberCount(int bomberCount) {
    this->bomberCount = bomberCount;
}

void Inputs::setObstacleInputs(std::vector<ObstacleInput> obstacleInputs) {
    this->obstacleInputs = (obstacleInputs);
}

void Inputs::setBomberInputs(std::vector<BomberInput> bomberInputs) {
    this->bomberInputs = (bomberInputs);
}

/**
 * @file bgame.cpp
 * @author Baran Yancı (e2449015)
 * @brief This file includes main function for bgame.
 * @version 0.0.0
 * @date 2023-04-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "lib/Bomber.h"
#include "lib/Bomb.h"
#include "lib/Obstacle.h"
#include "lib/Inputs.h"
#include "lib/Map.h"

int main() {
    // TODO: initialise variables

    Inputs inputs;

    Map map (15, 15, inputs.getObstacleInputs(), inputs.getBomberInputs());

    return 0;
}

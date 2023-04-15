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
#include "lib/Inputs.h"
#include "lib/Map.h"

#include "message.h"

int main() {
    Inputs inputs;
    Map map (inputs.getMapWidth(), inputs.getMapHeight(), inputs.getObstacleInputs(), inputs.getBomberInputs());

    /**
     * holds parent's fd's for communicating with bombers.\n
     * by time, bombers die but these fd's are still in the vector, despite being closed.\n
     * So, we will refer them to with the bomber's id, which will act as an index.\n
     */
    std::vector<int> bomberFds = createBomberPipes(&map);

    /**
     * holds parent's fd's for communicating with bombs.\n
     * we will destroy the fd's when the bomb explodes.
     */
    std::vector<int> bombFds;

    forkProcesses(&map, &bomberFds);

    while (!isGameFinished(map)) {
        /**
         * holds the ids of bombers that will die in this iteration.\n
         * we will destroy the fd's of these bombers and remove them from the Map vector after the iteration.
         */
        std::vector<int> bomberIdsToDestroy;

        // BOMBS
        for (int i = 0; i < map.getBombCount(); i++) {
            Bomb bomb = map.getBombs()[i];

            if (bomb.getIsExploded()) {
                continue;
            }

            int fd = bombFds[i];
            im* message;
            int res = read_data(fd, message);

            if (res == -1 || message == NULL) continue;

            if (message->type == BOMB_EXPLODE) {
                // TODO: consider the case where the bomb kills all the bombers.
                // NOTE: in that case, the farthest bomber is going to win the game
                bomberIdsToDestroy = map.explodeBomb(bomb.getX(), bomb.getY());

                for (int j = 0; j < bomberIdsToDestroy.size(); j++) {
                    int bomberId = bomberIdsToDestroy[j];
                    int bomberFd = bomberFds[bomberId];

                    close(bomberFd);
                    bomberFds[bomberId] = -1;
                }
            } else continue;
        }


        // BOMBER DEATHS
        for (int i = 0; i < bomberIdsToDestroy.size(); i++) {
            int bomberId = bomberIdsToDestroy[i];
            int fd = bomberFds[bomberId];

            close(fd);
            bomberFds[bomberId] = -1;
            map.killBomber(bomberId);
        }


        // BOMBERS
        for (int i = 0; i < map.getBomberCount(); i++) {
            Bomber bomber = map.getBombers()[i];
            int fd = bomberFds[bomber.getId()];
            im* message;
            int res = read_data(fd, message);

            if (res == -1 || message == NULL) continue;

            if (message->type == BOMBER_MOVE) {
                std::pair<int, int> newPosition = map.moveBomber(bomber.getId(), message->data.target_position.x, message->data.target_position.y);

                coordinate coor;
                om* outgoingMessage = new om;

                coor.x = newPosition.first; coor.y = newPosition.second;
                outgoingMessage->type = BOMBER_LOCATION;
                outgoingMessage->data.new_position = coor;

                send_message(fd, outgoingMessage);
            } else if (message->type == BOMBER_PLANT) {
                int bombFd = map.plantBomb(bomber.getId(), message->data.bomb_info.radius, message->data.bomb_info.interval);
                bool plantSuccessful = bombFd != -1;
                om* outgoingMessage = new om;

                outgoingMessage->type = BOMBER_PLANT_RESULT;
                outgoingMessage->data.planted = plantSuccessful;

                send_message(fd, outgoingMessage);

                if (plantSuccessful) bombFds.push_back(bombFd);
            } else if (message->type == BOMBER_START) {
                coordinate coor;
                om* outgoingMessage = new om;

                coor.x = bomber.getX(); coor.y = bomber.getY();
                outgoingMessage->type = BOMBER_LOCATION;
                outgoingMessage->data.new_position = coor;

                send_message(fd, outgoingMessage);
            } else if (message->type == BOMBER_SEE) {
                std::pair<int, std::vector<od>> seeResult = map.seeBomber(bomber.getId());
                om* outgoingMessage = new om;
                od* objects = new od[seeResult.first];

                outgoingMessage->type = BOMBER_VISION;
                outgoingMessage->data.object_count = seeResult.first;

                send_message(fd, outgoingMessage);

                for (int j = 0; j < seeResult.first; j++) {
                    objects[j] = seeResult.second[j];
                }

                send_object_data(fd, seeResult.first, objects);
            }
        }

        sleep(1);
    }

    // TODO: inform remaining bomber
    // TODO: wait for remaining bombs

    return 0;
}

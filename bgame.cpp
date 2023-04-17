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
#include "logging.h"

#include <poll.h>

int main() {
    Inputs inputs;
    Map map (inputs.getMapWidth(), inputs.getMapHeight(), inputs.getObstacleInputs(), inputs.getBomberInputs());

    /**
     * holds parent's fd's for communicating with bombers.\n
     * by time, bombers die but these fd's are still in the vector, despite being closed.\n
     * So, we will refer them to with the bomber's id, which will act as an index.\n
     * if the bomber is dead, we will set the fd to -1.\n
     */
    std::vector<int> bomberFds;

    /**
     * holds parent's fd's for communicating with bombs.\n
     * we will set the fd's of the exploded bombs to -1.\n
     */
    std::vector<int> bombFds;

    /**
     * holds the PIDs of the bombers.\n
     * used in outputting.
     */
    std::vector<int> bomberPIDs = forkBomberProcesses(&map, &bomberFds);

    while (!isGameFinished(&map)) {
        /**
         * holds the ```struct pollfd``` objects for bombers and bombs.\n
         * we will use these objects to poll the bombers and bombs.\n
         * we will use the bomber's id as an index to refer to the bomber's fd.\n
         * we may use the fd or the coordinates of the bombs as an index to refer to the bomb's fd.\n
         * these vectors should be passed to the ```poll``` function and in return, the function will fill the\n
         * ```revents``` field of the ```struct pollfd``` objects.\n
         */
        std::vector<struct pollfd> bomberPollFds, bombPollFds;

        bool gameFinished = false;

        // POLL BOMBS
        for (int i = 0; i < map.getBombCount(); i++) {
            struct pollfd pollFd;

            if (map.getBombs()[i].getIsExploded()) {
                continue;
            }

            pollFd.fd = map.getBombs()[i].getFd();
            pollFd.events = POLLIN;

            bombPollFds.push_back(pollFd);
        }
        poll(bombPollFds.data(), bombPollFds.size(), 0);

        // BOMBS
        for (int i = 0; i < map.getBombCount(); i++) {
            /**
             * holds the ids of bombers that will die in this iteration.\n
             * we will destroy the fd's of these bombers and call killBomber function on them after the iteration.
             */
            std::vector<int> bomberIdsToDestroy;
            Bomb bomb = map.getBombs()[i];

            if (bomb.getIsExploded()) {
                continue;
            }

            int fd = bomb.getFd();

            im* message = new im;

            bool shouldRead = (bombPollFds[i].revents & POLLIN);

            if (!shouldRead) continue;

            int res = read_data(fd, message);

            if (res == -1) continue;

            if (message->type == BOMB_EXPLODE) {
                bomberIdsToDestroy = map.explodeBomb(bomb.getX(), bomb.getY());
            } else continue;

            // BOMBER DEATHS
            for (int j = 0; j < bomberIdsToDestroy.size(); j++) {
                int bomberId = bomberIdsToDestroy[j];
                int killedBomberFd = bomberFds[bomberId];

                if (bomberId == map.getLuckyWinnerId()) continue;

                map.killBomber(bomberId);

                close(killedBomberFd);
                bomberFds[bomberId] = -1;
            }

            gameFinished = isGameFinished(&map);
        }

        if (gameFinished) break;

        // POLL BOMBERS
        for (int i = 0; i < map.getBomberCount(); i++) {
            struct pollfd pollFd;

            pollFd.fd = bomberFds[i];
            pollFd.events = POLLIN;

            bomberPollFds.push_back(pollFd);

            // std::cout << "BOMBER POLLLL " << i << " FD: " << bomberFds[i] << "\n";
        }
        poll(bomberPollFds.data(), bomberPollFds.size(), 0);

        // BOMBERS
        for (int i = 0; i < map.getBomberCount(); i++) {
            Bomber bomber = map.getBombers()[i];
            int fd = bomberFds[i];

            if (!bomber.getIsAlive() || fd == -1) continue;

            im* message = new im;

            bool shouldRead = (bomberPollFds[i].revents & POLLIN);

            if (!shouldRead) continue;

            int res = read_data(fd, message);

            imp* printMessage = new imp;

            if (res == -1) continue;

            if (message->type == BOMBER_MOVE) {
                std::pair<int, int> newPosition = map.moveBomber(bomber.getId(), message->data.target_position.x, message->data.target_position.y);

                coordinate coor;
                om* outgoingMessage = new om;
                omp* outputMessage = new omp;

                coor.x = newPosition.first; coor.y = newPosition.second;
                outgoingMessage->type = BOMBER_LOCATION;
                outgoingMessage->data.new_position = coor;

                send_message(fd, outgoingMessage);

                outputMessage->pid = bomberPIDs[bomber.getId()];
                outputMessage->m = outgoingMessage;

                print_output(NULL, outputMessage, NULL, NULL);
            } else if (message->type == BOMBER_PLANT) {
                std::pair<int, int> plantData = map.plantBomb(bomber.getId(), message->data.bomb_info.radius, message->data.bomb_info.interval);
                int bombFd = plantData.first, bombPID = plantData.second;
                bool plantSuccessful = bombFd != -1;
                om* outgoingMessage = new om;
                omp* outputMessage = new omp;

                outgoingMessage->type = BOMBER_PLANT_RESULT;
                outgoingMessage->data.planted = plantSuccessful;

                send_message(fd, outgoingMessage);

                outputMessage->pid = bomberPIDs[bomber.getId()];
                outputMessage->m = outgoingMessage;

                print_output(NULL, outputMessage, NULL, NULL);

                if (plantSuccessful) bombFds.push_back(bombFd);
            } else if (message->type == BOMBER_START) {
                coordinate coor;
                om* outgoingMessage = new om;
                omp* outputMessage = new omp;

                coor.x = bomber.getX(); coor.y = bomber.getY();
                outgoingMessage->type = BOMBER_LOCATION;
                outgoingMessage->data.new_position = coor;

                send_message(fd, outgoingMessage);

                outputMessage->pid = bomberPIDs[bomber.getId()];
                outputMessage->m = outgoingMessage;

                print_output(NULL, outputMessage, NULL, NULL);
            } else if (message->type == BOMBER_SEE) {
                std::pair<int, std::vector<od> > seeResult = map.seeBomber(bomber.getId());
                om* outgoingMessage = new om;
                od* objects = new od[seeResult.first];
                omp* outputMessage = new omp;

                outgoingMessage->type = BOMBER_VISION;
                outgoingMessage->data.object_count = seeResult.first;

                send_message(fd, outgoingMessage);

                for (int j = 0; j < seeResult.first; j++) {
                    objects[j] = seeResult.second[j];
                }

                send_object_data(fd, seeResult.first, objects);

                outputMessage->pid = bomberPIDs[bomber.getId()];
                outputMessage->m = outgoingMessage;

                print_output(NULL, outputMessage, NULL, objects);
            }

            printMessage->pid = bomberPIDs[bomber.getId()];
            printMessage->m = message;

            print_output(printMessage, NULL, NULL, NULL);
        }

        sleep(1);
    }

    if (map.getBomberCount() == 1) {
        om* outgoingMessage = new om;

        outgoingMessage->type = BOMBER_WIN;

        for (int i = 0; i < map.getBomberCount(); i++) {
            if (map.getBombers()[i].getIsAlive()) {
                int winnerId = map.getBombers()[i].getId();

                send_message(bomberFds[winnerId], outgoingMessage);

                map.killBomber(winnerId);

                close(bomberFds[winnerId]);
                break;
            }
        }
    } else {
        int winnerId = map.getLuckyWinnerId();

        om* outgoingMessage = new om;

        outgoingMessage->type = BOMBER_WIN;

        send_message(bomberFds[winnerId], outgoingMessage);

        // the bomber was not removed from the map, so we have to do it manually here
        map.killBomber(winnerId);

        close(bomberFds[winnerId]);
    }

    std::vector<struct pollfd> bombPollFds;

    while (true) {
        bool shouldBreak = true;

        // POLL BOMBS
        for (int i = 0; i < map.getBombCount(); i++) {
            struct pollfd pollFd;

            if (map.getBombs()[i].getIsExploded()) {
                continue;
            }

            pollFd.fd = bombFds[i];
            pollFd.events = POLLIN;

            bombPollFds.push_back(pollFd);
        }
        poll(bombPollFds.data(), bombPollFds.size(), 0);

        for (int i = 0; i < map.getBombCount(); i++) {
            if (!map.getBombs()[i].getIsExploded()) {
                int fd = bombFds[i];
                im* message = new im;

                bool shouldRead = (bombPollFds[i].revents & POLLIN);

                if (!shouldRead) continue;

                int res = read_data(fd, message);

                shouldBreak = false;

                if (res == -1) continue;

                if (message->type == BOMB_EXPLODE) {
                    map.explodeBomb(map.getBombs()[i].getX(), map.getBombs()[i].getY());
                }
            }
        }

        if (shouldBreak) break;
    }

    return 0;
}

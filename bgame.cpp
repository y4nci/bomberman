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

// TODO: check the allocated memory for the messages.

#include "lib/Bomber.h"
#include "lib/Bomb.h"
#include "lib/Inputs.h"
#include "lib/Map.h"

#include "message.h"
#include "logging.h"

// DEBUG
#include <iostream>
// DEBUG

#include <poll.h>

int main() {
    Inputs inputs;
    Map map (inputs.getMapWidth(), inputs.getMapHeight(), inputs.getObstacleInputs(), inputs.getBomberInputs());

    /**
     * holds parent's fd's for communicating with bombers.\n
     * by time, bombers die but these fd's are still in the vector, despite being closed.\n
     * So, we will refer them to with the bomber's id, which will act as an index.\n
     */
    std::vector<int> bomberFds;

    /**
     * holds parent's fd's for communicating with bombs.\n
     * we will destroy the fd's when the bomb explodes.
     */
    std::vector<int> bombFds;

    /**
     * holds the PIDs of the bombers.\n
     * used in outputting.
     */
    std::vector<int> bomberPIDs = forkBomberProcesses(&map, &bomberFds);

    /**
     * holds the PIDs of the bombs.\n
     * used in outputting.
     */
    // vector<int> bombPIDs;

    while (!isGameFinished(map)) {
        /**
         * holds the ids of bombers that will die in this iteration.\n
         * we will destroy the fd's of these bombers and remove them from the Map vector after the iteration.
         */
        std::vector<int> bomberIdsToDestroy;

        /**
         * holds the ```struct pollfd``` objects for bombers and bombs.\n
         * we will use these objects to poll the bombers and bombs.\n
         * we will use the bomber's id as an index to refer to the bomber's fd.\n
         * we may use the fd or the coordinates of the bombs as an index to refer to the bomb's fd.\n
         * these vectors should be passed to the ```poll``` function and in return, the function will fill the\n
         * ```revents``` field of the ```struct pollfd``` objects.\n
         *
         * TODO: do not forget to free the allocated memory for the ```struct pollfd``` objects.
         */
        std::vector<struct pollfd> bomberPollFds, bombPollFds;

        // DEBUG
        // std::cout << "ENTRY CHECKPOINT ill see the bomb fds" << bombFds.size() << "\n\n";
        // DEBUG

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
            Bomb bomb = map.getBombs()[i];

            // DEBUG
            // std::cout << "BOMB " << i << " CHECKPOINT\n\n";
            // DEBUG

            if (bomb.getIsExploded()) {
                continue;
            }

            int fd = bomb.getFd();

            im* message = new im;

            bool shouldRead = (bombPollFds[i].revents & POLLIN);

            // DEBUG
            // std::cout << "BOMB " << i << " CHECKPOINT 2\n";
            // std::cout << "BOMB " << i << " SHOULD READ: " << shouldRead << "\n";
            // std::cout << "BOMB " << i << " revent: " << bombPollFds[i].revents << "\n";
            // DEBUG

            if (!shouldRead) continue;

            int res = read_data(fd, message);

            // DEBUG
            std::cout << "dumahh hoe " << message->type << "\n";
            std::cout << "gayass " << fd << "\n";
            std::cout << "dumahh bih " << res << "\n";
            // DEBUG

            if (res == -1) continue;

            // DEBUG
            // std::cout << "BOMB " << i << " CHECKPOINT 3\n";
            // std::cout << "BOMB " << i << " MESSAGE TYPE: " << message->type << "\n";
            // DEBUG

            if (message->type == BOMB_EXPLODE) {
                int luckyBomberId;

                // DEBUG
                // std::cout << "AMBATAEXPLODE " << i << " \n";
                // DEBUG

                bomberIdsToDestroy = map.explodeBomb(bomb.getX(), bomb.getY(), &bombFds);
                luckyBomberId = map.getLuckyWinnerId();

                for (int j = 0; j < bomberIdsToDestroy.size(); j++) {
                    int bomberId = bomberIdsToDestroy[j];
                    int bomberFd = bomberFds[bomberId];

                    if (bomberId == luckyBomberId) continue;

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
            im* message = new im;

            bool shouldRead = (bomberPollFds[i].revents & POLLIN);

            // DEBUG
            // std::cout << "BOMBER " << i << " CHECKPOINT 2\n";
            // std::cout << "BOMBER " << i << " SHOULD READ: " << shouldRead << "\n";
            // std::cout << "BOMBER " << i << " revent: " << bomberPollFds[i].revents << "\n";
            // DEBUG

            if (!shouldRead) continue;

            int res = read_data(fd, message);

            // DEBUG
            // std::cout << "BOMBER " << i << " CHECKPOINT 3\n";
            // std::cout << "BOMBER " << i << " RES: " << res << "\n";
            // std::cout << "BOMBER " << i << " MESSAGE: " << message << "\n\n";
            // DEBUG

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

                // DEBUG
                // std::cout << bomberPIDs[bomber.getId()] << "\n";
                // DEBUG

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
                std::pair<int, std::vector<od>> seeResult = map.seeBomber(bomber.getId());
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

        send_message(bomberFds[map.getBombers()[0].getId()], outgoingMessage);
    } else {
        int winnerId = map.getLuckyWinnerId();

        om* outgoingMessage = new om;

        outgoingMessage->type = BOMBER_WIN;

        send_message(bomberFds[map.getBombers()[0].getId()], outgoingMessage);

        // the bomber were not removed from the map, so we have to do it manually here
        map.killBomber(winnerId);

        close(bomberFds[winnerId]);
    }

    std::vector<struct pollfd> bombPollFds;

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

    while (true) {
        bool shouldBreak = true;

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
                    map.explodeBomb(map.getBombs()[i].getX(), map.getBombs()[i].getY(), &bombFds);
                } else continue;
            } else continue;
        }

        if (shouldBreak) break;
    }

    return 0;
}

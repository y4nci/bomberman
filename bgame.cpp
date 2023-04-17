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
#include <sys/wait.h>
#include <sys/types.h>

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

    bool gameFinished = false;

    while (true) {
        /**
         * holds the ```struct pollfd``` objects for bombers and bombs.\n
         * we will use these objects to poll the bombers and bombs.\n
         * we will use the bomber's id as an index to refer to the bomber's fd.\n
         * we may use the fd or the coordinates of the bombs as an index to refer to the bomb's fd.\n
         * these vectors should be passed to the ```poll``` function and in return, the function will fill the\n
         * ```revents``` field of the ```struct pollfd``` objects.\n
         */
        std::vector<struct pollfd> bomberPollFds, bombPollFds;

        /**
         * @brief holds the ids of bombers that dies in this iteration.\n
         * if all bombers die in this round, we will pick the winner this way:\n\n
         *  - if there is a lucky winner, we will set the winner to the lucky winner.
         *  - if there is no lucky winner, we will set the winner to the last bomber who died.
         * in order to do this, we will store the ids of the bombers that dies in this iteration.\n
         */
        std::vector<int> freshlyDead;

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
                int status;

                bomberIdsToDestroy = map.explodeBomb(bomb.getX(), bomb.getY());

                close(bombFds[i]);
                bombFds[i] = -1;

                waitpid(bomberPIDs[i], &status, WNOHANG);
            } else continue;

            // BOMBER DEATHS
            for (int j = 0; j < bomberIdsToDestroy.size(); j++) {
                int bomberId = bomberIdsToDestroy[j];
                int killedBomberFd = bomberFds[bomberId];

                if (bomberId == map.getLuckyWinnerId()) continue;

                map.killBomber(bomberId);

                freshlyDead.push_back(bomberId);

                // their fd's are going to be closed when there is a request.
            }
        }

        gameFinished = isGameFinished(&map);

        // POLL BOMBERS
        for (int i = 0; i < map.getBomberCount(); i++) {
            struct pollfd pollFd;

            /**
             * @brief we only check for the fd's, because the bombers may have been marked as dead.\n
             * because that is how we should do it. after they are completely killed, we will close their fd's.\n
             */
            if (bomberFds[i] == -1) {
                continue;
            }

            pollFd.fd = bomberFds[i];
            pollFd.events = POLLIN;

            bomberPollFds.push_back(pollFd);
        }
        poll(bomberPollFds.data(), bomberPollFds.size(), 0);

        // BOMBERS
        for (int i = 0; i < map.getBomberCount(); i++) {
            Bomber bomber = map.getBombers()[i];
            int fd = bomberFds[i];

            if (fd == -1) continue;

            im* message = new im;

            bool shouldRead = (bomberPollFds[i].revents & POLLIN);

            if (!shouldRead) continue;

            int res = read_data(fd, message);

            imp* printMessage = new imp;

            if (res == -1) continue;

            /**
             * @brief the bomber is marked killed but their fd is still open.\n
             */
            if (!bomber.getIsAlive()) {
                om* outgoingMessage = new om;
                omp* outputMessage = new omp;

                outgoingMessage->type = BOMBER_DIE;

                send_message(fd, outgoingMessage);

                outputMessage->pid = bomberPIDs[bomber.getId()];
                outputMessage->m = outgoingMessage;

                print_output(NULL, outputMessage, NULL, NULL);

                if (freshlyDead[freshlyDead.size() - 1] != bomber.getId() && map.getLuckyWinnerId() != bomber.getId()) {
                    int status = 0;

                    close(fd);
                    bomberFds[i] = -1;

                    waitpid(bomberPIDs[bomber.getId()], &status, WNOHANG);
                    
                    continue;
                }
            }

            /**
             * @brief if the game is finished, we will send the bomber a message to exit.\n
             * we do not care about the bomber's response, or the initial message.\n
             */
            if (gameFinished) {
                int winnerId = bomber.getId(), status;

                // TODO: not sure if this is needed
                if (map.getLuckyWinnerId() != -1) {
                    winnerId = map.getLuckyWinnerId();
                }

                om* outgoingMessage = new om;
                omp* outputMessage = new omp;

                outgoingMessage->type = BOMBER_WIN;

                send_message(fd, outgoingMessage);

                outputMessage->pid = bomberPIDs[bomber.getId()];
                outputMessage->m = outgoingMessage;

                print_output(NULL, outputMessage, NULL, NULL);

                close(fd);
                bomberFds[i] = -1;
                map.killBomber(bomber.getId());

                waitpid(bomberPIDs[bomber.getId()], &status, WNOHANG);

                continue;
            }

            // we will print the message before we send the response.
            printMessage->pid = bomberPIDs[bomber.getId()];
            printMessage->m = message;

            print_output(printMessage, NULL, NULL, NULL);

            // BOMBER RESPONSE
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
        }

        /**
         * @brief if there are no bombers or bombs left, we will break the loop and exit the game.
         */
        if (bomberPollFds.size() == 0 && bombPollFds.size() == 0) {
            break;
        }

        sleep(1);
    }

    /**
     * @brief if everyone died at the same time, either from the same bomb or in the same iteration,
     * we will display the winner here.
     */
    

    return 0;
}

/**
 * @file bgame.cpp
 * @author Baran Yancı (e2449015)
 * @brief This file includes main function for bgame.
 * @version 0.0.1
 * @date 2023-04-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "lib/Bomber.h"
#include "lib/Bomb.h"
#include "lib/Inputs.h"
#include "lib/Map.h"

#include "logging.h"
#include "message.h"

#include <poll.h>
#include <string>
#include <sys/wait.h>
#include <sys/types.h>
#include <vector>

int main() {
    /**
     * there is no need to take inputs manually here, as the Inputs' constructor will do it for us.
     */
    Inputs inputs;
    Map map (inputs);
    bool gameFinished = false;

    /**
     * holds parent's fd's for communicating with bombers.
     * by time, bombers die but these fd's are still in the vector, despite being closed.
     * So, we will refer them with the bomber's id, which will act as an index.
     * if the bomber is dead, we will set the fd to -1.
     */
    std::vector<int> bomberFds;

    /**
     * holds parent's fd's for communicating with bombs.
     * we will set the fd's of the exploded bombs to -1.
     */
    std::vector<int> bombFds;

    /**
     * holds the PIDs of the bombers.
     * used in outputting.
     */
    std::vector<int> bomberPIDs = forkBomberProcesses(&map, &bomberFds);

    while (true) {
        /**
         * holds the ```struct pollfd``` objects for bombers and bombs.
         * we will use these objects to poll the bombers and bombs.
         * we will use the bomber's id as an index to refer to the bomber's fd.
         * we may use the fd or the coordinates of the bombs as an index to refer to the bomb's fd.
         * these vectors should be passed to the ```poll``` function and in return, the function will fill the
         * ```revents``` field of the ```struct pollfd``` objects.
         */
        std::vector<struct pollfd> bomberPollFds, bombPollFds;

        /**
         * holds the ids of bombers that dies in this iteration.
         * if all bombers die in this round, we will pick the winner this way:
         *  - if there is a lucky winner, we will set the winner to the lucky winner.
         *  - if there is no lucky winner, we will set the winner to the last bomber who died.
         * in order to do this, we will store the ids of the bombers that dies in this iteration.
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
        // POLL BOMBS



        // CHECK BOMBS
        for (int i = 0; i < map.getBombCount(); i++) {
            int fd, res;
            bool shouldRead;

            /**
             * holds the ids of bombers that will die in this iteration.
             * we will destroy the fd's of these bombers and call killBomber function on them after the iteration.
             */
            std::vector<int> bomberIdsToDestroy;
            Bomb bomb = map.getBombs()[i];
            im* message;

            if (bomb.getIsExploded()) {
                continue;
            }

            fd = bomb.getFd();

            message = new im;

            shouldRead = (bombPollFds[i].revents & POLLIN);

            if (!shouldRead) {
                delete message;
                continue;
            }

            res = read_data(fd, message);

            if (res == -1) {
                delete message;
                continue;
            }

            if (message->type == BOMB_EXPLODE) {
                int status;
                imp* printMessage = new imp;

                printMessage->pid = bomb.getPID();
                printMessage->m = message;

                print_output(printMessage, NULL, NULL, NULL);

                bomberIdsToDestroy = map.explodeBomb(bomb.getX(), bomb.getY());

                delete printMessage;

                close(bombFds[i]);
                bombFds[i] = -1;

                waitpid(bomb.getPID(), &status, WNOHANG);
            } else {
                delete message;
                continue;
            }


            // BOMBER DEATHS
            for (size_t j = 0; j < bomberIdsToDestroy.size(); j++) {
                int bomberId = bomberIdsToDestroy[j];

                if (bomberId == map.getLuckyWinnerId()) continue;

                map.killBomber(bomberId);

                freshlyDead.push_back(bomberId);

                // their fd's are going to be closed when there is a request.
            }
            // BOMBER DEATHS


            delete message;
        }
        // CHECK BOMBS



        gameFinished = isGameFinished(&map);

        
        
        // POLL BOMBERS
        for (int i = 0; i < map.getBomberCount(); i++) {
            struct pollfd pollFd;

            /**
             * we only check for the fd's, because the bombers may have been marked as dead but not informed about it.
             * because that is how we should do it. after they are completely killed, we will close their fd's.
             */
            if (bomberFds[i] == -1) {
                continue;
            }

            pollFd.fd = bomberFds[i];
            pollFd.events = POLLIN;

            bomberPollFds.push_back(pollFd);
        }
        poll(bomberPollFds.data(), bomberPollFds.size(), 0);
        // POLL BOMBERS



        // BOMBERS
        for (int i = 0; i < map.getBomberCount(); i++) {
            int fd = bomberFds[i], res;
            bool shouldRead;
            Bomber bomber = map.getBombers()[i];
            im* message;
            imp* printMessage;

            if (fd == -1) continue;

            message = new im;

            shouldRead = (bomberPollFds[i].revents & POLLIN);

            if (!shouldRead) {
                delete message;
                continue;
            }

            res = read_data(fd, message);

            printMessage = new imp;

            if (res == -1) {
                delete message;
                continue;
            }

            /**
             * the bomber is marked killed but their fd is still open.
             */
            if (!bomber.getIsAlive()) {
                om* outgoingMessage = new om;
                omp* outputMessage = new omp;

                printMessage->pid = bomberPIDs[bomber.getId()];
                printMessage->m = message;

                print_output(printMessage, NULL, NULL, NULL);

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
                    
                    delete outgoingMessage;
                    delete outputMessage;
                    continue;
                }

                delete outgoingMessage;
                delete outputMessage;
            }

            /**
             * @brief if the game is finished, we will send the bomber a message to exit.
             * we do not care about the bomber's response, or the initial message.
             */
            if (gameFinished) {
                int status;
                om* outgoingMessage;
                omp* outputMessage;

                outgoingMessage = new om;
                outputMessage = new omp;
                
                printMessage->pid = bomberPIDs[bomber.getId()];
                printMessage->m = message;

                print_output(printMessage, NULL, NULL, NULL);

                outgoingMessage->type = BOMBER_WIN;

                send_message(fd, outgoingMessage);

                outputMessage->pid = bomberPIDs[bomber.getId()];
                outputMessage->m = outgoingMessage;

                print_output(NULL, outputMessage, NULL, NULL);

                close(fd);
                bomberFds[i] = -1;
                map.killBomber(bomber.getId());

                waitpid(bomberPIDs[bomber.getId()], &status, WNOHANG);

                delete outgoingMessage;
                delete outputMessage;
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

                delete outgoingMessage;
                delete outputMessage;
            } else if (message->type == BOMBER_PLANT) {
                int bombFd = map.plantBomb(bomber.getId(), message->data.bomb_info.radius, message->data.bomb_info.interval);
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
                
                delete outgoingMessage;
                delete outputMessage;
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
                
                delete outgoingMessage;
                delete outputMessage;
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

                delete outgoingMessage;
                delete outputMessage;
                delete[] objects;
            }
            // BOMBER RESPONSE


            delete message;
            delete printMessage;
        }

        /**
         * if there are no bombers or bombs left, we will break the loop and exit the game.
         */
        if (bomberPollFds.size() == 0 && bombPollFds.size() == 0) {
            break;
        }

        sleep(1);
    }    

    return 0;
}

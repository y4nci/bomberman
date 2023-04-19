/**
 * @file Bomber.h
 * @author Baran Yancı (e2449015)
 * @brief This file includes class definitions for bomber for bgame.
 * @version 0.0.1
 * @date 2023-04-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef HW1_BOMBER_H
#define HW1_BOMBER_H

#include "Inputs.h"
#include <vector>
#include <string>

class Bomber {
private:
    /**
     * @brief x coordinate of the bomber.
     */
    int x;

    /**
     * @brief y coordinate of the bomber.
     */
    int y;

    /**
     * @brief the id of the bomber.
     */
    int id;

    /**
     * @brief the process id of the bomber executable.
     */
    int PID;

    /**
     * @brief the argument vector of the bomber executable.
     */
    std::vector<std::string> argv;

    /**
     * @brief the file descriptor of the pipe.
     */
    int fd;

    /**
     * @brief the status of the bomber.
     */
    bool isAlive;
public:
    Bomber(int x, int y, int id, std::vector<std::string> argv);
    ~Bomber();
    int getX() const;
    int getY() const;
    int getId() const;
    int getPID() const;
    std::vector<std::string> getArgv();
    int getFd() const;
    bool getIsAlive() const;

    void setX(int x);
    void setY(int y);
    void setId(int id);
    void setPID(int PID);
    void setArgv(std::vector<std::string> argv);
    void setFd(int fd);
    void setIsAlive(bool isAlive);
};

#endif //HW1_BOMBER_H

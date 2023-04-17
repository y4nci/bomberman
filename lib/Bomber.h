/**
 * @file Bomber.h
 * @author Baran Yancı (e2449015)
 * @brief This file includes class definitions for bomber for bgame.
 * @version 0.0.0
 * @date 2023-04-13
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
    int x;
    int y;
    int id;
    int PID;
    std::vector<std::string> argv;
    int fd;
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

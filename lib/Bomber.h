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
    std::vector<std::string> argv;
public:
    Bomber(int x, int y, int id, std::vector<std::string> argv);
    ~Bomber();
    int getX() const;
    int getY() const;
    int getId() const;
    std::vector<std::string> getArgv();
    void setX(int x);
    void setY(int y);
    void setId(int id);
    void setArgv(std::vector<std::string> argv);
};

#endif //HW1_BOMBER_H

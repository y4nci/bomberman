/**
 * @file Bomb.h
 * @author Baran Yancı (e2449015)
 * @brief This file includes class definitions about Bomb for bgame.
 * @version 0.0.0
 * @date 2023-04-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BOMB_H
#define BOMB_H

class Bomb {
private:
    int x;
    int y;
    int radius;
    int interval;
    bool isExploded;
    int fd;
    int PID;
public:
    Bomb(int x, int y, int radius, int interval);
    ~Bomb();
    int getX() const;
    int getY() const;
    int getRadius() const;
    int getInterval() const;
    int getIsExploded() const;
    int getFd() const;
    int getPID() const;

    void setX(int x);
    void setY(int y);
    void setRadius(int radius);
    void setInterval(int interval);
    void setIsExploded(bool isExploded);
    void setFd(int fd);
    void setPID(int PID);
};

#endif

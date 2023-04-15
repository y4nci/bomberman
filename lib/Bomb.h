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
    bool isExploded;
    int fd;
public:
    Bomb(int x, int y, int radius);
    ~Bomb();
    int getX() const;
    int getY() const;
    int getRadius() const;
    int getIsExploded() const;
    int getFd() const;

    void setX(int x);
    void setY(int y);
    void setRadius(int radius);
    void setIsExploded(bool isExploded);
    void setFd(int fd);
};

#endif

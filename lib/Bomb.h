/**
 * @file Bomb.h
 * @author Baran Yancı (e2449015)
 * @brief This file includes class definitions about Bomb for bgame.
 * @version 0.0.1
 * @date 2023-04-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BOMB_H
#define BOMB_H

class Bomb {
private:
    /**
     * @brief x coordinate of the bomb.
     */
    int x;

    /**
     * @brief y coordinate of the bomb.
     */
    int y;

    /**
     * @brief the radius of the explosion.
     */
    int radius;

    /**
     * @brief the interval to pass as an argument to bomb executable.
     */
    int interval;

    /**
     * @brief the file descriptor of the pipe.
     */
    int fd;

    /**
     * @brief the process id of the bomb executable.
     */
    int PID;

    /**
     * @brief the status of the bomb.
     */
    bool isExploded;
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

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

class Bomb {
private:
    int x;
    int y;
    int radius;
    int remainingTime;
public:
    Bomb(int x, int y, int radius, int durability);
    ~Bomb();
    int getX() const;
    int getY() const;
    int getRadius() const;
    int getRemainingTime() const;

    void setX(int x);
    void setY(int y);
    void setRadius(int radius);
    void setDurability(int remainingTime);

    void decrementTime();
};

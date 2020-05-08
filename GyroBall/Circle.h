#ifndef CIRCLE_H
#define CIRCLE_H

#include <Arduino.h>

class Circle {
  public:
    Circle(int32_t x, int32_t y, int32_t radius, uint32_t color);
    int32_t x();
    int32_t y();
    int32_t dx();
    int32_t dy();
    int32_t radius();
    uint32_t color();
    void setX(int32_t x);
    void setY(int32_t y);
    void setDx(int32_t dx);
    void setDy(int32_t dy);
  private:
    int32_t x_;
    int32_t y_;
    int32_t dx_;
    int32_t dy_;
    int32_t radius_;
    uint32_t color_;
};

#endif

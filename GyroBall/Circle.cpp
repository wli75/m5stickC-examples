#include "Circle.h"

Circle::Circle(int32_t x, int32_t y, int32_t radius, uint32_t color) {
  x_ = x;
  y_ = y;
  radius_ = radius;
  color_ = color;
  dx_ = 0;
  dy_ = 0;
}

int32_t Circle::x() {
  return x_; 
}

int32_t Circle::y() {
  return y_;
}

int32_t Circle::dx() {
  return dx_;
}

int32_t Circle::dy() {
  return dy_;
}

int32_t Circle::radius() {
  return radius_;
}

uint32_t Circle::color() {
  return color_;
}

void Circle::setX(int32_t x) {
  x_ = x;
}

void Circle::setY(int32_t y) {
  y_ = y;
}

void Circle::setDx(int32_t dx) {
  dx_ = dx;
}

void Circle::setDy(int32_t dy) {
  dy_ = dy;
}

#include <M5StickC.h>
#include "Circle.h"

// Screen boundaries
const int32_t kTop = 0;
const int32_t kBottom = 160;
const int32_t kLeft = 0;
const int32_t kRight = 80;

// Circle properties
const int32_t kRadius = 5;
const uint32_t kColorMin = 1;
const uint32_t kColorMax = 65536;
const int32_t kTerminalVelocity = 10;
const int32_t kAcceleration = 1;
const int kTotalNumOfCircles = 30;
int numOfCircles = 0;
Circle *circles[kTotalNumOfCircles];

// Sensor properties
const float kAngleBoundary = 0;
int prevM5Btn = HIGH; // released state

void setup() {
  // Initialize the M5StickC object
  M5.begin();
  // Set up screen
  M5.Axp.ScreenBreath(8);
  // Set up gyroscope
  M5.IMU.Init();
  // Set up serial port for debugging
  Serial.begin(115200);
}

void loop() {
  M5.Lcd.fillScreen(BLACK);
  
  float pitch = 0.0f;
  float roll  = 0.0f;
  float yaw   = 0.0f;
  M5.IMU.getAhrsData(&pitch,&roll,&yaw);
  Serial.println((String) "Roll: " + roll + ", pitch: " + pitch);

  createCircles();

  for (int i = 0; i < numOfCircles; ++i) {
    updateSpeed(circles[i], roll, pitch);
    updatePosition(circles[i]);
    Serial.println((String) "Drawing circle[" + i + "] x: " + circles[i]->x() + ", y: " + circles[i]->y() + ", dx: " + circles[i]->dx() + ", dy: " + circles[i]->dy());
    M5.Lcd.fillCircle(circles[i]->x(), circles[i]->y(), circles[i]->radius(), circles[i]->color());
  }
  
  delay(200);
}

void createCircles() {
  int currM5Btn = digitalRead(M5_BUTTON_HOME);
  if (prevM5Btn != currM5Btn) {
    if (currM5Btn == LOW && numOfCircles < kTotalNumOfCircles) { // Button has been pressed
      Serial.println("M5Btn is pressed, creating new circle");
      uint32_t color = random(kColorMin, kColorMax);
      circles[numOfCircles] = new Circle(kRight / 2, kBottom / 2, kRadius, color);
      ++numOfCircles;
    }
    prevM5Btn = currM5Btn;
  }
}

void updateSpeed(Circle *circle, float roll, float pitch) {
  updateSpeedDueToRebound(circle);
  updateSpeedDueToMotion(circle, roll, pitch);
}

void updateSpeedDueToRebound(Circle *circle) {
  if (isTouchingTop(circle) || isTouchingBottom(circle)) {
    circle->setDy(-circle->dy());
  }
  if (isTouchingLeft(circle) || isTouchingRight(circle)) {
    circle->setDx(-circle->dx());
  }
}

void updateSpeedDueToMotion(Circle *circle, float roll, float pitch) {
  int32_t dy = circle->dy();
  if (roll < -kAngleBoundary) {
    dy = isTouchingTop(circle) && circle->dy() <= 0 ? 0 : circle->dy() - kAcceleration;
  } else if (roll > kAngleBoundary) {
    dy = isTouchingBottom(circle) && circle->dy() >= 0 ? 0 : circle->dy() + kAcceleration;
  }
  circle->setDy(transformSpeed(dy));

  int32_t dx = circle->dx();
  if (pitch < -kAngleBoundary) {
    dx = isTouchingLeft(circle) && circle->dx() <= 0 ? 0 : circle->dx() - kAcceleration;
  } else if (pitch > kAngleBoundary) {
    dx = isTouchingRight(circle) && circle->dx() >= 0 ? 0 : circle->dx() + kAcceleration;
  }
  circle->setDx(transformSpeed(dx));
}

int32_t transformSpeed(int32_t s) {
  if (s < -kTerminalVelocity) return -kTerminalVelocity;
  else if (s > kTerminalVelocity) return kTerminalVelocity;
  else return s;
}

void updatePosition(Circle *circle) {
  int32_t x = circle->x() + circle->dx();
  int32_t y = circle->y() + circle->dy();

  if (x - circle->radius() < kLeft) x = kLeft + circle->radius();
  else if (x + circle->radius() > kRight) x = kRight - circle->radius();

  if (y - circle->radius() < kTop) y = kTop + circle->radius();
  else if (y + circle->radius() > kBottom) y = kBottom - circle->radius();

  circle->setX(x);
  circle->setY(y);
}

bool isTouchingTop(Circle *circle) {
  return circle->y() - circle->radius() <= kTop;
}

bool isTouchingBottom(Circle *circle) {
  return circle->y() + circle->radius() >= kBottom;
}

bool isTouchingLeft(Circle *circle) {
  return circle->x() - circle->radius() <= kLeft;
}

bool isTouchingRight(Circle *circle) {
  return circle->x() + circle->radius() >= kRight;
}

#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <iostream>

//Settings. Distance Unit are in inches
const char areaW = 8 * 12;
const char areaH = areaW / 2;
const char halfAreaW = 4 * 12;
const char halfAreaH = halfAreaW / 2;

const char ball_n = 16; // Number of ball
const float ball_d = 2.25f; // Ball Diameter
const float ball_f = 0.001f; // Ball Friction Constant
const float e = 0.95f; //Coefficient of resitution.

class ball{
    public:
        float x;
        float y;
        float i = 0.0f;
        float j = 0.0f;
        bool moving = false;
        bool show = true;
};

extern ball balls[ball_n]; // Array of balls
extern bool endGame; // Game status
extern bool active; // False if no balls are moving

void setup();
void run();
void hit(float, float);

void render();
void closeWindow();

#endif
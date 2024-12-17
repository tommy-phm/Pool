#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <iostream>

//Settings. Distance Unit are in inches
const char areaW = 8 * 12;
const char areaH = areaW / 2;
const char halfAreaW = 4 * 12;
const char halfAreaH = halfAreaW / 2;

const char ball_n = 16;           // Number of ball
const float ball_d = 2.25f;       // Ball Diameter
const float maxValocity = 200.0f; // Max Applied Ball Valocity

class ball{
    public:
        float x; 
        float y;
        float i = 0.0f;
        float j = 0.0f;
        bool moving = false;
        bool show = true;
};

extern ball balls[ball_n];  // Array of balls
extern bool running;        // Game status
extern bool active;         // True if any balls are moving
extern int gameMode;        // Game type

void setup();
void simulateGame();
void simulateRound();
void hit(float, float);

void render();

void hostSocket();
void joinSocket();
void sendGameState();
void receiveGameState();
void closeSocket();

#endif
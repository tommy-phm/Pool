#include <cmath>
#include <chrono>
#include "simulator.h"

ball balls[ball_n];
const char fps = 120;

//Move ball and apply friction
void move(ball &b){
    b.x += b.i;
    b.y += b.j;

    float magatudie = std::abs(b.i) + std::abs(b.j);
    if(magatudie > ball_f){
        b.i = b.i - ball_f * b.i / magatudie;
        b.j = b.j - ball_f * b.j / magatudie;
    }else{
        b.moving = false;
        b.i = 0.0f;
        b.j = 0.0f;
    }    
}

//Check and apply ball collision
int ballCollsion(ball &b1, ball &b2){
    // Calculate collision vector and its magnitude
    float vCollisionX = b2.x - b1.x;
    float vCollisionY = b2.y - b1.y;
    float distance = std::sqrt(vCollisionX * vCollisionX + vCollisionY * vCollisionY);
    if (distance <= ball_d){
        
        // Seperate Overplaping Balls
        float overLap = 0.5f * (distance - ball_d);
        b1.x -= overLap * (b1.x - b2.x) / distance;
        b1.y -= overLap * (b1.y - b2.y) / distance;
        b2.x += overLap * (b1.x - b2.x) / distance;
        b2.y += overLap * (b1.y - b2.y) / distance;

        // Calculate collision normal vector
        float vCollisionNormX = vCollisionX / distance;
        float vCollisionNormY = vCollisionY / distance;

        // Calculate relative velocity
        float vRelativeVelocityX = b1.i - b2.i;
        float vRelativeVelocityY = b1.j - b2.j;

        // Calculate speed
        float impulse = vRelativeVelocityX * vCollisionNormX + vRelativeVelocityY * vCollisionNormY;

        b1.i -= impulse * vCollisionNormX * e;
        b1.j -= impulse * vCollisionNormY * e;
        b2.i += impulse * vCollisionNormX * e;
        b2.j += impulse * vCollisionNormY * e;

        b2.moving = true;

    }else
        return false;
    return true;
}

//Check and apply wall collision
int wallCollsion(ball &b){
    bool collision = false;
    if (b.x > halfAreaW){
        b.x = areaW - b.x;
        b.i = -b.i;
        collision = true;
    }
    else if(b.x < -halfAreaW){
        b.x = -areaW - b.x;
        b.i = -b.i;
        collision = true;
    }
    if (b.y > halfAreaH){
        b.y = areaH - b.y;
        b.j = -b.j;
        collision = true;
    }
    else if(b.y < -halfAreaH){
        b.y = -areaH - b.y;
        b.j = -b.j;
        collision = true;
    }
    return collision;
}

// Simulate movment and collisions
void tick(){
    active = false;
    for(int i = 0; i < ball_n; i++){
        if (balls[i].moving == true){
                if(wallCollsion(balls[i]) == false){
                    for(int j = 0; j < ball_n; j++){
                        if(i != j && ballCollsion(balls[i], balls[j]) == true)
                            exit;
                    }
                }
            move(balls[i]);
            active = true;
        }
    }    
}

void run(){
    auto lastTime = std::chrono::high_resolution_clock::now();
    const double nsPerTick = 1000000000.0 / fps; // Limits tick rate to fps

    long lastTimer = 0; 
    double delta = 0;
    while (!endGame) {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = now - lastTime;
        lastTime = now;

        delta += elapsed.count() * 1000000000.0; // Convert seconds to nanoseconds

        while (delta >= nsPerTick) {
            delta -= nsPerTick;
            if (active)
                tick();
        }
    }
}

// Intilized ball
void setup(){
    int n = 0;
    for (int i = 0; i <= 5; i++) {
        float yOffset = -i * ball_d / 2.0f;
        float xOffset = halfAreaW / 2.0f - ball_d + i * ball_d / 2.0f * sqrt(3.0f);
        for (int j = 0; j < i; j++) {
            balls[n].x = xOffset;
            balls[n].y = yOffset + j * ball_d;
            n++;
        }
    }

    balls[ball_n - 1].x = 0.0f;
    balls[ball_n - 1].y = 0.0f;
}

// Apply vectors to ball
void hit(float i, float j){
    balls[ball_n - 1].i = i; 
    balls[ball_n - 1].j = j;
    balls[ball_n - 1].moving = true;
    active = true;
}
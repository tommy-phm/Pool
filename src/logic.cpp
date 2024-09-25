#include <cmath>
#include <chrono>
#include "simulator.h"

ball balls[ball_n];
const char fps = 120.0f;
const float peroid = 1.0f / fps; //Time per tick
const float cR = 0.95f; //Ball to ball coefficient  of restitution (e).
const float cRR = 0.01f; //Ball to cloth coefficient of Rolling Resistance (μ)
const float gravity = 386.08858267717f; // Gravity Acceleration (in / s^2)
const float deceleration = cRR * gravity * peroid; // Deceleration per tick

//Move ball and apply friction
void move(ball &b){
    float magatudie = std::abs(b.i) + std::abs(b.j);
    b.x += b.i * peroid - 0.5f * deceleration * (b.i / magatudie) * peroid;
    b.y += b.j * peroid - 0.5f * deceleration * (b.j / magatudie) * peroid; 
    
    if(magatudie > deceleration){
        b.i -= deceleration * (b.i / magatudie);
        b.j -= deceleration * (b.j / magatudie);
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

        b1.i -= impulse * vCollisionNormX * cR;
        b1.j -= impulse * vCollisionNormY * cR;
        b2.i += impulse * vCollisionNormX * cR;
        b2.j += impulse * vCollisionNormY * cR;

        b2.moving = true;

    }else
        return false;
    return true;
}

//Pocket
void pocketEntry(ball &b){
    b.moving = false;
    b.show = false;
    b.x = areaW;
    b.y = areaH;
    b.i = b.j = 0.0f;
}

//Check and apply wall collision
int wallCollsion(ball &b){
    bool collision = false;
    if (b.x > halfAreaW){
        if(abs(b.y) < halfAreaH - ball_d / 2.0f){
            b.x = areaW - b.x;
            b.i = -b.i;
            collision = true;     
        }
        else
            pocketEntry(b);
    }
    else if(b.x < -halfAreaW){
        if(abs(b.y) < halfAreaH - ball_d/ 2.0f){
            b.x = -areaW - b.x;
            b.i = -b.i;
            collision = true;
        }
        else
            pocketEntry(b);
    }
    if (b.y > halfAreaH){
        if((ball_d / 2.0f < abs(b.x) && abs(b.x) < halfAreaW - ball_d / 2.0f)){
            b.y = areaH - b.y;
            b.j = -b.j;
            collision = true;
        }
        else
            pocketEntry(b);

    }
    else if(b.y < -halfAreaH){
        if((ball_d / 2.0f < abs(b.x) && abs(b.x) < halfAreaW - ball_d / 2.0f)){
            b.y = -areaH - b.y;
            b.j = -b.j;
            collision = true;
        }
        else
            pocketEntry(b);
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

// Simulate a tick after each peroid
void run(){
    auto lastperoid = std::chrono::high_resolution_clock::now();
    long lastperoidr = 0; 
    double delta = 0;
    while (!endGame) {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = now - lastperoid;
        lastperoid = now;

        delta += elapsed.count();

        while (delta >= peroid) {
            delta -= peroid;
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
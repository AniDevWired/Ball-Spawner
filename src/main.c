#include "raylib.h"
#include "ll.h"
#include <time.h>
#include "stdlib.h"
#include <math.h>
#include <stdio.h>

#define WINDOW_HEIGHT 900
#define WINDOW_WIDTH 1080
#define SOUND_BUFFER 24

typedef struct Ball {
    Vector2 pos;
    Vector2 ballSpeed;
    float ballRadius;
    Color color;
    int powLvl;
    bool spawner;
    bool canIDestroy;
} Ball;

Ball createBall(void);
void destroyBall(void *ptr);

int main(void) {

    srand(time(NULL));

    // window related settings
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Bouncing Ball");

    // Ball
    LinkedList balls;
    ll_init(&balls, destroyBall);

    // audio
    InitAudioDevice();

    Sound pop = LoadSound("assets/pop.wav");
    Sound funny = LoadSound("assets/funny.wav");

    Sound popBuffer[SOUND_BUFFER];
    for(int i = 0; i < SOUND_BUFFER; i++) {
        popBuffer[i] = LoadSoundAlias(pop);
    }
    int soundIdx = 0;

    Sound popBuffer2[SOUND_BUFFER];
    for(int i = 0; i < SOUND_BUFFER; i++) {
        popBuffer2[i] = LoadSoundAlias(funny);
    }
    int soundIdx2 = 0;

    Ball *firstBall = (Ball *)malloc(sizeof(Ball));
    *firstBall = createBall();
    firstBall->spawner = true;
    ll_append(&balls, firstBall);

    //SetTargetFPS(100);

    Font font = GetFontDefault();

    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        LinkedList toSpawnBalls;
        ll_init(&toSpawnBalls, destroyBall);
        Node *curr = ll_begin(&balls);

        while (curr != NULL) {
            Ball *ball = (Ball *)curr->data;

            ball->pos.x += ball->ballSpeed.x*dt*50;
            ball->pos.y += ball->ballSpeed.y*dt*50;

            if(ball->pos.x  - ball->ballRadius < 0) {
                ball->pos.x = ball->ballRadius;
                ball->ballSpeed.x = fabsf(ball->ballSpeed.x);

                if(ball->spawner) {
                    Ball *newBall = (Ball *)malloc(sizeof(Ball));
                    *newBall = createBall();
                    newBall->spawner = true;
                    newBall->canIDestroy = false;
                    ll_append(&toSpawnBalls, newBall);
                }

                PlaySound(popBuffer[soundIdx%SOUND_BUFFER]);
                soundIdx++;
            }

            if(ball->pos.x  + ball->ballRadius > GetScreenWidth()) {
                ball->pos.x = GetScreenWidth() - ball->ballRadius;
                ball->ballSpeed.x = -fabsf(ball->ballSpeed.x);

                if(ball->spawner) {
                    Ball *newBall = (Ball *)malloc(sizeof(Ball));
                    *newBall = createBall();
                    newBall->spawner = true;
                    newBall->canIDestroy = false;
                    ll_append(&toSpawnBalls, newBall);
                }

                PlaySound(popBuffer[soundIdx%SOUND_BUFFER]);
                soundIdx++;
            }

            if(ball->pos.y - ball->ballRadius < 0) {
                ball->pos.y = ball->ballRadius;
                ball->ballSpeed.y = fabsf(ball->ballSpeed.y);

                if(ball->spawner) {
                    Ball *newBall = (Ball *)malloc(sizeof(Ball));
                    *newBall = createBall();
                    newBall->spawner = true;
                    newBall->canIDestroy = false;
                    ll_append(&toSpawnBalls, newBall);
                }

                PlaySound(popBuffer[soundIdx%SOUND_BUFFER]);
                soundIdx++;
            }

            if(ball->pos.y + ball->ballRadius > GetScreenHeight()) {
                ball->pos.y = GetScreenHeight() - ball->ballRadius;
                ball->ballSpeed.y = -fabsf(ball->ballSpeed.y);

                if(ball->spawner) {
                    Ball *newBall = (Ball *)malloc(sizeof(Ball));
                    *newBall = createBall();
                    newBall->spawner = true;
                    newBall->canIDestroy = false;
                    ll_append(&toSpawnBalls, newBall);
                }

                PlaySound(popBuffer[soundIdx%SOUND_BUFFER]);
                soundIdx++;
            }

            curr = curr->next;
        }

        Node *currN = ll_begin(&toSpawnBalls);
        while (currN != NULL) {
            Ball *ball = (Ball *)currN->data;
            ll_append(&balls, ball);
            currN = currN->next;
        }
        toSpawnBalls.destroy = NULL;
        ll_destroy(&toSpawnBalls); // destroy the toSpawnBall linkedList..

        // add collision
        Node *a = ll_begin(&balls);
        while (a != NULL) {
            Ball *ballA = (Ball *)a->data;

            Node *b = a->next;
            while (b != NULL) {
                Ball *ballB = (Ball *)b->data;

                float dx = ballB->pos.x - ballA->pos.x;
                float dy = ballB->pos.y - ballA->pos.y;

                float dist = sqrtf(dx*dx + dy*dy);

                if(dist < 0.001f) {
                    b = b->next;
                    continue;
                }

                float overlap = ballA->ballRadius + ballB->ballRadius - dist;

                if(overlap > 0.0f) {
                    float collisionNormalX = dx / dist;
                    float collisionNormalY = dy / dist;

                    float something = overlap * 0.51f;

                    ballA->pos.x -= collisionNormalX * something;
                    ballA->pos.y -= collisionNormalY * something;
                    ballB->pos.x += collisionNormalX * something;
                    ballB->pos.y += collisionNormalY * something;

                    float dvx = ballB->ballSpeed.x - ballA->ballSpeed.x;
                    float dvy = ballB->ballSpeed.y - ballA->ballSpeed.y;

                    float velocityAlongNormal = dvx * collisionNormalX + dvy * collisionNormalY;

                    if(velocityAlongNormal < 0) {

                        float restitution = 1.0f; // ideal for now.

                        float impulse = -(1.0f + restitution) * velocityAlongNormal;

                        impulse /= 2.0f;

                        float impulseX = impulse*collisionNormalX;
                        float impulseY = impulse*collisionNormalY;

                        ballA->ballSpeed.x -= impulseX;
                        ballA->ballSpeed.y -= impulseY;
                        ballB->ballSpeed.x += impulseX;
                        ballB->ballSpeed.y += impulseY;

                        // reduce points
                        int diff = abs(ballA->powLvl - ballB->powLvl);
                        if(diff == 0) {
                            ballA->powLvl = ballB->powLvl = -1;
                        }
                        ballA->powLvl -= diff;
                        ballB->powLvl -= diff;

                        if (ballA->powLvl <= 0) ballA->canIDestroy = true;
                        if (ballB->powLvl <= 0) ballB->canIDestroy = true;
                    }
                }

                b = b->next;
            }

            a = a->next;
        }

        Node *NcurrNN = ll_begin(&balls);
        while (NcurrNN != NULL) {

            Node *next = NcurrNN->next;
            Ball *ball = (Ball *)NcurrNN->data;

            if(ball->canIDestroy) {
                PlaySound(popBuffer2[soundIdx2%SOUND_BUFFER]);
                soundIdx2++;
                ll_delete(&balls, ball);
            }
            NcurrNN = next;
        }
        

        if(ll_length(&balls) >= 9) {
            Node *NcurrN = ll_begin(&balls);
            while(NcurrN != NULL) {
                Ball *ball = (Ball *)NcurrN->data;
                ball->spawner = false;
                NcurrN = NcurrN->next;
            }
        } else {
            Node *NcurrN = ll_begin(&balls);
            while(NcurrN != NULL) {
                Ball *ball = (Ball *)NcurrN->data;
                ball->spawner = true;
                NcurrN = NcurrN->next;
            }
        }
        
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            // traverse and draw each balls
            Node *currNN = ll_begin(&balls);
            while (currNN != NULL) {
                Ball *ball = (Ball *)currNN->data;
                
                char powLVL[4];

                snprintf(powLVL, sizeof(powLVL), "%d", ball->powLvl);

                int fontSize = ball->ballRadius - 10;

                Vector2 textSize = MeasureTextEx(font, powLVL, fontSize, 1);

                DrawCircleV(ball->pos, ball->ballRadius, ball->color);
                DrawText(powLVL, ball->pos.x - textSize.x/2, ball->pos.y - textSize.y/2, fontSize, RAYWHITE);

                currNN = currNN->next;
            }
                        
            DrawFPS(10, 10);
        EndDrawing();
    }

    ll_destroy(&balls);
    UnloadSound(pop);
    CloseAudioDevice();
    CloseWindow();
}

Ball createBall(void) {
    Ball newBall;

    // pos
    newBall.pos.x = GetRandomValue(0,GetScreenWidth());
    newBall.pos.y = GetRandomValue(0,GetScreenHeight());

    // vel
    newBall.ballSpeed.x = (float)GetRandomValue(3, 6);
    newBall.ballSpeed.y = (float)GetRandomValue(3, 6);

    // radius
    newBall.ballRadius = GetRandomValue(35, 90);

    // color

    Color colors[] = {
        RED,
        GREEN,
        BLUE,
        YELLOW,
        ORANGE,
        PURPLE,
        PINK,
        SKYBLUE,
        LIME,
        GOLD,
        MAROON
    };

    int count = sizeof(colors) / sizeof(colors[0]);

    newBall.color = colors[rand()%count];

    // powLvl
    newBall.powLvl = GetRandomValue(10,99);

    newBall.canIDestroy = false;
    newBall.spawner = false;

    return newBall;
}

void destroyBall(void *ptr) {
    Ball *ball = (Ball *) (ptr);
    free(ball);
}

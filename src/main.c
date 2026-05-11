#include "raylib.h"
#include "ll.h"
#include <time.h>
#include "stdlib.h"
#include <math.h>

#define WINDOW_HEIGHT 900
#define WINDOW_WIDTH 1080
#define SOUND_BUFFER 24

typedef struct Ball {
    Vector2 pos;
    Vector2 ballSpeed;
    float ballRadius;
    Color color;
    //int powLvl;
    bool spawner;
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

    Sound popBuffer[SOUND_BUFFER];
    for(int i = 0; i < SOUND_BUFFER; i++) {
        popBuffer[i] = LoadSoundAlias(pop);
    }
    int soundIdx = 0;

    Ball *firstBall = (Ball *)malloc(sizeof(Ball));
    *firstBall = createBall();
    firstBall->spawner = true;
    ll_append(&balls, firstBall);

    //SetTargetFPS(100);

    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        LinkedList toSpawnBalls;
        ll_init(&toSpawnBalls, destroyBall);
        Node *curr = ll_begin(&balls);

        while (curr != NULL) {
            Ball *ball = (Ball *)curr->data;

            ball->pos.x += ball->ballSpeed.x*dt*100;
            ball->pos.y += ball->ballSpeed.y*dt*100;

            if(ball->pos.x  - ball->ballRadius < 0) {
                ball->pos.x = ball->ballRadius;
                ball->ballSpeed.x = fabsf(ball->ballSpeed.x);

                if(ball->spawner) {
                    Ball *newBall = (Ball *)malloc(sizeof(Ball));
                    *newBall = createBall();
                    newBall->spawner = false;
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
                    newBall->spawner = false;
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
                    newBall->spawner = false;
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
                    newBall->spawner = false;
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

        ll_init(&toSpawnBalls, NULL); // destroy the toSpawnBall linkedList..

        // // add collision
        // Node *a = ll_begin(&balls);
        // while (a != NULL) {
        //     Ball *ballA = (Ball *)a->data;

        //     Node *b = a->next;
        //     while (b != NULL) {
        //         Ball *ballB = (Ball *)b->data;

        //         float dx = ballB->pos.x - ballA->pos.x;
        //         float dy = ballB->pos.y - ballB->pos.y;
        //         float dist = sqrtf(dx*dx + dy*dy);
        //         float minDist = ballA->ballRadius + ballB->ballRadius;

        //         if(dist < minDist && dist > 0.0f) {
        //             float nx = dx/dist;
        //             float ny = dy/dist;

        //             float overLap = (minDist - dist) / 2.0f;
        //             ballA->pos.x -= nx*overLap;
        //             ballA->pos.y -= ny*overLap;
        //             ballB->pos.x += nx*overLap;
        //             ballB->pos.y += ny*overLap;

        //             float dvx = ballA->ballSpeed.x - ballB->ballSpeed.x;
        //             float dvy = ballA->ballSpeed.y - ballB->ballSpeed.y;
        //             float dot = dvx * nx + dvy * ny;

        //             if(dot > 0.0f) {
        //                 ballA->ballSpeed.x -= dot * nx;
        //                 ballA->ballSpeed.y -= dot * ny;
        //                 ballB->ballSpeed.x += dot * nx;
        //                 ballB->ballSpeed.y += dot * ny;
        //             }
        //         }
        //         b = b->next;
        //     }

        //     a = a->next;
        // }
        
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            // traverse and draw each balls
            Node *currNN = ll_begin(&balls);
            while (currNN != NULL) {
                Ball *ball = (Ball *)currNN->data;
                DrawCircleV(ball->pos, ball->ballRadius, ball->color);
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

    // rad
    newBall.ballRadius = GetRandomValue(10, 90);

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

    return newBall;
}

void destroyBall(void *ptr) {
    Ball *ball = (Ball *) (ptr);
    free(ball);
}

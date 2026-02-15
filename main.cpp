#include <iostream>
#include <raylib.h>
#include <vector>
#include <string>
using namespace std;

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 500
#define TARGET_FPS 60

#define PLAYER_SIZE_MIN 20.0
#define PLAYER_SIZE_MAX 30.0
#define PLAYER_SIZE_DELTA 0.25
#define PLAYER_SPEED 1.0

#define START_AREA_WIDTH 50
#define FINISH_AREA_WIDTH 75
#define PLATE_HEIGHT 50
#define PLATE_TOP_Y 190.0
#define PLATE_BOTTOM_Y 260.0

#define fRAILS_TOP_Y 190.0
#define fRAILS_BOTTOM_Y 240.0
#define sRAILS_TOP_Y 260.0
#define sRAILS_BOTTOM_Y 310.0

#define LEVEL_1 8
#define LEVEL_2 12
#define LEVEL_3 16

int getDir() {
    if (IsKeyDown(KEY_A)) {
        return 1;
    }
    if (IsKeyDown(KEY_D)) {
        return 2;
    }
    if (IsKeyDown(KEY_W)) {
        return 3;
    }
    if (IsKeyDown(KEY_S)) {
        return 4;
    }
    if (IsKeyPressed(KEY_X)) {
        exit(0);
    }
    return 0;
}
bool onGlass(Rectangle& player, vector<vector<Rectangle>>& plates, int countOfPlates) {
    for (int i = 0; i < countOfPlates; i++) {
        if (CheckCollisionRecs(player, plates[i][0]) or CheckCollisionRecs(player, plates[i][1])) {
            return true;
        }
    }
    return false;
}
pair<bool, int> isBroken(Rectangle& player, vector<vector<Rectangle>>& plates, vector<vector<int>>& glassStates, int countOfPlates) {
    for (int i = 0; i < countOfPlates; i++) {
        if (CheckCollisionRecs(player, plates[i][0])) {
            if (glassStates[i][0] == 0) {
                return { true, i };
            }
        }
        else if (CheckCollisionRecs(player, plates[i][1])) {
            if (glassStates[i][1] == 0) {
                return { true, i };
            }
        }
    }
    return { false, -1 };
}
bool isIntoGap(Rectangle& player, Rectangle& startArea, Rectangle& finishArea, bool onGlass) {
    if (CheckCollisionRecs(player, startArea) or CheckCollisionRecs(player, finishArea)) {
        return false;
    }
    if (!onGlass) {
        return true;
    }
    return false;
}
void initPlates(vector<vector<Rectangle>>& plates, int countOfPlates) {
    plates.clear();
    float startX = 113.0f;
    int stepX;
    float plateWidth;
    if (countOfPlates == LEVEL_1) {
        stepX = 100;
        plateWidth = 50;
    }
    else if (countOfPlates == LEVEL_2) {
        stepX = 70;
        plateWidth = 30;
    }
    else if (countOfPlates == LEVEL_3) {
        stepX = 50;
        plateWidth = 25;
    }
    for (int i = 0; i < countOfPlates; i++) {
        Rectangle topPlate = { startX + i * stepX, PLATE_TOP_Y, plateWidth, PLATE_HEIGHT };
        Rectangle bottomPlate = { startX + i * stepX, PLATE_BOTTOM_Y, plateWidth, PLATE_HEIGHT };
        plates.push_back({ topPlate, bottomPlate });
    }
}
int main() {
    srand(time(NULL));
    SetTargetFPS(TARGET_FPS);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Glass bridge");
    int countOfPlates = LEVEL_1;
    Rectangle rec1{ 200, 125, 100, 20 };
    Rectangle rec2{ 200, 150, 100, 20 };
    Rectangle rec3{ 200, 175, 100, 20 };
    Color col1{ 0, 0, 0, 0 };
    Color col2{ 0, 0, 0, 0 };
    Color col3{ 0, 0, 0, 0 };
    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, rec1)) {
            col1 = { 0, 0, 0, 75 };
            col2 = { 0, 0, 0, 0 };
            col3 = { 0, 0, 0, 0 };
        }
        if (CheckCollisionPointRec(mouse, rec2)) {
            col2 = { 0, 0, 0, 75 };
            col1 = { 0, 0, 0, 0 };
            col3 = { 0, 0, 0, 0 };
        }
        if (CheckCollisionPointRec(mouse, rec3)) {
            col3 = { 0, 0, 0, 75 };
            col1 = { 0, 0, 0, 0 };
            col2 = { 0, 0, 0, 0 };
        }
        if (CheckCollisionPointRec(mouse, rec1) and IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            countOfPlates = LEVEL_1;
            break;
        }
        if (CheckCollisionPointRec(mouse, rec2) and IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            countOfPlates = LEVEL_2;
            break;
        }
        if (CheckCollisionPointRec(mouse, rec3) and IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            countOfPlates = LEVEL_3;
            break;
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("CHOOSE DIFFICULT LEVEL (1-3):", 150, 75, 40, BLACK);
        DrawRectangleRec(rec1, col1);
        DrawRectangleRec(rec2, col2);
        DrawRectangleRec(rec3, col3);
        DrawText("LEVEL 1", 200, 125, 20, BLACK);
        DrawText("LEVEL 2", 200, 150, 20, BLACK);
        DrawText("LEVEL 3", 200, 175, 20, BLACK);
        EndDrawing();
    }
    vector<vector<int>> glassStates(countOfPlates, vector<int>(2, 0));
    for (int i = 0; i < countOfPlates; i++) {
        glassStates[i][rand() % 2] = 1;
    }
    vector<vector<Rectangle>> plates;
    initPlates(plates, countOfPlates);
    float playerX = 15.0f;
    float playerY = 240.0f;
    float playerSize = PLAYER_SIZE_MIN;
    int playerState = 0;
    Rectangle startArea{ 0, 100, START_AREA_WIDTH, 300 };
    Rectangle finishArea{ WINDOW_WIDTH - FINISH_AREA_WIDTH, 100, FINISH_AREA_WIDTH, 300 };
    bool isplayeralive = true;
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleRec(startArea, RED);
        DrawRectangleRec(finishArea, RED);
        DrawLine(50, fRAILS_TOP_Y, 950, fRAILS_TOP_Y, RED);
        DrawLine(50, fRAILS_BOTTOM_Y, 950, fRAILS_BOTTOM_Y, RED);
        DrawLine(50, sRAILS_TOP_Y, 950, sRAILS_TOP_Y, RED);
        DrawLine(50, sRAILS_BOTTOM_Y, 950, sRAILS_BOTTOM_Y, RED);
        int direction = getDir();
        if (direction == 1 and playerX > 0) {
            playerX -= PLAYER_SPEED;
        }
        else if (direction == 2 and playerX < WINDOW_WIDTH - playerSize) {
            playerX += PLAYER_SPEED;
        }
        else if (direction == 3 and playerY > 0) {
            playerY -= PLAYER_SPEED;
        }
        else if (direction == 4 and playerY < WINDOW_HEIGHT - playerSize) {
            playerY += PLAYER_SPEED;
        }
        if (IsKeyPressed(KEY_SPACE) and playerState == 0) {
            playerState = 1;
        }
        if (playerState == 1) {
            playerSize += PLAYER_SIZE_DELTA;
            playerX -= PLAYER_SIZE_DELTA/2;
            playerY -= PLAYER_SIZE_DELTA/2;
            if (playerSize == PLAYER_SIZE_MAX) {
                playerState = 2;
            }
        }
        else if (playerState == 2) {
            playerSize -= PLAYER_SIZE_DELTA;
            playerX += PLAYER_SIZE_DELTA/2;
            playerY += PLAYER_SIZE_DELTA/2;
            if (playerSize == PLAYER_SIZE_MIN) {
                playerState = 0;
            }
        }
        Rectangle player{ playerX, playerY, playerSize, playerSize };
        bool onGlas = onGlass(player, plates, countOfPlates);
        if (playerState == 0 and isIntoGap(player, startArea, finishArea, onGlas)) {
            isplayeralive = false;
        }
        pair<bool, int> broke = isBroken(player, plates, glassStates, countOfPlates);
        if (playerState == 0 and broke.first) {
            isplayeralive = false;

        }
        for (int i = 0; i < countOfPlates; i++) {
            DrawRectangleRec(plates[i][0], { 102, 191, 255, 125 });
            DrawRectangleRec(plates[i][1], { 102, 191, 255, 125 });
        }
        DrawRectangleRec(player, WHITE);
        if (!isplayeralive or (playerState == 0 and CheckCollisionRecs(player, finishArea))) {
            EndDrawing();
            break;
        }
        EndDrawing();
    }
    string text;
    string newt1 = "You won, so you can get off here.";
    string newt2 = "   -Aaaaaaaaaaaaa!\n-Who was it?\n-It is a test subject 217Q6.\n-Why they can`t do it?\n-I don`t know.";
    float i = 0;
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        if (isplayeralive) {
            if (i < newt1.size()) {
                DrawText(text.c_str(), 100, 50, 50, WHITE);
                if (i == trunc(i)) {
                    text += newt1[i];
                }
                i += 0.5;
            }
            if (i == newt1.size()) {
                DrawText(text.c_str(), 100, 50, 50, WHITE);
                DrawText("YOU HAVE GOOD ENDING. Press LMB to start new test.", 125, 400, 30, WHITE);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    EndDrawing();
                    CloseWindow();
                    exit(0);
                }
            }
        }
        else {
            if (i < newt2.size()) {
                DrawText(text.c_str(), 100, 50, 50, WHITE);
                if (i == trunc(i)) {
                    text += newt2[i];
                }
                i += 0.5;
            }
            if (i == newt2.size()) {
                DrawText(text.c_str(), 100, 50, 50, WHITE);
                DrawText("YOU HAVE BAD ENDING. Press LMB to start new test.", 125, 400, 30, WHITE);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    EndDrawing();
                    CloseWindow();
                    exit(0);
                }
            }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
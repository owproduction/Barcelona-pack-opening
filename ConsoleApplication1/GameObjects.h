#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>

const int MAX_WIDTH = 600;
const int MAX_HEIGHT = 800;
const float damping = -0.05f;

// Структура для футболиста
struct Footballer {
    Texture2D texture;
    std::string name;
    bool unlocked;
};

extern std::vector<Footballer> footballers;
extern int playersPerPage;

// Структура для ворот
struct Goal {
    Rectangle leftPost;
    Rectangle rightPost;
    Rectangle crossbar;
    float width;
    float height;
    Vector2 position;
};

// Структура для вратаря
struct Goalkeeper {
    Rectangle bounds;
    Vector2 position;
    Vector2 velocity;
    Vector2 startPosition;
    float width;
    float height;
    bool isJumping;
    float jumpTimer;
    Vector2 jumpDirection;
    Texture2D texture;
    int playerControlled;
    float moveSpeed;
};

struct Circle {
    Vector2 position;
    Vector2 velocity;
    Vector2 accelerate;
    float radius;
    float weight;
    Vector2 startPosition;
    Vector2 hitPosition;
    bool hasSpin[4];
    float spinForce[4];
    Vector2 spinDirection[4];
    int controllingPlayer;
    bool canMoveFreely;
};

// Функции для создания объектов
Goal CreateGoal(float width, float height, Vector2 position);
Goalkeeper CreateGoalkeeper(float width, float height, Vector2 position, const char* texturePath, int playerControlled = 0);
int GenerateCircles(std::vector<Circle>& circles, int count = 1, float radius = 15.0f,
    int minVelocity = 0, int maxVelocity = 0, int gameMode = 0);

// Функции для работы с футболистами
void LoadFootballers();

// Функции отрисовки
void DrawGoalkeeper(const Goalkeeper& keeper);
void DrawGoal(const Goal& goal);

#endif
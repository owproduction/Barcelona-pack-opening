#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include "raylib.h"
#include "raymath.h"
#include <string>
#include "Game.h"  // Добавляем для GameMode

// Структура для кнопки
struct Button {
    Rectangle bounds;
    const char* text;
    Color color;
    Color textColor;
};

// Структура для ворот
struct Goal {
    Rectangle leftPost;   // Левая штанга
    Rectangle rightPost;  // Правая штанга
    Rectangle crossbar;   // Перекладина
    float width;          // Ширина ворот
    float height;         // Высота ворот
    Vector2 position;     // Позиция ворот (центр)
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
    Texture2D texture;    // Текстура вратаря
    int playerControlled; // 0 - AI, 1 - Игрок 1, 2 - Игрок 2
    float moveSpeed;      // Скорость движения для игрока
};

struct Circle {
    Vector2 position;
    Vector2 velocity;
    Vector2 accelerate;
    float radius;
    float weight;
    Vector2 startPosition; // Начальная позиция для возврата
    Vector2 hitPosition;   // Позиция удара для возврата при промахе
    bool hasSpin[4];       // Типы кручения: 0=влево, 1=вправо, 2=вверх, 3=вниз
    float spinForce[4];    // Сила каждого типа кручения
    Vector2 spinDirection[4]; // Направление каждого типа кручения
    int controllingPlayer; // Какой игрок управляет мячом (1 или 2)
    bool canMoveFreely;    // Может ли мяч свободно перемещаться
};

// Функции для создания объектов
Button CreateButton(float x, float y, float width, float height, const char* text, Color color, Color textColor);
Goal CreateGoal(float width, float height, Vector2 position);
Goalkeeper CreateGoalkeeper(float width, float height, Vector2 position, const char* texturePath, int playerControlled = 0);

#endif
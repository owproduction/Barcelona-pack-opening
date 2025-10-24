#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"
#include "Game.h"  // Добавляем для MAX_WIDTH, MAX_HEIGHT
#include <string>

// Структура для анимации
struct Animation {
    bool active;
    float duration;
    float timer;
    Vector2 position;
    Vector2 velocity;
    float scale;
    float rotation;
    Color color;
    std::string text;
    Texture2D texture;
};

// Новая структура для анимации открытия пака
struct PackAnimation {
    bool active;
    Texture2D playerTexture;
    std::string playerName;
    float timer;
    bool showSkipButton;
};

// Глобальные анимации
extern Animation goalAnimation;
extern PackAnimation packAnimation;

// Функции анимации
void StartGoalAnimation();
void StartPackAnimation(Texture2D playerTexture, const std::string& playerName);
void UpdateAnimations(float deltaTime);
void DrawAnimations();

#endif
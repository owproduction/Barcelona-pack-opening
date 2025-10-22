#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"
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

class AnimationSystem {
private:
    Animation goalAnimation;
    Animation packAnimation;

public:
    AnimationSystem();
    void UpdateAnimations(float deltaTime);
    void DrawAnimations();
    void StartGoalAnimation();
    void StartPackAnimation(Texture2D playerTexture, const std::string& playerName);
};

#endif
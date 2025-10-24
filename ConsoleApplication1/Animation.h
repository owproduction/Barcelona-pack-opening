#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"
#include "Game.h"  // ��������� ��� MAX_WIDTH, MAX_HEIGHT
#include <string>

// ��������� ��� ��������
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

// ���������� ��������
extern Animation goalAnimation;
extern Animation packAnimation;

// ������� ��������
void StartGoalAnimation();
void StartPackAnimation(Texture2D playerTexture, const std::string& playerName);
void UpdateAnimations(float deltaTime);
void DrawAnimations();

#endif
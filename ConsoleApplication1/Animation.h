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

// ����� ��������� ��� �������� �������� ����
struct PackAnimation {
    bool active;
    Texture2D playerTexture;
    std::string playerName;
    float timer;
    bool showSkipButton;
};

// ���������� ��������
extern Animation goalAnimation;
extern PackAnimation packAnimation;

// Структура для GIF анимации
struct GifAnimation {
    bool active;
    Vector2 position;
    Image gifImage;
    Texture2D texture;
    int totalFrames;
    int currentFrame;
    unsigned int nextFrameDataOffset;
    float frameDelay;        // В секундах
    float frameTimer;
    bool loop;
    float scale;
};

// Глобальная GIF анимация удара
extern GifAnimation kickGifAnimation;

// Функции для работы с GIF
bool LoadGifAnimation(GifAnimation& anim, const char* gifPath, float frameDelay = 0.08f);
void StartGifAnimation(GifAnimation& anim, float posX, float posY, bool loop = false);
void UpdateGifAnimation(GifAnimation& anim, float deltaTime);
void DrawGifAnimation(const GifAnimation& anim);
void UnloadGifAnimation(GifAnimation& anim);

// Вспомогательная функция для удара
void StartKickAnimation(float ballX, float ballY);


// ������� ��������
void StartGoalAnimation();
void StartPackAnimation(Texture2D playerTexture, const std::string& playerName);
void UpdateAnimations(float deltaTime);
void DrawAnimations();

#endif
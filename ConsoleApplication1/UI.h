#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "GameObjects.h"
#include <string>

// Структура для кнопки
struct Button {
    Rectangle bounds;
    const char* text;
    Color color;
    Color textColor;
};

// Функции для UI
Button CreateButton(float x, float y, float width, float height, const char* text, Color color, Color textColor);
bool IsButtonClicked(Button button);
void DrawButton(Button button);

// Функции отрисовки экранов
void DrawMainMenu(Button playButton, Button twoPlayersButton, Button shopButton, 
                 Button collectionButton, Button exitButton, int coins);
void DrawGameHUD(int score, int coins, int gameMode, bool slowMoActive, bool spinActive[4], int selectedPlayerIndex);
void DrawPowerBar(const Circle& circle, const Vector2& mousePosition, bool isDragging, bool spinActive[4]);
void DrawDirectionArrow(const Circle& circle, const Vector2& mousePosition, bool isDragging, bool spinActive[4]);
void DrawGameField(const Goal& goal);

#endif
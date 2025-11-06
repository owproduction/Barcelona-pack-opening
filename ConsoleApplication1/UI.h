#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "GameObjects.h"
#include "Game.h"

// ������� UI
bool IsButtonClicked(Button button);
void DrawButton(Button button);
void DrawGoal(const Goal& goal);
void DrawGoalkeeper(const Goalkeeper& keeper);
void DrawPowerBar(const Circle& circle, const Vector2& mousePosition, bool isDragging, bool spinActive[4]);
void DrawArrow(const Arrow& arrow);
void DrawMainMenu(Button playButton, Button twoPlayersButton, Button shopButton, Button collectionButton, Button exitButton);
void DrawTwoPlayersMenu(Button player1KeeperButton, Button player2KeeperButton, Button backButton);
void DrawGameModeSelection(Button freeKickButton, Button penaltyButton, Button backButton);
void DrawShop();
void DrawCollection();
void DrawTwoPlayersScore(int player1Score, int player2Score);
void DrawGameModeInfo(GameMode mode);
void DrawPackAnimationScreen(Texture2D playerTexture, const std::string& playerName, bool showSkipButton);

#endif
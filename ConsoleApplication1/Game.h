#ifndef GAME_H
#define GAME_H
#pragma once

#include "raylib.h"
#include "GameObjects.h"
#include "UI.h"
#include "Animation.h"
#include "SaveSystem.h"
#include "Collision.h"
#include <vector>

// ��������� ����
enum GameState {
    MENU,
    SELECT_PLAYER,
    PLAYING,
    SHOP,
    COLLECTION,
    TWO_PLAYERS_MENU,
    TWO_PLAYERS_GAME,
    GAME_MODE_SELECTION
};

// ������ ����
enum GameMode {
    FREE_KICK,
    PENALTY
};

class Game {
private:
    // �������� ����������
    GameState gameState = MENU;
    GameMode currentGameMode = FREE_KICK;
    bool isSinglePlayer = true;
    bool slowMoActive = false;
    float slowMoFactor = 0.3f;

    // ������� �������
    std::vector<Circle> circles;
    Goal goal;
    Goalkeeper goalkeeper;

    // UI ��������
    Button playButton, twoPlayersButton, shopButton, collectionButton, exitButton;
    Button player1KeeperButton, player2KeeperButton, backButton;
    Button freeKickButton, penaltyButton, modeBackButton;

    // ������� ������
    bool dragging = false;
    Circle* selectedCircle = nullptr;
    Vector2 dragStartPosition;
    int score = 0;
    int player1Score = 0, player2Score = 0;
    bool spinActive[4] = { false, false, false, false };
    int goalkeeperController = 0;
    int selectedPlayerIndex = -1;
    int collectionPage = 0;

    // �������
    SaveSystem saveSystem;
    AnimationSystem animationSystem;  // ����������: AnimationSystem ������ Animation

public:
    Game();
    void Run();
    void Update(float deltaTime);
    void Render();
    void HandleInput();
    void ResetGame();
    void SwitchToGameModeSelection(bool singlePlayer);
};

#endif
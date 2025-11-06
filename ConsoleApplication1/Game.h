#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>

const int MAX_WIDTH = 600;
const int MAX_HEIGHT = 800;
const float damping = -0.05f;

// ��������� ����
enum GameState {
    MENU,
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

// ��������� ��� ����������
struct Footballer {
    Texture2D texture;
    std::string name;
    bool unlocked;
};

// ���������� ����������
extern int coins;
extern std::vector<Footballer> footballers;
extern Texture2D packTexture;
extern GameMode currentGameMode;
extern int collectionPage;
extern int playersPerPage;

// ������� ����
void LoadFootballers();
void OpenPack();
void UpdateAnimations(float deltaTime);
void DrawAnimations();

#endif
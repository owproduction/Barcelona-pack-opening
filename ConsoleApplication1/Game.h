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
// Убираем enum PlayerRarity, оставляем только индивидуальные бонусы

struct Footballer {
    Texture2D texture;
    std::string name;
    bool unlocked;
    float powerBonus;  // Индивидуальный бонус силы для каждого игрока
    Texture2D ballTexture; // Текстура мяча для этого игрока
};

// Глобальные переменные
extern std::vector<Footballer> footballers;
extern int coins;
extern int collectionPage;
extern int playersPerPage;
extern Texture2D packTexture;
extern GameMode currentGameMode;                
extern Footballer* selectedPlayer;
extern float playerPowerBonus;
extern Texture2D currentBallTexture;

// Функции
void LoadFootballers();
void OpenPack();
void SelectPlayer(int index);



#endif
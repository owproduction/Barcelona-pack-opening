#include "Game.h"
#include "GameObjects.h"
#include "SaveSystem.h"
#include "Animation.h"
#include <iostream>

// Глобальные переменные
std::vector<Footballer> footballers;
int coins = 0;
int collectionPage = 0;
int playersPerPage = 9;
Texture2D packTexture = { 0 };
GameMode currentGameMode = FREE_KICK;
Footballer* selectedPlayer = nullptr;
float playerPowerBonus = 1.0f;
Texture2D currentBallTexture = { 0 };


void LoadFootballers() {

    Footballer player1 = {
        LoadTexture("player1.png"), "Messi", false, 1.3f, // +30%
        LoadTexture("ball_messi.png")
    };
    Footballer player2 = {
        LoadTexture("player2.png"), "Ronaldo", false, 1.3f, // +30%
        LoadTexture("ball_ronaldo.png")
    };
    Footballer player3 = {
        LoadTexture("player3.png"), "Neymar", false, 1.25f, // +25%
        LoadTexture("ball_neymar.png")
    };
    Footballer player4 = {
        LoadTexture("player4.png"), "Mbappe", false, 1.25f, // +25%
        LoadTexture("ball_mbappe.png")
    };
    Footballer player5 = {
        LoadTexture("player5.png"), "Haaland", false, 1.2f, // +20%
        LoadTexture("ball_haaland.png")
    };
    Footballer player6 = {
        LoadTexture("player6.png"), "Lewandowski", false, 1.2f, // +20%
        LoadTexture("ball_lewandowski.png")
    };
    Footballer player7 = {
        LoadTexture("player7.png"), "De Bruyne", false, 1.15f, // +15%
        LoadTexture("ball_debruyne.png")
    };
    Footballer player8 = {
        LoadTexture("player8.png"), "Salah", false, 1.1f, // +10%
        LoadTexture("ball_salah.png")
    };
    Footballer player9 = {
        LoadTexture("player9.png"), "Kane", false, 1.1f, // +10%
        LoadTexture("ball_kane.png")
    };
    Footballer player10 = {
        LoadTexture("player10.png"), "Benzema", false, 1.1f, // +10%
        LoadTexture("ball_benzema.png")
    };
    Footballer player11 = {
        LoadTexture("player11.png"), "Modric", false, 1.05f, // +5%
        LoadTexture("ball_modric.png")
    };
    Footballer player12 = {
        LoadTexture("player12.png"), "Van Dijk", false, 1.05f, // +5%
        LoadTexture("ball_vandijk.png")
    };
    
    footballers = { player1, player2, player3, player4, player5, player6, player7, player8,
                   player9, player10, player11, player12 }; 

    // Устанавливаем текстуру мяча по умолчанию
    currentBallTexture = LoadTexture("ball.png");
}

void SelectPlayer(int index) {
    if (index >= 0 && index < footballers.size() && footballers[index].unlocked) {
        selectedPlayer = &footballers[index];

        // Устанавливаем индивидуальный бонус силы игрока
        playerPowerBonus = footballers[index].powerBonus;

        // Меняем текстуру мяча на текстуру игрока
        if (footballers[index].ballTexture.id != 0) {
            currentBallTexture = footballers[index].ballTexture;
        }
    }
}

void OpenPack() {
    if (coins >= 10) {
        coins -= 10;

        std::vector<int> lockedIndices;
        for (int i = 0; i < footballers.size(); i++) {
            if (!footballers[i].unlocked) {
                lockedIndices.push_back(i);
            }
        }

        if (!lockedIndices.empty()) {
            int randomIndex = lockedIndices[GetRandomValue(0, lockedIndices.size() - 1)];
            footballers[randomIndex].unlocked = true;

            // Автоматически выбираем нового игрока
            SelectPlayer(randomIndex);

            StartPackAnimation(footballers[randomIndex].texture, footballers[randomIndex].name);
        }

        SaveProgress();
    }
}
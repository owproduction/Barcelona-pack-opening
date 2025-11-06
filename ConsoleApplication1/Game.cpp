#include "Game.h"
#include "GameObjects.h"
#include "SaveSystem.h"
#include "Animation.h"
#include <iostream>

// ���������� ����������
int coins = 0;
std::vector<Footballer> footballers;
Texture2D packTexture = { 0 };
GameMode currentGameMode = FREE_KICK;
int collectionPage = 0;
int playersPerPage = 6;

void LoadFootballers() {
    footballers.clear();

    // ��������� 15 ����������� (������ ���� �� ���� PNG �����)
    Footballer f1;
    f1.texture = LoadTexture("player1.png");
    f1.name = "Messi";
    f1.unlocked = false;
    footballers.push_back(f1);

    Footballer f2;
    f2.texture = LoadTexture("player2.png");
    f2.name = "Ronaldo";
    f2.unlocked = false;
    footballers.push_back(f2);

    Footballer f3;
    f3.texture = LoadTexture("player3.png");
    f3.name = "Neymar";
    f3.unlocked = false;
    footballers.push_back(f3);

    Footballer f4;
    f4.texture = LoadTexture("player4.png");
    f4.name = "Mbappe";
    f4.unlocked = false;
    footballers.push_back(f4);

    Footballer f5;
    f5.texture = LoadTexture("player5.png");
    f5.name = "Haaland";
    f5.unlocked = false;
    footballers.push_back(f5);

    // ��������� ��� 10 �����������
    Footballer f6;
    f6.texture = LoadTexture("player6.png");
    f6.name = "Lewandowski";
    f6.unlocked = false;
    footballers.push_back(f6);

    Footballer f7;
    f7.texture = LoadTexture("player7.png");
    f7.name = "Benzema";
    f7.unlocked = false;
    footballers.push_back(f7);

    Footballer f8;
    f8.texture = LoadTexture("player8.png");
    f8.name = "Salah";
    f8.unlocked = false;
    footballers.push_back(f8);

    Footballer f9;
    f9.texture = LoadTexture("player9.png");
    f9.name = "Kane";
    f9.unlocked = false;
    footballers.push_back(f9);

    Footballer f10;
    f10.texture = LoadTexture("player10.png");
    f10.name = "De Bruyne";
    f10.unlocked = false;
    footballers.push_back(f10);

    Footballer f11;
    f11.texture = LoadTexture("player11.png");
    f11.name = "Modric";
    f11.unlocked = false;
    footballers.push_back(f11);

    Footballer f12;
    f12.texture = LoadTexture("player12.png");
    f12.name = "Van Dijk";
    f12.unlocked = false;
    footballers.push_back(f12);

    Footballer f13;
    f13.texture = LoadTexture("player13.png");
    f13.name = "Courtois";
    f13.unlocked = false;
    footballers.push_back(f13);

    Footballer f14;
    f14.texture = LoadTexture("player14.png");
    f14.name = "Son";
    f14.unlocked = false;
    footballers.push_back(f14);

    Footballer f15;
    f15.texture = LoadTexture("player15.png");
    f15.name = "Zlatan";
    f15.unlocked = false;
    footballers.push_back(f15);
}

void OpenPack() {
    if (coins >= 10) {
        coins -= 10;

        // ��������� �������� ����� ����� �����
        SaveProgress();

        // ��������� ����� ����������
        int randomIndex = GetRandomValue(0, footballers.size() - 1);

        // ���������, ��� �� ��� ������������� ���� ���������
        bool wasUnlocked = footballers[randomIndex].unlocked;
        footballers[randomIndex].unlocked = true;

        // ��������� �������� �������� ����
        StartPackAnimation(footballers[randomIndex].texture, footballers[randomIndex].name);

        // ��������� �������� ����� ������������� ����������
        SaveProgress();
    }
}
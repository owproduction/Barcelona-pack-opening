#include "SaveSystem.h"
#include "Game.h"
#include <fstream>

void SaveProgress() {
    std::ofstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        // ��������� ���������� �����
        file.write(reinterpret_cast<const char*>(&coins), sizeof(coins));

        // ��������� ��������� ������������� �����������
        for (const auto& footballer : footballers) {
            file.write(reinterpret_cast<const char*>(&footballer.unlocked), sizeof(bool));
        }

        file.close();
    }
}

void LoadProgress() {
    std::ifstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        // ��������� ���������� �����
        file.read(reinterpret_cast<char*>(&coins), sizeof(coins));

        // ��������� ��������� ������������� �����������
        for (auto& footballer : footballers) {
            file.read(reinterpret_cast<char*>(&footballer.unlocked), sizeof(bool));
        }

        file.close();
    }
}

void ResetProgress() {
    coins = 0;
    for (auto& footballer : footballers) {
        footballer.unlocked = false;
    }
    SaveProgress();
}
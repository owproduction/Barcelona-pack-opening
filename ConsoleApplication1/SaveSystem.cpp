#include "SaveSystem.h"
#include "Game.h"
#include <fstream>

void SaveProgress() {
    std::ofstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        // Сохраняем количество монет
        file.write(reinterpret_cast<const char*>(&coins), sizeof(coins));

        // Сохраняем состояние разблокировки футболистов
        for (const auto& footballer : footballers) {
            file.write(reinterpret_cast<const char*>(&footballer.unlocked), sizeof(bool));
        }

        file.close();
    }
}

void LoadProgress() {
    std::ifstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        // Загружаем количество монет
        file.read(reinterpret_cast<char*>(&coins), sizeof(coins));

        // Загружаем состояние разблокировки футболистов
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
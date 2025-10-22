#include "SaveSystem.h"
#include <fstream>

SaveSystem::SaveSystem() {
    footballersRef = &footballers;
}

void SaveSystem::SetFootballersReference(std::vector<Footballer>* footballers) {
    footballersRef = footballers;
}

void SaveSystem::SaveProgress() {
    std::ofstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&coins), sizeof(coins));
        for (const auto& footballer : *footballersRef) {
            file.write(reinterpret_cast<const char*>(&footballer.unlocked), sizeof(bool));
        }
        file.close();
    }
}

void SaveSystem::LoadProgress() {
    std::ifstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&coins), sizeof(coins));
        for (auto& footballer : *footballersRef) {
            file.read(reinterpret_cast<char*>(&footballer.unlocked), sizeof(bool));
        }
        file.close();
    }
}

void SaveSystem::ResetProgress() {
    coins = 0;
    for (auto& footballer : *footballersRef) {
        footballer.unlocked = false;
    }
    SaveProgress();
}

void SaveSystem::AddCoins(int amount) {
    coins += amount;
}

int SaveSystem::GetCoins() const {
    return coins;
}
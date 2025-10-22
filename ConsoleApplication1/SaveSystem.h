#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H

#include "GameObjects.h"
#include <vector>

class SaveSystem {
private:
    int coins = 0;
    std::vector<Footballer>* footballersRef;

public:
    SaveSystem();
    void SetFootballersReference(std::vector<Footballer>* footballers);
    void SaveProgress();
    void LoadProgress();
    void ResetProgress();
    void AddCoins(int amount);
    int GetCoins() const;
};

#endif
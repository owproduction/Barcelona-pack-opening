#include "GameObjects.h"

std::vector<Footballer> footballers;
int playersPerPage = 6;

// ... остальные существующие функции ...

void DrawGoalkeeper(const Goalkeeper& keeper) {
    if (keeper.texture.id != 0) {
        Rectangle sourceRec = { 0, 0, (float)keeper.texture.width, (float)keeper.texture.height };
        Rectangle destRec = { keeper.position.x, keeper.position.y, keeper.width, keeper.height };
        Vector2 origin = { keeper.width / 2, keeper.height / 2 };
        DrawTexturePro(keeper.texture, sourceRec, destRec, origin, 0, WHITE);
    }
    else {
        DrawRectangleRec(keeper.bounds, ORANGE);
    }
}

void DrawGoal(const Goal& goal) {
    DrawRectangleRec(goal.leftPost, WHITE);
    DrawRectangleRec(goal.rightPost, WHITE);
    DrawRectangleRec(goal.crossbar, WHITE);
}
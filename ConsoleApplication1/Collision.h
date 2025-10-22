#ifndef COLLISION_H
#define COLLISION_H

#include "GameObjects.h"
#include <vector>
#include <utility>

// Функции столкновений между объектами
void HandleCollision(std::vector<std::pair<Circle*, Circle*>>& collisions, Circle& a, Circle& b);
void StaticCollisionResolution(Circle& a, Circle& b);
void DynamicCollisionResolution(Circle& a, Circle& b);

// Функции столкновений с воротами
bool CheckGoalCollision(const Circle& circle, const Goal& goal, Vector2& normal);
void HandleGoalCollision(Circle& circle, const Goal& goal);

// Функции столкновений с вратарем
bool CheckGoalkeeperCollision(const Circle& circle, const Goalkeeper& keeper);
void HandleGoalkeeperCollision(Circle& circle, Goalkeeper& keeper);

// Функции столкновений со стенами
void HandleWallCollision(Circle& circle);
bool IsCircleOutOfBounds(const Circle& circle);

// Функции проверки гола и линий
bool CheckGoalLineCrossing(const Circle& circle, const Goal& goal);
bool CheckGreenLineTouch(const Circle& circle, const Goal& goal);
bool CheckCompleteGoal(const Circle& circle, const Goal& goal);
bool CheckSideLinesCollision(const Circle& circle, const Goal& goal);

// Функции для работы с мячом
void ResetCircle(Circle& circle, int gameMode);
void ResetToHitPosition(Circle& circle, int gameMode);
void ApplySpin(Circle& circle, const Vector2& direction, bool spinActive[4]);
void UpdateSpin(Circle& circle, float deltaTime);

// Функции для вратаря
void MakeGoalkeeperJump(Goalkeeper& keeper);
void UpdateGoalkeeper(Goalkeeper& keeper, float deltaTime);

#endif
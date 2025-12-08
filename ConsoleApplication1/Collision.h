#ifndef COLLISION_H
#define COLLISION_H

#include "raylib.h"
#include "GameObjects.h"
#include "Game.h"  
#include <vector>


bool CheckGoalCollision(const Circle& circle, const Goal& goal, Vector2& normal);
bool CheckGoalkeeperCollision(const Circle& circle, const Goalkeeper& keeper);
void HandleGoalkeeperCollision(Circle& circle, Goalkeeper& keeper);
void HandleGoalCollision(Circle& circle, const Goal& goal);
bool CheckGoalLineCrossing(const Circle& circle, const Goal& goal);
bool CheckGreenLineTouch(const Circle& circle, const Goal& goal);
bool CheckCompleteGoal(const Circle& circle, const Goal& goal);
bool CheckSideLinesCollision(const Circle& circle, const Goal& goal);
void MakeGoalkeeperJump(Goalkeeper& keeper);
void UpdateGoalkeeper(Goalkeeper& keeper, float deltaTime);
void ApplySpin(Circle& circle, const Vector2& direction, bool spinActive[4]);
void UpdateSpin(Circle& circle, float deltaTime);
void UpdateArrow(Arrow& arrow, const Circle& circle, const Vector2& mousePosition, bool isDragging);
int GenerateCircles(std::vector<Circle>& circles, int count = 1, float radius = 15.0f,
    int minVelocity = 0, int maxVelocity = 0, GameMode mode = FREE_KICK);
void StaticCollisionResolution(Circle& a, Circle& b);
void DynamicCollisionResolution(Circle& a, Circle& b);
void HandleCollision(std::vector<std::pair<Circle*, Circle*>>& collisions, Circle& a, Circle& b);
bool IsCircleOutOfBounds(const Circle& circle);
void ResetCircle(Circle& circle, GameMode mode);
void ResetToHitPosition(Circle& circle, GameMode mode);
void HandleWallCollision(Circle& circle);

#endif
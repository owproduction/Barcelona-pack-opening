#ifndef COLLISION_H
#define COLLISION_H

#include "GameObjects.h"
#include <vector>
#include <utility>

// ������� ������������ ����� ���������
void HandleCollision(std::vector<std::pair<Circle*, Circle*>>& collisions, Circle& a, Circle& b);
void StaticCollisionResolution(Circle& a, Circle& b);
void DynamicCollisionResolution(Circle& a, Circle& b);

// ������� ������������ � ��������
bool CheckGoalCollision(const Circle& circle, const Goal& goal, Vector2& normal);
void HandleGoalCollision(Circle& circle, const Goal& goal);

// ������� ������������ � ��������
bool CheckGoalkeeperCollision(const Circle& circle, const Goalkeeper& keeper);
void HandleGoalkeeperCollision(Circle& circle, Goalkeeper& keeper);

// ������� ������������ �� �������
void HandleWallCollision(Circle& circle);
bool IsCircleOutOfBounds(const Circle& circle);

// ������� �������� ���� � �����
bool CheckGoalLineCrossing(const Circle& circle, const Goal& goal);
bool CheckGreenLineTouch(const Circle& circle, const Goal& goal);
bool CheckCompleteGoal(const Circle& circle, const Goal& goal);
bool CheckSideLinesCollision(const Circle& circle, const Goal& goal);

// ������� ��� ������ � �����
void ResetCircle(Circle& circle, int gameMode);
void ResetToHitPosition(Circle& circle, int gameMode);
void ApplySpin(Circle& circle, const Vector2& direction, bool spinActive[4]);
void UpdateSpin(Circle& circle, float deltaTime);

// ������� ��� �������
void MakeGoalkeeperJump(Goalkeeper& keeper);
void UpdateGoalkeeper(Goalkeeper& keeper, float deltaTime);

#endif
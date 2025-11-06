#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include "raylib.h"
#include "raymath.h"
#include <string>
#include "Game.h"  // ��������� ��� GameMode

// ��������� ��� ������
struct Button {
    Rectangle bounds;
    const char* text;
    Color color;
    Color textColor;
};

// ��������� ��� �����
struct Goal {
    Rectangle leftPost;   // ����� ������
    Rectangle rightPost;  // ������ ������
    Rectangle crossbar;   // �����������
    float width;          // ������ �����
    float height;         // ������ �����
    Vector2 position;     // ������� ����� (�����)
};

// ��������� ��� �������
struct Goalkeeper {
    Rectangle bounds;
    Vector2 position;
    Vector2 velocity;
    Vector2 startPosition;
    float width;
    float height;
    bool isJumping;
    float jumpTimer;
    Vector2 jumpDirection;
    Texture2D texture;    // �������� �������
    int playerControlled; // 0 - AI, 1 - ����� 1, 2 - ����� 2
    float moveSpeed;      // �������� �������� ��� ������
};

struct Circle {
    Vector2 position;
    Vector2 velocity;
    Vector2 accelerate;
    float radius;
    float weight;
    Vector2 startPosition; // ��������� ������� ��� ��������
    Vector2 hitPosition;   // ������� ����� ��� �������� ��� �������
    bool hasSpin[4];       // ���� ��������: 0=�����, 1=������, 2=�����, 3=����
    float spinForce[4];    // ���� ������� ���� ��������
    Vector2 spinDirection[4]; // ����������� ������� ���� ��������
    int controllingPlayer; // ����� ����� ��������� ����� (1 ��� 2)
    bool canMoveFreely;    // ����� �� ��� �������� ������������
};

// ��������� ��� ������� ����������� �����
struct Arrow {
    Vector2 position;
    Vector2 direction;
    float length;
    float angle;
    Color color;
    bool visible;
};

// ������� ��� �������� ��������
Button CreateButton(float x, float y, float width, float height, const char* text, Color color, Color textColor);
Goal CreateGoal(float width, float height, Vector2 position);
Goalkeeper CreateGoalkeeper(float width, float height, Vector2 position, const char* texturePath, int playerControlled = 0);
Arrow CreateArrow(Vector2 position);

#endif
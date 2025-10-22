#include "Collision.h"
#include "raymath.h"

void HandleCollision(std::vector<std::pair<Circle*, Circle*>>& collisions, Circle& a, Circle& b) {
    Vector2 delta = { b.position.x - a.position.x, b.position.y - a.position.y };
    float distance = Vector2Length(delta);
    float minDistance = a.radius + b.radius;
    if (distance < minDistance) {
        StaticCollisionResolution(a, b);
        collisions.push_back({ &a, &b });
    }
}

void StaticCollisionResolution(Circle& a, Circle& b) {
    Vector2 delta = { b.position.x - a.position.x, b.position.y - a.position.y };
    float distance = Vector2Length(delta);
    float overlap = distance - a.radius - b.radius;
    Vector2 direction = Vector2Scale(Vector2Normalize(delta), overlap / 2.0);
    a.position = Vector2Add(a.position, direction);
    b.position = Vector2Add(b.position, Vector2Negate(direction));
}

void DynamicCollisionResolution(Circle& a, Circle& b) {
    Vector2 first = a.position;
    Vector2 second = b.position;
    Vector2 dir = Vector2Subtract(second, first);
    Vector2 normal = Vector2Normalize(dir);
    Vector2 tangent = { -normal.y, normal.x };

    float dpNormA = Vector2DotProduct(a.velocity, normal);
    float dpNormB = Vector2DotProduct(b.velocity, normal);
    float dpTangA = Vector2DotProduct(a.velocity, tangent);
    float dpTangB = Vector2DotProduct(b.velocity, tangent);

    float p1 = (dpNormA * (a.weight - b.weight) + 2 * b.weight * dpNormB) / (a.weight + b.weight);
    float p2 = (dpNormB * (b.weight - a.weight) + 2 * a.weight * dpNormA) / (a.weight + b.weight);

    a.velocity = Vector2Add(Vector2Scale(tangent, dpTangA), Vector2Scale(normal, p1));
    b.velocity = Vector2Add(Vector2Scale(tangent, dpTangB), Vector2Scale(normal, p2));
}

bool CheckGoalCollision(const Circle& circle, const Goal& goal, Vector2& normal) {
    // Проверяем столкновение с левой штангой
    if (CheckCollisionCircleRec(circle.position, circle.radius, goal.leftPost)) {
        normal = { 1.0f, 0.0f };
        return true;
    }

    // Проверяем столкновение с правой штангой
    if (CheckCollisionCircleRec(circle.position, circle.radius, goal.rightPost)) {
        normal = { -1.0f, 0.0f };
        return true;
    }

    // Проверяем столкновение с перекладиной
    if (CheckCollisionCircleRec(circle.position, circle.radius, goal.crossbar)) {
        normal = { 0.0f, 1.0f };
        return true;
    }

    return false;
}

void HandleGoalCollision(Circle& circle, const Goal& goal) {
    Vector2 normal;
    if (CheckGoalCollision(circle, goal, normal)) {
        float dotProduct = Vector2DotProduct(circle.velocity, normal);

        if (dotProduct < 0) {
            circle.velocity = Vector2Subtract(circle.velocity,
                Vector2Scale(normal, 2.0f * dotProduct));
            circle.velocity = Vector2Scale(circle.velocity, 0.9f);
            circle.position = Vector2Add(circle.position, Vector2Scale(normal, 2.0f));
        }
    }
}

bool CheckGoalkeeperCollision(const Circle& circle, const Goalkeeper& keeper) {
    return CheckCollisionCircleRec(circle.position, circle.radius, keeper.bounds);
}

void HandleGoalkeeperCollision(Circle& circle, Goalkeeper& keeper) {
    if (CheckGoalkeeperCollision(circle, keeper)) {
        Vector2 normal = Vector2Normalize(Vector2Subtract(circle.position, keeper.position));
        float dotProduct = Vector2DotProduct(circle.velocity, normal);

        if (dotProduct < 0) {
            circle.velocity = Vector2Subtract(circle.velocity,
                Vector2Scale(normal, 2.0f * dotProduct));
            circle.velocity = Vector2Scale(circle.velocity, 0.8f);
            circle.position = Vector2Add(circle.position, Vector2Scale(normal, 5.0f));
        }
    }
}

void HandleWallCollision(Circle& circle) {
    // Столкновение с левой стеной
    if (circle.position.x - circle.radius < 0) {
        circle.position.x = circle.radius;
        circle.velocity.x = -circle.velocity.x * 0.9f;
    }
    // Столкновение с правой стеной
    else if (circle.position.x + circle.radius > MAX_WIDTH) {
        circle.position.x = MAX_WIDTH - circle.radius;
        circle.velocity.x = -circle.velocity.x * 0.9f;
    }

    // Столкновение с верхней стеной
    if (circle.position.y - circle.radius < 0) {
        circle.position.y = circle.radius;
        circle.velocity.y = -circle.velocity.y * 0.9f;
    }
    // Столкновение с нижней стеной
    else if (circle.position.y + circle.radius > MAX_HEIGHT) {
        circle.position.y = MAX_HEIGHT - circle.radius;
        circle.velocity.y = -circle.velocity.y * 0.9f;
    }
}

bool IsCircleOutOfBounds(const Circle& circle) {
    return circle.position.x < -circle.radius ||
        circle.position.x > MAX_WIDTH + circle.radius ||
        circle.position.y < -circle.radius ||
        circle.position.y > MAX_HEIGHT + circle.radius;
}

bool CheckGoalLineCrossing(const Circle& circle, const Goal& goal) {
    float goalLineY = goal.position.y - goal.height / 2;

    if (circle.position.y - circle.radius <= goalLineY) {
        float leftPostX = goal.position.x - goal.width / 2;
        float rightPostX = goal.position.x + goal.width / 2;

        if (circle.position.x < leftPostX || circle.position.x > rightPostX) {
            return true;
        }
    }
    return false;
}

bool CheckGreenLineTouch(const Circle& circle, const Goal& goal) {
    float greenLineY = goal.position.y + goal.height / 2 - 50.0f;

    if (abs(circle.position.y - greenLineY) <= circle.radius) {
        float leftPostX = goal.position.x - goal.width / 2;
        float rightPostX = goal.position.x + goal.width / 2;

        if (circle.position.x >= leftPostX && circle.position.x <= rightPostX) {
            return true;
        }
    }
    return false;
}

bool CheckCompleteGoal(const Circle& circle, const Goal& goal) {
    float innerLineY = goal.position.y + goal.height / 2;

    if (circle.position.y - circle.radius <= innerLineY) {
        float leftPostX = goal.position.x - goal.width / 2;
        float rightPostX = goal.position.x + goal.width / 2;

        if (circle.position.x >= leftPostX && circle.position.x <= rightPostX) {
            return true;
        }
    }
    return false;
}

bool CheckSideLinesCollision(const Circle& circle, const Goal& goal) {
    float lineY = goal.position.y + goal.height / 2;

    if (abs(circle.position.y - lineY) <= circle.radius) {
        float leftLineStartX = 0;
        float leftLineEndX = goal.position.x - goal.width / 2;
        if (circle.position.x >= leftLineStartX && circle.position.x <= leftLineEndX) {
            return true;
        }

        float rightLineStartX = goal.position.x + goal.width / 2;
        float rightLineEndX = MAX_WIDTH;
        if (circle.position.x >= rightLineStartX && circle.position.x <= rightLineEndX) {
            return true;
        }
    }
    return false;
}

void ResetCircle(Circle& circle, int gameMode) {
    if (gameMode == 1) { // PENALTY
        circle.position = { MAX_WIDTH / 2.0f, MAX_HEIGHT - 150.0f };
    }
    else { // FREE_KICK
        circle.position = circle.startPosition;
    }

    circle.velocity = { 0, 0 };
    circle.accelerate = { 0, 0 };
    for (int i = 0; i < 4; i++) {
        circle.hasSpin[i] = false;
        circle.spinForce[i] = 0.0f;
        circle.spinDirection[i] = { 0, 0 };
    }
    circle.canMoveFreely = true;
}

void ResetToHitPosition(Circle& circle, int gameMode) {
    if (gameMode == 1) { // PENALTY
        circle.position = { MAX_WIDTH / 2.0f, MAX_HEIGHT - 150.0f };
    }
    else {
        circle.position = circle.hitPosition;
    }

    circle.velocity = { 0, 0 };
    circle.accelerate = { 0, 0 };
    for (int i = 0; i < 4; i++) {
        circle.hasSpin[i] = false;
        circle.spinForce[i] = 0.0f;
        circle.spinDirection[i] = { 0, 0 };
    }
    circle.canMoveFreely = true;
}

void ApplySpin(Circle& circle, const Vector2& direction, bool spinActive[4]) {
    for (int i = 0; i < 4; i++) {
        circle.hasSpin[i] = false;
        circle.spinForce[i] = 0.0f;
    }

    for (int i = 0; i < 4; i++) {
        if (spinActive[i]) {
            circle.hasSpin[i] = true;
            circle.spinForce[i] = 80.0f;

            switch (i) {
            case 0: // Кручение влево
                circle.spinDirection[i] = { direction.y, -direction.x };
                break;
            case 1: // Кручение вправо
                circle.spinDirection[i] = { -direction.y, direction.x };
                break;
            case 2: // Верхнее кручение
                circle.spinDirection[i] = { 0, -1 };
                break;
            case 3: // Нижнее кручение
                circle.spinDirection[i] = { 0, 1 };
                break;
            }

            circle.spinDirection[i] = Vector2Normalize(circle.spinDirection[i]);
        }
    }
}

void UpdateSpin(Circle& circle, float deltaTime) {
    for (int i = 0; i < 4; i++) {
        if (circle.hasSpin[i] && circle.spinForce[i] > 0) {
            Vector2 spinEffect = Vector2Scale(circle.spinDirection[i], circle.spinForce[i] * deltaTime);
            circle.velocity = Vector2Add(circle.velocity, spinEffect);

            circle.spinForce[i] -= 60.0f * deltaTime;

            if (circle.spinForce[i] <= 0) {
                circle.hasSpin[i] = false;
                circle.spinForce[i] = 0;
            }
        }
    }

    // Визуальный эффект кручения
    bool anySpinActive = false;
    for (int i = 0; i < 4; i++) {
        if (circle.hasSpin[i] && circle.spinForce[i] > 0) {
            anySpinActive = true;
            break;
        }
    }

    if (anySpinActive) {
        for (int i = 0; i < 6; i++) {
            float angle = GetTime() * 15.0f + i * 60.0f * DEG2RAD;
            Vector2 particlePos = {
                circle.position.x + cosf(angle) * (circle.radius + 10),
                circle.position.y + sinf(angle) * (circle.radius + 10)
            };

            Color particleColor = WHITE;
            if (circle.hasSpin[0] && circle.spinForce[0] > 0) particleColor = BLUE;
            if (circle.hasSpin[1] && circle.spinForce[1] > 0) particleColor = RED;
            if (circle.hasSpin[2] && circle.spinForce[2] > 0) particleColor = GREEN;
            if (circle.hasSpin[3] && circle.spinForce[3] > 0) particleColor = YELLOW;

            DrawCircleV(particlePos, 3, particleColor);
        }
    }
}

void MakeGoalkeeperJump(Goalkeeper& keeper) {
    if (!keeper.isJumping && keeper.playerControlled == 0) {
        keeper.isJumping = true;
        keeper.jumpTimer = 0.5f;

        int direction = GetRandomValue(0, 2);
        switch (direction) {
        case 0: // Прыжок влево
            keeper.jumpDirection = { -1.0f, 0.0f };
            break;
        case 1: // Прыжок вправо
            keeper.jumpDirection = { 1.0f, 0.0f };
            break;
        case 2: // Остаться по центру
            keeper.jumpDirection = { 0.0f, -0.5f };
            break;
        }

        keeper.velocity = Vector2Scale(keeper.jumpDirection, 200.0f);
    }
}

void UpdateGoalkeeper(Goalkeeper& keeper, float deltaTime) {
    if (keeper.playerControlled == 0) {
        // AI вратарь
        if (keeper.isJumping) {
            keeper.jumpTimer -= deltaTime;
            keeper.position = Vector2Add(keeper.position, Vector2Scale(keeper.velocity, deltaTime));
            keeper.velocity.y += 400.0f * deltaTime;

            // Ограничение позиции вратаря
            float goalLeft = keeper.startPosition.x - 50;
            float goalRight = keeper.startPosition.x + 50;
            float goalTop = keeper.startPosition.y - 20;
            float goalBottom = keeper.startPosition.y + 20;

            if (keeper.position.x < goalLeft) {
                keeper.position.x = goalLeft;
                keeper.velocity.x = -keeper.velocity.x * 0.3f;
            }
            if (keeper.position.x > goalRight) {
                keeper.position.x = goalRight;
                keeper.velocity.x = -keeper.velocity.x * 0.3f;
            }
            if (keeper.position.y < goalTop) {
                keeper.position.y = goalTop;
                keeper.velocity.y = -keeper.velocity.y * 0.3f;
            }
            if (keeper.position.y > goalBottom) {
                keeper.position.y = goalBottom;
                keeper.velocity.y = -keeper.velocity.y * 0.3f;
            }

            if (keeper.jumpTimer <= 0) {
                keeper.isJumping = false;
                Vector2 direction = Vector2Subtract(keeper.startPosition, keeper.position);
                if (Vector2Length(direction) > 2.0f) {
                    keeper.velocity = Vector2Scale(Vector2Normalize(direction), 150.0f);
                }
                else {
                    keeper.velocity = { 0, 0 };
                    keeper.position = keeper.startPosition;
                }
            }
        }
        else {
            Vector2 direction = Vector2Subtract(keeper.startPosition, keeper.position);
            if (Vector2Length(direction) > 2.0f) {
                keeper.velocity = Vector2Scale(Vector2Normalize(direction), 150.0f);
                keeper.position = Vector2Add(keeper.position, Vector2Scale(keeper.velocity, deltaTime));
            }
            else {
                keeper.velocity = { 0, 0 };
                keeper.position = keeper.startPosition;
            }
        }
    }
    else {
        // Игрок управляет вратарем
        Vector2 newVelocity = { 0, 0 };

        if (IsKeyDown(KEY_LEFT)) newVelocity.x = -keeper.moveSpeed;
        if (IsKeyDown(KEY_RIGHT)) newVelocity.x = keeper.moveSpeed;
        if (IsKeyDown(KEY_UP)) newVelocity.y = -keeper.moveSpeed;
        if (IsKeyDown(KEY_DOWN)) newVelocity.y = keeper.moveSpeed;

        if (IsKeyPressed(KEY_SPACE) && !keeper.isJumping) {
            keeper.isJumping = true;
            keeper.jumpTimer = 0.3f;
            keeper.velocity.y = -300.0f;
        }

        if (keeper.isJumping) {
            keeper.jumpTimer -= deltaTime;
            keeper.velocity.y += 600.0f * deltaTime;

            if (keeper.jumpTimer <= 0 && keeper.position.y >= keeper.startPosition.y) {
                keeper.isJumping = false;
                keeper.velocity.y = 0;
                keeper.position.y = keeper.startPosition.y;
            }
        }

        if (!keeper.isJumping) {
            keeper.velocity = newVelocity;
        }

        keeper.position = Vector2Add(keeper.position, Vector2Scale(keeper.velocity, deltaTime));

        // Ограничение позиции вратаря
        float goalLeft = keeper.startPosition.x - 70;
        float goalRight = keeper.startPosition.x + 70;
        float goalTop = keeper.startPosition.y - 40;
        float goalBottom = keeper.startPosition.y + 40;

        if (keeper.position.x < goalLeft) keeper.position.x = goalLeft;
        if (keeper.position.x > goalRight) keeper.position.x = goalRight;
        if (keeper.position.y < goalTop) keeper.position.y = goalTop;
        if (keeper.position.y > goalBottom) keeper.position.y = goalBottom;
    }

    // Обновляем границы вратаря
    keeper.bounds.x = keeper.position.x - keeper.width / 2;
    keeper.bounds.y = keeper.position.y - keeper.height / 2;
}
#include "Collision.h"

bool CheckGoalCollision(const Circle& circle, const Goal& goal, Vector2& normal) {
    // Проверяем столкновение с левой штангой
    if (CheckCollisionCircleRec(circle.position, circle.radius, goal.leftPost)) {
        normal = { 1.0f, 0.0f }; // Нормаль направлена вправо (от левой штанги)
        return true;
    }

    // Проверяем столкновение с правой штангой
    if (CheckCollisionCircleRec(circle.position, circle.radius, goal.rightPost)) {
        normal = { -1.0f, 0.0f }; // Нормаль направлена влево (от правой штанги)
        return true;
    }

    // Проверяем столкновение с перекладиной
    if (CheckCollisionCircleRec(circle.position, circle.radius, goal.crossbar)) {
        normal = { 0.0f, 1.0f }; // Нормаль направлена вниз (от перекладины)
        return true;
    }

    return false;
}

bool CheckGoalkeeperCollision(const Circle& circle, const Goalkeeper& keeper) {
    return CheckCollisionCircleRec(circle.position, circle.radius, keeper.bounds);
}

void HandleGoalkeeperCollision(Circle& circle, Goalkeeper& keeper) {
    if (CheckGoalkeeperCollision(circle, keeper)) {
        // Вычисляем нормаль столкновения
        Vector2 normal = Vector2Normalize(Vector2Subtract(circle.position, keeper.position));

        // Отражение мяча от вратаря
        float dotProduct = Vector2DotProduct(circle.velocity, normal);

        if (dotProduct < 0) {
            // Отражение скорости относительно нормали
            circle.velocity = Vector2Subtract(circle.velocity,
                Vector2Scale(normal, 2.0f * dotProduct));

            // Добавляем затухание
            circle.velocity = Vector2Scale(circle.velocity, 0.8f);

            // Немного отодвигаем мяч от вратаря
            circle.position = Vector2Add(circle.position, Vector2Scale(normal, 5.0f));
        }
    }
}

void HandleGoalCollision(Circle& circle, const Goal& goal) {
    Vector2 normal;
    if (CheckGoalCollision(circle, goal, normal)) {
        // Отскок от ворот
        float dotProduct = Vector2DotProduct(circle.velocity, normal);

        // Если круг движется в направлении препятствия
        if (dotProduct < 0) {
            // Отражение скорости относительно нормали
            circle.velocity = Vector2Subtract(circle.velocity,
                Vector2Scale(normal, 2.0f * dotProduct));

            // Добавляем затухание
            circle.velocity = Vector2Scale(circle.velocity, 0.9f);

            // Немного отодвигаем круг от препятствия чтобы избежать залипания
            circle.position = Vector2Add(circle.position, Vector2Scale(normal, 2.0f));
        }
    }
}

bool CheckGoalLineCrossing(const Circle& circle, const Goal& goal) {
    // Высота линии ворот (уровень перекладины)
    float goalLineY = goal.position.y - goal.height / 2;

    // Проверяем, пересек ли мяч линию ворот (с учетом радиуса)
    if (circle.position.y - circle.radius <= goalLineY) {
        // Проверяем, пролетел ли мяч между штангами
        float leftPostX = goal.position.x - goal.width / 2;
        float rightPostX = goal.position.x + goal.width / 2;

        // Если мяч пересек линию и не попал между штангами
        if (circle.position.x < leftPostX || circle.position.x > rightPostX) {
            return true;
        }
    }
    return false;
}

bool CheckGreenLineTouch(const Circle& circle, const Goal& goal) {
    // Высота зеленой линии (выше желтой на 50 пикселей)
    float greenLineY = goal.position.y + goal.height / 2 - 50.0f;

    // Проверяем, коснулся ли мяч зеленой линии
    if (abs(circle.position.y - greenLineY) <= circle.radius) {
        // Проверяем, находится ли мяч между штангами
        float leftPostX = goal.position.x - goal.width / 2;
        float rightPostX = goal.position.x + goal.width / 2;

        // Если мяч коснулся зеленой линии и находится между штангами
        if (circle.position.x >= leftPostX && circle.position.x <= rightPostX) {
            return true;
        }
    }
    return false;
}

bool CheckCompleteGoal(const Circle& circle, const Goal& goal) {
    // Высота внутренней линии ворот (на уровне ника штанг)
    float innerLineY = goal.position.y + goal.height / 2;

    // Проверяем, пересек ли мяч внутреннюю линию ворот
    if (circle.position.y - circle.radius <= innerLineY) {
        // Проверяем, пролетел ли мяч между штангами
        float leftPostX = goal.position.x - goal.width / 2;
        float rightPostX = goal.position.x + goal.width / 2;

        // Если мяч пересек внутреннюю линию и попал между штангами
        if (circle.position.x >= leftPostX && circle.position.x <= rightPostX) {
            return true;
        }
    }
    return false;
}

bool CheckSideLinesCollision(const Circle& circle, const Goal& goal) {
    // Высота линии (уровень низа штанг)
    float lineY = goal.position.y + goal.height / 2;

    // Проверяем, находится ли мяч на уровне линии
    if (abs(circle.position.y - lineY) <= circle.radius) {
        // Левая ГОРИЗОНТАЛЬНАЯ белая линия СЛЕВА от левой штанги (прилегает к воротам)
        float leftLineStartX = 0;
        float leftLineEndX = goal.position.x - goal.width / 2; // Теперь прилегает к левой штанге
        if (circle.position.x >= leftLineStartX && circle.position.x <= leftLineEndX) {
            return true;
        }

        // Правая ГОРИЗОНТАЛЬНАЯ белая линия СПРАВА от правой штанги (прилегает к воротам)
        float rightLineStartX = goal.position.x + goal.width / 2;
        float rightLineEndX = MAX_WIDTH;
        if (circle.position.x >= rightLineStartX && circle.position.x <= rightLineEndX) {
            return true;
        }
    }
    return false;
}

void MakeGoalkeeperJump(Goalkeeper& keeper) {
    if (!keeper.isJumping && keeper.playerControlled == 0) {
        keeper.isJumping = true;
        keeper.jumpTimer = 0.5f; // Время прыжка в секундах

        // Случайное направление прычка (влево, вправо или по центру)
        int direction = GetRandomValue(0, 2);
        switch (direction) {
        case 0: // Прыжок влево
            keeper.jumpDirection = { -1.0f, 0.0f };
            break;
        case 1: // Прыжок вправо
            keeper.jumpDirection = { 1.0f, 0.0f };
            break;
        case 2: // Остаться по центру (небольшой прыжок на месте)
            keeper.jumpDirection = { 0.0f, -0.5f };
            break;
        }

        // Начальная скорость прыжка
        keeper.velocity = Vector2Scale(keeper.jumpDirection, 200.0f); // Уменьшил скорость прыжка
    }
}

void UpdateGoalkeeper(Goalkeeper& keeper, float deltaTime) {
    if (keeper.playerControlled == 0) {
        // AI вратарь
        if (keeper.isJumping) {
            // Обновляем таймер прыжка
            keeper.jumpTimer -= deltaTime;

            // Обновляем позицию
            keeper.position = Vector2Add(keeper.position, Vector2Scale(keeper.velocity, deltaTime));

            // Добавляем гравитацию
            keeper.velocity.y += 400.0f * deltaTime; // Уменьшил гравитацию

            // ОГРАНИЧИВАЕМ ПОЗИЦИЮ ВРАТАРЯ В ПРЕДЕЛАх ВОРОТ (уже границы)
            float goalLeft = keeper.startPosition.x - 50; // Левая граница ворот (уже)
            float goalRight = keeper.startPosition.x + 50; // Правая граница ворот (уже)
            float goalTop = keeper.startPosition.y - 20; // Верхняя граница
            float goalBottom = keeper.startPosition.y + 20; // Нижняя граница

            // Ограничение по горизонтали
            if (keeper.position.x < goalLeft) {
                keeper.position.x = goalLeft;
                keeper.velocity.x = -keeper.velocity.x * 0.3f; // Сильнее гасим скорость
            }
            if (keeper.position.x > goalRight) {
                keeper.position.x = goalRight;
                keeper.velocity.x = -keeper.velocity.x * 0.3f; // Сильнее гасим скорость
            }

            // Ограничение по вертикали
            if (keeper.position.y < goalTop) {
                keeper.position.y = goalTop;
                keeper.velocity.y = -keeper.velocity.y * 0.3f;
            }
            if (keeper.position.y > goalBottom) {
                keeper.position.y = goalBottom;
                keeper.velocity.y = -keeper.velocity.y * 0.3f;
            }

            // Завершение прыжка
            if (keeper.jumpTimer <= 0) {
                keeper.isJumping = false;
                // Плавное возвращение на стартовую позицию
                Vector2 direction = Vector2Subtract(keeper.startPosition, keeper.position);
                if (Vector2Length(direction) > 2.0f) { // Более точное возвращение
                    keeper.velocity = Vector2Scale(Vector2Normalize(direction), 150.0f); // Медленнее возвращение
                }
                else {
                    keeper.velocity = { 0, 0 };
                    keeper.position = keeper.startPosition;
                }
            }
        }
        else {
            // Плавное возвращение на стартовую позицию
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

        // Управление стрелками
        if (IsKeyDown(KEY_LEFT)) {
            newVelocity.x = -keeper.moveSpeed;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            newVelocity.x = keeper.moveSpeed;
        }
        if (IsKeyDown(KEY_UP)) {
            newVelocity.y = -keeper.moveSpeed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            newVelocity.y = keeper.moveSpeed;
        }

        // Прыжок на пробел
        if (IsKeyPressed(KEY_SPACE) && !keeper.isJumping) {
            keeper.isJumping = true;
            keeper.jumpTimer = 0.3f;
            keeper.velocity.y = -300.0f; // Прыжок вверх
        }

        // Обновление прыжка
        if (keeper.isJumping) {
            keeper.jumpTimer -= deltaTime;
            keeper.velocity.y += 600.0f * deltaTime; // Гравитация

            if (keeper.jumpTimer <= 0 && keeper.position.y >= keeper.startPosition.y) {
                keeper.isJumping = false;
                keeper.velocity.y = 0;
                keeper.position.y = keeper.startPosition.y;
            }
        }

        // Применяем скорость движения
        if (!keeper.isJumping) {
            keeper.velocity = newVelocity;
        }

        // Обновляем позицию
        keeper.position = Vector2Add(keeper.position, Vector2Scale(keeper.velocity, deltaTime));

        // Ограничиваем позицию вратаря в пределах ворот
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

void ApplySpin(Circle& circle, const Vector2& direction, bool spinActive[4]) {
    // Сбрасываем все типы кручения
    for (int i = 0; i < 4; i++) {
        circle.hasSpin[i] = false;
        circle.spinForce[i] = 0.0f;
    }

    // Применяем активные типы кручения
    for (int i = 0; i < 4; i++) {
        if (spinActive[i]) {
            circle.hasSpin[i] = true;
            circle.spinForce[i] = 80.0f; // Сила для каждого типа

            // Устанавливаем направление для каждого типа
            switch (i) {
            case 0: // Кручение влево (против часовой стрелки)
                circle.spinDirection[i] = { direction.y, -direction.x };
                break;
            case 1: // Кручение вправо (по часовой стрелке)
                circle.spinDirection[i] = { -direction.y, direction.x };
                break;
            case 2: // Верхнее кручение (подкрутка вверх)
                circle.spinDirection[i] = { 0, -1 };
                break;
            case 3: // Нижнее кручение (подкрутка вниз)
                circle.spinDirection[i] = { 0, 1 };
                break;
            }

            // Нормализуем направление кручения
            circle.spinDirection[i] = Vector2Normalize(circle.spinDirection[i]);
        }
    }
}

void UpdateSpin(Circle& circle, float deltaTime) {
    for (int i = 0; i < 4; i++) {
        if (circle.hasSpin[i] && circle.spinForce[i] > 0) {
            // ПРИМЕНЯЕМ КРУЧЕНИЕ К СКОРОСТИ МЯЧА КАЖДЫЙ КАДР
            Vector2 spinEffect = Vector2Scale(circle.spinDirection[i], circle.spinForce[i] * deltaTime);
            circle.velocity = Vector2Add(circle.velocity, spinEffect);

            // Постепенно уменьшаем силу кручения
            circle.spinForce[i] -= 60.0f * deltaTime;

            if (circle.spinForce[i] <= 0) {
                circle.hasSpin[i] = false;
                circle.spinForce[i] = 0;
            }
        }
    }

    // Визуальный эффект - рисуем маленькие частицы вокруг мяча для показа кручения
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

            // Цвет частицы в зависимости от активных типов кручения
            Color particleColor = WHITE;
            if (circle.hasSpin[0] && circle.spinForce[0] > 0) particleColor = BLUE; // Влево
            if (circle.hasSpin[1] && circle.spinForce[1] > 0) particleColor = RED;  // Вправо
            if (circle.hasSpin[2] && circle.spinForce[2] > 0) particleColor = GREEN; // Вверх
            if (circle.hasSpin[3] && circle.spinForce[3] > 0) particleColor = YELLOW; // Вниз

            DrawCircleV(particlePos, 3, particleColor);
        }
    }
}

int GenerateCircles(std::vector<Circle>& circles, int count, float radius,
    int minVelocity, int maxVelocity, GameMode mode) {
    for (int i = 0; i < count; i++)
    {
        Vector2 velocity = {
           GetRandomValue(minVelocity,maxVelocity),
           GetRandomValue(minVelocity,maxVelocity)
        };
        Vector2 accelerate = { 0,0 };
        float weight = radius / 3;

        // Позиция мяча в зависимости от режима
        Vector2 position;
        if (mode == PENALTY) {
            // Для пенальти - фиксированная позиция
            position = {
                MAX_WIDTH / 2.0f,
                MAX_HEIGHT - 150.0f  // Фиксированная позиция для пенальти
            };
        }
        else {
            // Для свободного удара - начальная позиция как раньше
            position = {
                MAX_WIDTH / 2.0f,
                MAX_HEIGHT / 2.0f + 100
            };
        }

        Circle newCircle = { position,velocity,accelerate,radius,weight,position,position };
        for (int j = 0; j < 4; j++) {
            newCircle.hasSpin[j] = false;
            newCircle.spinForce[j] = 0.0f;
            newCircle.spinDirection[j] = { 0, 0 };
        }
        newCircle.controllingPlayer = 1; // По умолчанию управляет игрок 1
        newCircle.canMoveFreely = (mode == FREE_KICK); // В свободном ударе можно перемещать мяч
        circles.push_back(newCircle);
    }
    return count;
}

void StaticCollisionResolution(Circle& a, Circle& b) {
    Vector2 delta = {
        b.position.x - a.position.x,
        b.position.y - a.position.y
    };
    float distance = Vector2Length(delta);
    float overlap = distance - a.radius - b.radius;
    Vector2 direction = Vector2Scale(Vector2Normalize(delta), overlap / 2.0);
    a.position = Vector2Add(a.position, direction);
    b.position = Vector2Add(b.position, Vector2Negate(direction));
}

void DynamicCollisionResolution(Circle& a, Circle& b) {
    Vector2 first = a.position;
    Vector2 second = b.position;
    //оси столкновения - нормаль и касательная
    Vector2 dir = Vector2Subtract(second, first);
    Vector2 normal = Vector2Normalize(dir);
    Vector2 tangent = { -normal.y,normal.x };
    //Проекции на оси столкновения
    float dpNormA = Vector2DotProduct(a.velocity, normal);
    float dpNormB = Vector2DotProduct(b.velocity, normal);
    float dpTangA = Vector2DotProduct(a.velocity, tangent);
    float dpTangB = Vector2DotProduct(b.velocity, tangent);
    //Сохранение импульса в 1мерном пространстве
    float p1 = (dpNormA * (a.weight - b.weight) + 2 * b.weight * dpNormB) / (a.weight + b.weight);
    float p2 = (dpNormB * (b.weight - a.weight) + 2 * a.weight * dpNormA) / (a.weight + b.weight);
    //Применяем измененный импульс к скоростям кругов
    a.velocity = Vector2Add(Vector2Scale(tangent, dpTangA), Vector2Scale(normal, p1));
    b.velocity = Vector2Add(Vector2Scale(tangent, dpTangB), Vector2Scale(normal, p2));
}

void HandleCollision(std::vector<std::pair<Circle*, Circle*>>& collisions, Circle& a, Circle& b) {
    Vector2 delta = { b.position.x - a.position.x,b.position.y - a.position.y, };
    float distance = Vector2Length(delta);
    float minDistance = a.radius + b.radius;
    if (distance < minDistance) {
        StaticCollisionResolution(a, b);
        Circle* pa = &a;
        Circle* pb = &b;
        collisions.push_back({ pa,pb });
    }
}

bool IsCircleOutOfBounds(const Circle& circle) {
    return circle.position.x < -circle.radius ||
        circle.position.x > MAX_WIDTH + circle.radius ||
        circle.position.y < -circle.radius ||
        circle.position.y > MAX_HEIGHT + circle.radius;
}

void ResetCircle(Circle& circle, GameMode mode) {
    if (mode == PENALTY) {
        // В режиме пенальти всегда возвращаем на фиксированную позицию
        circle.position = { MAX_WIDTH / 2.0f, MAX_HEIGHT - 150.0f };
    }
    else {
        // В режиме свободного удара возвращаем на стартовую позицию
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

void ResetToHitPosition(Circle& circle, GameMode mode) {
    if (mode == PENALTY) {
        // В режиме пенальти всегда возвращаем на фиксированную позицию
        circle.position = { MAX_WIDTH / 2.0f, MAX_HEIGHT - 150.0f };
    }
    else {
        // В режиме свободного удара возвращаем в точку удара
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
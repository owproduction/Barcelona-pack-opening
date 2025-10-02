#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include <vector>

const int MAX_WIDTH = 600;
const int MAX_HEIGHT = 800;
const float damping = -0.05f;

// Структура для ворот
struct Goal {
    Rectangle leftPost;   // Левая штанга
    Rectangle rightPost;  // Правая штанга
    Rectangle crossbar;   // Перекладина
    float width;          // Ширина ворот
    float height;         // Высота ворот
    Vector2 position;     // Позиция ворот (центр)
};

struct Circle {
    Vector2 position;
    Vector2 velocity;
    Vector2 accelerate;
    float radius;
    float weight;
    Vector2 startPosition; // Начальная позиция для возврата
    Vector2 hitPosition;   // Позиция удара для возврата при промахе
};

// Функция для создания ворот
Goal CreateGoal(float width, float height, Vector2 position) {
    Goal goal;
    goal.width = width;
    goal.height = height;
    goal.position = position;

    // Размеры штанг и перекладины
    float postWidth = 10.0f;
    float crossbarHeight = 10.0f;

    // Левая штанга
    goal.leftPost = {
        position.x - width / 2 - postWidth / 2,
        position.y - height / 2,
        postWidth,
        height
    };

    // Правая штанга
    goal.rightPost = {
        position.x + width / 2 - postWidth / 2,
        position.y - height / 2,
        postWidth,
        height
    };

    // Перекладина
    goal.crossbar = {
        position.x - width / 2,
        position.y - height / 2 - crossbarHeight / 2,
        width,
        crossbarHeight
    };

    return goal;
}

// Функция для отрисовки ворот
void DrawGoal(const Goal& goal) {
    // Рисуем левую штангу
    DrawRectangleRec(goal.leftPost, WHITE);
    // Рисуем правую штангу
    DrawRectangleRec(goal.rightPost, WHITE);
    // Рисуем перекладину
    DrawRectangleRec(goal.crossbar, WHITE);
}

// Функция для проверки столкновения круга с воротами
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

// Функция для обработки столкновения с воротами
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

// Функция для проверки пересечения линии ворот
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

// Функция для проверки гола (мяч пересек линию ворот между штангами)
bool CheckGoalScored(const Circle& circle, const Goal& goal) {
    // Высота линии ворот (уровень перекладины)
    float goalLineY = goal.position.y - goal.height / 2;

    // Проверяем, пересек ли мяч линию ворот (с учетом радиуса)
    if (circle.position.y - circle.radius <= goalLineY) {
        // Проверяем, пролетел ли мяч между штангами
        float leftPostX = goal.position.x - goal.width / 2;
        float rightPostX = goal.position.x + goal.width / 2;

        // Если мяч пересек линию и попал между штангами
        if (circle.position.x >= leftPostX && circle.position.x <= rightPostX) {
            return true;
        }
    }
    return false;
}

// Функция для проверки полного залета мяча в ворота
bool CheckCompleteGoal(const Circle& circle, const Goal& goal) {
    // Высота внутренней линии ворот (на уровне низа штанг)
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

// Функция для проверки касания ГОРИЗОНТАЛЬНЫХ белых линий СБОКУ ОТ ШТАНГ
bool CheckSideLinesCollision(const Circle& circle, const Goal& goal) {
    // Высота линии (уровень низа штанг)
    float lineY = goal.position.y + goal.height / 2;
    
    // Проверяем, находится ли мяч на уровне линии
    if (abs(circle.position.y - lineY) <= circle.radius) {
        // Левая ГОРИЗОНТАЛЬНАЯ белая линия СЛЕВА от левой штанги
        float leftLineStartX = 0;
        float leftLineEndX = goal.position.x - goal.width / 2 - 20.0f;
        if (circle.position.x >= leftLineStartX && circle.position.x <= leftLineEndX) {
            return true;
        }
        
        // Правая ГОРИЗОНТАЛЬНАЯ белая линия СПРАВА от правой штанги
        float rightLineStartX = goal.position.x + goal.width / 2 + 20.0f;
        float rightLineEndX = MAX_WIDTH;
        if (circle.position.x >= rightLineStartX && circle.position.x <= rightLineEndX) {
            return true;
        }
    }
    return false;
}

int GenerateCircles(std::vector<Circle>& circles, int count = 1, float radius = 15.0f,
    int minVelocity = 0, int maxVelocity = 0) {
    for (int i = 0; i < count; i++)
    {
        Vector2 velocity = {
           GetRandomValue(minVelocity,maxVelocity),
           GetRandomValue(minVelocity,maxVelocity)
        };
        Vector2 accelerate = { 0,0 };
        float weight = radius / 3;

        // Генерация по центру экрана (но ниже ворот)
        Vector2 position = {
            MAX_WIDTH / 2.0f,
            MAX_HEIGHT / 2.0f + 100  // Немного ниже центра чтобы не попасть сразу в ворота
        };

        circles.push_back(Circle{ position,velocity,accelerate,radius,weight,position,position });
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

// Функция для проверки вылета за границы
bool IsCircleOutOfBounds(const Circle& circle) {
    return circle.position.x < -circle.radius ||
        circle.position.x > MAX_WIDTH + circle.radius ||
        circle.position.y < -circle.radius ||
        circle.position.y > MAX_HEIGHT + circle.radius;
}

// Функция для возврата круга на стартовую позицию
void ResetCircle(Circle& circle) {
    circle.position = circle.startPosition;
    circle.velocity = { 0, 0 };
    circle.accelerate = { 0, 0 };
}

// Функция для возврата круга в точку удара
void ResetToHitPosition(Circle& circle) {
    circle.position = circle.hitPosition;
    circle.velocity = { 0, 0 };
    circle.accelerate = { 0, 0 };
}

// Функция для обработки столкновений со стенами экрана
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

// Функция для отрисовки шкалы силы
void DrawPowerBar(const Circle& circle, const Vector2& mousePosition, bool isDragging) {
    if (isDragging) {
        // Вычисляем расстояние между мячом и курсором
        float distance = Vector2Distance(circle.position, mousePosition);

        // Ограничиваем максимальную дистанцию для лучшего визуального отображения
        float maxDistance = 150.0f;
        float power = (distance > maxDistance) ? 1.0f : distance / maxDistance;

        // Позиция шкалы (над мячом)
        Vector2 barPosition = { circle.position.x - 50, circle.position.y - 40 };
        float barWidth = 100.0f;
        float barHeight = 10.0f;

        // Рисуем фон шкалы
        DrawRectangleRec({ barPosition.x, barPosition.y, barWidth, barHeight }, GRAY);

        // Рисуем заполнение шкалы (красный цвет, пропорциональный силе)
        DrawRectangleRec({ barPosition.x, barPosition.y, barWidth * power, barHeight }, RED);

        // Рисуем контур шкалы
        DrawRectangleLines(barPosition.x, barPosition.y, barWidth, barHeight, WHITE);
    }
}

int main()
{
    InitWindow(MAX_WIDTH, MAX_HEIGHT, "Bounce with Goal");
    SetTargetFPS(60);

    bool dragging = false;
    Circle* selectedCircle = nullptr;
    Vector2 dragStartPosition;
    int score = 0; // Счет голов
    int coins = 0; // Счет монеток

    std::vector<Circle> circles;
    GenerateCircles(circles, 1, 15.0f, 0, 0);

    // Создаем ворота вверху экрана (опущены ниже)
    Goal goal = CreateGoal(200.0f, 100.0f, { MAX_WIDTH / 2.0f, 150.0f });

    while (!WindowShouldClose()) {
        Vector2 mousePosition = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (auto& circle : circles) {
                if (CheckCollisionPointCircle(mousePosition, circle.position, circle.radius)) {
                    dragging = true;
                    selectedCircle = &circle;
                    dragStartPosition = mousePosition;
                    // Сохраняем позицию удара
                    circle.hitPosition = circle.position;
                    break;
                }
            }
        }

        std::vector<std::pair<Circle*, Circle*>> collisions;

        for (int i = 0; i < circles.size() - 1; ++i) {
            for (int j = i + 1; j < circles.size(); ++j) {
                HandleCollision(collisions, circles[i], circles[j]);
            }
        }

        for (auto& collision : collisions) {
            DynamicCollisionResolution(*collision.first, *collision.second);
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (selectedCircle != nullptr) {
                // Вычисляем направление и силу удара
                Vector2 direction = Vector2Subtract(selectedCircle->position, mousePosition);
                float distance = Vector2Length(direction);

                // Ограничиваем максимальную дистанцию для контроля силы
                float maxDistance = 150.0f;
                if (distance > maxDistance) {
                    distance = maxDistance;
                    direction = Vector2Scale(Vector2Normalize(direction), maxDistance);
                }

                // Применяем силу (чем дальше тянем, тем сильнее удар)
                Vector2 acceleration = Vector2Scale(Vector2Normalize(direction), distance * 0.15f);
                selectedCircle->velocity = acceleration;
            }
            dragging = false;
            selectedCircle = nullptr;
        }

        for (auto& circle : circles) {
            circle.accelerate = Vector2Scale(circle.velocity, damping);
            circle.velocity = Vector2Add(circle.velocity, circle.accelerate);
            circle.position = Vector2Add(circle.position, circle.velocity);

            // Проверяем вылет за границы
            if (IsCircleOutOfBounds(circle)) {
                ResetCircle(circle);
            }

            // Проверяем пересечение линии ворот (мимо ворот)
            if (CheckGoalLineCrossing(circle, goal)) {
                ResetCircle(circle);
            }

            // Проверяем гол (мяч залетел в ворота)
            if (CheckGoalScored(circle, goal)) {
                score++; // Увеличиваем счет
                ResetCircle(circle);
            }

            // Проверяем полный залет мяча в ворота (для монетки)
            if (CheckCompleteGoal(circle, goal)) {
                coins++; // Добавляем монетку
                ResetCircle(circle);
            }

            // Проверяем касание ГОРИЗОНТАЛЬНЫХ белых линий СБОКУ ОТ ШТАНГ
            if (CheckSideLinesCollision(circle, goal)) {
                ResetToHitPosition(circle);
            }

            // Обработка столкновений со стенами
            HandleWallCollision(circle);

            // Обработка столкновений с воротами (мяч отскакивает от штанг)
            HandleGoalCollision(circle, goal);
        }

        BeginDrawing();
        ClearBackground(DARKGREEN);

        // Координаты для линий на уровне низа штанг
        float postBottomY = goal.position.y + goal.height / 2;
        float leftPostX = goal.position.x - goal.width / 2;
        float rightPostX = goal.position.x + goal.width / 2;

        // Рисуем желтую линию на уровне низа штанг (горизонтальная) - для монеток
        DrawLine(leftPostX, postBottomY, rightPostX, postBottomY, YELLOW);

        // Рисуем ГОРИЗОНТАЛЬНЫЕ белые линии СБОКУ ОТ ШТАНГ
        // Левая ГОРИЗОНТАЛЬНАЯ белая линия СЛЕВА от левой штанги
        DrawLine(0, postBottomY, leftPostX - 20.0f, postBottomY, WHITE);
        
        // Правая ГОРИЗОНТАЛЬНАЯ белая линия СПРАВА от правой штанги
        DrawLine(rightPostX + 20.0f, postBottomY, MAX_WIDTH, postBottomY, WHITE);
       
        // Рисуем ворота (штангИ)
        DrawGoal(goal);

        // Рисуем круги
        for (const auto& circle : circles) {
            DrawCircleV(circle.position, circle.radius, WHITE);
        }

        // Рисуем шкалу силы, если перетаскиваем мяч
        if (dragging && selectedCircle != nullptr) {
            DrawPowerBar(*selectedCircle, mousePosition, dragging);

            // Рисуем линию от мяча к курсору
            DrawLineV(selectedCircle->position, mousePosition, RED);
        }

        // Отображаем счет и монетки
        DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
        DrawText(TextFormat("Coins: %d", coins), 10, 40, 20, YELLOW);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
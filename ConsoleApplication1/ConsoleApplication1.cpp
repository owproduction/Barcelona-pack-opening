#include "raylib.h" 
#include "raymath.h"
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

const int MAX_WIDTH = 600;
const int MAX_HEIGHT = 800;
const float damping = -0.05f;

// Состояния игры
enum GameState {
    MENU,
    PLAYING,
    SHOP,
    COLLECTION
};

// Структура для футболиста
struct Footballer {
    Texture2D texture;
    std::string name;
    bool unlocked;
};

// Структура для кнопки
struct Button {
    Rectangle bounds;
    const char* text;
    Color color;
    Color textColor;
};

// Структура для ворот
struct Goal {
    Rectangle leftPost;   // Левая штанга
    Rectangle rightPost;  // Правая штанга
    Rectangle crossbar;   // Перекладина
    float width;          // Ширина ворот
    float height;         // Высота ворот
    Vector2 position;     // Позиция ворот (центр)
};

// Структура для вратаря
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
    Texture2D texture;    // Текстура вратаря
};

struct Circle {
    Vector2 position;
    Vector2 velocity;
    Vector2 accelerate;
    float radius;
    float weight;
    Vector2 startPosition; // Начальная позиция для возврата
    Vector2 hitPosition;   // Позиция удара для возврата при промахе
    bool hasSpin[4];       // Типы кручения: 0=влево, 1=вправо, 2=вверх, 3=вниз
    float spinForce[4];    // Сила каждого типа кручения
    Vector2 spinDirection[4]; // Направление каждого типа кручения
};

// Структура для анимации
struct Animation {
    bool active;
    float duration;
    float timer;
    Vector2 position;
    Vector2 velocity;
    float scale;
    float rotation;
    Color color;
    std::string text;
    Texture2D texture;
};

// Глобальные переменные
int coins = 0;
std::vector<Footballer> footballers;
Texture2D packTexture = { 0 };

// Анимации
Animation goalAnimation = { false };
Animation packAnimation = { false };

// Функция для сохранения прогресса
void SaveProgress() {
    std::ofstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        // Сохраняем количество монет
        file.write(reinterpret_cast<const char*>(&coins), sizeof(coins));

        // Сохраняем состояние разблокировки футболистов
        for (const auto& footballer : footballers) {
            file.write(reinterpret_cast<const char*>(&footballer.unlocked), sizeof(bool));
        }

        file.close();
    }
}

// Функция для загрузки прогресса
void LoadProgress() {
    std::ifstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        // Загружаем количество монет
        file.read(reinterpret_cast<char*>(&coins), sizeof(coins));

        // Загружаем состояние разблокировки футболистов
        for (auto& footballer : footballers) {
            file.read(reinterpret_cast<char*>(&footballer.unlocked), sizeof(bool));
        }

        file.close();
    }
}

// Функция для сброса прогресса (для тестирования)
void ResetProgress() {
    coins = 0;
    for (auto& footballer : footballers) {
        footballer.unlocked = false;
    }
    SaveProgress();
}

// Функция для создания кнопки
Button CreateButton(float x, float y, float width, float height, const char* text, Color color, Color textColor) {
    Button button;
    button.bounds = { x, y, width, height };
    button.text = text;
    button.color = color;
    button.textColor = textColor;
    return button;
}

// Функция для проверки нажатия на кнопку
bool IsButtonClicked(Button button) {
    return CheckCollisionPointRec(GetMousePosition(), button.bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// Функция для отрисовки кнопки
void DrawButton(Button button) {
    DrawRectangleRec(button.bounds, button.color);
    DrawRectangleLinesEx(button.bounds, 2, WHITE);

    // Центрирование текста
    int textWidth = MeasureText(button.text, 20);
    int textHeight = 20;
    DrawText(button.text,
        button.bounds.x + button.bounds.width / 2 - textWidth / 2,
        button.bounds.y + button.bounds.height / 2 - textHeight / 2,
        20, button.textColor);
}

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

// Функция для создания вратаря
Goalkeeper CreateGoalkeeper(float width, float height, Vector2 position, const char* texturePath) {
    Goalkeeper keeper;
    keeper.width = width;
    keeper.height = height;
    keeper.position = position;
    keeper.startPosition = position;
    keeper.bounds = { position.x - width / 2, position.y - height / 2, width, height };
    keeper.velocity = { 0, 0 };
    keeper.isJumping = false;
    keeper.jumpTimer = 0;
    keeper.jumpDirection = { 0, 0 };

    // Загружаем текстуру вратаря
    Image image = LoadImage(texturePath);
    if (image.data != NULL) {
        keeper.texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }
    else {
        // Если текстура не загрузилась, создаем пустую текстуру
        keeper.texture = { 0 };
    }

    return keeper;
}

// Функция для загрузки футболистов
void LoadFootballers() {
    footballers.clear();

    // Добавляем 5 футболистов (замени пути на свои PNG файлы)
    Footballer f1;
    f1.texture = LoadTexture("player1.png");
    f1.name = "Messi";
    f1.unlocked = false;
    footballers.push_back(f1);

    Footballer f2;
    f2.texture = LoadTexture("player2.png");
    f2.name = "Ronaldo";
    f2.unlocked = false;
    footballers.push_back(f2);

    Footballer f3;
    f3.texture = LoadTexture("player3.png");
    f3.name = "Neymar";
    f3.unlocked = false;
    footballers.push_back(f3);

    Footballer f4;
    f4.texture = LoadTexture("player4.png");
    f4.name = "Mbappe";
    f4.unlocked = false;
    footballers.push_back(f4);

    Footballer f5;
    f5.texture = LoadTexture("player5.png");
    f5.name = "Haaland";
    f5.unlocked = false;
    footballers.push_back(f5);
}

// Функция для запуска анимации гола
void StartGoalAnimation() {
    goalAnimation.active = true;
    goalAnimation.duration = 2.0f; // 2 секунды
    goalAnimation.timer = goalAnimation.duration;
    goalAnimation.position = { MAX_WIDTH / 2.0f, MAX_HEIGHT / 2.0f };
    goalAnimation.scale = 1.0f;
    goalAnimation.rotation = 0.0f;
    goalAnimation.color = YELLOW;
    goalAnimation.text = "GOOOOAL!";
}

// Функция для запуска анимации открытия пака - УПРОЩЕННАЯ ВЕРСИЯ
void StartPackAnimation(Texture2D playerTexture, const std::string& playerName) {
    packAnimation.active = true;
    packAnimation.duration = 3.0f; // 3 секунды
    packAnimation.timer = packAnimation.duration;
    packAnimation.position = { MAX_WIDTH / 2.0f, MAX_HEIGHT / 2.0f }; // Сразу по центру
    packAnimation.velocity = { 0, 0 }; // Без движения
    packAnimation.scale = 0.1f; // Начинаем маленьким
    packAnimation.rotation = 0.0f;
    packAnimation.color = WHITE;
    packAnimation.texture = playerTexture;
    packAnimation.text = playerName;
}

// Функция для открытия пака
void OpenPack() {
    if (coins >= 10) {
        coins -= 10;

        // Сохраняем прогресс после траты монет
        SaveProgress();

        // Случайный выбор футболиста
        int randomIndex = GetRandomValue(0, footballers.size() - 1);

        // Проверяем, был ли уже разблокирован этот футболист
        bool wasUnlocked = footballers[randomIndex].unlocked;
        footballers[randomIndex].unlocked = true;

        // Запускаем анимацию открытия пака
        StartPackAnimation(footballers[randomIndex].texture, footballers[randomIndex].name);

        // Сохраняем прогресс после разблокировки футболиста
        SaveProgress();


    }

}

// Функция для обновления анимаций
void UpdateAnimations(float deltaTime) {
    // Обновление анимации гола
    if (goalAnimation.active) {
        goalAnimation.timer -= deltaTime;

        // Пульсация текста
        goalAnimation.scale = 1.0f + sinf(GetTime() * 10.0f) * 0.2f;

        if (goalAnimation.timer <= 0) {
            goalAnimation.active = false;
        }
    }

    // Обновление анимации пака - УПРОЩЕННАЯ ВЕРСИЯ
    if (packAnimation.active) {
        packAnimation.timer -= deltaTime;

        // ПЛАВНОЕ УВЕЛИЧЕНИЕ МАСШТАБА без движения и вращения
        packAnimation.scale += deltaTime * 1.0f; // Быстрее увеличиваем масштаб
        if (packAnimation.scale > 1.0f) packAnimation.scale = 1.0f;

        if (packAnimation.timer <= 0) {
            packAnimation.active = false;
        }
    }
}

// Функция для отрисовки анимаций
void DrawAnimations() {
    // Отрисовка анимации гола
    if (goalAnimation.active) {
        float alpha = goalAnimation.timer / goalAnimation.duration;
        Color textColor = Fade(goalAnimation.color, alpha);

        int fontSize = 60 + (int)(20.0f * (1.0f - goalAnimation.scale));
        const char* text = goalAnimation.text.c_str();
        int textWidth = MeasureText(text, fontSize);

        // Тень
        DrawText(text,
            goalAnimation.position.x - textWidth / 2 + 3,
            goalAnimation.position.y - fontSize / 2 + 3,
            fontSize, Fade(BLACK, alpha * 0.5f));

        // Основной текст
        DrawText(text,
            goalAnimation.position.x - textWidth / 2,
            goalAnimation.position.y - fontSize / 2,
            fontSize, textColor);

        // Эффект частиц
        for (int i = 0; i < 20; i++) {
            float angle = GetTime() * 5.0f + i * 18.0f * DEG2RAD;
            float distance = 100.0f * (1.0f - alpha);
            Vector2 particlePos = {
                goalAnimation.position.x + cosf(angle) * distance,
                goalAnimation.position.y + sinf(angle) * distance
            };
            DrawCircleV(particlePos, 3, Fade(YELLOW, alpha * 0.5f));
        }
    }

    // Отрисовка анимации пака - УПРОЩЕННАЯ ВЕРСИЯ
    if (packAnimation.active) {
        float alpha = packAnimation.timer / packAnimation.duration;

        if (packAnimation.texture.id != 0) {
            Rectangle sourceRec = { 0, 0, (float)packAnimation.texture.width, (float)packAnimation.texture.height };
            Rectangle destRec = {
                packAnimation.position.x,
                packAnimation.position.y,
                packAnimation.texture.width * packAnimation.scale,
                packAnimation.texture.height * packAnimation.scale
            };
            Vector2 origin = {
                packAnimation.texture.width * packAnimation.scale / 2,
                packAnimation.texture.height * packAnimation.scale / 2
            };

            DrawTexturePro(packAnimation.texture, sourceRec, destRec, origin, packAnimation.rotation, Fade(WHITE, alpha));
        }

        // Текст с именем игрока
        const char* playerText = packAnimation.text.c_str();
        int textWidth = MeasureText(playerText, 30);
        DrawText(playerText,
            packAnimation.position.x - textWidth / 2,
            packAnimation.position.y + 80,
            30, Fade(GREEN, alpha));

        // Текст "NEW PLAYER!"
        const char* newPlayerText = "NEW PLAYER!";
        int newTextWidth = MeasureText(newPlayerText, 25);
        DrawText(newPlayerText,
            packAnimation.position.x - newTextWidth / 2,
            packAnimation.position.y + 110,
            25, Fade(GOLD, alpha));

        // Эффект сияния (упрощенный)
        for (int i = 0; i < 3; i++) {
            float pulse = sinf(GetTime() * 8.0f + i) * 0.2f + 0.8f;
            DrawCircleLines(packAnimation.position.x, packAnimation.position.y,
                50 + i * 20 + pulse * 10, Fade(GOLD, alpha * 0.3f));
        }
    }
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

// Функция для отрисовки вратаря
void DrawGoalkeeper(const Goalkeeper& keeper) {
    if (keeper.texture.id != 0) {
        // Рисуем текстуру вратаря
        Rectangle sourceRec = { 0, 0, (float)keeper.texture.width, (float)keeper.texture.height };
        Rectangle destRec = { keeper.position.x, keeper.position.y, keeper.width, keeper.height };
        Vector2 origin = { keeper.width / 2, keeper.height / 2 };
        DrawTexturePro(keeper.texture, sourceRec, destRec, origin, 0, WHITE);
    }
    else {
        // Резервный вариант - оранжевый прямоугольник
        DrawRectangleRec(keeper.bounds, ORANGE);
    }
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

// Функция для проверки столкновения с вратарем
bool CheckGoalkeeperCollision(const Circle& circle, const Goalkeeper& keeper) {
    return CheckCollisionCircleRec(circle.position, circle.radius, keeper.bounds);
}

// Функция для обработки столкновения с вратарем
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

// Функция для проверки касания зеленой линии (гол)
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

// Функция для проверки полного залета мяча в ворота
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

// Функция для проверки касания ГОРИЗОНТАЛЬНЫХ белых линий СБОКУ ОТ ШТАНГ
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

// Функция для прыжка вратаря
void MakeGoalkeeperJump(Goalkeeper& keeper) {
    if (!keeper.isJumping) {
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

// Функция для обновления вратаря
void UpdateGoalkeeper(Goalkeeper& keeper, float deltaTime) {
    if (keeper.isJumping) {
        // Обновляем таймер прыжка
        keeper.jumpTimer -= deltaTime;

        // Обновляем позицию
        keeper.position = Vector2Add(keeper.position, Vector2Scale(keeper.velocity, deltaTime));

        // Добавляем гравитацию
        keeper.velocity.y += 400.0f * deltaTime; // Уменьшил гравитацию

        // ОГРАНИЧИВАЕМ ПОЗИЦИЮ ВРАТАРЯ В ПРЕДЕЛАХ ВОРОТ (уже границы)
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

    // Обновляем границы вратаря
    keeper.bounds.x = keeper.position.x - keeper.width / 2;
    keeper.bounds.y = keeper.position.y - keeper.height / 2;
}

// Функция для применения крученого удара с комбинациями
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

// Функция для обновления кручения мяча
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

        Circle newCircle = { position,velocity,accelerate,radius,weight,position,position };
        for (int j = 0; j < 4; j++) {
            newCircle.hasSpin[j] = false;
            newCircle.spinForce[j] = 0.0f;
            newCircle.spinDirection[j] = { 0, 0 };
        }
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
    for (int i = 0; i < 4; i++) {
        circle.hasSpin[i] = false;
        circle.spinForce[i] = 0.0f;
        circle.spinDirection[i] = { 0, 0 };
    }
}

// Функция для возврата круга в точку удара
void ResetToHitPosition(Circle& circle) {
    circle.position = circle.hitPosition;
    circle.velocity = { 0, 0 };
    circle.accelerate = { 0, 0 };
    for (int i = 0; i < 4; i++) {
        circle.hasSpin[i] = false;
        circle.spinForce[i] = 0.0f;
        circle.spinDirection[i] = { 0, 0 };
    }
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
void DrawPowerBar(const Circle& circle, const Vector2& mousePosition, bool isDragging, bool spinActive[4]) {
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

        // Определяем цвет и тип удара на основе активных кручений
        Color fillColor = WHITE;
        std::string shotType = "NORMAL SHOT";

        int activeSpins = 0;
        for (int i = 0; i < 4; i++) {
            if (spinActive[i]) activeSpins++;
        }

        if (activeSpins == 1) {
            if (spinActive[0]) { fillColor = BLUE; shotType = "LEFT SPIN"; }
            else if (spinActive[1]) { fillColor = RED; shotType = "RIGHT SPIN"; }
            else if (spinActive[2]) { fillColor = GREEN; shotType = "TOP SPIN"; }
            else if (spinActive[3]) { fillColor = YELLOW; shotType = "BACK SPIN"; }
        }
        else if (activeSpins > 1) {
            fillColor = PURPLE;
            shotType = "COMBO SPIN";
        }

        // Рисуем заполнение шкалы (цвет зависит от типа удара)
        DrawRectangleRec({ barPosition.x, barPosition.y, barWidth * power, barHeight }, fillColor);

        // Рисуем контур шкалы
        DrawRectangleLines(barPosition.x, barPosition.y, barWidth, barHeight, WHITE);

        // Отображаем тип удара
        DrawText(shotType.c_str(), barPosition.x, barPosition.y - 20, 15, fillColor);
    }
}

// Функция для отрисовки главного меню
void DrawMainMenu(Button playButton, Button shopButton, Button collectionButton, Button exitButton) {
    // Заголовок
    DrawText("FOOTBALL GAME", MAX_WIDTH / 2 - MeasureText("FOOTBALL GAME", 40) / 2, 100, 40, WHITE);

    // Отображаем количество монеток
    DrawText(TextFormat("Coins: %d", coins), MAX_WIDTH / 2 - MeasureText(TextFormat("Coins: %d", coins), 30) / 2, 160, 30, GOLD);

    // Кнопки
    DrawButton(playButton);
    DrawButton(shopButton);
    DrawButton(collectionButton);
    DrawButton(exitButton);
}

// Функция для отрисовки магазина
void DrawShop() {
    DrawText("SHOP", MAX_WIDTH / 2 - MeasureText("SHOP", 40) / 2, 50, 40, WHITE);
    DrawText(TextFormat("Coins: %d", coins), MAX_WIDTH / 2 - MeasureText(TextFormat("Coins: %d", coins), 30) / 2, 100, 30, GOLD);

    // Отображаем пак
    if (packTexture.id != 0) {
        Rectangle packRect = { MAX_WIDTH / 2 - 100, 150, 200, 200 };
        DrawTexturePro(packTexture, { 0, 0, (float)packTexture.width, (float)packTexture.height },
            packRect, { 0, 0 }, 0, WHITE);

        // Цена пака
        DrawText("10 coins", MAX_WIDTH / 2 - MeasureText("10 coins", 25) / 2, 370, 25, GOLD);

        // Кнопка покупки
        if (coins >= 10) {
            DrawText("CLICK TO BUY", MAX_WIDTH / 2 - MeasureText("CLICK TO BUY", 20) / 2, 400, 20, GREEN);
        }
        else {
            DrawText("NOT ENOUGH COINS", MAX_WIDTH / 2 - MeasureText("NOT ENOUGH COINS", 20) / 2, 400, 20, RED);
        }

        // Информация о коллекции
        int unlockedCount = 0;
        for (const auto& footballer : footballers) {
            if (footballer.unlocked) unlockedCount++;
        }
        DrawText(TextFormat("Collection: %d/%d", unlockedCount, footballers.size()),
            MAX_WIDTH / 2 - MeasureText(TextFormat("Collection: %d/%d", unlockedCount, footballers.size()), 20) / 2,
            430, 20, WHITE);
    }

    DrawText("H - Back to Menu", 10, MAX_HEIGHT - 30, 20, WHITE);
}

// Функция для отрисовки коллекции - С УВЕЛИЧЕННЫМИ КАРТОЧКАМИ
void DrawCollection() {
    DrawText("COLLECTION", MAX_WIDTH / 2 - MeasureText("COLLECTION", 40) / 2, 50, 40, WHITE);

    int unlockedCount = 0;
    for (const auto& footballer : footballers) {
        if (footballer.unlocked) unlockedCount++;
    }

    DrawText(TextFormat("Unlocked: %d/%d", unlockedCount, footballers.size()),
        MAX_WIDTH / 2 - MeasureText(TextFormat("Unlocked: %d/%d", unlockedCount, footballers.size()), 25) / 2,
        100, 25, WHITE);

    // Отображаем футболистов с УВЕЛИЧЕННЫМИ КАРТОЧКАМИ
    float startX = 50;
    float startY = 150;
    float cardWidth = 160;  // УВЕЛИЧЕНА ШИРИНА КАРТОЧКИ
    float cardHeight = 190; // УВЕЛИЧЕНА ВЫСОТА КАРТОЧКИ
    float spacingX = 170;   // УВЕЛИЧЕНО РАССТОЯНИЕ МЕЖДУ КАРТОЧКАМИ ПО X
    float spacingY = 200;   // УВЕЛИЧЕНО РАССТОЯНИЕ МЕЖДУ КАРТОЧКАМИ ПО Y

    for (int i = 0; i < footballers.size(); i++) {
        float x = startX + (i % 3) * spacingX;
        float y = startY + (i / 3) * spacingY;

        if (footballers[i].unlocked) {
            // Отображаем разблокированного футболиста
            if (footballers[i].texture.id != 0) {
                Rectangle playerRect = { x, y, cardWidth, cardHeight - 40 }; // УВЕЛИЧЕНА ОБЛАСТЬ ТЕКСТУРЫ
                DrawTexturePro(footballers[i].texture, { 0, 0, (float)footballers[i].texture.width, (float)footballers[i].texture.height },
                    playerRect, { 0, 0 }, 0, WHITE);

                // Рамка вокруг карточки
                DrawRectangleLinesEx(playerRect, 2, GREEN);
            }

            // Имя футболиста под картинкой
            DrawText(footballers[i].name.c_str(),
                x + cardWidth / 2 - MeasureText(footballers[i].name.c_str(), 20) / 2,
                y + cardHeight - 35, 20, GREEN);
        }
        else {
            // Отображаем заблокированного футболиста
            Rectangle cardRect = { x, y, cardWidth, cardHeight - 40 };
            DrawRectangleRec(cardRect, DARKGRAY);
            DrawRectangleLinesEx(cardRect, 2, GRAY);

            DrawText("???", x + cardWidth / 2 - MeasureText("???", 25) / 2, y + (cardHeight - 40) / 2 - 12, 25, WHITE);

            // Текст "LOCKED" под картинкой
            DrawText("LOCKED",
                x + cardWidth / 2 - MeasureText("LOCKED", 18) / 2,
                y + cardHeight - 35, 18, GRAY);
        }
    }

    DrawText("H - Back to Menu", 10, MAX_HEIGHT - 30, 20, WHITE);
}

int main()
{
    InitWindow(MAX_WIDTH, MAX_HEIGHT, "Bounce with Goal");
    SetTargetFPS(60);

    // Состояние игры
    GameState gameState = MENU;

    // Кнопки меню
    Button playButton = CreateButton(MAX_WIDTH / 2 - 100, 200, 200, 50, "PLAY", BLUE, WHITE);
    Button shopButton = CreateButton(MAX_WIDTH / 2 - 100, 270, 200, 50, "SHOP", GREEN, WHITE);
    Button collectionButton = CreateButton(MAX_WIDTH / 2 - 100, 340, 200, 50, "COLLECTION", PURPLE, WHITE);
    Button exitButton = CreateButton(MAX_WIDTH / 2 - 100, 410, 200, 50, "EXIT", RED, WHITE);

    // Игровые объекты
    bool dragging = false;
    Circle* selectedCircle = nullptr;
    Vector2 dragStartPosition;
    int score = 0; // Счет голов
    bool spinActive[4] = { false, false, false, false }; // Типы кручения: 0=влево, 1=вправо, 2=вверх, 3=вниз
    bool slowMoActive = false; // Активен ли режим слоумо
    float slowMoFactor = 0.3f; // Коэффициент замедления

    std::vector<Circle> circles;
    GenerateCircles(circles, 1, 15.0f, 0, 0);

    // Создаем ворота вверху экрана (опущены ниже)
    Goal goal = CreateGoal(200.0f, 100.0f, { MAX_WIDTH / 2.0f, 150.0f });

    // Создаем вратаря с PNG текстурой
    Goalkeeper goalkeeper = CreateGoalkeeper(80.0f, 60.0f, { MAX_WIDTH / 2.0f, goal.position.y + 20.0f }, "goalkeeper (1).png");

    // Загружаем пак
    Image packImage = LoadImage("pack.png"); // Замени на свой PNG файл пака
    if (packImage.data != NULL) {
        packTexture = LoadTextureFromImage(packImage);
        UnloadImage(packImage);
    }

    // Загружаем футболистов
    LoadFootballers();

    // Загружаем сохраненный прогресс
    LoadProgress();

    float deltaTime = 0.0f;

    while (!WindowShouldClose()) {
        float rawDeltaTime = GetFrameTime();

        // Применяем слоумо если активно
        if (slowMoActive) {
            deltaTime = rawDeltaTime * slowMoFactor;
        }
        else {
            deltaTime = rawDeltaTime;
        }

        Vector2 mousePosition = GetMousePosition();

        // Обработка нажатия H для возврата в меню
        if (IsKeyPressed(KEY_H)) {
            gameState = MENU;
            slowMoActive = false;
        }

        // Обработка сброса прогресса (для тестирования) - Shift+R
        if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_R)) {
            ResetProgress();
            LoadProgress(); // Перезагружаем прогресс
        }

        // Обновляем анимации
        UpdateAnimations(deltaTime);

        // Обработка состояний игры
        switch (gameState) {
        case MENU:
            if (IsButtonClicked(playButton)) {
                gameState = PLAYING;
                score = 0;
            }
            if (IsButtonClicked(shopButton)) {
                gameState = SHOP;
            }
            if (IsButtonClicked(collectionButton)) {
                gameState = COLLECTION;
            }
            if (IsButtonClicked(exitButton)) {
                // Сохраняем прогресс перед выходом
                SaveProgress();
                CloseWindow();
                return 0;
            }
            break;

        case PLAYING:
        {
            // Проверка нажатия кнопок для выбора типа кручения (можно комбинировать)
            spinActive[0] = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT); // Влево
            spinActive[1] = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT); // Вправо
            spinActive[2] = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP); // Вверх
            spinActive[3] = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN); // Вниз

            // Проверка нажатия кнопки для режима слоумо (SPACE)
            if (IsKeyPressed(KEY_SPACE)) {
                slowMoActive = !slowMoActive;
            }

            // Игровая логика
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

                    // УВЕЛИЧИВАЕМ СИЛУ УДАРА (было 0.15f, стало 0.25f)
                    float powerMultiplier = 0.25f;

                    // ПРИМЕНЯЕМ СИЛУ БЕЗ ИЗМЕНЕНИЙ В РЕЖИМЕ СЛОУМО
                    // (сила удара остается прежней, замедляется только время)
                    Vector2 acceleration = Vector2Scale(Vector2Normalize(direction), distance * powerMultiplier);

                    selectedCircle->velocity = acceleration;

                    // Применяем крученый удар если выбран тип
                    bool anySpinActive = false;
                    for (int i = 0; i < 4; i++) {
                        if (spinActive[i]) {
                            anySpinActive = true;
                            break;
                        }
                    }

                    if (anySpinActive) {
                        ApplySpin(*selectedCircle, direction, spinActive);
                    }

                    // Вратарь прыгает при ударе
                    MakeGoalkeeperJump(goalkeeper);
                }
                dragging = false;
                selectedCircle = nullptr;
                // Не сбрасываем spinActive, чтобы можно было делать несколько ударов с одинаковой комбинацией
            }

            // Обновляем вратаря с учетом слоумо
            UpdateGoalkeeper(goalkeeper, deltaTime);

            for (auto& circle : circles) {
                // ОБНОВЛЯЕМ КРУЧЕНИЕ МЯЧА ПЕРЕД ОБНОВЛЕНИЕМ ПОЗИЦИИ
                UpdateSpin(circle, deltaTime);

                circle.accelerate = Vector2Scale(circle.velocity, damping);
                circle.velocity = Vector2Add(circle.velocity, circle.accelerate);
                circle.position = Vector2Add(circle.position, circle.velocity);

                // Проверяем столкновение с вратарем
                HandleGoalkeeperCollision(circle, goalkeeper);

                // Проверяем вылет за границы
                if (IsCircleOutOfBounds(circle)) {
                    ResetCircle(circle);
                }

                // Проверяем пересечение линии ворот (мимо ворот)
                if (CheckGoalLineCrossing(circle, goal)) {
                    ResetCircle(circle);
                }

                // Проверяем касание зеленой линии (гол)
                if (CheckGreenLineTouch(circle, goal)) {
                    score++; // Увеличиваем счет
                    coins++; // Добавляем монетку за гол
                    SaveProgress(); // Сохраняем прогресс после получения монет
                    StartGoalAnimation(); // Запускаем анимацию гола
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
        }
        break;

        case SHOP:
            // Обработка покупки пака
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Rectangle packRect = { MAX_WIDTH / 2 - 100, 150, 200, 200 };
                if (CheckCollisionPointRec(mousePosition, packRect) && coins >= 10) {
                    OpenPack();
                }
            }
            break;

        case COLLECTION:
            // Коллекция - только просмотр
            break;
        }

        BeginDrawing();
        ClearBackground(DARKGREEN);

        switch (gameState) {
        case MENU:
            DrawMainMenu(playButton, shopButton, collectionButton, exitButton);
            break;

        case PLAYING:
        {
            // Координаты для линий на уровне низа штанг
            float postBottomY = goal.position.y + goal.height / 2;
            float leftPostX = goal.position.x - goal.width / 2;
            float rightPostX = goal.position.x + goal.width / 2;

            // Рисуем зеленую линию (выше желтой на 50 пикселей) - для гола
            float greenLineY = postBottomY - 50.0f;
            DrawLine(leftPostX, greenLineY, rightPostX, greenLineY, GREEN);

            // Рисуем желтую линию на уровне низа штанг (горизонтальная)
            DrawLine(leftPostX, postBottomY, rightPostX, postBottomY, WHITE);

            // Рисуем ГОРИЗОНТАЛЬНЫЕ белые линии СБОКУ ОТ ШТАНГ (теперь прилегают к воротам)
            // Левая ГОРИЗОНТАЛЬНАЯ белая линия СЛЕВА от левой штанги
            DrawLine(0, postBottomY, leftPostX, postBottomY, WHITE);

            // Правая ГОРИЗОНТАЛЬНАЯ белая линия СПРАВА от правой штанги
            DrawLine(rightPostX, postBottomY, MAX_WIDTH, postBottomY, WHITE);

            // Рисуем ворота (штангИ)
            DrawGoal(goal);

            // Рисуем вратаря (PNG текстура)
            DrawGoalkeeper(goalkeeper);

            // Рисуем круги
            for (const auto& circle : circles) {
                DrawCircleV(circle.position, circle.radius, WHITE);
            }

            // Рисуем шкалу силы, если перетаскиваем мяч
            if (dragging && selectedCircle != nullptr) {
                DrawPowerBar(*selectedCircle, mousePosition, dragging, spinActive);

                // Рисуем линию от мяча к курсору
                Color lineColor = WHITE;
                int activeSpins = 0;
                for (int i = 0; i < 4; i++) {
                    if (spinActive[i]) activeSpins++;
                }

                if (activeSpins == 1) {
                    if (spinActive[0]) lineColor = BLUE;
                    else if (spinActive[1]) lineColor = RED;
                    else if (spinActive[2]) lineColor = GREEN;
                    else if (spinActive[3]) lineColor = YELLOW;
                }
                else if (activeSpins > 1) {
                    lineColor = PURPLE;
                }

                DrawLineV(selectedCircle->position, mousePosition, lineColor);
            }

            // Отрисовываем анимации (поверх всего)
            DrawAnimations();

            // Отображаем счет
            DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);

            // Отображаем количество монеток (просто текст)
            DrawText(TextFormat("Coins: %d", coins), 10, 40, 20, GOLD);

            // Подсказка управления
            DrawText("A/LEFT - Left Spin", 10, MAX_HEIGHT - 140, 15, spinActive[0] ? BLUE : WHITE);
            DrawText("D/RIGHT - Right Spin", 10, MAX_HEIGHT - 120, 15, spinActive[1] ? RED : WHITE);
            DrawText("W/UP - Top Spin", 10, MAX_HEIGHT - 100, 15, spinActive[2] ? GREEN : WHITE);
            DrawText("S/DOWN - Back Spin", 10, MAX_HEIGHT - 80, 15, spinActive[3] ? YELLOW : WHITE);
            DrawText("SPACE - Slow Motion", 10, MAX_HEIGHT - 60, 15, slowMoActive ? ORANGE : WHITE);
            DrawText("H - Menu", 10, MAX_HEIGHT - 40, 15, WHITE);

            // Подсказка для сброса прогресса
            DrawText("Shift+R - Reset Progress", MAX_WIDTH - 200, MAX_HEIGHT - 20, 15, GRAY);

            // Отображаем статус слоумо
            if (slowMoActive) {
                DrawText("SLOW MOTION", MAX_WIDTH - 150, 40, 20, ORANGE);
            }
        }
        break;

        case SHOP:
            DrawShop();
            // Отрисовываем анимацию пака поверх магазина
            DrawAnimations();
            break;

        case COLLECTION:
            DrawCollection();
            break;
        }

        EndDrawing();
    }

    // Сохраняем прогресс перед закрытием игры
    SaveProgress();

    // Освобождаем ресурсы
    if (goalkeeper.texture.id != 0) {
        UnloadTexture(goalkeeper.texture);
    }
    if (packTexture.id != 0) {
        UnloadTexture(packTexture);
    }
    for (auto& footballer : footballers) {
        if (footballer.texture.id != 0) {
            UnloadTexture(footballer.texture);
        }
    }

    CloseWindow();
    return 0;
}
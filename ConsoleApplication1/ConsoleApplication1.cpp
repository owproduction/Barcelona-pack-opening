#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "Game.h"
#include "GameObjects.h"
#include "SaveSystem.h"
#include "UI.h"
#include "Animation.h"
#include "Collision.h"

int main()
{
    InitWindow(MAX_WIDTH, MAX_HEIGHT, "Bounce with Goal");
    SetTargetFPS(60);

    // Состояние игры
    GameState gameState = MENU;

    // Кнопки меню
    Button playButton = CreateButton(MAX_WIDTH / 2 - 100, 200, 200, 50, "1 PLAYER", BLUE, WHITE);
    Button twoPlayersButton = CreateButton(MAX_WIDTH / 2 - 100, 270, 200, 50, "2 PLAYERS", GREEN, WHITE);
    Button shopButton = CreateButton(MAX_WIDTH / 2 - 100, 340, 200, 50, "SHOP", PURPLE, WHITE);
    Button collectionButton = CreateButton(MAX_WIDTH / 2 - 100, 410, 200, 50, "COLLECTION", ORANGE, WHITE);
    Button exitButton = CreateButton(MAX_WIDTH / 2 - 100, 480, 200, 50, "EXIT", RED, WHITE);

    // Кнопки меню двух игроков (УВЕЛИЧЕНЫ В ШИРИНУ)
    Button player1KeeperButton = CreateButton(MAX_WIDTH / 2 - 150, 200, 300, 50, "PLAYER 1 - KEEPER", BLUE, WHITE);
    Button player2KeeperButton = CreateButton(MAX_WIDTH / 2 - 150, 270, 300, 50, "PLAYER 2 - KEEPER", RED, WHITE);
    Button backButton = CreateButton(MAX_WIDTH / 2 - 150, 340, 300, 50, "BACK", GRAY, WHITE);

    // Кнопки выбора режима игры (УВЕЛИЧЕНЫ В ШИРИНУ)
    Button freeKickButton = CreateButton(MAX_WIDTH / 2 - 150, 350, 300, 50, "FREE KICK", GREEN, WHITE);
    Button penaltyButton = CreateButton(MAX_WIDTH / 2 - 150, 420, 300, 50, "PENALTY", YELLOW, WHITE);
    Button modeBackButton = CreateButton(MAX_WIDTH / 2 - 150, 490, 300, 50, "BACK", GRAY, WHITE);

    // Игровые объекты
    bool dragging = false;
    Circle* selectedCircle = nullptr;
    Vector2 dragStartPosition;
    int score = 0; // Счет голов для одного игрока
    int player1Score = 0, player2Score = 0; // Счет для двух игроков
    bool spinActive[4] = { false, false, false, false }; // Типы кручения: 0=влево, 1=вправо, 2=вверх, 3=вниз
    bool slowMoActive = false; // Активен ли режим слоумо
    float slowMoFactor = 0.3f; // Коэффициент замедления
    int goalkeeperController = 0; // Кто управляет вратарем в режиме двух игроков
    bool isSinglePlayer = true; // Режим одного игрока по умолчанию

    std::vector<Circle> circles;

    // Создаем ворота вверху экрана (опущены ниже)
    Goal goal = CreateGoal(200.0f, 100.0f, { MAX_WIDTH / 2.0f, 150.0f });

    // Создаем вратаря с PNG текстурой (по умолчанию AI)
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
            if (gameState == TWO_PLAYERS_GAME || gameState == PLAYING) {
                gameState = GAME_MODE_SELECTION;
            }
            else if (gameState == GAME_MODE_SELECTION) {
                if (isSinglePlayer) {
                    gameState = MENU;
                }
                else {
                    gameState = TWO_PLAYERS_MENU;
                }
            }
            else if (gameState == TWO_PLAYERS_MENU) {
                gameState = MENU;
            }
            else if (gameState == COLLECTION) {
                gameState = MENU;
                collectionPage = 0; // Сбрасываем страницу при выходе
            }
            else {
                gameState = MENU;
            }
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
                gameState = GAME_MODE_SELECTION;
                isSinglePlayer = true;
                score = 0;
                // Сбрасываем вратаря в AI режим
                goalkeeper.playerControlled = 0;
            }
            if (IsButtonClicked(twoPlayersButton)) {
                gameState = TWO_PLAYERS_MENU;
                isSinglePlayer = false;
                player1Score = 0;
                player2Score = 0;
            }
            if (IsButtonClicked(shopButton)) {
                gameState = SHOP;
            }
            if (IsButtonClicked(collectionButton)) {
                gameState = COLLECTION;
                collectionPage = 0; // Сбрасываем на первую страницу
            }
            if (IsButtonClicked(exitButton)) {
                // Сохраняем прогресс перед выходом
                SaveProgress();
                CloseWindow();
                return 0;
            }
            break;

        case TWO_PLAYERS_MENU:
            if (IsButtonClicked(player1KeeperButton)) {
                gameState = GAME_MODE_SELECTION;
                goalkeeperController = 1;
                goalkeeper.playerControlled = 1;
            }
            if (IsButtonClicked(player2KeeperButton)) {
                gameState = GAME_MODE_SELECTION;
                goalkeeperController = 2;
                goalkeeper.playerControlled = 2;
            }
            if (IsButtonClicked(backButton)) {
                gameState = MENU;
            }
            break;

        case GAME_MODE_SELECTION:
            if (IsButtonClicked(freeKickButton)) {
                currentGameMode = FREE_KICK;
                // Генерируем мяч в зависимости от режима
                circles.clear();
                GenerateCircles(circles, 1, 15.0f, 0, 0, currentGameMode);

                if (isSinglePlayer) {
                    gameState = PLAYING;
                }
                else {
                    gameState = TWO_PLAYERS_GAME;
                }
            }
            if (IsButtonClicked(penaltyButton)) {
                currentGameMode = PENALTY;
                // Генерируем мяч в зависимости от режима
                circles.clear();
                GenerateCircles(circles, 1, 15.0f, 0, 0, currentGameMode);

                if (isSinglePlayer) {
                    gameState = PLAYING;
                }
                else {
                    gameState = TWO_PLAYERS_GAME;
                }
            }
            if (IsButtonClicked(modeBackButton)) {
                if (isSinglePlayer) {
                    gameState = MENU;
                }
                else {
                    gameState = TWO_PLAYERS_MENU;
                }
            }
            break;

        case COLLECTION:
            // Обработка перелистывания страниц
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Кнопка "PREV"
                if (collectionPage > 0 && CheckCollisionPointRec(mousePosition, { 20, MAX_HEIGHT - 40, 80, 20 })) {
                    collectionPage--;
                }
                // Кнопка "NEXT"
                if ((collectionPage + 1) * playersPerPage < footballers.size() &&
                    CheckCollisionPointRec(mousePosition, { MAX_WIDTH - 100, MAX_HEIGHT - 40, 80, 20 })) {
                    collectionPage++;
                }
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
                    // В режиме пенальти проверяем, можно ли перемещать мяч
                    if (currentGameMode == PENALTY && !circle.canMoveFreely) {
                        continue; // Пропускаем мяч, который нельзя перемещать
                    }

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

                    // Вратарь прыгает при ударе (только если AI)
                    if (goalkeeper.playerControlled == 0) {
                        MakeGoalkeeperJump(goalkeeper);
                    }

                    // В режиме пенальти после удара мяч нельзя больше перемещать
                    if (currentGameMode == PENALTY) {
                        selectedCircle->canMoveFreely = false;
                    }
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
                    ResetCircle(circle, currentGameMode);
                }

                // Проверяем пересечение линии ворот (мимо ворот)
                if (CheckGoalLineCrossing(circle, goal)) {
                    ResetCircle(circle, currentGameMode);
                }

                // Проверяем касание зеленой линии (гол)
                if (CheckGreenLineTouch(circle, goal)) {
                    score++; // Увеличиваем счет
                    coins++; // Добавляем монетку за гол
                    SaveProgress(); // Сохраняем прогресс после получения монет
                    StartGoalAnimation(); // Запускаем анимацию гола
                    ResetCircle(circle, currentGameMode);
                }

                // Проверяем касание ГОРИЗОНТАЛЬНЫХ белых линий СБОКУ ОТ ШТАНГ
                if (CheckSideLinesCollision(circle, goal)) {
                    ResetToHitPosition(circle, currentGameMode);
                }

                // Обработка столкновений со стенами
                HandleWallCollision(circle);

                // Обработка столкновений с воротами (мяч отскакивает от штанг)
                HandleGoalCollision(circle, goal);

                // В режиме пенальти, если мяч почти остановился, возвращаем его на стартовую позицию
                if (currentGameMode == PENALTY && !circle.canMoveFreely && Vector2Length(circle.velocity) < 10.0f) {
                    ResetCircle(circle, currentGameMode);
                }
            }
        }
        break;

        case TWO_PLAYERS_GAME:
        {
            // Управление для игрока 1 (бьющий) - WASD для кручения
            spinActive[0] = IsKeyDown(KEY_A); // Влево
            spinActive[1] = IsKeyDown(KEY_D); // Вправо
            spinActive[2] = IsKeyDown(KEY_W); // Вверх
            spinActive[3] = IsKeyDown(KEY_S); // Вниз

            // Игровая логика для игрока 1
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                for (auto& circle : circles) {
                    // В режиме пенальти проверяем, можно ли перемещать мяч
                    if (currentGameMode == PENALTY && !circle.canMoveFreely) {
                        continue; // Пропускаем мяч, который нельзя перемещать
                    }

                    if (CheckCollisionPointCircle(mousePosition, circle.position, circle.radius) && circle.controllingPlayer == 1) {
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

                    float powerMultiplier = 0.25f;
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

                    // В режиме пенальти после удара мяч нельзя больше перемещать
                    if (currentGameMode == PENALTY) {
                        selectedCircle->canMoveFreely = false;
                    }
                }
                dragging = false;
                selectedCircle = nullptr;
            }

            // Обновляем вратаря (управляется игроком)
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
                    ResetCircle(circle, currentGameMode);
                    // При вылете мяча за границы очко получает вратарь
                    if (goalkeeperController == 1) {
                        player1Score++;
                    }
                    else {
                        player2Score++;
                    }
                }

                // Проверяем пересечение линии ворот (мимо ворот)
                if (CheckGoalLineCrossing(circle, goal)) {
                    ResetCircle(circle, currentGameMode);
                    // При промахе очко получает вратарь
                    if (goalkeeperController == 1) {
                        player1Score++;
                    }
                    else {
                        player2Score++;
                    }
                }

                // Проверяем касание зеленой линии (гол)
                if (CheckGreenLineTouch(circle, goal)) {
                    // Гол! Очко получает бьющий (игрок 1)
                    player1Score++;
                    StartGoalAnimation(); // Запускаем анимацию гола
                    ResetCircle(circle, currentGameMode);
                }

                // Проверяем касание ГОРИЗОНТАЛЬНЫХ белых линий СБОКУ ОТ ШТАНГ
                if (CheckSideLinesCollision(circle, goal)) {
                    ResetToHitPosition(circle, currentGameMode);
                    // При касании боковых линий очко получает вратарь
                    if (goalkeeperController == 1) {
                        player1Score++;
                    }
                    else {
                        player2Score++;
                    }
                }

                // Обработка столкновений со стенами
                HandleWallCollision(circle);

                // Обработка столкновений с воротами (мяч отскакивает от штанг)
                HandleGoalCollision(circle, goal);

                // В режиме пенальти, если мяч почти остановился, возвращаем его на стартовую позицию
                if (currentGameMode == PENALTY && !circle.canMoveFreely && Vector2Length(circle.velocity) < 10.0f) {
                    ResetCircle(circle, currentGameMode);
                }
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
        }

        BeginDrawing();
        ClearBackground(DARKGREEN);

        switch (gameState) {
        case MENU:
            DrawMainMenu(playButton, twoPlayersButton, shopButton, collectionButton, exitButton);
            break;

        case TWO_PLAYERS_MENU:
            DrawTwoPlayersMenu(player1KeeperButton, player2KeeperButton, backButton);
            break;

        case GAME_MODE_SELECTION:
            DrawGameModeSelection(freeKickButton, penaltyButton, modeBackButton);
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

            // Отображаем режим игры
            DrawGameModeInfo(currentGameMode);

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

        case TWO_PLAYERS_GAME:
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

            // Отображаем режим игры
            DrawGameModeInfo(currentGameMode);

            // Отображаем счет двух игроков
            DrawTwoPlayersScore(player1Score, player2Score);

            // Отображаем управление
            DrawText("PLAYER 1 (Shooter):", 10, MAX_HEIGHT - 160, 15, BLUE);
            DrawText("WASD - Spin, Mouse - Aim/Shoot", 10, MAX_HEIGHT - 140, 15, BLUE);

            if (goalkeeperController == 1) {
                DrawText("PLAYER 1 (Goalkeeper):", 10, MAX_HEIGHT - 110, 15, BLUE);
                DrawText("Arrows - Move, SPACE - Jump", 10, MAX_HEIGHT - 90, 15, BLUE);
            }
            else {
                DrawText("PLAYER 2 (Goalkeeper):", MAX_WIDTH - 200, MAX_HEIGHT - 110, 15, RED);
                DrawText("Arrows - Move, SPACE - Jump", MAX_WIDTH - 200, MAX_HEIGHT - 90, 15, RED);
            }

            DrawText("H - Back to Menu", 10, MAX_HEIGHT - 40, 15, WHITE);
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
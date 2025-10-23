#include "Game.h"


int GenerateCircles(std::vector<Circle>& circles, int count = 1, float radius = 15.0f,
    int minVelocity = 0, int maxVelocity = 0, GameMode mode = FREE_KICK) {
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

Game::Game() {
    InitWindow(MAX_WIDTH, MAX_HEIGHT, "Football Game");
    SetTargetFPS(60);

    // Инициализация объектов
    goal = CreateGoal(200.0f, 100.0f, { MAX_WIDTH / 2.0f, 150.0f });
    goalkeeper = CreateGoalkeeper(80.0f, 60.0f, { MAX_WIDTH / 2.0f, goal.position.y + 20.0f }, "goalkeeper.png");

    // Инициализация UI
    playButton = CreateButton(MAX_WIDTH / 2 - 100, 200, 200, 50, "1 PLAYER", BLUE, WHITE);
    twoPlayersButton = CreateButton(MAX_WIDTH / 2 - 100, 270, 200, 50, "2 PLAYERS", GREEN, WHITE);
    shopButton = CreateButton(MAX_WIDTH / 2 - 100, 340, 200, 50, "SHOP", PURPLE, WHITE);
    collectionButton = CreateButton(MAX_WIDTH / 2 - 100, 410, 200, 50, "COLLECTION", ORANGE, WHITE);
    exitButton = CreateButton(MAX_WIDTH / 2 - 100, 480, 200, 50, "EXIT", RED, WHITE);

    player1KeeperButton = CreateButton(MAX_WIDTH / 2 - 150, 200, 300, 50, "PLAYER 1 - KEEPER", BLUE, WHITE);
    player2KeeperButton = CreateButton(MAX_WIDTH / 2 - 150, 270, 300, 50, "PLAYER 2 - KEEPER", RED, WHITE);
    backButton = CreateButton(MAX_WIDTH / 2 - 150, 340, 300, 50, "BACK", GRAY, WHITE);

    freeKickButton = CreateButton(MAX_WIDTH / 2 - 150, 350, 300, 50, "FREE KICK", GREEN, WHITE);
    penaltyButton = CreateButton(MAX_WIDTH / 2 - 150, 420, 300, 50, "PENALTY", YELLOW, WHITE);
    modeBackButton = CreateButton(MAX_WIDTH / 2 - 150, 490, 300, 50, "BACK", GRAY, WHITE);

    // Загрузка данных
    saveSystem.LoadProgress();
    LoadFootballers();

    // Выбор первого разблокированного игрока
    for (int i = 0; i < footballers.size(); i++) {
        if (footballers[i].unlocked) {
            selectedPlayerIndex = i;
            break;
        }
    }
}

void Game::Run() {
    while (!WindowShouldClose()) {
        float rawDeltaTime = GetFrameTime();
        float deltaTime = slowMoActive ? rawDeltaTime * slowMoFactor : rawDeltaTime;

        HandleInput();
        Update(deltaTime);
        Render();
    }

    saveSystem.SaveProgress();
    CloseWindow();
}

void Game::Update(float deltaTime) {
    animationSystem.UpdateAnimations(deltaTime); // Исправлено: правильный вызов

    switch (gameState) {
    case PLAYING:
    case TWO_PLAYERS_GAME:
        UpdateGoalkeeper(goalkeeper, deltaTime);

        // Обработка столкновений между кругами
        std::vector<std::pair<Circle*, Circle*>> collisions;
        for (int i = 0; i < circles.size() - 1; ++i) {
            for (int j = i + 1; j < circles.size(); ++j) {
                HandleCollision(collisions, circles[i], circles[j]);
            }
        }
        for (auto& collision : collisions) {
            DynamicCollisionResolution(*collision.first, *collision.second);
        }

        for (auto& circle : circles) {
            UpdateSpin(circle, deltaTime);
            circle.accelerate = Vector2Scale(circle.velocity, damping);
            circle.velocity = Vector2Add(circle.velocity, circle.accelerate);
            circle.position = Vector2Add(circle.position, circle.velocity);

            // Обработка столкновений и игровой логики
            HandleGoalkeeperCollision(circle, goalkeeper);
            HandleWallCollision(circle);
            HandleGoalCollision(circle, goal);

            if (IsCircleOutOfBounds(circle)) {
                ResetCircle(circle, currentGameMode);
            }

            if (CheckGoalLineCrossing(circle, goal)) {
                ResetCircle(circle, currentGameMode);
            }

            if (CheckGreenLineTouch(circle, goal)) {
                score++;
                saveSystem.AddCoins(1);
                animationSystem.StartGoalAnimation(); // Исправлено: правильный вызов
                ResetCircle(circle, currentGameMode);
            }

            if (CheckSideLinesCollision(circle, goal)) {
                ResetToHitPosition(circle, currentGameMode);
            }
        }
        break;
    }
}

void Game::Render() {
    BeginDrawing();
    ClearBackground(DARKGREEN);

    switch (gameState) {
    case MENU:
        DrawMainMenu(playButton, twoPlayersButton, shopButton, collectionButton, exitButton, saveSystem.GetCoins());
        break;

    case PLAYING:
        // Отрисовка игрового поля
        DrawGameField(goal);
        DrawGoalkeeper(goalkeeper);

        for (const auto& circle : circles) {
            DrawCircleV(circle.position, circle.radius, WHITE);
        }

        if (dragging && selectedCircle != nullptr) {
            DrawPowerBar(*selectedCircle, GetMousePosition(), dragging, spinActive);
            DrawDirectionArrow(*selectedCircle, GetMousePosition(), dragging, spinActive);
        }

        animationSystem.DrawAnimations(); // Исправлено: правильный вызов
        DrawGameHUD(score, saveSystem.GetCoins(), currentGameMode, slowMoActive, spinActive, selectedPlayerIndex);
        break;

    case TWO_PLAYERS_GAME:
        // Аналогично PLAYING, но с другим HUD
        DrawGameField(goal);
        DrawGoalkeeper(goalkeeper);

        for (const auto& circle : circles) {
            DrawCircleV(circle.position, circle.radius, WHITE);
        }

        if (dragging && selectedCircle != nullptr) {
            DrawPowerBar(*selectedCircle, GetMousePosition(), dragging, spinActive);
            DrawDirectionArrow(*selectedCircle, GetMousePosition(), dragging, spinActive);
        }

        animationSystem.DrawAnimations();
        // Здесь будет другой HUD для двух игроков
        break;

        // Другие состояния...
    case GAME_MODE_SELECTION:
        // Отрисовка выбора режима игры
        
        break;

    case SELECT_PLAYER:
        // Отрисовка выбора игрока
        break;

    case SHOP:
        // Отрисовка магазина
        break;

    case COLLECTION:
        // Отрисовка коллекции
        break;

    case TWO_PLAYERS_MENU:
        // Отрисовка меню двух игроков
        break;
    }

    EndDrawing();
}

void Game::HandleInput() {
    Vector2 mousePosition = GetMousePosition();

    if (IsKeyPressed(KEY_H)) {
        // Обработка возврата в меню
        if (gameState == PLAYING || gameState == TWO_PLAYERS_GAME) {
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
        else {
            gameState = MENU;
        }
        slowMoActive = false;
    }

    // Обработка сброса прогресса (для тестирования)
    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_R)) {
        saveSystem.ResetProgress();
        saveSystem.LoadProgress();
    }

    switch (gameState) {
    case MENU:
        if (IsButtonClicked(playButton)) {
            gameState = SELECT_PLAYER;
            isSinglePlayer = true;
            score = 0;
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
            collectionPage = 0;
        }
        if (IsButtonClicked(exitButton)) {
            saveSystem.SaveProgress();
            CloseWindow();
        }
        break;

    case GAME_MODE_SELECTION:
        if (IsButtonClicked(freeKickButton)) {
            currentGameMode = FREE_KICK;
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
                gameState = SELECT_PLAYER;
            }
            else {
                gameState = TWO_PLAYERS_MENU;
            }
        }
        break;

    case PLAYING:
        // Обработка ввода для игры
        spinActive[0] = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
        spinActive[1] = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
        spinActive[2] = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
        spinActive[3] = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);

        if (IsKeyPressed(KEY_SPACE)) {
            slowMoActive = !slowMoActive;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (auto& circle : circles) {
                if (currentGameMode == PENALTY && !circle.canMoveFreely) continue;

                if (CheckCollisionPointCircle(mousePosition, circle.position, circle.radius)) {
                    dragging = true;
                    selectedCircle = &circle;
                    dragStartPosition = mousePosition;
                    circle.hitPosition = circle.position;
                    break;
                }
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && selectedCircle != nullptr) {
            Vector2 direction = Vector2Subtract(selectedCircle->position, mousePosition);
            float distance = Vector2Length(direction);
            float maxDistance = 150.0f;

            if (distance > maxDistance) {
                distance = maxDistance;
                direction = Vector2Scale(Vector2Normalize(direction), maxDistance);
            }

            Vector2 acceleration = Vector2Scale(Vector2Normalize(direction), distance * 0.25f);
            selectedCircle->velocity = acceleration;

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

            if (goalkeeper.playerControlled == 0) {
                MakeGoalkeeperJump(goalkeeper);
            }

            if (currentGameMode == PENALTY) {
                selectedCircle->canMoveFreely = false;
            }

            dragging = false;
            selectedCircle = nullptr;
        }
        break;

        // Другие состояния...
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
    }
}

void Game::ResetGame() {
    circles.clear();
    GenerateCircles(circles, 1, 15.0f, 0, 0, currentGameMode);
    score = 0;
    player1Score = 0;
    player2Score = 0;
}

void Game::SwitchToGameModeSelection(bool singlePlayer) {
    isSinglePlayer = singlePlayer;
    gameState = GAME_MODE_SELECTION;
}
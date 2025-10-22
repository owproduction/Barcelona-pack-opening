#include "UI.h"

Button CreateButton(float x, float y, float width, float height, const char* text, Color color, Color textColor) {
    Button button;
    button.bounds = { x, y, width, height };
    button.text = text;
    button.color = color;
    button.textColor = textColor;
    return button;
}

bool IsButtonClicked(Button button) {
    return CheckCollisionPointRec(GetMousePosition(), button.bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void DrawButton(Button button) {
    DrawRectangleRec(button.bounds, button.color);
    DrawRectangleLinesEx(button.bounds, 2, WHITE);

    int textWidth = MeasureText(button.text, 20);
    int textHeight = 20;
    DrawText(button.text,
        button.bounds.x + button.bounds.width / 2 - textWidth / 2,
        button.bounds.y + button.bounds.height / 2 - textHeight / 2,
        20, button.textColor);
}

void DrawMainMenu(Button playButton, Button twoPlayersButton, Button shopButton,
    Button collectionButton, Button exitButton, int coins) {
    DrawText("FOOTBALL GAME", MAX_WIDTH / 2 - MeasureText("FOOTBALL GAME", 40) / 2, 100, 40, WHITE);
    DrawText(TextFormat("Coins: %d", coins), MAX_WIDTH / 2 - MeasureText(TextFormat("Coins: %d", coins), 30) / 2, 160, 30, GOLD);

    DrawButton(playButton);
    DrawButton(twoPlayersButton);
    DrawButton(shopButton);
    DrawButton(collectionButton);
    DrawButton(exitButton);
}

void DrawGameHUD(int score, int coins, int gameMode, bool slowMoActive, bool spinActive[4], int selectedPlayerIndex) {
    // Отображение счета
    DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
    DrawText(TextFormat("Coins: %d", coins), 10, 40, 20, GOLD);

    // Отображение режима игры
    const char* modeText = (gameMode == 0) ? "FREE KICK" : "PENALTY";
    Color modeColor = (gameMode == 0) ? GREEN : YELLOW;
    DrawText(TextFormat("MODE: %s", modeText), MAX_WIDTH / 2 - MeasureText(TextFormat("MODE: %s", modeText), 20) / 2, 70, 20, modeColor);

    // Отображение выбранного игрока
    if (selectedPlayerIndex >= 0 && selectedPlayerIndex < footballers.size()) {
        DrawText(TextFormat("PLAYER: %s", footballers[selectedPlayerIndex].name.c_str()),
            MAX_WIDTH / 2 - MeasureText(TextFormat("PLAYER: %s", footballers[selectedPlayerIndex].name.c_str()), 20) / 2,
            40, 20, GREEN);
    }

    // Подсказки управления
    DrawText("A/LEFT - Left Spin", 10, MAX_HEIGHT - 140, 15, spinActive[0] ? BLUE : WHITE);
    DrawText("D/RIGHT - Right Spin", 10, MAX_HEIGHT - 120, 15, spinActive[1] ? RED : WHITE);
    DrawText("W/UP - Top Spin", 10, MAX_HEIGHT - 100, 15, spinActive[2] ? GREEN : WHITE);
    DrawText("S/DOWN - Back Spin", 10, MAX_HEIGHT - 80, 15, spinActive[3] ? YELLOW : WHITE);
    DrawText("SPACE - Slow Motion", 10, MAX_HEIGHT - 60, 15, slowMoActive ? ORANGE : WHITE);
    DrawText("H - Menu", 10, MAX_HEIGHT - 40, 15, WHITE);

    if (slowMoActive) {
        DrawText("SLOW MOTION", MAX_WIDTH - 150, 40, 20, ORANGE);
    }
}

void DrawPowerBar(const Circle& circle, const Vector2& mousePosition, bool isDragging, bool spinActive[4]) {
    if (isDragging) {
        float distance = Vector2Distance(circle.position, mousePosition);
        float maxDistance = 150.0f;
        float power = (distance > maxDistance) ? 1.0f : distance / maxDistance;

        Vector2 barPosition = { circle.position.x - 50, circle.position.y - 40 };
        float barWidth = 100.0f;
        float barHeight = 10.0f;

        DrawRectangleRec({ barPosition.x, barPosition.y, barWidth, barHeight }, GRAY);

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

        DrawRectangleRec({ barPosition.x, barPosition.y, barWidth * power, barHeight }, fillColor);
        DrawRectangleLines(barPosition.x, barPosition.y, barWidth, barHeight, WHITE);
        DrawText(shotType.c_str(), barPosition.x, barPosition.y - 20, 15, fillColor);
    }
}

void DrawDirectionArrow(const Circle& circle, const Vector2& mousePosition, bool isDragging, bool spinActive[4]) {
    if (isDragging) {
        Vector2 direction = Vector2Subtract(mousePosition, circle.position);
        direction = Vector2Normalize(direction);

        Vector2 arrowPos = {
            circle.position.x + direction.x * (circle.radius + 20),
            circle.position.y + direction.y * (circle.radius + 20)
        };

        float distance = Vector2Distance(circle.position, mousePosition);
        float arrowLength = 15.0f + (distance / 150.0f) * 10.0f;

        Color arrowColor = WHITE;
        int activeSpins = 0;
        for (int i = 0; i < 4; i++) {
            if (spinActive[i]) activeSpins++;
        }

        if (activeSpins == 1) {
            if (spinActive[0]) arrowColor = BLUE;
            else if (spinActive[1]) arrowColor = RED;
            else if (spinActive[2]) arrowColor = GREEN;
            else if (spinActive[3]) arrowColor = YELLOW;
        }
        else if (activeSpins > 1) {
            arrowColor = PURPLE;
        }

        Vector2 arrowEnd = {
            arrowPos.x + direction.x * arrowLength,
            arrowPos.y + direction.y * arrowLength
        };

        DrawLineEx(arrowPos, arrowEnd, 3, arrowColor);

        Vector2 perp = { -direction.y, direction.x };
        Vector2 arrowTip1 = {
            arrowEnd.x - direction.x * 8 + perp.x * 4,
            arrowEnd.y - direction.y * 8 + perp.y * 4
        };
        Vector2 arrowTip2 = {
            arrowEnd.x - direction.x * 8 - perp.x * 4,
            arrowEnd.y - direction.y * 8 - perp.y * 4
        };

        DrawLineEx(arrowEnd, arrowTip1, 3, arrowColor);
        DrawLineEx(arrowEnd, arrowTip2, 3, arrowColor);
    }
}

void DrawGameField(const Goal& goal) {
    float postBottomY = goal.position.y + goal.height / 2;
    float leftPostX = goal.position.x - goal.width / 2;
    float rightPostX = goal.position.x + goal.width / 2;

    // Зеленая линия для гола
    float greenLineY = postBottomY - 50.0f;
    DrawLine(leftPostX, greenLineY, rightPostX, greenLineY, GREEN);

    // Желтая линия
    DrawLine(leftPostX, postBottomY, rightPostX, postBottomY, WHITE);

    // Боковые линии
    DrawLine(0, postBottomY, leftPostX, postBottomY, WHITE);
    DrawLine(rightPostX, postBottomY, MAX_WIDTH, postBottomY, WHITE);

    // Ворота
    DrawRectangleRec(goal.leftPost, WHITE);
    DrawRectangleRec(goal.rightPost, WHITE);
    DrawRectangleRec(goal.crossbar, WHITE);
}
#include "UI.h"

bool IsButtonClicked(Button button) {
    return CheckCollisionPointRec(GetMousePosition(), button.bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void DrawButton(Button button) {
    DrawRectangleRec(button.bounds, button.color);
    DrawRectangleLinesEx(button.bounds, 2, WHITE);

    // ������������� ������
    int textWidth = MeasureText(button.text, 20);
    int textHeight = 20;
    DrawText(button.text,
        button.bounds.x + button.bounds.width / 2 - textWidth / 2,
        button.bounds.y + button.bounds.height / 2 - textHeight / 2,
        20, button.textColor);
}

void DrawGoal(const Goal& goal) {
    // ������ ����� ������
    DrawRectangleRec(goal.leftPost, WHITE);
    // ������ ������ ������
    DrawRectangleRec(goal.rightPost, WHITE);
    // ������ �����������
    DrawRectangleRec(goal.crossbar, WHITE);
}

void DrawGoalkeeper(const Goalkeeper& keeper) {
    if (keeper.texture.id != 0) {
        // ������ �������� �������
        Rectangle sourceRec = { 0, 0, (float)keeper.texture.width, (float)keeper.texture.height };
        Rectangle destRec = { keeper.position.x, keeper.position.y, keeper.width, keeper.height };
        Vector2 origin = { keeper.width / 2, keeper.height / 2 };
        DrawTexturePro(keeper.texture, sourceRec, destRec, origin, 0, WHITE);
    }
    else {
        // ��������� ������� - ��������� �������������
        DrawRectangleRec(keeper.bounds, ORANGE);
    }
}

void DrawPowerBar(const Circle& circle, const Vector2& mousePosition, bool isDragging, bool spinActive[4]) {
    if (isDragging) {
        // ��������� ���������� ����� ����� � ��������
        float distance = Vector2Distance(circle.position, mousePosition);

        // ������������ ������������ ��������� ��� �������� ����
        float maxDistance = 150.0f;
        float power = (distance > maxDistance) ? 1.0f : distance / maxDistance;

        // ������� ����� (��� �����)
        Vector2 barPosition = { circle.position.x - 50, circle.position.y - 40 };
        float barWidth = 100.0f;
        float barHeight = 10.0f;

        // ������ ��� �����
        DrawRectangleRec({ barPosition.x, barPosition.y, barWidth, barHeight }, GRAY);

        // ���������� ���� � ��� ����� �� ������ �������� ��������
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

        // ������ ���������� ����� (���� ������� �� ���� �����)
        DrawRectangleRec({ barPosition.x, barPosition.y, barWidth * power, barHeight }, fillColor);

        // ������ ������ �����
        DrawRectangleLines(barPosition.x, barPosition.y, barWidth, barHeight, WHITE);

        // ���������� ��� �����
        DrawText(shotType.c_str(), barPosition.x, barPosition.y - 20, 15, fillColor);
    }
}

void DrawArrow(const Arrow& arrow) {
    if (!arrow.visible || arrow.length <= 0) return;

    // ������ ����� �������
    Vector2 endPoint = {
        arrow.position.x + cosf(arrow.angle) * arrow.length,
        arrow.position.y + sinf(arrow.angle) * arrow.length
    };

    // �������� ����� �������
    DrawLineEx(arrow.position, endPoint, 3, arrow.color);

    // ���������� ������� (�����������)
    float arrowHeadLength = 15.0f;
    float arrowHeadAngle = 30.0f * DEG2RAD;

    Vector2 direction = Vector2Normalize(Vector2Subtract(endPoint, arrow.position));

    // ����� ����� �����������
    Vector2 leftHead = {
        endPoint.x - arrowHeadLength * cosf(arrow.angle - arrowHeadAngle),
        endPoint.y - arrowHeadLength * sinf(arrow.angle - arrowHeadAngle)
    };

    // ������ ����� �����������
    Vector2 rightHead = {
        endPoint.x - arrowHeadLength * cosf(arrow.angle + arrowHeadAngle),
        endPoint.y - arrowHeadLength * sinf(arrow.angle + arrowHeadAngle)
    };

    // ������ ����������
    DrawTriangle(endPoint, leftHead, rightHead, arrow.color);

    // ������� ��� ������ ���������
    DrawLineEx(endPoint, leftHead, 2, Fade(BLACK, 0.5f));
    DrawLineEx(endPoint, rightHead, 2, Fade(BLACK, 0.5f));
}

void DrawPackAnimationScreen(Texture2D playerTexture, const std::string& playerName, bool showSkipButton) {
    // ������� ���
    DrawRectangle(0, 0, MAX_WIDTH, MAX_HEIGHT, GREEN);

    // ����� ��� ��������
    Rectangle cardRect = { MAX_WIDTH / 2 - 150, MAX_HEIGHT / 2 - 200, 300, 400 };
    DrawRectangleRec(cardRect, WHITE);
    DrawRectangleLinesEx(cardRect, 5, GOLD);

    // �������� ������
    if (playerTexture.id != 0) {
        Rectangle playerRect = { cardRect.x + 50, cardRect.y + 50, 200, 250 };
        DrawTexturePro(playerTexture,
            { 0, 0, (float)playerTexture.width, (float)playerTexture.height },
            playerRect, { 0, 0 }, 0, WHITE);
    }

    // ��� ������
    DrawText(playerName.c_str(),
        cardRect.x + cardRect.width / 2 - MeasureText(playerName.c_str(), 30) / 2,
        cardRect.y + 320, 30, DARKGREEN);

    // ����� "NEW PLAYER!"
    const char* newText = "NEW PLAYER!";
    DrawText(newText,
        cardRect.x + cardRect.width / 2 - MeasureText(newText, 25) / 2,
        cardRect.y + 360, 25, GOLD);

    // ������ "SKIP" ���� �����
    if (showSkipButton) {
        Rectangle skipButton = { MAX_WIDTH / 2 - 60, MAX_HEIGHT - 80, 120, 40 };
        DrawRectangleRec(skipButton, RED);
        DrawRectangleLinesEx(skipButton, 2, WHITE);
        DrawText("SKIP", skipButton.x + skipButton.width / 2 - MeasureText("SKIP", 20) / 2,
            skipButton.y + 10, 20, WHITE);
    }

    // ����������
    DrawText("Press SPACE to continue", MAX_WIDTH / 2 - MeasureText("Press SPACE to continue", 20) / 2,
        MAX_HEIGHT - 40, 20, WHITE);
}

void DrawMainMenu(Button playButton, Button twoPlayersButton, Button shopButton, Button collectionButton, Button exitButton) {
    // ���������
    DrawText("FOOTBALL GAME", MAX_WIDTH / 2 - MeasureText("FOOTBALL GAME", 40) / 2, 100, 40, WHITE);

    // ���������� ���������� �������
    DrawText(TextFormat("Coins: %d", coins), MAX_WIDTH / 2 - MeasureText(TextFormat("Coins: %d", coins), 30) / 2, 160, 30, GOLD);

    // ������
    DrawButton(playButton);
    DrawButton(twoPlayersButton);
    DrawButton(shopButton);
    DrawButton(collectionButton);
    DrawButton(exitButton);
}

void DrawTwoPlayersMenu(Button player1KeeperButton, Button player2KeeperButton, Button backButton) {
    DrawText("2 PLAYERS MODE", MAX_WIDTH / 2 - MeasureText("2 PLAYERS MODE", 40) / 2, 100, 40, WHITE);
    DrawText("Choose who controls goalkeeper:", MAX_WIDTH / 2 - MeasureText("Choose who controls goalkeeper:", 25) / 2, 160, 25, WHITE);

    DrawButton(player1KeeperButton);
    DrawButton(player2KeeperButton);
    DrawButton(backButton);
}

void DrawGameModeSelection(Button freeKickButton, Button penaltyButton, Button backButton) {
    DrawText("SELECT GAME MODE", MAX_WIDTH / 2 - MeasureText("SELECT GAME MODE", 40) / 2, 100, 40, WHITE);

    DrawText("FREE KICK:", MAX_WIDTH / 2 - MeasureText("FREE KICK:", 25) / 2, 170, 25, GREEN);
    DrawText("- Move ball anywhere", MAX_WIDTH / 2 - MeasureText("- Move ball anywhere", 20) / 2, 200, 20, WHITE);
    DrawText("- Multiple shots allowed", MAX_WIDTH / 2 - MeasureText("- Multiple shots allowed", 20) / 2, 225, 20, WHITE);

    DrawText("PENALTY:", MAX_WIDTH / 2 - MeasureText("PENALTY:", 25) / 2, 270, 25, YELLOW);
    DrawText("- Fixed ball position", MAX_WIDTH / 2 - MeasureText("- Fixed ball position", 20) / 2, 300, 20, WHITE);
    DrawText("- One shot per round", MAX_WIDTH / 2 - MeasureText("- One shot per round", 20) / 2, 325, 20, WHITE);

    DrawButton(freeKickButton);
    DrawButton(penaltyButton);
    DrawButton(backButton);
}

void DrawShop() {
    DrawText("SHOP", MAX_WIDTH / 2 - MeasureText("SHOP", 40) / 2, 50, 40, WHITE);
    DrawText(TextFormat("Coins: %d", coins), MAX_WIDTH / 2 - MeasureText(TextFormat("Coins: %d", coins), 30) / 2, 100, 30, GOLD);

    // ���������� ���
    if (packTexture.id != 0) {
        Rectangle packRect = { MAX_WIDTH / 2 - 100, 150, 200, 200 };
        DrawTexturePro(packTexture, { 0, 0, (float)packTexture.width, (float)packTexture.height },
            packRect, { 0, 0 }, 0, WHITE);

        // ���� ����
        DrawText("10 coins", MAX_WIDTH / 2 - MeasureText("10 coins", 25) / 2, 370, 25, GOLD);

        // ������ �������
        if (coins >= 10) {
            DrawText("CLICK TO BUY", MAX_WIDTH / 2 - MeasureText("CLICK TO BUY", 20) / 2, 400, 20, GREEN);
        }
        else {
            DrawText("NOT ENOUGH COINS", MAX_WIDTH / 2 - MeasureText("NOT ENOUGH COINS", 20) / 2, 400, 20, RED);
        }

        // ���������� � ���������
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

void DrawCollection() {
    DrawText("COLLECTION", MAX_WIDTH / 2 - MeasureText("COLLECTION", 40) / 2, 50, 40, WHITE);

    int unlockedCount = 0;
    for (const auto& footballer : footballers) {
        if (footballer.unlocked) unlockedCount++;
    }

    DrawText(TextFormat("Unlocked: %d/%d", unlockedCount, footballers.size()),
        MAX_WIDTH / 2 - MeasureText(TextFormat("Unlocked: %d/%d", unlockedCount, footballers.size()), 25) / 2,
        100, 25, WHITE);

    // ���������� ����� ��������
    int totalPages = (footballers.size() + playersPerPage - 1) / playersPerPage;
    DrawText(TextFormat("Page %d/%d", collectionPage + 1, totalPages),
        MAX_WIDTH / 2 - MeasureText(TextFormat("Page %d/%d", collectionPage + 1, totalPages), 20) / 2,
        130, 20, WHITE);

    // ���������� ����������� � �������� ����������
    float startX = 50;
    float startY = 170;
    float cardWidth = 160;  // ������� ������ ��������
    float cardHeight = 200; // ������� ������ ��������
    float spacingX = 180;   // ���������� ����� ���������� �� X
    float spacingY = 220;   // ���������� ����� ���������� �� Y

    int startIndex = collectionPage * playersPerPage;
    int endIndex = startIndex + playersPerPage;
    if (endIndex > footballers.size()) endIndex = footballers.size();

    for (int i = startIndex; i < endIndex; i++) {
        int indexOnPage = i - startIndex;
        float x = startX + (indexOnPage % 3) * spacingX;
        float y = startY + (indexOnPage / 3) * spacingY;

        if (footballers[i].unlocked) {
            // ���������� ����������������� ����������
            if (footballers[i].texture.id != 0) {
                Rectangle playerRect = { x, y, cardWidth, cardHeight - 40 };
                DrawTexturePro(footballers[i].texture, { 0, 0, (float)footballers[i].texture.width, (float)footballers[i].texture.height },
                    playerRect, { 0, 0 }, 0, WHITE);

                // ����� ������ ��������
                DrawRectangleLinesEx(playerRect, 2, GREEN);
            }

            // ��� ���������� ��� ���������
            DrawText(footballers[i].name.c_str(),
                x + cardWidth / 2 - MeasureText(footballers[i].name.c_str(), 20) / 2,
                y + cardHeight - 35, 20, GREEN);
        }
        else {
            // ���������� ���������������� ����������
            Rectangle cardRect = { x, y, cardWidth, cardHeight - 40 };
            DrawRectangleRec(cardRect, DARKGRAY);
            DrawRectangleLinesEx(cardRect, 2, GRAY);

            DrawText("???", x + cardWidth / 2 - MeasureText("???", 25) / 2, y + (cardHeight - 40) / 2 - 12, 25, WHITE);

            // ����� "LOCKED" ��� ���������
            DrawText("LOCKED",
                x + cardWidth / 2 - MeasureText("LOCKED", 18) / 2,
                y + cardHeight - 35, 18, GRAY);
        }
    }

    // ������ �������������� �������
    if (collectionPage > 0) {
        DrawText("<< PREV", 20, MAX_HEIGHT - 40, 20, WHITE);
    }
    if ((collectionPage + 1) * playersPerPage < footballers.size()) {
        DrawText("NEXT >>", MAX_WIDTH - 100, MAX_HEIGHT - 40, 20, WHITE);
    }

    DrawText("H - Back to Menu", 10, MAX_HEIGHT - 70, 20, WHITE);
}

void DrawTwoPlayersScore(int player1Score, int player2Score) {
    DrawText(TextFormat("PLAYER 1: %d", player1Score), 10, 10, 20, BLUE);
    DrawText(TextFormat("PLAYER 2: %d", player2Score), MAX_WIDTH - 150, 10, 20, RED);
}

void DrawGameModeInfo(GameMode mode) {
    const char* modeText = (mode == FREE_KICK) ? "FREE KICK" : "PENALTY";
    Color modeColor = (mode == FREE_KICK) ? GREEN : YELLOW;

    DrawText(TextFormat("MODE: %s", modeText), MAX_WIDTH / 2 - MeasureText(TextFormat("MODE: %s", modeText), 20) / 2, 70, 20, modeColor);
}
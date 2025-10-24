#include "Animation.h"

// Анимации
Animation goalAnimation = { false };
PackAnimation packAnimation = { false };

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

void StartPackAnimation(Texture2D playerTexture, const std::string& playerName) {
    packAnimation.active = true;
    packAnimation.playerTexture = playerTexture;
    packAnimation.playerName = playerName;
    packAnimation.timer = 5.0f; // 5 секунд на просмотр карточки
    packAnimation.showSkipButton = false;
}

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

    // Обновление анимации пака
    if (packAnimation.active) {
        packAnimation.timer -= deltaTime;

        // Показываем кнопку SKIP после 1 секунды
        if (packAnimation.timer < 4.0f) {
            packAnimation.showSkipButton = true;
        }

        if (packAnimation.timer <= 0) {
            packAnimation.active = false;
        }

        // Проверка нажатия SPACE для пропуска
        if (IsKeyPressed(KEY_SPACE)) {
            packAnimation.active = false;
        }
    }
}

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
}
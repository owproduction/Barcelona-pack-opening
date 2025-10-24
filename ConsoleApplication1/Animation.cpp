#include "Animation.h"

// Анимации
Animation goalAnimation = { false };
Animation packAnimation = { false };

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
#include "Animation.h"

// ��������
Animation goalAnimation = { false };
PackAnimation packAnimation = { false };

void StartGoalAnimation() {
    goalAnimation.active = true;
    goalAnimation.duration = 2.0f; // 2 �������
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
    packAnimation.timer = 5.0f; // 5 ������ �� �������� ��������
    packAnimation.showSkipButton = false;
}



void DrawAnimations() {
    // ��������� �������� ����
    if (goalAnimation.active) {
        float alpha = goalAnimation.timer / goalAnimation.duration;
        Color textColor = Fade(goalAnimation.color, alpha);

        int fontSize = 60 + (int)(20.0f * (1.0f - goalAnimation.scale));
        const char* text = goalAnimation.text.c_str();
        int textWidth = MeasureText(text, fontSize);

        // ����
        DrawText(text,
            goalAnimation.position.x - textWidth / 2 + 3,
            goalAnimation.position.y - fontSize / 2 + 3,
            fontSize, Fade(BLACK, alpha * 0.5f));

        // �������� �����
        DrawText(text,
            goalAnimation.position.x - textWidth / 2,
            goalAnimation.position.y - fontSize / 2,
            fontSize, textColor);

        // ������ ������
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
    DrawGifAnimation(kickGifAnimation);
}
void UpdateGifAnimation(GifAnimation& anim, float deltaTime) {
    if (!anim.active || anim.gifImage.data == NULL) return;

    anim.frameTimer += deltaTime;

    if (anim.frameTimer >= anim.frameDelay) {
        anim.frameTimer = 0;
        anim.currentFrame++;

        if (anim.currentFrame >= anim.totalFrames) {
            if (anim.loop) {
                anim.currentFrame = 0;
                anim.nextFrameDataOffset = 0;
            }
            else {
                anim.active = false;
                return;
            }
        }

        // Вычисляем смещение для следующего кадра
        anim.nextFrameDataOffset = anim.gifImage.width * anim.gifImage.height * 4 * anim.currentFrame;

        // Обновляем текстуру следующим кадром
        UpdateTexture(anim.texture, ((unsigned char*)anim.gifImage.data) + anim.nextFrameDataOffset);
    }
}

// Глобальная GIF анимация удара
GifAnimation kickGifAnimation = { false };

bool LoadGifAnimation(GifAnimation& anim, const char* gifPath, float frameDelay) {
    // Сначала выгружаем предыдущую анимацию
    UnloadGifAnimation(anim);

    // Загружаем GIF анимацию
    anim.gifImage = LoadImageAnim(gifPath, &anim.totalFrames);

    if (anim.gifImage.data != NULL) {
        // Создаем текстуру из первого кадра
        anim.texture = LoadTextureFromImage(anim.gifImage);

        anim.frameDelay = frameDelay;
        anim.frameTimer = 0;
        anim.currentFrame = 0;
        anim.nextFrameDataOffset = 0;
        anim.scale = 1.0f;
        anim.active = false;

        TraceLog(LOG_INFO, "GIF loaded: %s, frames: %d, size: %dx%d",
            gifPath, anim.totalFrames, anim.gifImage.width, anim.gifImage.height);
        return true;
    }
    else {
        TraceLog(LOG_WARNING, "Failed to load GIF: %s", gifPath);
        return false;
    }
}

void StartGifAnimation(GifAnimation& anim, float posX, float posY, bool loop) {
    if (anim.gifImage.data == NULL) return;

    anim.active = true;
    anim.position.x = posX;
    anim.position.y = posY;
    anim.currentFrame = 0;
    anim.frameTimer = 0;
    anim.loop = loop;
    anim.nextFrameDataOffset = 0;

    // Сбрасываем текстуру к первому кадру
    UpdateTexture(anim.texture, (unsigned char*)anim.gifImage.data);
}

void UpdateAnimations(float deltaTime) {
    // Обновление анимации гола
    if (goalAnimation.active) {
        goalAnimation.timer -= deltaTime;
        goalAnimation.scale = 1.0f + sinf(GetTime() * 10.0f) * 0.2f;
        if (goalAnimation.timer <= 0) {
            goalAnimation.active = false;
        }
    }

    // Обновление анимации пака
    if (packAnimation.active) {
        packAnimation.timer -= deltaTime;
        if (packAnimation.timer < 4.0f) {
            packAnimation.showSkipButton = true;
        }
        if (packAnimation.timer <= 0) {
            packAnimation.active = false;
        }
        if (IsKeyPressed(KEY_SPACE)) {
            packAnimation.active = false;
        }
    }

    // ОБНОВЛЯЕМ GIF АНИМАЦИЮ УДАРА
    UpdateGifAnimation(kickGifAnimation, deltaTime);
}


void DrawGifAnimation(const GifAnimation& anim) {
    if (!anim.active || anim.texture.id == 0) return;

    // Рисуем текущий кадр анимации
    Rectangle destRec = {
        anim.position.x,
        anim.position.y,
        anim.texture.width * anim.scale,
        anim.texture.height * anim.scale
    };

    DrawTexturePro(anim.texture,
        { 0, 0, (float)anim.texture.width, (float)anim.texture.height },
        destRec,
        { destRec.width / 2, destRec.height / 2 },
        0.0f, WHITE);
}

void UnloadGifAnimation(GifAnimation& anim) {
    if (anim.texture.id != 0) {
        UnloadTexture(anim.texture);
    }
    if (anim.gifImage.data != NULL) {
        UnloadImage(anim.gifImage);
    }
    anim.active = false;
}

// Вспомогательная функция для удара
void StartKickAnimation(float ballX, float ballY) {
    // Загружаем GIF анимацию (если еще не загружена)
    if (kickGifAnimation.gifImage.data == NULL) {
        if (!LoadGifAnimation(kickGifAnimation, "resources/kick.gif", 0.06f)) {
            return; // Если не удалось загрузить, выходим
        }
        kickGifAnimation.scale = 0.7f;
    }

    StartGifAnimation(kickGifAnimation, ballX, ballY + 40, false);
}
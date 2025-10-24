#include "GameObjects.h"

Button CreateButton(float x, float y, float width, float height, const char* text, Color color, Color textColor) {
    Button button;
    button.bounds = { x, y, width, height };
    button.text = text;
    button.color = color;
    button.textColor = textColor;
    return button;
}

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

Goalkeeper CreateGoalkeeper(float width, float height, Vector2 position, const char* texturePath, int playerControlled) {
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
    keeper.playerControlled = playerControlled;
    keeper.moveSpeed = 300.0f; // Скорость движения для игрока

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

Arrow CreateArrow(Vector2 position) {
    Arrow arrow;
    arrow.position = position;
    arrow.direction = { 0, 0 };
    arrow.length = 0;
    arrow.angle = 0;
    arrow.color = WHITE;
    arrow.visible = false;
    return arrow;
}
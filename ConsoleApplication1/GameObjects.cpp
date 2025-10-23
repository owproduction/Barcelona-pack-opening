#include "GameObjects.h"
#include "Game.h"

std::vector<Footballer> footballers;
int playersPerPage = 6;

void LoadFootballers() {
    footballers.clear();

    // Добавляем 15 футболистов (замени пути на свои PNG файлы)
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

    // Добавляем еще 10 футболистов
    Footballer f6;
    f6.texture = LoadTexture("player6.png");
    f6.name = "Lewandowski";
    f6.unlocked = false;
    footballers.push_back(f6);

    Footballer f7;
    f7.texture = LoadTexture("player7.png");
    f7.name = "Benzema";
    f7.unlocked = false;
    footballers.push_back(f7);

    Footballer f8;
    f8.texture = LoadTexture("player8.png");
    f8.name = "Salah";
    f8.unlocked = false;
    footballers.push_back(f8);

    Footballer f9;
    f9.texture = LoadTexture("player9.png");
    f9.name = "Kane";
    f9.unlocked = false;
    footballers.push_back(f9);

    Footballer f10;
    f10.texture = LoadTexture("player10.png");
    f10.name = "De Bruyne";
    f10.unlocked = false;
    footballers.push_back(f10);

    Footballer f11;
    f11.texture = LoadTexture("player11.png");
    f11.name = "Modric";
    f11.unlocked = false;
    footballers.push_back(f11);

    Footballer f12;
    f12.texture = LoadTexture("player12.png");
    f12.name = "Van Dijk";
    f12.unlocked = false;
    footballers.push_back(f12);

    Footballer f13;
    f13.texture = LoadTexture("player13.png");
    f13.name = "Courtois";
    f13.unlocked = false;
    footballers.push_back(f13);

    Footballer f14;
    f14.texture = LoadTexture("player14.png");
    f14.name = "Son";
    f14.unlocked = false;
    footballers.push_back(f14);

    Footballer f15;
    f15.texture = LoadTexture("player15.png");
    f15.name = "Zlatan";
    f15.unlocked = false;
    footballers.push_back(f15);
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

void DrawGoalkeeper(const Goalkeeper& keeper) {
    if (keeper.texture.id != 0) {
        Rectangle sourceRec = { 0, 0, (float)keeper.texture.width, (float)keeper.texture.height };
        Rectangle destRec = { keeper.position.x, keeper.position.y, keeper.width, keeper.height };
        Vector2 origin = { keeper.width / 2, keeper.height / 2 };
        DrawTexturePro(keeper.texture, sourceRec, destRec, origin, 0, WHITE);
    }
    else {
        DrawRectangleRec(keeper.bounds, ORANGE);
    }
}

void DrawGoal(const Goal& goal) {
    DrawRectangleRec(goal.leftPost, WHITE);
    DrawRectangleRec(goal.rightPost, WHITE);
    DrawRectangleRec(goal.crossbar, WHITE);
}
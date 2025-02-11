#include <iostream>
#include<deque>
#include<string>
#include <raylib.h>
#include <raymath.h>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(linker, "/ENTRY:mainCRTStartup")

using namespace std;

Color green{ 173,204,96,255 };
Color Dark_green{ 43,51,24,255 };


int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;
bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;

}
bool ElementInDeque(Vector2 element, deque<Vector2> deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(deque[i], element)) {
            return true;
        }
    }
    return false;
}


class Snake {
public: 
    deque<Vector2> body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };

    Vector2 direction = { 1,0 };
    bool addSegment = false;

    void Draw() {
        for (unsigned int i = 0; i < body.size(); i++) {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment{ offset + x*cellSize,offset + y*cellSize, (float)cellSize, (float)cellSize };
            DrawRectangleRounded(segment, 0.5, 6, Dark_green);
        }
    }
    void Update() {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true) {
            addSegment = false;
        }
        else {
            body.pop_back();
        }
       
    }
    void Reset() {
        body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
        direction = { 1,0 };
    }


};

class Food {
public:
    Vector2 position;
    Color color = RED;  // Цвет круга

    Food(deque<Vector2> snakeBody) {
        position = GenerateRandomPos(snakeBody);
    }

    void Draw() {
        // Рассчитываем центр круга
        Vector2 center = {
            offset + position.x * cellSize + cellSize / 2.0f,
            offset + position.y * cellSize + cellSize / 2.0f
        };
        DrawCircleV(center, cellSize / 2.0f, color);
    }

    Vector2 GenerateRandomCell() {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{ x,y };
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody) {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody)) {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game {
public: 
    Snake snake{};
    Food food{ snake.body };
    bool running = true;
    int score = 0;
    void Draw() {
        food.Draw();
        snake.Draw();
    }
    void Update() {
        if (running) {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
        
    }
    void CheckCollisionWithFood() {
        if (Vector2Equals(snake.body[0], food.position)) {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
        }
    }
    void CheckCollisionWithEdges() 
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1) {
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1) {
            GameOver();
        }
    }
    void GameOver() {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
    }
    void CheckCollisionWithTail() {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody)) {
            GameOver();
        }
    }
};

int main()
{
    InitWindow(2* offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Retro Snake");
    
    SetTargetFPS(60);
    Game game{};
  
    while (!WindowShouldClose()) 
    {  
        
        
        BeginDrawing();
        if(eventTriggered(0.1)) game.Update();
        
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
            game.snake.direction = { 0, -1 };
            
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
            game.snake.direction = { 0, 1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
            game.snake.direction = { -1, 0 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
            game.snake.direction = { 1, 0 };
            game.running = true;
        }

        game.Draw();
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset - 5, (float)cellSize*cellCount+10, (float)cellSize*cellCount+10}, 5, Dark_green);
        DrawText("Retro Snake", offset - 5, 20, 40, Dark_green);
        DrawText(TextFormat("%i",game.score), offset - 5, offset + cellSize * cellCount + 10, 50, Dark_green);
        EndDrawing();
    }
    CloseWindow();
}
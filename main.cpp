#include <iostream>
#include "raylib.h"
#include <deque>
#include "raymath.h"
using namespace std;

Color cream = {255, 253, 208, 255};
Color purple = {255, 0, 255, 250};
Color midnight = {48, 25, 52, 255};
Color red = {255, 0, 0, 255};
Color darkgreen = {0, 100, 0, 255};
Color black{0, 0, 0, 255};

int cellsize = 30;
int cellcount = 25;
int offset = 75;
double lastupdatetime = 0;

bool eventtriggered(double interval)
{
  double currentime = GetTime();
  if (currentime - lastupdatetime >= interval)
  {
    lastupdatetime = currentime;
    return true;
  }
  return false;
}

bool elementindeque(Vector2 element, deque<Vector2> d)
{
  for (unsigned int i = 0; i < d.size(); i++)
  {
    if (Vector2Equals(element, d[i]))
    {
      return true;
    }
  }
  return false;
}

class apple
{
public:
  Texture2D appleTexture;
  Vector2 position;
  apple(deque<Vector2> snakebody)
  {
    Image apple = LoadImage("apple.png");
    appleTexture = LoadTextureFromImage(apple);
    position = getrandompos(snakebody);
    UnloadImage(apple);
  }
  ~apple()
  {
    UnloadTexture(appleTexture);
  }
  Vector2 generaterandomcell()
  {
    float x = GetRandomValue(0, cellcount - 1);
    float y = GetRandomValue(0, cellcount - 1);
    return Vector2{x, y};
  }
  Vector2 getrandompos(deque<Vector2> snakebody)
  {
    Vector2 pos = generaterandomcell();
    while (elementindeque(pos, snakebody))
    {
      pos = generaterandomcell();
    }
    return pos;
  }
  // once an image is loaded it can be used to create a texture which is a
  // gpu recourse that is used for rendering image on screen. The image can
  // be unloaded after the texture is created.
  //  we can create a texture from the image using the LoadTextureFromImage() function
  // it takes image as an argument and returns a texture2d struct which can be used to
  // draw the image on screen.
  // texture 2d is an optimized data type for gpu processing and fast rendering
  void draw()
  {
    DrawTexture(appleTexture, offset + position.x * cellsize, offset + position.y * cellsize, WHITE);
  }
  // When you write Vector2 position = {9, 10};, Raylib automatically assigns
  // 9 to position.x and 10 to position.y
};

class snake
{
public:
  deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
  Vector2 direction = {1, 0};
  bool addsegment = false;
  void reset()
  {
    body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    direction = {1, 0};
  }
  void draw()
  {

    for (unsigned int i = 0; i < body.size(); i++)
    {
      float x = body[i].x;
      float y = body[i].y;
      Rectangle segment = {offset + x * cellsize, offset + y * cellsize, (float)cellsize, (float)cellsize};
      DrawRectangleRounded(segment, 0.5, 6, darkgreen);
    }
    // body[i] -> "Go to the snake's body list and grab the package at position i." (Let's say it grabs {6, 9}).
    //. -> "Now, open that package..."
    // x -> "...and pull out ONLY the x value." (Which is 6).
  }

  void update()
  {
    body.push_front(Vector2Add(body[0], direction));
    if (addsegment == true)
    {
      addsegment = false;
    }
    else
    {
      body.pop_back();
    }
  }
  // a deque which stands for double ended queue is a data structure that allows you
  // to remove and add elements from both ends of the queue. It is similar to a vector
  // but it is more efficient for adding and removing elements from the front of the queue.
  // It is implemented as a dynamic array that can grow and shrink in size as needed. It is
  // also implemented as a circular buffer which means that when the end of the array is reached,
  // it wraps around to the beginning of the array. This allows for efficient use of memory and avoids
  // the need for resizing the array when adding or removing elements.
};

class game
{
public:
  snake s;
  apple a = apple(s.body);
  bool running = true;
  int score = 0;
  Sound eat;
  Sound wall;
  game()
  {
    InitAudioDevice();
    eat = LoadSound("sounds/eat.mp3.mp3");
    wall = LoadSound("sounds/bonk.mp3.mp3");
  }
  ~game()
  {
    UnloadSound(eat);
    UnloadSound(wall);
    CloseAudioDevice();
  }
  void draw()
  {
    s.draw();
    a.draw();
  }

  void gameover()
  {
    s.reset();
    a.position = a.getrandompos(s.body);
    running = false;
    score = 0;
    PlaySound(wall);
  }

  void edgescollision()
  {
    if (s.body[0].x == cellcount || s.body[0].x == -1)
    {
      gameover();
    }
    if (s.body[0].y == cellcount || s.body[0].y == -1)
    {
      gameover();
    }
  }
  void bodycollision()
  {
    deque<Vector2> headlessbody = s.body;
    headlessbody.pop_front();
    if (elementindeque(s.body[0], headlessbody))
    {
      gameover();
      PlaySound(wall);
    }
  }
  void update()
  {
    if (running == true)
    {
      s.update();
      foodcollision();
      edgescollision();
      bodycollision();
    }
  }
  void foodcollision()
  {
    if (Vector2Equals(s.body[0], a.position))
    {
      a.position = a.getrandompos(s.body);
      s.addsegment = true;
      score++;
      PlaySound(eat);
    }
  }
};

int main()
{

  InitWindow(2 * offset + cellsize * cellcount, 2 * offset + cellsize * cellcount, "jojo");

  SetTargetFPS(60);

  game g;

  while (WindowShouldClose() == false)
  {

    BeginDrawing();

    ClearBackground(cream);

    g.draw();

    if (eventtriggered(0.15))
    {
      g.update();
    }

    if (IsKeyPressed(KEY_UP) && g.s.direction.y != 1)
    {
      g.s.direction = {0, -1};
      g.running = true;
    }

    if (IsKeyPressed(KEY_DOWN) && g.s.direction.y != -1)
    {
      g.s.direction = {0, 1};
      g.running = true;
    }

    if (IsKeyPressed(KEY_RIGHT) && g.s.direction.x != -1)
    {
      g.s.direction = {1, 0};
      g.running = true;
    }

    if (IsKeyPressed(KEY_LEFT) && g.s.direction.x != 1)
    {
      g.s.direction = {-1, 0};
      g.running = true;
    }
    DrawRectangleLinesEx(Rectangle{(float)(offset - 5), (float)(offset - 5), (float)(cellsize * cellcount + 10), (float)(cellsize * cellcount + 10)}, 5, black);
    DrawText("GTA 7", offset - 5, 20, 40, red);
    DrawText(TextFormat("Score: %i", g.score), offset - 5, offset + cellsize * cellcount + 15, 40, red);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}

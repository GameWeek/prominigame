#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10 // Number of snowflakes in the animation example

#define PLAYER_HEIGHT 8
#define PLAYER_WIDTH 8

#define ENEMY_WIDTH 12
#define ENEMY_HEIGHT 12

const int buttonMid = 9;
const int buttonUP = 8;
const int buttonDOWN = 7;
const int buttonLEFT = 6;
const int buttonRIGHT = 5;
const int buttonSET = 4;
const int buttonRST = 3;

int player_x = 0;
int player_y = 0;

int btntime = 0;

boolean looping = false;
int life = 1;

static const unsigned char PROGMEM player_bmp[] =
    {
        B00000000,
        B00100000,
        B00010000,
        B10001000,
        B11111111,
        B10001000,
        B00010000,
        B00100000,
};

static const unsigned char PROGMEM enemy_bmp[] =
    {
        B000000,B000010,
        B000000,B000100,
        B000000,B001000,
        B000110,B010001,
        B001110,B100001,
        B111111,B111111,
        B000001,B000000,
        B000000,B100000,
        B000000,B010000,
        B000000,B001000,
        B000000,B000100,
        B000000,B000010,
};

struct Projectile
{
  int x;
  int y;
};

struct Enemy
{
  int x;
  int y;
};

int time = 0;

struct Projectile fires[10] = {
    {-10, 0}, {-10, 0}, {-10, 0}, {-10, 0}, {-10, 0}, {-10, 0}, {-10, 0}, {-10, 0}, {-10, 0}, {-10, 0}};

struct Enemy enemys[10] = {
    {0, 150}, {0, 150}, {0, 150}, {0, 150}, {0, 150}, {0, 150}, {0, 150}, {0, 150}, {0, 150}, {0, 150}};

void setup()
{
  pinMode(13, OUTPUT);
  pinMode(buttonUP, INPUT_PULLUP);
  pinMode(buttonDOWN, INPUT_PULLUP);
  pinMode(buttonLEFT, INPUT_PULLUP);
  pinMode(buttonRIGHT, INPUT_PULLUP);
  pinMode(buttonMid, INPUT_PULLUP);
  pinMode(buttonSET, INPUT_PULLUP);
  pinMode(buttonRST, INPUT_PULLUP);

  digitalWrite(13, HIGH);
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  // display.drawPixel(10, 10, SSD1306_WHITE);
  // display.display();
  display.clearDisplay();
  display.setTextSize(2);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.println(F("codetyphon"));
  display.display();
  delay(2000); // Pause for 2 seconds
  looping = true;
}

void drawPlayer()
{
  display.drawBitmap(player_x, player_y, player_bmp, PLAYER_WIDTH, PLAYER_HEIGHT, 1);
}

void loop()
{
  btntime += 1;
  display.clearDisplay();
  if (looping)
  {
    if (life > 0)
    {
      if (digitalRead(buttonUP) == LOW)
      {
        player_y -= 1;
      }
      if (digitalRead(buttonDOWN) == LOW)
      {
        player_y += 1;
      }
      if (digitalRead(buttonLEFT) == LOW)
      {
        player_x -= 1;
      }
      if (digitalRead(buttonRIGHT) == LOW)
      {
        player_x += 1;
      }

      if (player_x < 0)
      {
        player_x = 0;
      }

      if (player_y < 0)
      {
        player_y = 0;
      }

      if (player_x > SCREEN_WIDTH - PLAYER_WIDTH)
      {
        player_x = SCREEN_WIDTH - PLAYER_WIDTH;
      }

      if (player_y > SCREEN_HEIGHT - PLAYER_HEIGHT)
      {
        player_y = SCREEN_HEIGHT - PLAYER_HEIGHT;
      }

      if (digitalRead(buttonMid) == LOW)
      {
        //file
        if (btntime > 5)
        {
          for (size_t i = 0; i < 10; i++)
          {
            /* code */
            Projectile fire = fires[i];
            if (fire.x == -10)
            {
              fires[i].x = player_x;
              fires[i].y = player_y + PLAYER_HEIGHT / 2;
              break;
            }
          }
        }
        btntime = 0;
      }

      drawPlayer();

      time += 1;
      if (time == 50)
      {
        //add a enemy
        for (size_t i = 0; i < 20; i++)
        {
          if (enemys[i].x == 150)
          {
            enemys[i].x = 128;
            enemys[i].y = random(0, SCREEN_HEIGHT);
            break;
          }
        }

        time = 0;
      }

      //fire
      for (size_t i = 0; i < 10; i++)
      {
        Projectile fire = fires[i];
        if (fire.x != -10)
        {
          fires[i].x += 2;
          display.drawPixel(fire.x, fire.y, SSD1306_WHITE);
          //check is overlapsWith player?

          for (size_t j = 0; j < 10; j++)
          {
            if (abs(fires[i].x - enemys[j].x) < ENEMY_WIDTH / 2 && abs(enemys[i].y - enemys[j].y) < ENEMY_HEIGHT / 2)
            {
              enemys[i].x = 150;
              fires[i].x = -10;
            }
          }
        }
        if (fires[i].x > SCREEN_WIDTH)
        {
          fires[i].x = -10;
        }
      }

      //enemy
      for (size_t i = 0; i < 10; i++)
      {
        if (enemys[i].x < 129)
        {
          enemys[i].x -= 1;
          display.drawBitmap(enemys[i].x, enemys[i].y, enemy_bmp, ENEMY_WIDTH, ENEMY_HEIGHT, 1);
          if (enemys[i].x < -20)
          {
            enemys[i].x = 150;
          }
          //check is overlapsWith player?
          if (abs(enemys[i].x - player_x) < PLAYER_WIDTH / 2 && abs(enemys[i].y - player_y) < PLAYER_HEIGHT / 2)
          {
            enemys[i].x = 150;
            life -= 1;
            break;
          }
        }
      }
    }
    else
    {
      display.setTextSize(2);              // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.setCursor(10, 10);           // Start at top-left corner
      display.println(F("Game Over"));
      if (digitalRead(buttonRST) == LOW)
      {
        life = 1;
      }
    }

    if (digitalRead(buttonSET) == LOW)
    {
      //yes
    }
  }
  display.display();
}
#include <Arduino.h>

//Includes 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#include <Snake.h>
#include <Game.h>

#include <time.h>

#define PIN_BL  27
#define PIN_CLK 18
#define PIN_DIN 23
#define PIN_DC  19
#define PIN_CE  5
#define PIN_RST 14

#define PIN_UP     33
#define PIN_RIGHT  32
#define PIN_LEFT   12
#define PIN_DOWN   25

#define _PIX_DEFAULT 1

#define UP    0
#define DOWN  1
#define LEFT  2
#define RIGHT 3

#define BUSY      0
#define WAIT      1
#define GAMEOVER  2
#define SHOWSCORE 3

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(PIN_CLK, PIN_DIN, PIN_DC, PIN_CE, PIN_RST);

int xMax = display.width()-1;
int yMax = display.height()-1;

int resPixel = 6;
int xPxMax = display.width()/resPixel;
int yPxMax = display.height()/resPixel;
int xCenter = xPxMax/2;
int yCenter = yPxMax/2;
unsigned long previousMillis = 0; 

Snake kobra = Snake(xCenter,yCenter);
Game  game  = Game();

int iX = xCenter;
int8_t dirOld = kobra.getDir();
int8_t nextPos[2];
int8_t kbSize;
volatile  int8_t statusGame = BUSY;

// struct Flag{
//   volatile bool isCollision;
//   volatile bool isNewFood;
//   volatile bool isNewFoodValid;
//   volatile bool isGameFinish;
// };

struct Button {
  volatile bool up;
  volatile bool left;
  volatile bool right;
  volatile bool down;
};

Button button = {false,false,false,false};

// Flag flag = {false,false,false,false};

const unsigned char bitmapFood [] PROGMEM = {0b00110000,0b00110000,0b11001100,0b11001100,0b00110000,0b00110000};

void IRAM_ATTR isr_up() {
  button.up = true;
}

void IRAM_ATTR isr_left() {
  button.left = true;
}

void IRAM_ATTR isr_right() {
  button.right = true;
}

void IRAM_ATTR isr_down() {
  button.down = true;
}

void fillMetaPixel(int8_t x,int8_t y){
  display.fillRect(x*resPixel,y*resPixel,resPixel,resPixel,BLACK);
}

void fillMetaPixelFood(int8_t x,int8_t y){
  display.drawBitmap(x*resPixel, y*resPixel, bitmapFood,resPixel, resPixel, BLACK);
  
  // display.drawPixel(x*resPixel,y*resPixel,BLACK);
}

void printFood(){
  fillMetaPixelFood(kobra.food.x,kobra.food.y);
}

void walk(){
  kobra.crawl(nextPos[0],nextPos[1]);
}

void displaySnake(){
  for(int i=0;i<kobra.getSize();i++){
    fillMetaPixel(kobra.bodyX[i],kobra.bodyY[i]);
  } 
}

bool checkSelfCollision(int8_t locX,int8_t locY){
  bool tempReturn = false;
  for(int i =1;(i<kobra.getSize())&&(tempReturn == false);i++){
    if((locX == kobra.bodyX[i])&&(locY == kobra.bodyY[i])){
      tempReturn = true;
    }
  }
  return tempReturn;
}

void growFoodCollision(){
  game.isNewFoodValid = false;
  int8_t px,py;

  if(game.isNewFood == true){
    srand (time(NULL));
    game.isNewFood = false;
    //Sorteia a nova isca
    kobra.food.x = random(xPxMax); 
    kobra.food.y = random(yPxMax);

    if(checkSelfCollision(kobra.food.x,kobra.food.y) == true){ //Oops. The new random food lies inside the snake
      
      px = kobra.food.x;
      py = kobra.food.y;

      while(game.isNewFoodValid == false){
        //Varre x(+) a procura de um válido
        px++;
      
        if(px > xPxMax - 1){
          px = 0;
        }
      
        if(px == kobra.food.x){
      
          py++;
      
          if(py>yPxMax - 1){
            py = 0;
          }
      
          if(py == kobra.food.y){
            //FIM DE JOGO
            game.isNewFoodValid = true;
            game.isGameFinish = true;
          }
        }

        if(checkSelfCollision(kobra.food.x,kobra.food.y) == false){
          kobra.food.x = px;
          kobra.food.y = py;
          game.isNewFoodValid = true;
        }
      }
    }
  } 
  else{
    game.isCollision = checkSelfCollision(nextPos[0],nextPos[1]);
  }
}

void drawWall(){
    display.drawLine(0,       0, xMax, 0, BLACK);
    display.drawLine(xMax, yMax, xMax, 0, BLACK);
    display.drawLine(xMax, yMax,    0, yMax, BLACK);
    display.drawLine(0,       0,    0, yMax, BLACK);
}

void updateDir(){
   if(dirOld != kobra.getDir()){
        switch(kobra.getDir()) {
          case UP :
              if(dirOld == DOWN){
                kobra.setDir(DOWN); //Does not affect
              }
              break; //optional
          case DOWN :
              if(dirOld == UP){
                kobra.setDir(UP); //Does not affect
              }
              break; //optional
          case LEFT :
              if(dirOld == RIGHT){
                kobra.setDir(RIGHT); //Does not affect
              }
              break; //optional
          case RIGHT :
              if(dirOld == LEFT){
                kobra.setDir(LEFT); //Does not affect
              }
              break; //optional            
          default:
              break; //optional            
        }
    }
}

void computeNextPosition(){
  switch(kobra.getDir()) {
      case UP :
          nextPos[0] = kobra.getHead(0);
          nextPos[1] = kobra.getHead(1) - 1;            
          if(nextPos[1]<0)
            nextPos[1] = yPxMax-1;
          break; //optional
      case DOWN :
          nextPos[0] = kobra.getHead(0);
          nextPos[1] = kobra.getHead(1) + 1;
          if(nextPos[1] > yPxMax-1)
            nextPos[1] = 0;
          break; //optional
      case LEFT :
          nextPos[0] = kobra.getHead(0)-1;
          nextPos[1] = kobra.getHead(1);
          if(nextPos[0] < 0)
            nextPos[0] = xPxMax-1;            
          break; //optional
      case RIGHT :
          nextPos[0] = kobra.getHead(0)+1;
          nextPos[1] = kobra.getHead(1);
          if(nextPos[0] > xPxMax-1)
            nextPos[0] = 0;            
          break; //optional            
      default:
          break; //optional            
    }
}

void checkCollision(){
  if((kobra.getHead(0)==kobra.food.x)&&(kobra.getHead(1)==kobra.food.y)){
      kobra.grow(kobra.food.x,kobra.food.y);
      game.isNewFood = true;
   }
}

void handleJoy(){
  if(button.up){
    dirOld = kobra.getDir(); //Possível direção antiga
    kobra.setDir(UP); ////Novo candidato a direção
    button.up = false;
  } else if(button.left){    
    dirOld = kobra.getDir(); //Possível direção antiga
    kobra.setDir(LEFT); ////Novo candidato a direção
    button.left = false;
  }else if(button.right){
    dirOld = kobra.getDir(); //Possível direção antiga
    kobra.setDir(RIGHT); ////Novo candidato a direção
    button.right = false;
  }else if(button.down){
    dirOld = kobra.getDir(); //Possível direção antiga
    kobra.setDir(DOWN); ////Novo candidato a direção
    button.down = false;
  }

}

void setup() {
  
  pinMode(PIN_BL,OUTPUT);
  digitalWrite(PIN_BL,HIGH);

  pinMode(PIN_UP,     INPUT_PULLUP);
  pinMode(PIN_LEFT,   INPUT_PULLUP);
  pinMode(PIN_RIGHT,  INPUT_PULLUP);
  pinMode(PIN_DOWN,   INPUT_PULLUP);
  
  attachInterrupt(PIN_UP,    isr_up,    FALLING);
  attachInterrupt(PIN_LEFT,  isr_left,  FALLING);
  attachInterrupt(PIN_RIGHT, isr_right, FALLING);
  attachInterrupt(PIN_DOWN,  isr_down,  FALLING);
  
  
  display.begin();
  display.setContrast(50);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("Welcome to the Snake Game!");
  display.display(); //splash screen
  /* initialize random seed: */
  randomSeed(analogRead(0));
  delay(1000);
}

void loop() {

    //01 - Mostra a tela
    if(statusGame == BUSY){
          
      previousMillis = millis();

      //CRAWL        
      handleJoy();
      
      updateDir();
      
      //Verifica onde tá a cabeça da cobra e calcula qual sera a proxima posição (baseada em Dir)
      computeNextPosition();

      //Check if it grows (Food)
      checkCollision();

      //Verifica se precisa sortear uma nova comida ou se bateu
      growFoodCollision();

      //Walk = Muda o vetor da cobra pra nova posição
      walk();

      // // Valida direção
      // updateDir(); 

      //Apaga a tela
      display.clearDisplay();
      
      //Desenha a Cobra
      displaySnake();

      //Desenha a Comida
      printFood();

      display.display(); //splash screen  
      
      statusGame = WAIT;
  }
  else if(statusGame == WAIT){
    if(millis() - previousMillis > kobra.getSpeed()){
      if(game.isCollision == true){
        statusGame = GAMEOVER;
      } else {
        statusGame = BUSY;
      }

    }
  } else if(statusGame == GAMEOVER){
    if(millis() - previousMillis > 1000){
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(BLACK);
        display.setCursor(15,17);
        display.println("GAME OVER");
        display.display(); //splash screen
        statusGame = SHOWSCORE;
    }
  } else if(statusGame == SHOWSCORE){
    if(millis() - previousMillis > 2000){
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(BLACK);
        display.setCursor(15,17);
        display.print("Score: ");
        display.print(kobra.getSize());
        display.display(); //splash screen
    }
    if(millis() - previousMillis > 3000){
      kobra.NewGame(); 
      statusGame = BUSY;
    }
  }
}

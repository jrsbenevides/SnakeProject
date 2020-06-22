#include <vector>

#ifndef SNAKE_H
#define SNAKE_H

using namespace std;

class Snake 
{
public:
  
  Snake(int8_t X, int8_t Y);	 // Constructor
  ~Snake();  // Destructor
  
  int8_t getSize(); 
  int8_t getDir();  // Change Direction
  int8_t getHead(int8_t dim);
  unsigned long getSpeed();

  void setDir(int8_t dir);      // Change Direction

  void grow(int8_t x,int8_t y);	// Grow 1 metapixel
  void crawl(int8_t x,int8_t y); //Move
  void NewGame(); //Novo jogo
  
  struct prize{
    int8_t x;
    int8_t y;
  } food;
  
  vector<int8_t> bodyX;
  vector<int8_t> bodyY;
  
private:
  unsigned long speed;            // Update rate of the game
  int8_t dir;               // Direction (UP(0), DOWN(1), LEFT(2), RIGHT(3))
  
};

#endif
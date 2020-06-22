#ifndef GAME_H
#define GAME_H

class Game 
{
public:
  
  Game();	 // Constructor
  ~Game();  // Destructor
  
  volatile bool isCollision;
  volatile bool isNewFood;
  volatile bool isNewFoodValid;
  volatile bool isGameFinish;  
  
private:

};

#endif
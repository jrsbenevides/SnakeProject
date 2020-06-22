#include <Game.h>

/*!
  @brief Constructor for Snake Object
  @param X Location of initial metapixel x
  @param Y Location of initial metapixel y
 */

Game::Game(){
  isCollision = false;
  isNewFood = false;
  isNewFoodValid = false;
  isGameFinish = false; 	
}

Game::~Game(){
    //Destructor
}


// int8_t Game::getSize(void){
// 	return this->bodyX.size();
// }
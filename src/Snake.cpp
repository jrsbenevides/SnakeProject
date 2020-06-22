#include <Snake.h>

/*!
  @brief Constructor for Snake Object
  @param X Location of initial metapixel x
  @param Y Location of initial metapixel y
 */

Snake::Snake(int8_t X, int8_t Y){
	this->speed  = 120; //DEFAULT = 400ms
	setDir(3);  //DEFAULT = 3 = RIGHT
	grow(X,Y);
}

Snake::~Snake(){
    //Destructor
}

void Snake::NewGame(void){
    //Destructor Artificial
	bodyX.erase(bodyX.begin(),bodyX.end());
	bodyY.erase(bodyY.begin(),bodyY.end());
	Snake(0,0);
}

void Snake::setDir(int8_t dir){
	this->dir = dir;  
}

int8_t Snake::getDir(){
	return this->dir;
}

unsigned long Snake::getSpeed(){
	return this->speed;
}

int8_t Snake::getHead(int8_t dim){
	if(dim == 0)
		return this->bodyX.back();
	else
		return this->bodyY.back();
}

void Snake::grow(int8_t x,int8_t y){
	this->bodyX.push_back(x);
	this->bodyY.push_back(y);
}

void Snake::crawl(int8_t x,int8_t y){
	this->bodyX.erase(bodyX.begin());
	this->bodyY.erase(bodyY.begin());
	grow(x,y);
}

int8_t Snake::getSize(void){
	return this->bodyX.size();
}
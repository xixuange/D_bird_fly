#include "cocos2d.h"

using namespace cocos2d;

class  Obstacle :public Node
{
public:
	Array* obstacleList;

	Obstacle();
	virtual void onEnter();
	void update(); 
	void addOne(int offsetX);
	void checkDel();
	int addCount = 0;
	int GAME_STATUS = 0;
};

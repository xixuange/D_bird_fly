#include "cocos2d.h"
#include "Obstacle.h"
using namespace std;

class FlyBirdGame :public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(FlyBirdGame);
	void initUI();
	void gameStart(Object* pSender);
	void update(float time);
	Obstacle* obstacle;
	void onTouchesEnded(const vector<Touch*>& touches, Event* event);
	void onTouchesBegan(const vector<Touch*>& touches, Event* event);
	bool isFlying = false;
	float velocity = -2;
	Rect spriteRect(Sprite* s);
	int GAME_STATUS;
	int score = 0;
	float gravity=0.2;
};
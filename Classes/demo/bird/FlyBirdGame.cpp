#include "cocos2d.h"
#include "FlyBirdGame.h"
#include "resource.h"; 

USING_NS_CC;
using namespace std;

Scene* FlyBirdGame::createScene()
{
	auto scene = Scene::create();
	auto layer = FlyBirdGame::create();
	scene->addChild(layer);
	return scene;
}

bool FlyBirdGame::init()
{
	if (!Layer::init())
	{
		return false;
	}
	initUI();
	return true;
}

void FlyBirdGame::initUI()
{
	// win size
	auto winSize = Director::getInstance()->getVisibleSize();

	// game bg
	auto bg = Sprite::create(bird_bg);
	bg->setPosition(winSize.width / 2, winSize.height / 2);
	bg->setScale(winSize.width / bg->getContentSize().width, winSize.height / bg->getContentSize().height);
	this->addChild(bg);

	// logo
	auto logo = Sprite::create(bird_logo);
	logo->setPosition(ccp(winSize.width / 2, winSize.height / 2 + logo->getContentSize().height * 2));
	logo->setTag(TAG_LOGO);
	this->addChild(logo, 1);

	// over logo
	auto overLogo = Sprite::create(bird_gameover);
	overLogo->setPosition(ccp(winSize.width / 2, winSize.height / 2 + overLogo->getContentSize().height));
	overLogo->setTag(TAG_OVER);
	overLogo->setVisible(false);
	this->addChild(overLogo, 1);

	// start btn
	auto startBtn = MenuItemImage::create(bird_start_btn, bird_start_btn_pressed, CC_CALLBACK_1(FlyBirdGame::gameStart, this));
	auto menu = Menu::create(startBtn, NULL);
	menu->setTag(TAG_START_BTN);
	this->addChild(menu);

	// hero
	auto hero = Sprite::create(bird_hero);
	hero->setPosition(winSize.width / 3, winSize.height*0.8);
	hero->setVisible(false);
	hero->setTag(TAG_HERO);
	this->addChild(hero, 1);
	Animation* an = Animation::create();
	an->addSpriteFrameWithFileName(bird_hero);
	an->addSpriteFrameWithFileName(bird_hero2);
	an->addSpriteFrameWithFileName(bird_hero3);
	an->setDelayPerUnit(0.5f / 3.0f);
	an->setLoops(-1);
	Animate* anim = Animate::create(an);
	hero->runAction(anim);

	// score
	auto score = LabelBMFont::create("0", "fonts/futura-48.fnt");
	score->setPosition(Point(winSize.width / 2, winSize.height / 4 * 3));
	addChild(score, 1);
	score->setVisible(false);
	score->setTag(TAG_SCORE);

	// Obstacle
	obstacle = new Obstacle();
	this->addChild(obstacle);

	// update 
	scheduleUpdate();

	// touch
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesEnded = CC_CALLBACK_2(FlyBirdGame::onTouchesEnded, this);
	listener->onTouchesBegan = CC_CALLBACK_2(FlyBirdGame::onTouchesBegan, this);
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	// init status
	GAME_STATUS = GAME_STATUS_START;
}

void FlyBirdGame::gameStart(Object* pSender)
{
	this->getChildByTag(TAG_START_BTN)->setVisible(false);
	this->getChildByTag(TAG_LOGO)->setVisible(false);
	this->getChildByTag(TAG_SCORE)->setVisible(true);
	this->getChildByTag(TAG_HERO)->setVisible(true);
	obstacle->GAME_STATUS = GAME_STATUS_PLAYING;
	GAME_STATUS = GAME_STATUS_PLAYING;
	isFlying = false;
	score = 0;
	velocity = -3;
}

void FlyBirdGame::update(float time)
{
	auto winSize = Director::getInstance()->getVisibleSize();
	auto hero = this->getChildByTag(TAG_HERO);
	Rect rHero = ((Sprite*)hero)->getBoundingBox();
	log("time=%f", time);
	switch (GAME_STATUS)
	{
	case GAME_STATUS_PLAYING:
		obstacle->update();
		// update bird positionY
		if (hero->getPositionY() > 0 && hero->getPositionY() < winSize.height)
		{
			velocity -= gravity;
			hero->setPositionY(hero->getPositionY() + velocity);
		}
		//if (isFlying&&hero->getPositionY() < winSize.height)
		//{
		//	hero->setPositionY(hero->getPositionY() + velocity);
		//}
		//else if (hero->getPositionY()>0)
		//{
		//	hero->setPositionY(hero->getPositionY() - velocity);
		//}
		//check collision
		for (int i = 0; i < obstacle->obstacleList->count(); i++)
		{
			Sprite* obstacleSprite = (Sprite*)obstacle->obstacleList->getObjectAtIndex(i);
			bool pia = rHero.intersectsRect(obstacleSprite->getBoundingBox());
			if (pia == true)
			{
				GAME_STATUS = GAME_STATUS_GAME_OVER;
				break;
			}
			int oPosX = obstacleSprite->getPositionX() + obstacleSprite->getContentSize().width / 2;
			int heroX = hero->getPositionX() - hero->getContentSize().width;
			if (oPosX == heroX)
			{
				score++;
				auto scoreSprite = (LabelBMFont*)this->getChildByTag(TAG_SCORE);
				String* s = String::createWithFormat("%d", score / 2);
				scoreSprite->setString(s->getCString());
			}
		}
		break;
	case GAME_STATUS_GAME_OVER:
		this->getChildByTag(TAG_OVER)->setVisible(true);
		break;
	case GAME_STATUS_RESTART:
		//reset game
		obstacle->removeAllChildren();
		obstacle->obstacleList->removeAllObjects();
		// reset hero
		hero->setPosition(winSize.width / 5, winSize.height*0.8);
		hero->setVisible(false);
		// show btn
		this->getChildByTag(TAG_START_BTN)->setVisible(true);
		// show logo
		this->getChildByTag(TAG_LOGO)->setVisible(true);
		// hide over log
		this->getChildByTag(TAG_OVER)->setVisible(false);
		// hide score
		this->getChildByTag(TAG_SCORE)->setVisible(false);
		break;
	}
}

void FlyBirdGame::onTouchesEnded(const vector<Touch*>& touches, Event* event)
{
	Touch* touch = touches[0];
	Point location = touch->getLocation();
	if (GAME_STATUS == GAME_STATUS_PLAYING)
	{
		isFlying = false;
	}
}

void FlyBirdGame::onTouchesBegan(const vector<Touch*>& touches, Event* event)
{
	Touch* touch = touches[0];
	Point location = touch->getLocation();
	if (GAME_STATUS == GAME_STATUS_PLAYING)
	{
		log("GAME_STATUS_PLAYING");
		isFlying = true;
		velocity = 5;
	}
	else if (GAME_STATUS == GAME_STATUS_GAME_OVER)
	{
		GAME_STATUS = GAME_STATUS_RESTART;
		log("GAME_STATUS_GAME_OVER");
	}
}

Rect FlyBirdGame::spriteRect(Sprite* s)
{
	Point pos = s->getPosition();
	Size cs = s->getContentSize();
	return CCRectMake(pos.x - cs.width / 2, pos.y - cs.height / 2, cs.width, cs.height / 2);
}
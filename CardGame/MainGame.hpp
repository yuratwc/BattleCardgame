#pragma once

#include "Util.hpp"
#include "Effect.hpp"
#include "Card.hpp"


enum class GameState
{
	MyTurnSelect = 0,
	PlaceSelect = 1,
	EnemyTurn = 2,
	AttackSelect = 3,
	AnimationWait = 10,
};

enum class RenderResult
{
	Success = 0,
	Fault = 1,
	LeftAction = 3,
	RightAction = 7,
};

class Place
{
public:
	Array<Card*> cards;
	int32 maxhp;
	int32 hp;
	int32 offence;
	int32 defence;
	bool killed;

	bool isEmpty();
	bool isValid();
	bool isFightable();
	bool canAdd();
	bool add(Card* card);
	int attack(int damage, bool simulate);
	void draw(int32 x, int32 y);
	void drawToolTip(int32 x, int32 y, bool drawBackGround);

};

class User
{
public:
	String name;
	Array<Card*> deck = { Card::cards[101], Card::cards[102] , Card::cards[103] , Card::cards[104] , Card::cards[105] , Card::cards[106], Card::cards[107], Card::cards[108], Card::cards[109], Card::cards[201], Card::cards[202] };
	Array<Card*> hand;
	Array<Place*> place;
	int killCount;

	User();
	void drawCardFromDeck();
	bool summonFromHand(Card* card, int32 place, bool simulate);
	void dispose();

};


class GameInfo
{
public:
	User me;
	User enemy;
	GameState state;
	int32 actionCount;
	int32 maxActionCount;
	bool gameOver;
	bool win;
	int currentTurn;

	int32 _userHandHas;
	int32 _handScroll;
	Place* tooltipPlace;
	Card* tooltipCard;
	int32 _targetPlace;
	int32 _targetAttack;
};

class MainGameRenderer
{
private:
	int32 handScrollX;
public:
	Stopwatch* toastWatch;
	String toastText;

	void showToast(String str);
	void hideToast();
	void updateToast();
	~MainGameRenderer();

	void renderDeck(User& user, int32 posx = 490, int32 posy = 220);
	RenderResult renderHandCards(GameInfo& info);
	void renderGameover(GameInfo& info);
	void renderTurnStart(GameInfo& info);

	RenderResult renderPlace(User& user, GameInfo& info, int32 y);
	bool renderPlaceFrame(User& user, GameInfo& info, Card* c, int32 y);
	bool renderPlaceFrameAttack(User& enemy, GameInfo& info, Place* target, int32 targetIndex);
	void renderKillCounts(User& user, User& enemy);
	void renderCircle(GameInfo& info);

	void renderButtons(GameInfo& info);

	Vec2 getHandCardPos(int index, GameInfo& info);
	Vec2 getUserPlacePos(int index);
	Vec2 getEnemyPlacePos(int index);

};

#include "CPU.h"

class MainGame
{
private:
	GameInfo info;

	AITest enemyAI;

	Card* toolTip;
	Place* targetPlace;
	Card* have;
	int32 targetIndex;

public:
	MainGameRenderer renderer;

	MainGame();
	void update();
	void nextTurn(User& enemy, User& user);
	void dispose();
};

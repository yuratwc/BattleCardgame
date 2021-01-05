#pragma once
#include<Siv3D.hpp>

class User;
class GameInfo;
class MainGame;

class AI
{
protected:
	MainGame* main_game;
public:
	AI(MainGame* ptr);
	virtual void turnStart(User& user, User& enemy, GameInfo& info);
	virtual void update(User& user, User& enemy, GameInfo& info);
	virtual void turnEnd(User& user, User& enemy, GameInfo& info);

};

class AITest : AI
{
public:
	AITest(MainGame* ptr);

	void turnStart(User& user, User& enemy, GameInfo& info) override;
	void turnDo(User& user, User& enemy, GameInfo& info);
	void update(User& user, User& enemy, GameInfo& info) override;

	bool drawAny(User& user, User& enemy, GameInfo& info);

};

enum MouseUseType { Wait, SelectAttacker, SelectEnemyAndMyPlace, SelectEnemyPlace, SelectMyPlace };

class AIMouseUse : AI
{
private:
	MouseUseType state;
	int32 handSelect;
	std::stack<MouseUseType> undoStack;
public:
	AIMouseUse(MainGame* ptr);

	void turnStart(User& user, User& enemy, GameInfo& info) override;
	void update(User& user, User& enemy, GameInfo& info) override;

	bool containsCard(int32 x, int32 y, Vec2 pos);
	void selectAttacker(User& user, User& enemy, GameInfo& info);
	void selectEnemyAndMyPlace(User& user, User& enemy, GameInfo& info);
	void selectMyPlace(User& user, User& enemy, GameInfo& info);

	void undoControl();
};
#include "CPU.h"

#include "MainGame.hpp"

AI::AI(MainGame* ptr)
	:main_game(ptr) {}

void AI::update(User& user, User& enemy, GameInfo& info)
{

}

void AI::turnStart(User& user, User& enemy, GameInfo& info)
{
}

void AI::turnEnd(User& user, User& enemy, GameInfo& info)
{
	main_game->nextTurn(enemy, user);
}



AITest::AITest(MainGame* ptr) : AI::AI(ptr) {}

bool AITest::drawAny(User& user, User& enemy, GameInfo& info)
{
	auto firstCard = user.hand.sorted_by([](Card* c1, Card* c2)->int {
		auto c1t = c1->getType();
		auto c2t = c2->getType();
		if (c1t == CardType::Fighter && c2t == CardType::Fighter)
		{
			return ((CardFighter*)c1)->hp > ((CardFighter*)c2)->hp;
		}
		if (c1t == CardType::Spell && c2t == CardType::Spell)
		{
			return false;
		}
		if (c1t == CardType::Spell)
		{
			return false;
		}
		return true; });

	if (firstCard[0]->getType() == CardType::Fighter)
	{
		auto targetPlace = -1;
		for (auto i = 0; i < 15; i++)
		{
			targetPlace = Random(0, 2);
			if (user.place[targetPlace]->isEmpty())
			{
				user.summonFromHand(firstCard[0], targetPlace, true);
				info.state = GameState::MyTurnSelect;
				return true;
			}
		}
	}
	return false;
}

void AITest::update(User& user, User& enemy, GameInfo& info)
{

}

void AITest::turnDo(User& user, User& enemy, GameInfo& info)
{
	info.actionCount--;

	if (info.actionCount < 0)
	{
		info.state = GameState::MyTurnSelect;
		turnEnd(user, enemy, info);
		return;
	}

	//もし場が空なら
	int placeEmpty = 0;
	for (int32 i = 0; i < 3; i++)
	{
		if (user.place[i]->isEmpty())
			placeEmpty++;
	}

	if (placeEmpty != 0)
	{
		if (drawAny(user, enemy, info))
		{
			turnDo(user, enemy, info);
			return;

		}
	}

	bool continuous = true;
	for (int32 i = 0; i < 3; i++)
	{
		if (!user.place[i]->isFightable())
			continuous = false;
	}
	if (!continuous)
	{
		//end;
		info.state = GameState::MyTurnSelect;

		return;
	}

	//attack
	//int32 j = 0;


	int32 maxDmg = 0;
	int32 myTargetIndex = -1;
	int32 enemyTargetIndex = -1;
	for (int32 i = 0; i < 3; i++)
	{
		if (user.place[i]->isFightable())
		{
			for (int32 j = -1; j < 2; j++)
			{
				if (j + i < 0 || j + i >= 3 || !enemy.place[i + j]->isFightable())
					break;
				int32 dmg = enemy.place[i + j]->attack(user.place[i]->offence - enemy.place[i + j]->defence, false);
				if (dmg > maxDmg)
				{
					maxDmg = dmg;
					myTargetIndex = i;
					enemyTargetIndex = i + j;
				}
			}
		}
	}
	if (maxDmg == 0)
	{
		if (drawAny(user, enemy, info))
		{
			turnDo(user, enemy, info);
			return;

		}
	}

	int32 myTarget = myTargetIndex;
	int32 enemyTarget = enemyTargetIndex;

	if (myTargetIndex == -1 || enemyTargetIndex == -1)
	{
		myTarget = -1;
		for (; myTarget == -1 || !user.place[myTarget]->isFightable();)
		{
			myTarget = Random(0, 2);
		}
		enemyTarget = -1;
		for (; enemyTarget == -1 || !enemy.place[enemyTarget]->isFightable();)
		{
			enemyTarget = Random(0, 2);
		}
	}


	Place* enemyFighter = enemy.place[enemyTarget];
	Place* userFighter = user.place[myTarget];

	auto to_vec = Vec2((Card::CardWidth + 10) * enemyTarget + Card::CardWidth / 2, 220 + Card::CardHeight / 2);
	auto from_vec = Vec2((Card::CardWidth + 10) * myTarget + Card::CardWidth / 2, 20 + Card::CardHeight / 2);
	int32 dmg = enemyFighter->attack((userFighter->offence - enemyFighter->defence), false);
	GameUtil::effects->add<AttackEffect>(new DamageEffect(to_vec, dmg), new ArrowEffect(from_vec, to_vec));

	info.state = GameState::AnimationWait;

	info._targetPlace = myTarget;
	info._targetAttack = enemyTarget;
	GameUtil::Wait(2, [&]() {

		int32 k = info.enemy.place[info._targetPlace]->offence - info.me.place[info._targetAttack]->defence;
		info.me.place[info._targetAttack]->attack(k, true);
		//placePtr->attack(dmg, true);
		info.me.killCount = 0;
		Print << info._targetPlace << U"/" << info._targetAttack;

		info._targetAttack = -1;


		for (int32 i = 0; i < info.me.place.size(); i++)
		{
			if (info.me.place[i]->killed)
				info.me.killCount++;
		}

		//GameUtil::effects->add<DamageEffect>(to_vec, dmg);
		//GameUtil::effects->add<ArrowEffect>(from_vec, to_vec);
		if (info.me.killCount == 3)
		{
			info.gameOver = true;
			info.win = false;
		}
		turnDo(user, enemy, info);
		});
}


void AITest::turnStart(User& user, User& enemy, GameInfo& info)
{
	Print << U"oioiioioioiiiiiiiiiiiiiii";
	turnDo(user, enemy, info);

};



///user

AIMouseUse::AIMouseUse(MainGame* ptr) : AI::AI(ptr), state(MouseUseType::Wait),handSelect(-1) {}


void AIMouseUse::turnStart(User& user, User& enemy, GameInfo& info)
{

}

void AIMouseUse::update(User& user, User& enemy, GameInfo& info)
{
	if (state == MouseUseType::SelectAttacker)
	{
		selectAttacker(user, enemy, info);
	}
	if (state == MouseUseType::SelectEnemyAndMyPlace)
	{
		selectAttacker(user, enemy, info);
	}
}



bool AIMouseUse::containsCard(int32 x, int32 y, Vec2 pos)
{
	return pos.x >= x && pos.y >= y && y + Card::CardWidth > pos.y&& x + Card::CardHeight > pos.x;
}

void AIMouseUse::undoControl()
{
	if (undoStack.size() == 0)
	{
		state = MouseUseType::SelectAttacker;
		return;
	}

	state = undoStack.top();
	undoStack.pop();
}

void AIMouseUse::selectAttacker(User& user, User& enemy, GameInfo& info)
{
	//一番最初の状態で攻撃者かスペルか場のカードを選択する

	//hands
	for (int32 i = 0; i < user.hand.size(); i++)
	{
		auto vec = main_game->renderer.getHandCardPos(i, info);
		if (MouseL.down() && containsCard(vec.x, vec.y, Cursor::Pos()))
		{
			//this card
			if (handSelect != i)
			{
				handSelect = i;
			}
			else if (user.hand[i]->getType() == CardType::Spell)
			{
				//use spell
			}
		}
	}

	for (int32 i = 0; i < user.place.size(); i++)
	{
		auto vec = main_game->renderer.getUserPlacePos(i);
		if (MouseL.down() && containsCard(vec.x, vec.y, Cursor::Pos()))
		{
			//this card
			handSelect = -1;
			undoStack.push(state);
			state = MouseUseType::SelectEnemyAndMyPlace;
			return;
		}
	}
}

void AIMouseUse::selectEnemyAndMyPlace(User& user, User& enemy, GameInfo& info)
{
	if (MouseR.down())
	{
		undoControl();
		return;
	}
	for (int32 i = 0; i < user.place.size(); i++)
	{
		auto vec = main_game->renderer.getUserPlacePos(i);
		if (containsCard(vec.x, vec.y, Cursor::Pos()))
		{
			//this card
		}
	}

	for (int32 i = 0; i < enemy.place.size(); i++)
	{
		auto vec = main_game->renderer.getEnemyPlacePos(i);
		if (enemy.place[i]->isValid() && !enemy.place[i]->isEmpty() && containsCard(vec.x, vec.y, Cursor::Pos()))
		{
			//this card
		}
	}
}

void AIMouseUse::selectMyPlace(User& user, User& enemy, GameInfo& info)
{
	for (int32 i = 0; i < user.place.size(); i++)
	{
		auto vec = main_game->renderer.getUserPlacePos(i);
		if (containsCard(vec.x, vec.y, Cursor::Pos()))
		{
			//this card
		}
	}
}
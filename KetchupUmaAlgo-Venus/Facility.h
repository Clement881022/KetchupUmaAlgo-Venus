#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Globals.h"
#include "SupportCard.h"

class Facility
{
public:
	Facility(Train train, float levelScore);
	void Reset(float levelScore);
	void ResetTurn();
	bool HaveShine();
	void UpdateLevel();
	void UpdateSupportCards(int turn, int blueVenusLevel);
	void Click();
	void VenusRace();
	void RedWisdom();
	void YellowWisdom();
	void BlueWisdom();
	void SetSupportCard(SupportCard* card);

	int Level;
	int MaxLevel;
	float LevelScore;
	std::vector<SupportCard*> SupportCards;
	Train TrainType;
};
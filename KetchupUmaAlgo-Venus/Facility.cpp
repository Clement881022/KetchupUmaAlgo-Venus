#include "Facility.h"

Facility::Facility(Train train, float levelScore)
{
	TrainType = train;
	LevelScore = levelScore;
	MaxLevel = 4;
	UpdateLevel();
}

void Facility::Reset(float levelScore)
{
	LevelScore = levelScore;
	std::vector<SupportCard*>().swap(SupportCards);
	UpdateLevel();
}

void Facility::ResetTurn()
{
	std::vector<SupportCard*>().swap(SupportCards);
	UpdateLevel();
}

bool Facility::HaveShine()
{
	for (SupportCard* card : SupportCards)
	{
		if (card->IsShining)
		{
			return true;
		}
	}
	return false;
}

void Facility::UpdateLevel()
{
	Level = static_cast<int>(LevelScore) / 12;
	Level = std::min(Level, MaxLevel);
}

void Facility::UpdateSupportCards(int turn, int blueVenusLevel)
{
	for (const auto& card : SupportCards)
	{
		card->UpdateStatus(TrainType, Level, turn, blueVenusLevel);
	}
}

void Facility::Click()
{
	LevelScore += 2;
	UpdateLevel();

	std::vector<SupportCard*> hintCards;
	for (SupportCard* card : SupportCards)
	{
		card->Click();

		if (card->HaveBlueVenusHint)
		{
			card->TriggerBlueVenusHint();
		}
		if (card->HaveHint)
		{
			hintCards.emplace_back(card);
		}
	}

	if (!hintCards.empty())
	{
		float randomFloat = GetZeroOneRandomFloat() * hintCards.size();
		int random = static_cast<int>(randomFloat);
		if (random >= 0 && random < hintCards.size())
		{
			hintCards[random]->TriggerHint();
		}
	}
}

void Facility::VenusRace()
{
	LevelScore += 8;
	UpdateLevel();
}

void Facility::RedWisdom()
{
	Level = 5;
}

void Facility::YellowWisdom()
{
	for (SupportCard* card : SupportCards)
	{
		card->IsShining = true;
	}
}

void Facility::BlueWisdom()
{
	for (SupportCard* card : SupportCards)
	{
		card->BlueWisdom();
	}
}

void Facility::SetSupportCard(SupportCard* card)
{
	SupportCards.emplace_back(card);
}

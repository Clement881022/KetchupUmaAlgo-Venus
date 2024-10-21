#include "Facility.h"

Facility::Facility(Train train, float levelScore)
{
	TrainType = train;
	LevelScore = levelScore;
	MaxLevel = 4;
	std::fill(SupportCards.begin(), SupportCards.end(), nullptr);
	UpdateLevel();
}

void Facility::Reset(float levelScore)
{
	LevelScore = levelScore;
	std::fill(SupportCards.begin(), SupportCards.end(), nullptr);
	UpdateLevel();
}

void Facility::ResetTurn()
{
	std::fill(SupportCards.begin(), SupportCards.end(), nullptr);
	UpdateLevel();
}

bool Facility::HaveShine()
{
	for (SupportCard* card : SupportCards)
	{
		if (card == nullptr) continue;
		if (card->IsShining) { return true; }
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
		if (card == nullptr) continue;
		card->UpdateStatus(TrainType, Level, turn, blueVenusLevel);
	}
}

void Facility::Click()
{
	LevelScore += 2;
	UpdateLevel();

	std::array<SupportCard*, 6> hintCards = { nullptr };
	int hintCardCount = 0;

	for (SupportCard* card : SupportCards)
	{
		if (card == nullptr) continue;
		card->Click();
		if (card->HaveBlueVenusHint) { card->TriggerBlueVenusHint(); }
		if (card->HaveHint) { hintCards[hintCardCount++] = card; }
	}

	if (hintCardCount > 0)
	{
		// 產生一個隨機索引
		int randomIndex = static_cast<int>(GetZeroOneRandomFloat() * hintCardCount);
		if (randomIndex >= 0 && randomIndex < hintCardCount)
		{
			hintCards[randomIndex]->TriggerHint();
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
		if (card == nullptr) continue;
		card->IsShining = true;
	}
}

void Facility::BlueWisdom()
{
	for (SupportCard* card : SupportCards)
	{
		if (card == nullptr) continue;
		card->BlueWisdom();
	}
}

void Facility::SetSupportCard(SupportCard* card)
{
	for (int i = 0;i < 6;i++)
	{
		if (SupportCards[i] == nullptr)
		{
			SupportCards[i] = card;
			break;
		}
	}
}

int Facility::GetSupportCardsSize()
{
	int result = 0;
	for (int i = 0;i < 6;i++)
	{
		if (SupportCards[i] != nullptr) result++;
	}
	return result;
}

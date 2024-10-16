#include "SupportCard.h"

SupportCard::SupportCard(SupportCardData cardData, bool haveHint, int currentBond)
{
	CardData = cardData;
	HaveHint = haveHint;
	Bond = currentBond;
	IsShining = false;
	HaveBlueVenusHint = false;
}

void SupportCard::Reset(SupportCardData cardData, bool haveHint, int currentBond)
{
	CardData = cardData;
	HaveHint = haveHint;
	Bond = currentBond;
	IsShining = false;
	HaveBlueVenusHint = false;
}

void SupportCard::Event()
{
	Bond = std::min(100, Bond + 5);
}

void SupportCard::UpdateStatus(Train trainType, int facilityLevel, int currentTurn, int blueVenusLevel)
{
	float basicHintChance = 0.075f;
	float hintChancePerPoint = 0.0005f;
	float hintChance = basicHintChance + hintChancePerPoint * static_cast<float>(CardData.HintChance + BlueVenusLevelToHintChance[blueVenusLevel]);
	HaveBlueVenusHint = false;
	HaveHint = GetZeroOneRandomFloat() <= hintChance;
    IsShining = Bond >= 80 && CardData.Type == static_cast<int>(trainType);

    switch (CardData.CardId)
    {
    case 30107:
        CardData.TrainBonus = 5 + 5 * facilityLevel;
        break;

    case 30134:
        if (currentTurn <= 32)
        {
            CardData.TrainBonus = 4 + currentTurn * 0.5f;
        }
        break;

    case 30137:
        if (Bond == 100)
        {
            CardData.FriendBonus = 32;
            CardData.MotivationBonus = 15;
            CardData.StatusBonus[5] = 1;
        }
        break;

    case 30130:
        if (Bond >= 80)
        {
            CardData.StatusBonus[5] = 2;
        }
        break;

    case 30037:
        if (Bond >= 80)
        {
            CardData.StatusBonus[0] = 2;
        }
        break;

    case 30147:
        if (Bond == 100)
        {
            CardData.StatusBonus[0] = 3;
        }
        break;

    case 30152:
        if (Bond >= 80)
        {
            CardData.StatusBonus[0] = 2;
        }
        break;

    case 30153:
        if (Bond == 100)
        {
            CardData.StatusBonus[3] = 3;
        }
        break;

    case 30101:
        if (Bond == 100)
        {
            CardData.FriendBonus = 44;
        }
        break;

    default:
        break;
    }
}

void SupportCard::Click()
{
	Bond = std::min(100, Bond + 7);
}

void SupportCard::TriggerHint()
{
	Bond = std::min(100, Bond + 5);
}

void SupportCard::TriggerBlueVenusHint()
{
	Bond = std::min(100, Bond + 5);
}

void SupportCard::BlueWisdom()
{
	HaveHint = false;
	HaveBlueVenusHint = true;
}

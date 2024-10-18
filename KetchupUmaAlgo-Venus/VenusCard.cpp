#include "VenusCard.h"

VenusCard::VenusCard(SupportCardData cardData, bool haveHint, int currentBond) : SupportCard(cardData, haveHint, currentBond)
{
}

void VenusCard::Init(bool isActivated, bool canHangOut, bool isZone, int zoneContinueTurns)
{
	IsActivated = isActivated;
	CanHangOut = canHangOut;
	IsZone = isZone;
	ZoneContinueTurns = zoneContinueTurns;
	TriggerSpecialEvent = false;
	SpecialEventChance = 0.4f;
}

void VenusCard::UpdateStatus(Train trainType, int facilityLevel, int currentTurn, int blueVenusLevel)
{
	if (!CanHangOut && IsActivated)
	{
		float hangOutEventRate = Bond > 60 ? 0.25f : 0.05f;
		if (GetZeroOneRandomFloat() <= hangOutEventRate)
		{
			CanHangOut = true;
			IsZone = true;
			ZoneContinueTurns = 0;
			Bond = std::min(100, Bond + 5);
		}
	}

	HaveBlueVenusHint = false;
	TriggerSpecialEvent = false;
	SpecialEventChance = 0.4f + 0.004f * static_cast<float>(BlueVenusLevelToHintChance[blueVenusLevel]);

	if (IsZone)
	{
		IsShining = true;
		ZoneContinueTurns += 1;
	}
	else
	{
		IsShining = false;
	}

}

void VenusCard::Click()
{
	Bond = std::min(100, Bond + 5);
	IsActivated = true;
}

void VenusCard::TriggerHint()
{
	// this card has no hint event
}

void VenusCard::TriggerBlueVenusHint()
{
	TriggerSpecialEvent = true;
}

void VenusCard::CheckZoneEnd()
{
	if (!IsZone) return;
	if (GetZeroOneRandomFloat() <= ZoneLastTurnChance[ZoneContinueTurns])
	{
		IsZone = false;
		IsShining = false;
		ZoneContinueTurns = 0;
	}
}

void VenusCard::SpecialEvent()
{
	if (TriggerSpecialEvent && CanHangOut && !IsZone)
	{
		IsZone = true;
		IsShining = true;
		ZoneContinueTurns = 0;
		Bond = std::min(100, Bond + 5);
		return;
	}
	Bond = std::min(100, Bond + 5);
}

void VenusCard::_rollSpecialEvent()
{
	if (GetZeroOneRandomFloat() <= SpecialEventChance)
	{
		TriggerSpecialEvent = true;
	}
}

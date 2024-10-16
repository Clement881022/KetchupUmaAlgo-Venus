#pragma once

#include "SupportCard.h"

class VenusCard : public SupportCard
{
public:
	VenusCard(SupportCardData cardData, bool haveHint, int currentBond);
	void Init(bool isActivated, bool canHangOut, bool isZone, int zoneContinueTurns);

	void UpdateStatus(Train trainType, int facilityLevel, int currentTurn, int blueVenusLevel) override;
	void Click() override;
	void TriggerHint() override;
	void TriggerBlueVenusHint() override;
	void CheckZoneEnd();
	void SpecialEvent();

	bool IsActivated;
	bool CanHangOut;
	bool IsZone;
	int ZoneContinueTurns;
	bool TriggerSpecialEvent;
	float SpecialEventChance;

private:
	void _rollSpecialEvent();
};

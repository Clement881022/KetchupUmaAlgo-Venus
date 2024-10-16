#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "Globals.h"

class SupportCard
{
public:
    SupportCard(SupportCardData cardData, bool haveHint, int currentBond);
    void Reset(SupportCardData cardData, bool haveHint, int currentBond);
    void Event();
    void BlueWisdom();

    virtual void UpdateStatus(Train trainType, int facilityLevel, int currentTurn, int blueVenusLevel);
    virtual void Click();
    virtual void TriggerHint();
    virtual void TriggerBlueVenusHint();

    SupportCardData CardData;
    int Bond;
    bool HaveHint;
    bool HaveBlueVenusHint;
    bool IsShining;
};
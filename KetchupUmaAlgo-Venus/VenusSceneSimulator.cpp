#include "VenusSceneSimulator.h"

VenusSceneSimulator::VenusSceneSimulator(TurnData thisTurn)
{
	UmaId = thisTurn.UmaId;
	UmaLimitBreak = thisTurn.UmaLimitBreak;
	Turn = thisTurn.Turn;
	Vital = thisTurn.Vital;
	MaxVital = thisTurn.MaxVital;
	Motivation = thisTurn.Motivation;
	CurrentStatus = thisTurn.Status;
	StatusLimit = thisTurn.StatusLimit;
	InheritStatus = thisTurn.InheritStatus;
	IsVenusCardHangoutUsed = thisTurn.IsVenusCardHangoutUsed;
	SpiritDistribution = thisTurn.SpiritDistribution;
	VenusCardPtr = nullptr;

	for (int i = 0; i < 6; i++)
	{
		SupportCardData cardData = DataManager::Instance().GetSupportCardData(thisTurn.CardId[i], thisTurn.CardLimitBreak[i]);

		if (cardData.CardId == 30137)
		{
			Deck[i] = std::make_unique<VenusCard>(cardData, thisTurn.CardHint[i], thisTurn.CardBond[i]);
			VenusCardPtr = static_cast<VenusCard*>(Deck[i].get());
			VenusCardPtr->Init(thisTurn.IsVenusCardActivated, thisTurn.CanVenusCardHangout, thisTurn.IsVenusCardZone, thisTurn.ZoneContinueTurns);
		}
		else
		{
			Deck[i] = std::make_unique<SupportCard>(cardData, thisTurn.CardHint[i], thisTurn.CardBond[i]);
		}
	}

	for (int i = 0; i < 5; i++)
	{
		Facilities[i] = std::make_unique<Facility>(static_cast<Train>(i), static_cast<float>(thisTurn.FacilityLevelScore[i]));
	}

	for (int i = 0; i < 6; i++)
	{
		if (thisTurn.CardDistribution[i] == Train::None) { continue; }
		int index = static_cast<int>(thisTurn.CardDistribution[i]);
		Facilities[index]->SetSupportCard(Deck[i].get());
	}

	TowerSystem = std::make_unique<SpiritTower>(thisTurn.VenusSpirits, thisTurn.VenusLevel);
	_continuousRaceCount = 0;
	UmaDatas = DataManager::Instance().UmaMap.at(UmaId);

	_updateSupportCards();
	_updateFailRate();
	_updateTrainValue();
	_updateVitalCost();
	_updateVenusCard();
	_updateVenusHangout();
	_resetRaceBonus();
	_updateStatusRatio();
}

void VenusSceneSimulator::Reset(TurnData thisTurn)
{
	UmaId = thisTurn.UmaId;
	UmaLimitBreak = thisTurn.UmaLimitBreak;
	Turn = thisTurn.Turn;
	Vital = thisTurn.Vital;
	MaxVital = thisTurn.MaxVital;
	Motivation = thisTurn.Motivation;
	CurrentStatus = thisTurn.Status;
	StatusLimit = thisTurn.StatusLimit;
	InheritStatus = thisTurn.InheritStatus;
	IsVenusCardHangoutUsed = thisTurn.IsVenusCardHangoutUsed;
	SpiritDistribution = thisTurn.SpiritDistribution;

	for (int i = 0; i < 6; i++)
	{
		SupportCardData cardData = DataManager::Instance().GetSupportCardData(thisTurn.CardId[i], thisTurn.CardLimitBreak[i]);
		Deck[i]->Reset(cardData, thisTurn.CardHint[i], thisTurn.CardBond[i]);
	}
	VenusCardPtr->Init(thisTurn.IsVenusCardActivated, thisTurn.CanVenusCardHangout, thisTurn.IsVenusCardZone, thisTurn.ZoneContinueTurns);

	for (int i = 0; i < 5; i++)
	{
		Facilities[i]->Reset(static_cast<float>(thisTurn.FacilityLevelScore[i]));
	}

	for (int i = 0; i < 6; i++)
	{
		if (thisTurn.CardDistribution[i] == Train::None) { continue; }
		int index = static_cast<int>(thisTurn.CardDistribution[i]);
		Facilities[index]->SetSupportCard(Deck[i].get());
	}

	TowerSystem->Reset(thisTurn.VenusSpirits, thisTurn.VenusLevel);
	_continuousRaceCount = 0;
	UmaDatas = DataManager::Instance().UmaMap.at(UmaId);

	_updateSupportCards();
	_updateFailRate();
	_updateTrainValue();
	_updateVitalCost();
	_updateVenusCard();
	_updateVenusHangout();
	_resetRaceBonus();
	_updateStatusRatio();
}

void VenusSceneSimulator::PrintInfo()
{
	std::cout << std::endl;
	std::cout << "=================================\n";
	std::cout << "Turn: " << Turn << "\n";
	std::cout << "UmaId: " << UmaId << "\n";
	std::cout << "UmaLimitBreak: " << UmaLimitBreak << "\n";
	std::cout << "Vital: " << Vital << "/" << MaxVital << "\n";
	std::cout << "Motivation: " << Motivation << "\n";
	std::cout << "CurrentStatus: ";
	for (int i = 0; i < 6; ++i)
		std::cout << CurrentStatus[i] << " ";
	std::cout << "\n";
	std::cout << "StatusLimit: ";
	for (int i = 0; i < 6; ++i)
		std::cout << StatusLimit[i] << " ";
	std::cout << "\n";
	std::cout << "InheritStatus: ";
	for (int i = 0; i < 6; ++i)
		std::cout << InheritStatus[i] << " ";
	std::cout << "\n";
	std::cout << "FailRate: ";
	for (int i = 0; i < 5; ++i)
		std::cout << FailRate[i] << " ";
	std::cout << "\n";
	std::cout << "VitalCost: ";
	for (int i = 0; i < 5; ++i)
		std::cout << VitalCost[i] << " ";
	std::cout << "\n";
	std::cout << "TrainValue: ";
	for (int i = 0; i < 5; ++i)
	{
		std::cout << "[";
		for (int j = 0; j < 6; ++j)
			std::cout << TrainValue[i][j] << (j < 5 ? ", " : "");
		std::cout << "] ";
	}
	std::cout << "\n";
	std::cout << "Race Bonus: " << _raceBonus << "\n";
	std::cout << "Continuous Race Count: " << _continuousRaceCount << "\n";
	std::cout << "Status Ratio: ";
	for (float ratio : _statusRatio)
		std::cout << ratio << " ";
	std::cout << "\n\n";
	for (int i = 0; i < 5; i++)
	{
		std::cout << TrainToString[i] << std::endl;
		for (SupportCard* card : Facilities[i]->SupportCards)
		{
			if (card->HaveBlueVenusHint) std::cout << "[!!]";
			if (card->HaveHint) std::cout << "[!]";
			if (card->IsShining) std::cout << "[*]";
			std::cout << card->CardData.Name;
			std::cout << "(" << card->Bond << ")";
			std::cout << std::endl;
			if (card == VenusCardPtr)
			{
				std::cout << "IsActivated: " << VenusCardPtr->IsActivated << std::endl;
				std::cout << "CanHangOut: " << VenusCardPtr->CanHangOut << std::endl;
				std::cout << "IsZone: " << VenusCardPtr->IsZone << std::endl;
				std::cout << "ZoneContinueTurns: " << VenusCardPtr->ZoneContinueTurns << std::endl;
				std::cout << "TriggerSpecialEvent: " << VenusCardPtr->TriggerSpecialEvent << std::endl;
			}
		}
		std::cout << std::endl;
	}
	std::cout << "Trains: ";
	for (int i = 0; i < 5; i++)
	{
		std::cout << _decisionValue[i] << " ";
	}
	std::cout << std::endl << "Rests: ";
	for (int i = 5; i < 7; i++)
	{
		std::cout << _decisionValue[i] << " ";
	}
	std::cout << std::endl << "Hangouts: ";
	for (int i = 7; i < 13; i++)
	{
		std::cout << _decisionValue[i] << " ";
	}
	std::cout << std::endl << "Races: ";
	std::cout << _decisionValue[13] << std::endl;
	std::cout << std::endl;
}

float VenusSceneSimulator::Simulate()
{
	while (Turn <= 77)
	{
		ResetTurn();
		BeforeTurnEvent();
		_beforeTurnDecision = _evaluateBeforeTurnDecision();
		BeforeTurnDecision(_beforeTurnDecision);
		_duringTurnDecision = _evaluateDuringTurnDecision();
		DuringTurnDecision(_duringTurnDecision);
		AfterTurnEvent();
		_afterTurnDecision = _evaluateAfterTurnDecision();
		AfterTurnDecision(_afterTurnDecision);
		Turn += 1;
	}
	return _evaluateStatusScore(CurrentStatus);
}

void VenusSceneSimulator::ResetTurn()
{
	for (int i = 0; i < 5; i++) { Facilities[i]->ResetTurn(); }
	_reshuffleSupportCard();
	_updateSupportCards();
	_reshuffleSpirits();
	_updateFailRate();
	_updateTrainValue();
	_updateVenusCard();
	_updateVenusHangout();
	_handleContinueRaces();
	TowerSystem->Clear();
	_resetRaceBonus();
	_updateStatusRatio();
}

void VenusSceneSimulator::BeforeTurnEvent()
{
	// random events
	if (Turn % 2 == 0)
	{
		StatusChange({ 2,2,2,2,2,6 }, false);
	}
	if (Turn <= 23)
	{
		MotivationChange(0.1f);
	}
	else if (Turn <= 47)
	{
		MotivationChange(0.05f);
	}
	MotivationChange(-0.02f);

	if (GetZeroOneRandomFloat() <= 0.15f)
	{
		int randomCardIndex = GetZeroOneRandomFloat() * Deck.size();
		if (Deck[randomCardIndex].get() != VenusCardPtr)
		{
			Deck[randomCardIndex]->Event();
		}
	}

	// event
	if (Turn == 24)
	{
		StatusChange({ 5,5,5,5,5,0 }, false);
		if (VenusCardPtr->CanHangOut)
		{
			VitalChange(19);
			StatusChange({ 0,0,0,0,0,60 }, false);
		}
	}
	else if (Turn == 30)
	{
		StatusChange(InheritStatus, false);
	}
	else if (Turn >= 36 && Turn <= 39)
	{
		for (int i = 0; i < 5; i++)
		{
			Facilities[i]->Level = 4;
		}
	}
	else if (Turn == 48)
	{
		StatusChange({ 8,8,8,8,8,0 }, false);
	}
	else if (Turn == 49)
	{
		float random = GetZeroOneRandomFloat();
		if (random <= 0.16)
		{
			VitalChange(30);
			StatusChange({ 10,10,10,10,10,0 }, false);
			MotivationChange(2.0f);
		}
		else if (random > 0.16 && random <= 0.43)
		{
			VitalChange(20);
			StatusChange({ 5,5,5,5,5,0 }, false);
			MotivationChange(1.0f);
		}
		else if (random > 0.43 && random <= 0.89)
		{
			VitalChange(20);
		}
		else
		{
			MotivationChange(-1.0f);
		}
	}
	else if (Turn == 54)
	{
		StatusChange(InheritStatus, false);
	}
	else if (Turn >= 60 && Turn <= 63)
	{
		for (int i = 0; i < 5; i++)
		{
			Facilities[i]->Level = 4;
		}
	}
}

void VenusSceneSimulator::BeforeTurnDecision(bool activated)
{
	if (!activated) return;
	Color color = TowerSystem->Tower[0][0].SpiritColor;
	if (color == Color::None) return;

	switch (color)
	{
	case Color::Red:
		VitalChange(50);
		MotivationChange(4.0f);
		_raceBonus *= 1.35f;
		for (auto& facility : Facilities)
		{
			facility->RedWisdom();
		}
		break;
	case Color::Blue:
		for (auto& facility : Facilities)
		{
			facility->BlueWisdom();
		}
		break;
	case Color::Yellow:
		for (auto& facility : Facilities)
		{
			facility->YellowWisdom();
		}
		break;
	default:
		break;
	}
	TowerSystem->Activate();
	_updateFailRate();
	_updateTrainValue();
	_updateVitalCost();
}

void VenusSceneSimulator::DuringTurnDecision(Decision decision)
{
	switch (decision)
	{
	case Decision::TrainSpeed:
		Click(Train::Speed);
		break;
	case Decision::TrainStamina:
		Click(Train::Stamina);
		break;
	case Decision::TrainPower:
		Click(Train::Power);
		break;
	case Decision::TrainWill:
		Click(Train::Will);
		break;
	case Decision::TrainInt:
		Click(Train::Int);
		break;
	case Decision::Rest:
		Rest();
		break;
	case Decision::SummerRest:
		SummerRest();
		break;
	case Decision::Hangout:
		Hangout();
		break;
	case Decision::HangoutRed:
		VenusHangout(0);
		break;
	case Decision::HangoutBlue:
		VenusHangout(1);
		break;
	case Decision::HangoutYellow:
		VenusHangout(2);
		break;
	case Decision::HangoutAll:
		VenusHangout(3);
		break;
	case Decision::HangoutEnd:
		VenusHangout(4);
		break;
	case Decision::Race:
		Race();
		break;
	case Decision::TargetRace:
		TargetRace();
		break;
	case Decision::PreferRace:
		PreferRace();
		break;
	default:
		break;
	}
}

void VenusSceneSimulator::AfterTurnEvent()
{
	int redVenusLevel = TowerSystem->VenusLevel[0];
	int blueVenusLevel = TowerSystem->VenusLevel[1];
	int yellowVenusLevel = TowerSystem->VenusLevel[2];

	if (Turn == 23)
	{
		StatusChange(YearlyRaceAward[0], true);
		if (blueVenusLevel >= 1) StatusChange({ 0,0,0,0,0,10 }, false);
		if (redVenusLevel >= 1) StatusChange({ 0,0,0,0,0,20 }, true);
		if (yellowVenusLevel >= 1) RandomStatusChange(10, true);
		for (int i = 0; i < 5; i++)
		{
			Facilities[i]->VenusRace();
		}
	}
	else if (Turn == 47)
	{
		StatusChange(YearlyRaceAward[1], true);
		if (blueVenusLevel >= 2) StatusChange({ 0,0,0,0,0,20 }, false);
		if (redVenusLevel >= 2) StatusChange({ 0,0,0,0,0,30 }, true);
		if (yellowVenusLevel >= 2)
		{
			RandomStatusChange(10, true);
			RandomStatusChange(10, true);
		}
		for (int i = 0; i < 5; i++)
		{
			Facilities[i]->VenusRace();
		}
	}
	else if (Turn == 71)
	{
		StatusChange(YearlyRaceAward[2], true);
		if (blueVenusLevel >= 3) StatusChange({ 0,0,0,0,0,30 }, false);
		if (redVenusLevel >= 3) StatusChange({ 0,0,0,0,0,45 }, true);
		if (yellowVenusLevel >= 3)
		{
			RandomStatusChange(10, true);
			RandomStatusChange(10, true);
			RandomStatusChange(10, true);
		}
		for (int i = 0; i < 5; i++)
		{
			Facilities[i]->VenusRace();
		}
	}
	else if (Turn == 77)
	{
		StatusChange(YearlyRaceAward[3], true);
		if (redVenusLevel >= 3 && blueVenusLevel >= 3 && yellowVenusLevel >= 3)
		{
			int masterOfGoodFieldPtValue = DataManager::Instance().MasterOfGoodFieldPtValue;
			StatusChange({ 0,0,0,0,0,masterOfGoodFieldPtValue }, false);
		}
		if (redVenusLevel >= 4 || blueVenusLevel >= 4 || yellowVenusLevel >= 4)
		{
			StatusChange({ 10,10,10,10,10,40 }, false);
		}
		if (IsVenusCardHangoutUsed[4])
		{
			EventStatusChange({ 2,2,2,2,2,10 });
			StatusChange({ 0,0,0,0,0,60 }, false);
		}
		EventStatusChange({ 8,8,8,8,8,0 });
		StatusChange({ 5,5,5,5,5,20 }, false);
	}

	if (Turn == 41 && UmaId == 105001)
	{
		MotivationChange(-2);
	}
	else if (Turn == 53 && UmaId == 105001)
	{
		StatusChange({ 20,20,20,20,20,70 }, true);
	}
}

void VenusSceneSimulator::AfterTurnDecision(Color color)
{
	if (!VenusCardPtr->TriggerSpecialEvent) return;

	if (color == Color::Random)
	{
		int randomColorIndex = GetZeroOneRandomFloat() * 3;
		color = static_cast<Color>(randomColorIndex);
	}

	switch (color)
	{
	case Color::Red:
		StatusChange({ 0,0,0,0,0,6 }, false);
		break;
	case Color::Blue:
		StatusChange({ 4,0,0,0,0,2 }, false);
		break;
	case Color::Yellow:
		StatusChange({ 0,4,0,0,0,2 }, false);
		break;
	default:
		break;
	}

	int randomStatusIndex = GetZeroOneRandomFloat() * 6;
	Status status = static_cast<Status>(randomStatusIndex);

	SpiritData data = SpiritData(color, status, 0, 1);
	TowerSystem->Push(data);
}

void VenusSceneSimulator::StatusChange(std::array<int, 6> status, bool takeRaceBonus)
{
	float raceBonus = takeRaceBonus ? _raceBonus : 1.0f;

	for (int i = 0; i < 6; i++)
	{
		int result = CurrentStatus[i] > 1200 ? CurrentStatus[i] * 2 - 1200 : CurrentStatus[i];
		result += static_cast<int>((static_cast<float>(status[i]) * raceBonus));
		result = result > 1200 ? (result + 1200) / 2 : result;
		result = std::min(StatusLimit[i], result);
		result = std::max(0, result);
		CurrentStatus[i] = result;
	}
}

void VenusSceneSimulator::RandomStatusChange(int amount, bool takeRaceBonus)
{
	std::array<int, 6> status = { 0,0,0,0,0,0 };
	int randomIndex = static_cast<int>(GetZeroOneRandomFloat() * 5.0f);
	status[randomIndex] = amount;
	StatusChange(status, takeRaceBonus);
}

void VenusSceneSimulator::EventStatusChange(std::array<int, 6> status)
{
	int yellowVenusLevel = TowerSystem->VenusLevel[2];
	float ratio = YellowVenusLevelToEventBonus[yellowVenusLevel];
	for (int i = 0; i < 6; i++)
	{
		status[i] = static_cast<int>(static_cast<float>(status[i]) * ratio);
	}
	StatusChange(status, false);
}

void VenusSceneSimulator::VitalChange(int amount)
{
	Vital += amount;
	Vital = std::min(MaxVital, Vital);
	Vital = std::max(0, Vital);
}

void VenusSceneSimulator::MotivationChange(float amount)
{
	bool isMotivationUp = amount >= 0;
	int intAmount = static_cast<int>(abs(amount));
	bool extraChange = GetZeroOneRandomFloat() < (abs(amount) - intAmount);

	if (isMotivationUp)
	{
		Motivation = std::min(Motivation + intAmount, 4);
		if (extraChange) { Motivation = std::min(Motivation + 1, 4); }
	}
	else
	{
		Motivation = std::max(Motivation - intAmount, 0);
		if (extraChange) { Motivation = std::max(Motivation - 1, 0); }
	}
}

void VenusSceneSimulator::Click(Train train)
{
	int trainIndex = static_cast<int>(train);
	float random = GetZeroOneRandomFloat();
	float failChance = FailRate[trainIndex];
	float criticalFailChance = failChance * failChance;

	if (random <= criticalFailChance && train != Train::Int)
	{
		MotivationChange(-3.0f);
		RandomStatusChange(-10, false);
		RandomStatusChange(-10, false);
		RandomStatusChange(-10, false);
	}
	else if (random <= failChance && train != Train::Int)
	{
		MotivationChange(-1.0f);
		std::array<int, 6> failStatus = { 0,0,0,0,0,0 };
		failStatus[trainIndex] = -5;
		StatusChange(failStatus, false);
	}
	else if (random <= failChance && train != Train::Int)
	{
		VitalChange(5);
	}
	else
	{
		VitalChange(VitalCost[trainIndex]);
		StatusChange(TrainValue[trainIndex], false);
		_pushSpirit(train);
		Facilities[trainIndex]->Click();
	}

	_continuousRaceCount = 0;
}

void VenusSceneSimulator::Rest()
{
	float random = GetZeroOneRandomFloat();
	if (random <= 0.134f) { VitalChange(30); }
	else if (random <= 0.134f + 0.613f) { VitalChange(50); }
	else { VitalChange(70); }
	_pushSpirit(Train::Rest);
	_continuousRaceCount = 0;
}

void VenusSceneSimulator::SummerRest()
{
	VitalChange(40);
	MotivationChange(1.0f);
	_pushSpirit(Train::Rest);
	_continuousRaceCount = 0;
}

void VenusSceneSimulator::Hangout()
{
	float random = GetZeroOneRandomFloat();
	if (random <= 0.364f)
	{
		MotivationChange(2.0f);
	}
	else if (random <= 0.364f + 0.281f)
	{
		VitalChange(10);
		MotivationChange(1.0f);
	}
	else if (random <= 0.364f + 0.281f + 0.207f)
	{
		VitalChange(10);
		MotivationChange(1.0f);
	}
	else if (random <= 0.364f + 0.281f + 0.207f + 0.107f)
	{
		VitalChange(20);
		MotivationChange(1.0f);
	}
	else
	{
		VitalChange(30);
		MotivationChange(1.0f);
	}

	if (Turn >= 24 && Turn <= 71 && GetZeroOneRandomFloat() < 0.2f)
	{
		VitalChange(10);
		MotivationChange(1.0f);
	}

	_pushSpirit(Train::Hangout);
	_continuousRaceCount = 0;
}

void VenusSceneSimulator::VenusHangout(int index)
{
	if (!VenusCardHangoutAvailable[index]) return;

	switch (index)
	{
	case 0:
		VitalChange(45);
		StatusChange({ 0,0,0,0,0,45 }, false);
		MotivationChange(1.0f);
		_pushSpirit(Train::Hangout);
		VenusCardPtr->Event();
		IsVenusCardHangoutUsed[0] = true;
		break;
	case 1:
		VitalChange(32);
		StatusChange({ 12,0,0,0,12,0 }, false);
		MotivationChange(1.0f);
		_pushSpirit(Train::Hangout);
		VenusCardPtr->Event();
		IsVenusCardHangoutUsed[1] = true;
		break;
	case 2:
		MaxVital += 4;
		VitalChange(32);
		StatusChange({ 0,8,8,8,0,0 }, false);
		MotivationChange(1.0f);
		_pushSpirit(Train::Hangout);
		VenusCardPtr->Event();
		IsVenusCardHangoutUsed[2] = true;
		break;
	case 3:
		VitalChange(45);
		StatusChange({ 0,15,15,15,0,0 }, false);
		MotivationChange(1.0f);
		_pushSpirit(Train::Hangout);
		VenusCardPtr->Event();
		IsVenusCardHangoutUsed[3] = true;
		break;
	case 4:
		VitalChange(52);
		StatusChange({ 9,9,9,9,9,36 }, false);
		MotivationChange(1.0f);
		_pushSpirit(Train::Hangout);
		VenusCardPtr->SpecialEvent();
		IsVenusCardHangoutUsed[4] = true;
		break;
	default:
		break;
	}
	_continuousRaceCount = 0;
}

void VenusSceneSimulator::Race()
{
	VitalChange(-15);
	RandomStatusChange(8, true);
	StatusChange({ 0,0,0,0,0,35 }, true);
	MotivationChange(0.2);
	_pushSpirit(Train::Race);
	_continuousRaceCount += 1;
}

void VenusSceneSimulator::TargetRace()
{
	StatusChange({ 3,3,3,3,3,35 }, true);
	_pushSpirit(Train::Race);
	_continuousRaceCount += 1;
}

void VenusSceneSimulator::PreferRace()
{
	VitalChange(-15);
	RandomStatusChange(10, true);
	StatusChange({ 0,0,0,0,0,45 }, true);
	MotivationChange(0.2);
	_pushSpirit(Train::Race);
	_continuousRaceCount += 1;
}

bool VenusSceneSimulator::IsTargetRace()
{
	return any_of(UmaDatas.TargetRaces.begin(), UmaDatas.TargetRaces.end(), [this](int num) {return num == Turn; });
}

bool VenusSceneSimulator::IsPreferRace()
{
	if (!DataManager::Instance().UsePreferedRace) return false;
	return any_of(UmaDatas.PreferRaces.begin(), UmaDatas.PreferRaces.end(), [this](int num) {return num == Turn; });
}

bool VenusSceneSimulator::IsLegal(Decision decision)
{
	bool isSummerVacation = (Turn >= 36 && Turn <= 39) || (Turn >= 60 && Turn <= 63);
	bool haveRace = !(Turn <= 14 || Turn >= 72);
	switch (decision)
	{
	case Decision::TrainSpeed:
	case Decision::TrainStamina:
	case Decision::TrainPower:
	case Decision::TrainWill:
	case Decision::TrainInt:
		return !IsTargetRace() && !IsPreferRace();

	case Decision::Rest:
	case Decision::Hangout:
		return !IsTargetRace() && !isSummerVacation && !IsPreferRace();

	case Decision::Race:
		return !IsTargetRace() && haveRace && !isSummerVacation && !IsPreferRace();

	case Decision::SummerRest:
		return !IsTargetRace() && isSummerVacation && !IsPreferRace();

	case Decision::HangoutRed:
		return !IsTargetRace() && !isSummerVacation && VenusCardHangoutAvailable[0] && !IsPreferRace();

	case Decision::HangoutBlue:
		return !IsTargetRace() && !isSummerVacation && VenusCardHangoutAvailable[1] && !IsPreferRace();

	case Decision::HangoutYellow:
		return !IsTargetRace() && !isSummerVacation && VenusCardHangoutAvailable[2] && !IsPreferRace();

	case Decision::HangoutAll:
		return !IsTargetRace() && !isSummerVacation && VenusCardHangoutAvailable[3] && !IsPreferRace();

	case Decision::HangoutEnd:
		return !IsTargetRace() && !isSummerVacation && VenusCardHangoutAvailable[4] && !IsPreferRace();

	case Decision::TargetRace:
		return IsTargetRace();

	case Decision::PreferRace:
		return !IsTargetRace() && IsPreferRace();
	default:
		return false;
	}
}

bool VenusSceneSimulator::CanActivate()
{
	return TowerSystem->IsFull();
}

bool VenusSceneSimulator::SpiritEventPossible(Decision decision)
{
	int decisionIndex = static_cast<int>(decision);
	if (decisionIndex <= 4)
	{
		for (SupportCard* card : Facilities[decisionIndex]->SupportCards)
		{
			if (card == VenusCardPtr) return true;
		}
	}
	return false;
}

void VenusSceneSimulator::_pushSpirit(Train train)
{
	int index = static_cast<int>(train);
	SpiritData data = SpiritDistribution[index];
	TowerSystem->Push(data);

	for (int i = 0; i < data.SpiritAmount; i++)
	{
		if (index <= 4) Facilities[index]->LevelScore += 0.2f;
		if (data.SpiritStatus != Status::SkillPt) Facilities[static_cast<int>(data.SpiritStatus)]->LevelScore += 1;
	}
}

void VenusSceneSimulator::_reshuffleSupportCard()
{
	std::vector<int> weight = { 100,100,100,100,100,0,0,0,50 };
	for (int i = 0; i < 6; i++)
	{
		weight = { 100,100,100,100,100,0,0,0,50 };
		Train cardType = (Deck[i]->CardData.Type <= 4) ? static_cast<Train>(Deck[i]->CardData.Type) : Train::None;
		float ratio = 1.0f + Deck[i]->CardData.MasterfulLevel / 100.0f;
		weight[static_cast<int>(cardType)] *= ratio;
		int random = WeightedRandom(weight);
		if (random <= 4) Facilities[random]->SetSupportCard(Deck[i].get());
	}
}

void VenusSceneSimulator::_updateSupportCards()
{
	for (int i = 0; i < 5; i++)
	{
		int blueVenusLevel = TowerSystem->VenusLevel[1];
		Facilities[i]->UpdateSupportCards(Turn, blueVenusLevel);
	}
}

void VenusSceneSimulator::_reshuffleSpirits()
{
	// randomize spirit color
	std::array<Color, 3> colors = { Color::Red,Color::Blue,Color::Yellow };
	std::shuffle(colors.begin(), colors.end(), generator);
	SpiritDistribution[0].SpiritColor = colors[0];
	SpiritDistribution[1].SpiritColor = colors[0];
	SpiritDistribution[2].SpiritColor = colors[1];
	SpiritDistribution[3].SpiritColor = colors[1];
	SpiritDistribution[4].SpiritColor = colors[2];
	SpiritDistribution[5].SpiritColor = colors[0];
	SpiritDistribution[6].SpiritColor = colors[1];
	SpiritDistribution[7].SpiritColor = colors[2];
	std::shuffle(SpiritDistribution.begin(), SpiritDistribution.begin() + 5, generator);

	// randomize spirit status
	for (int i = 0; i < 8; i++)
	{
		std::vector<int> vec(SpiritTypeWeight[i].begin(), SpiritTypeWeight[i].end());
		int statusRandomIndex = WeightedRandom(vec);
		SpiritDistribution[i].SpiritStatus = static_cast<Status>(statusRandomIndex);
	}

	// randomize spirit amount
	for (int i = 0; i < 4; i++)
	{
		SpiritDistribution[i].SpiritAmount = Facilities[i]->HaveShine() ? 2 : 1;
	}
	SpiritDistribution[4].SpiritAmount = Facilities[4]->HaveShine() && GetZeroOneRandomFloat() <= 0.4 ? 2 : 1;
	for (int i = 5; i < 8; i++)
	{
		SpiritDistribution[i].SpiritAmount = GetZeroOneRandomFloat() <= 0.2 ? 2 : 1;
	}

	// reset status amount
	for (int i = 0; i < 8; i++)
	{
		SpiritDistribution[i].StatusAmount = 0;
	}
}

void VenusSceneSimulator::_updateFailRate()
{
	for (int i = 0; i < 5; i++)
	{
		int level = Facilities[i]->Level;
		int failRateStardard = FailRateStandard[i][level];
		FailRate[i] = (100 - Vital) * (failRateStardard * 0.1f - Vital) / 4000;
		FailRate[i] = std::min(FailRate[i], 1.0f);
		FailRate[i] = std::max(FailRate[i], 0.0f);
	}
}

void VenusSceneSimulator::_updateTrainValue()
{
	for (int trainIndex = 0; trainIndex < 5; trainIndex++)
	{
		for (int statusIndex = 0; statusIndex < 6; statusIndex++)
		{
			int level = Facilities[trainIndex]->Level;
			int basicValue = BasicTrainValue[trainIndex][level][statusIndex];
			if (basicValue == 0) { TrainValue[trainIndex][statusIndex] = 0; continue; }

			int statusBonus = 0;
			float growRate = 1.0f + UmaDatas.StatusBonus[statusIndex] / 100.0f;
			float trainBonus = 1.0f;
			float motivationBonus = MotivationEffect[Motivation];
			float friendBonus = 1.0f;
			float headBonus = 1.0f + Facilities[trainIndex]->SupportCards.size() * 0.05f;
			int spiritBonus = TowerSystem->GetStatusBonus()[statusIndex];
			float venusBonus = 1.0f;

			for (SupportCard* card : Facilities[trainIndex]->SupportCards)
			{
				statusBonus += card->CardData.StatusBonus[statusIndex];
				trainBonus += card->CardData.TrainBonus / 100.0f;
				friendBonus *= (1.0f + card->CardData.FriendBonus / 100.0f);
			}

			for (int i = 0; i < 3; i++)
			{
				int venusLevel = TowerSystem->VenusLevel[i];
				venusBonus += VenusLevelToTrainBonus[venusLevel];
			}

			int floorTrainValue = (basicValue + statusBonus) * growRate * trainBonus * motivationBonus * friendBonus * headBonus;
			int sceneTrainValue = (floorTrainValue + spiritBonus) * venusBonus;
			TrainValue[trainIndex][statusIndex] = sceneTrainValue;
		}

		// simulate hint
		for (SupportCard* card : Facilities[trainIndex]->SupportCards)
		{
			if (card->HaveHint)
			{
				TrainValue[trainIndex][5] += 20;
				break;
			}
		}

		// simulate BlueVenusHint
		std::array<int, 6> blueVenusHintStatus = { 0,0,0,0,0,0 };
		for (SupportCard* card : Facilities[trainIndex]->SupportCards)
		{
			if (!card->HaveBlueVenusHint || card == VenusCardPtr) continue;
			for (int statusIndex = 0; statusIndex < 6; statusIndex++)
			{
				blueVenusHintStatus[statusIndex] += BlueVenusHintValue[card->CardData.Type][statusIndex];
			}
		}

		std::array<int, 6> blueVenusSpiritStatus = { 0,0,0,0,0,0 };
		for (int statusIndex = 0; statusIndex < 6; statusIndex++)
		{
			if (blueVenusHintStatus[statusIndex] != 0)
			{
				blueVenusSpiritStatus[statusIndex] = TowerSystem->GetStatusBonus()[statusIndex];
			}
		}

		for (int statusIndex = 0; statusIndex < 6; statusIndex++)
		{
			TrainValue[trainIndex][statusIndex] += blueVenusHintStatus[statusIndex];
			TrainValue[trainIndex][statusIndex] += blueVenusSpiritStatus[statusIndex];
		}
	}
}

void VenusSceneSimulator::_updateVitalCost()
{
	int redVenusLevel = TowerSystem->VenusLevel[0];
	float redVenusDiscount = RedVenusLevelToVitalDiscount[redVenusLevel];
	for (int trainIndex = 0; trainIndex < 4; trainIndex++)
	{
		int level = Facilities[trainIndex]->Level;
		VitalCost[trainIndex] = BasicTrainValue[trainIndex][level][6];
		if (VitalCost[trainIndex] < 0)
		{
			VitalCost[trainIndex] *= redVenusDiscount;
		}
	}

	VitalCost[4] = 5;
	for (SupportCard* card : Facilities[4]->SupportCards)
	{
		if (card->IsShining)
		{
			VitalCost[4] += card->CardData.VitalBonus;
		}
	}
}

void VenusSceneSimulator::_updateVenusCard()
{
	VenusCardPtr->CheckZoneEnd();
}

void VenusSceneSimulator::_updateVenusHangout()
{
	if (!VenusCardPtr->CanHangOut) { VenusCardHangoutAvailable = { false,false,false,false,false }; return; }
	VenusCardHangoutAvailable[0] = !IsVenusCardHangoutUsed[0];
	VenusCardHangoutAvailable[1] = !IsVenusCardHangoutUsed[1];
	VenusCardHangoutAvailable[2] = !IsVenusCardHangoutUsed[2];
	VenusCardHangoutAvailable[3] = IsVenusCardHangoutUsed[0] && IsVenusCardHangoutUsed[1] && IsVenusCardHangoutUsed[2] && !IsVenusCardHangoutUsed[3];
	VenusCardHangoutAvailable[4] = IsVenusCardHangoutUsed[3] && !IsVenusCardHangoutUsed[4];
}

void VenusSceneSimulator::_handleContinueRaces()
{
	if (_continuousRaceCount == 3)
	{
		MotivationChange(-0.6);
		// simulate debuff loss
		StatusChange({ 0,0,0,0,0,-10 }, false);
	}
	else if (_continuousRaceCount >= 4)
	{
		MotivationChange(-0.93);
		// simulate debuff loss
		StatusChange({ 0,0,0,0,0,-21 }, false);
		// simulate status loss
		RandomStatusChange(-4, false);
		RandomStatusChange(-4, false);
		RandomStatusChange(-4, false);
	}
}

void VenusSceneSimulator::_resetRaceBonus()
{
	_raceBonus = 1.0f;
	for (int i = 0; i < 6; i++)
	{
		_raceBonus += static_cast<float>(Deck[i]->CardData.RaceBonus) / 100.0f;
	}
}

void VenusSceneSimulator::_updateStatusRatio()
{
	std::array<int, 6> targetStatus = DataManager::Instance().TargetStatus;
	std::array<float, 6> _expectedStatus = { 0.0f };
	std::array<float, 5> totalTrainStatus = { 0.0f };
	for (int i = 0; i < 5; i++)
	{
		totalTrainStatus[i] = DataManager::Instance().TargetStatus[i] - UmaDatas.StatusInitial[i] - InheritStatus[i] * 3 - 255;
	}

	for (int i = 0; i < 5; i++)
	{
		_expectedStatus[i] = totalTrainStatus[i] * TrainStatusPercentage[Turn];
		_expectedStatus[i] += Turn;
		if (Turn >= 0) { _expectedStatus[i] += UmaDatas.StatusInitial[i] + InheritStatus[i]; }
		if (Turn >= 24) { _expectedStatus[i] += 20; }
		if (Turn >= 30) { _expectedStatus[i] += InheritStatus[i]; }
		if (Turn >= 48) { _expectedStatus[i] += 39; }
		if (Turn >= 49) { _expectedStatus[i] += 3; }
		if (Turn >= 54) { _expectedStatus[i] += InheritStatus[i]; }
		if (Turn >= 72) { _expectedStatus[i] += 42; }
		if (Turn >= 77) { _expectedStatus[i] += 73; }
	}

	float statusRatioSum = 0.0f;
	for (int i = 0; i < 5; i++)
	{
		_statusRatio[i] = (targetStatus[i] - CurrentStatus[i]) / (targetStatus[i] - _expectedStatus[i]) * StatusValue[i];
		_statusRatio[i] = std::max(0.1f, _statusRatio[i]);
		statusRatioSum += _statusRatio[i];
	}
	for (int i = 0; i < 5; i++)
	{
		_statusRatio[i] = _statusRatio[i] * 5 / statusRatioSum;
	}
	_statusRatio[5] = StatusValue[5];
}

bool VenusSceneSimulator::_evaluateBeforeTurnDecision()
{
	if (!TowerSystem->IsFull()) return false;
	if (Turn == 77) return true;
	Color wisdomColor = TowerSystem->Tower[0][0].SpiritColor;
	int remainVital = MaxVital - Vital;

	switch (wisdomColor)
	{
	case Color::Red:
		if (Turn == 22 || (Turn >= 42 && Turn <= 46) || (Turn >= 66 && Turn <= 70 || (Turn >= 72 && Turn <= 76))) return false;
		if (Turn == 47 || Turn == 71 || Turn == 77) return true;
		if (IsTargetRace() && remainVital >= 20) return true;
		if (Motivation == 3 && remainVital >= 20) return true;
		if (Motivation == 4) return true;
		if (remainVital >= 45) return true;
		break;
	case Color::Blue:
		if (IsTargetRace() || IsPreferRace()) return false;
		for (int i = 0; i < 5; i++)
		{
			if (Facilities[i]->SupportCards.size() >= 2) return true;
		}
		break;
	case Color::Yellow:
		if (IsTargetRace() || IsPreferRace()) return false;
		for (int i = 0; i < 5; i++)
		{
			if (Facilities[i]->SupportCards.size() >= 3) return true;
			int additionalShineCount = 0;
			for (SupportCard* card : Facilities[i]->SupportCards)
			{
				if (!card->IsShining) additionalShineCount++;
			}
			if (additionalShineCount >= 2) return true;
		}
		break;
	default:
		break;
	}
	return false;
}

Decision VenusSceneSimulator::_evaluateDuringTurnDecision()
{
	if (Turn == 77) return Decision::None;
	if (IsTargetRace()) return Decision::TargetRace;
	if (IsPreferRace() && DataManager::Instance().UsePreferedRace) return Decision::PreferRace;
	_decisionValue.fill(0.0f);
	int remainVital = MaxVital - Vital;

	// train
	for (int decisionIndex = 0; decisionIndex < 5; decisionIndex++)
	{
		// success
		float successRate = 1.0f - FailRate[decisionIndex];
		for (int statusIndex = 0; statusIndex < 6; statusIndex++)
		{
			_decisionValue[decisionIndex] += TrainValue[decisionIndex][statusIndex] * _statusRatio[statusIndex] * successRate;
		}
		_decisionValue[decisionIndex] += std::min(VitalCost[decisionIndex], remainVital) * successRate;
		// critial fail & fail
		if (decisionIndex == 4) { _decisionValue[decisionIndex] += IntFailValue * FailRate[decisionIndex]; continue; }
		_decisionValue[decisionIndex] += CriticalFailValue * FailRate[decisionIndex] * FailRate[decisionIndex];
		_decisionValue[decisionIndex] += FailValue * FailRate[decisionIndex];
	}

	// rest
	int criticalRestValue = std::min(70, remainVital);
	int normalRestValue = std::min(50, remainVital);
	int failRestValue = std::min(30, remainVital);
	_decisionValue[5] = _decisionValue[5] + 0.25f * criticalRestValue + 0.613f * normalRestValue + 0.137f * failRestValue;

	// summer rest
	if (Motivation != 4) { _decisionValue[6] += 30; }
	_decisionValue[6] += std::max(40, remainVital);

	// hangout
	if (Motivation <= 3) { _decisionValue[7] += 30; }
	if (Motivation <= 2) { _decisionValue[7] += 11; }
	int tenVitalValue = std::min(10, remainVital);
	int twentyVitalValue = std::min(20, remainVital);
	int thirtyVitalValue = std::min(30, remainVital);
	_decisionValue[7] = 0.488f * tenVitalValue + 0.107f * twentyVitalValue + 0.041f * thirtyVitalValue;

	// venusHangout
	if (Motivation != 4)
	{
		_decisionValue[8] += 30;
		_decisionValue[9] += 30;
		_decisionValue[10] += 30;
		_decisionValue[11] += 30;
		_decisionValue[12] += 30;
	}
	_decisionValue[8] += std::min(45, remainVital) + 35 * Turn / 78;
	_decisionValue[9] += std::min(32, remainVital) + 47 * Turn / 78;
	_decisionValue[10] += std::min(32, remainVital) + 48 * Turn / 78;
	_decisionValue[11] += std::min(45, remainVital) + 35 * Turn / 78;
	_decisionValue[12] += std::min(52, remainVital) + 28 * Turn / 78;

	// race
	if (Motivation != 4) { _decisionValue[13] += 6; }
	_decisionValue[13] -= 15;
	_decisionValue[13] += 35.0f * _statusRatio[5] * _raceBonus + 8.0f * 0.8f * _raceBonus;

	// spirits
	int availableSpiritNum = 0;
	if (TowerSystem->Tower[3][7].SpiritColor != Color::None) availableSpiritNum++;
	if (TowerSystem->Tower[3][6].SpiritColor != Color::None) availableSpiritNum++;
	float spiritValue = 20.0f - Turn / 78.0f * 5.0f;

	_decisionValue[0] += std::min(SpiritDistribution[0].SpiritAmount, availableSpiritNum) * spiritValue * (1.0f - FailRate[0]);
	_decisionValue[1] += std::min(SpiritDistribution[1].SpiritAmount, availableSpiritNum) * spiritValue * (1.0f - FailRate[1]);
	_decisionValue[2] += std::min(SpiritDistribution[2].SpiritAmount, availableSpiritNum) * spiritValue * (1.0f - FailRate[2]);
	_decisionValue[3] += std::min(SpiritDistribution[3].SpiritAmount, availableSpiritNum) * spiritValue * (1.0f - FailRate[3]);
	_decisionValue[4] += std::min(SpiritDistribution[4].SpiritAmount, availableSpiritNum) * spiritValue * (1.0f - FailRate[4]);
	_decisionValue[5] += std::min(SpiritDistribution[5].SpiritAmount, availableSpiritNum) * spiritValue;
	_decisionValue[6] += std::min(SpiritDistribution[5].SpiritAmount, availableSpiritNum) * spiritValue;
	_decisionValue[7] += std::min(SpiritDistribution[6].SpiritAmount, availableSpiritNum) * spiritValue;
	_decisionValue[8] += std::min(SpiritDistribution[6].SpiritAmount, availableSpiritNum) * spiritValue;
	_decisionValue[9] += std::min(SpiritDistribution[6].SpiritAmount, availableSpiritNum) * spiritValue;
	_decisionValue[10] += std::min(SpiritDistribution[6].SpiritAmount, availableSpiritNum) * spiritValue;
	_decisionValue[11] += std::min(SpiritDistribution[6].SpiritAmount, availableSpiritNum) * spiritValue;
	_decisionValue[12] += std::min(SpiritDistribution[6].SpiritAmount, availableSpiritNum) * spiritValue;
	_decisionValue[13] += std::min(SpiritDistribution[7].SpiritAmount, availableSpiritNum) * spiritValue;

	// bonds
	for (int i = 0; i < 5; i++)
	{
		float successRate = 1.0f - FailRate[i];
		for (SupportCard* card : Facilities[i]->SupportCards)
		{
			if (card == VenusCardPtr)
			{
				if (!VenusCardPtr->IsActivated) { _decisionValue[i] += 40; }
				else if (!VenusCardPtr->CanHangOut && VenusCardPtr->Bond < 60) { _decisionValue[i] += 25 * successRate; }
				else if (!VenusCardPtr->CanHangOut && VenusCardPtr->Bond >= 60) { _decisionValue[i] += 15 * successRate; }
				else if (VenusCardPtr->CanHangOut && !VenusCardPtr->IsShining) { _decisionValue[i] += 35 * successRate; }
				else if (VenusCardPtr->CanHangOut && VenusCardPtr->IsShining) { _decisionValue[i] += 25 * successRate; }
			}
			else
			{
				if (card->Bond <= 80)	_decisionValue[i] += 10 * successRate;
				if (card->HaveBlueVenusHint && card->Bond <= 73) _decisionValue[i] += 7 * successRate;
			}
		}
	}

	// hint bonds
	for (int i = 0; i < 5; i++)
	{
		float successRate = 1.0f - FailRate[i];
		for (SupportCard* card : Facilities[i]->SupportCards)
		{
			if (card->Bond <= 73 && card->HaveHint) _decisionValue[i] += 7 * successRate;
			break;
		}
	}

	// facility score
	for (int i = 0; i < 5; i++)
	{
		float successRate = 1.0f - FailRate[i];
		_decisionValue[i] += 10 * successRate;
	}

	// check legal
	for (int i = 0; i < 13; i++)
	{
		Decision decision = static_cast<Decision>(i);
		if (!IsLegal(decision)) _decisionValue[i] = 0;
	}

	// return
	auto maxElementIter = std::max_element(_decisionValue.begin(), _decisionValue.end());
	int maxIndex = std::distance(_decisionValue.begin(), maxElementIter);
	return static_cast<Decision>(maxIndex);
}

Color VenusSceneSimulator::_evaluateAfterTurnDecision()
{
	int redLevel = TowerSystem->VenusLevel[0];
	int blueLevel = TowerSystem->VenusLevel[1];
	int yellowLevel = TowerSystem->VenusLevel[2];

	if (redLevel <= blueLevel && redLevel <= yellowLevel)  return Color::Red;
	if (blueLevel <= redLevel && blueLevel <= yellowLevel) return Color::Blue;
	if (yellowLevel <= blueLevel && yellowLevel <= redLevel)  return Color::Yellow;
	return Color::Red;
}

float VenusSceneSimulator::_evaluateStatusScore(std::array<int, 6> status)
{
	std::array<int, 6> targetStatus = DataManager::Instance().TargetStatus;
	std::array<float, 6> belowTargetStatusScore = DataManager::Instance().BelowTargetStatusScore;
	std::array<float, 6> overTargetStatusScore = DataManager::Instance().OverTargetStatusScore;

	for (int i = 0; i < 5; i++)
	{
		if (status[i] > 1200)
		{
			status[i] = static_cast<int>(1200.0f + (status[i] - 1200.0f) * 0.5f);
		}
		if (targetStatus[i] > 1200)
		{
			targetStatus[i] = static_cast <int>(1200.0f + (targetStatus[i] - 1200.0f) * 0.5f);
		}
	}

	float result = 0;
	for (int i = 0; i < 6; i++)
	{
		if (status[i] <= targetStatus[i])
		{
			result += static_cast<float>(status[i] * belowTargetStatusScore[i]);
		}
		else
		{
			int overTargetPart = status[i] - targetStatus[i];
			result += (static_cast<float>(targetStatus[i]) * belowTargetStatusScore[i] + static_cast<float>(overTargetPart) * overTargetStatusScore[i]);
		}
	}

	return result;
}

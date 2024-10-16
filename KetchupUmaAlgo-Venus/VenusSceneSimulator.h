#pragma once
#include <memory>
#include <algorithm>
#include "Globals.h"
#include "VenusCard.h"
#include "SupportCard.h"
#include "DataManager.h"
#include "Facility.h"
#include "SpiritTower.h"

class VenusSceneSimulator
{
public:
	VenusSceneSimulator(TurnData thisTurn);
	void Reset(TurnData thisTurn);
	void PrintInfo();

	// process
	float Simulate();
	void ResetTurn();
	void BeforeTurnEvent();
	void BeforeTurnDecision(bool activated);
	void DuringTurnDecision(Decision decision);
	void AfterTurnEvent();
	void AfterTurnDecision(Color color);

	// variable changes
	void StatusChange(std::array<int, 6> status, bool takeRaceBonus);
	void RandomStatusChange(int amount, bool takeRaceBonus);
	void EventStatusChange(std::array<int, 6> status);
	void VitalChange(int amount);
	void MotivationChange(float amount);

	// actions
	void Click(Train train);
	void Rest();
	void SummerRest();
	void Hangout();
	void VenusHangout(int index);
	void Race();
	void TargetRace();
	void PreferRace();

	// tools
	bool IsTargetRace();
	bool IsPreferRace();
	bool IsLegal(Decision decision);
	bool CanActivate();
	bool SpiritEventPossible(Decision decision);

	// init variables
	int UmaId;
	int UmaLimitBreak;
	int Turn;
	int Vital;
	int MaxVital;
	int Motivation;
	std::array<int, 6> CurrentStatus;
	std::array<int, 6> StatusLimit;
	std::array<int, 6> InheritStatus;
	std::array<bool, 5> IsVenusCardHangoutUsed;
	std::array<SpiritData, 8> SpiritDistribution;
	VenusCard* VenusCardPtr;
	std::array<std::unique_ptr<SupportCard>, 6> Deck;
	std::array<std::unique_ptr<Facility>, 5> Facilities;
	std::unique_ptr <SpiritTower> TowerSystem;
	UmaData UmaDatas;

	// generated variables
	std::array<bool, 5> VenusCardHangoutAvailable;
	std::array<float, 5> FailRate;
	std::array<std::array<int, 6>, 5> TrainValue;
	std::array<int, 5> VitalCost;

private:
	// actions
	void _pushSpirit(Train train);
	void _reshuffleSupportCard();
	void _updateSupportCards();
	void _reshuffleSpirits();
	void _updateFailRate();
	void _updateTrainValue();
	void _updateVitalCost();
	void _updateVenusCard();
	void _updateVenusHangout();
	void _handleContinueRaces();
	void _resetRaceBonus();
	void _updateStatusRatio();

	// decisions
	bool _evaluateBeforeTurnDecision();
	Decision _evaluateDuringTurnDecision();
	Color _evaluateAfterTurnDecision();
	float _evaluateStatusScore(std::array<int, 6> status);

	// variables
	float _raceBonus;
	int _continuousRaceCount;
	std::array<float, 6> _statusRatio;

	// decision usage
	std::array<float, 14> _decisionValue;
	bool _beforeTurnDecision;
	Decision _duringTurnDecision;
	Color _afterTurnDecision;
};

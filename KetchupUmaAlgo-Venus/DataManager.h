#pragma once

#include <fstream>
#include <map>
#include <string>
#include "Globals.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class DataManager
{
public:
	static DataManager& Instance();
	TurnData ThisTurn;
	std::string ThisTurnJsonLocation;
	std::map<int, std::map<int, SupportCardData>> SupportCardMap;
	std::map<int, UmaData> UmaMap;
	int SimulateUnitTurn;
	int ShowDecisionNum;
	int DecisionExtraRound;
	int MasterOfGoodFieldPtValue;
	int HintPtValue;
	bool UsePreferedRace;
	bool AlwaysShowRace;
	std::array<int, 6> TargetStatus;
	std::array<float, 6> BelowTargetStatusScore;
	std::array<float, 6> OverTargetStatusScore;

	void Initialize();
	TurnData GetThisTurn();
	SupportCardData GetSupportCardData(int id, int limitBreak);
	json LoadJsonFile(const std::string& filename);
	void InitSetting(json jsonSetting);
};
#include "DataManager.h"

DataManager& DataManager::Instance()
{
	static DataManager instance;
	return instance;
}

json DataManager::LoadJsonFile(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Could not open the file: " << filename << std::endl;
		return json();
	}

	json j;
	file >> j;
	file.close();
	return j;
}

void DataManager::Initialize()
{
	std::cout << "Loading Card Datas ..." << std::endl;
	json jsonCardDB = LoadJsonFile("Data/cardDB.json");
	for (json::const_iterator it = jsonCardDB.begin(); it != jsonCardDB.end(); ++it)
	{
		int id = std::stoi(it.key());
		json cardData = it.value();

		for (int limitBreak = 0; limitBreak <= 4; ++limitBreak)
		{
			SupportCardMap[id][limitBreak] = SupportCardData(cardData, limitBreak);
		}
	}

	std::cout << "Loading Uma Data..." << std::endl;
	json jsonUmaDB = LoadJsonFile("Data/umaDB.json");
	for (json::const_iterator it = jsonUmaDB.begin(); it != jsonUmaDB.end(); ++it)
	{
		int id = std::stoi(it.key());
		json umaData = it.value();
		UmaMap[id] = UmaData(umaData);
	}

	std::cout << "Loading thisTurn.json location..." << std::endl;
	std::ifstream packetsFile("Data/packetsFolderLocation.txt");
	if (packetsFile.is_open())
	{
		std::string input;
		std::getline(packetsFile, input);
		input += "\\thisTurn.json";

		std::string result;
		result.reserve(input.size());
		for (char ch : input)
		{
			if (ch == '\\')
			{
				result += '\\';
			}
			result += ch;
		}
		packetsFile.close();

		ThisTurnJsonLocation = result;
		std::cout << "thisTurn.json location: " << ThisTurnJsonLocation << std::endl;
	}
	else
	{
		std::cerr << "can't open packetsFolderLocation.txt" << std::endl;
	}

	// Åª¨ú³]¸m¤¤
	std::cout << "Loaing setting.json ..." << std::endl;
	json jsonSetting = LoadJsonFile("Data/setting.json");
	InitSetting(jsonSetting);
}

void DataManager::InitSetting(json jsonSetting)
{
	std::cout << "====================================" << std::endl;

	SimulateUnitTurn = jsonSetting.value("SimulateUnitTurn", 3500);
	std::cout << "SimulateUnitTurn: " << SimulateUnitTurn << std::endl;

	ShowDecisionNum = jsonSetting.value("ShowDecisionNum", 6);
	std::cout << "ShowDecisionNum: " << ShowDecisionNum << std::endl;

	DecisionExtraRound = jsonSetting.value("DecisionExtraRound", 3);
	std::cout << "DecisionExtraRound: " << DecisionExtraRound << std::endl;

	std::cout << std::endl;

	MasterOfGoodFieldPtValue = jsonSetting.value("MasterOfGoodFieldPtValue", 100);
	std::cout << "MasterOfGoodFieldPtValue: " << MasterOfGoodFieldPtValue << std::endl;

	HintPtValue = jsonSetting.value("HintPtValue", 20);
	std::cout << "HintPtValue: " << HintPtValue << std::endl;

	UsePreferedRace = jsonSetting.value("UsePreferRaces", false);
	UsePreferedRace ? std::cout << "UsePreferRaces" << std::endl : std::cout << "NotUsePreferRaces" << std::endl;

	AlwaysShowRace = jsonSetting.value("AlwaysShowRace", false);
	AlwaysShowRace ? std::cout << "AlwaysShowRace" << std::endl : std::cout << "NotAlwaysShowRace" << std::endl;

	ShowScore = jsonSetting.value("ShowScore", false);
	ShowScore ? std::cout << "ShowScore" << std::endl : std::cout << "NotShowScore" << std::endl;

	std::cout << std::endl;

	TargetStatus = jsonSetting.value("TargetStatus", std::array<int, 6>{});
	std::cout << "TargetStatus: ";
	for (int i = 0; i < TargetStatus.size(); i++)
	{
		std::cout << TargetStatus[i] << "/";
		if (TargetStatus[i] >= 1200)
		{
			TargetStatus[i] = TargetStatus[i] * 2 - 1200;
		}
	}
	std::cout << std::endl;

	BelowTargetStatusScore = jsonSetting.value("BelowTargetStatusScore", std::array<float, 6>{});
	std::cout << "BelowTargetStatusScore: ";
	for (int i = 0; i < BelowTargetStatusScore.size(); i++)
	{
		std::cout << BelowTargetStatusScore[i] << "/";
	}
	std::cout << std::endl;

	OverTargetStatusScore = jsonSetting.value("OverTargetStatusScore", std::array<float, 6>{});
	std::cout << "OverTargetStatusScore: ";
	for (int i = 0; i < OverTargetStatusScore.size(); i++)
	{
		std::cout << OverTargetStatusScore[i] << "/";
	}
	std::cout << std::endl;
	std::cout << "====================================" << std::endl;
}

TurnData DataManager::GetThisTurn()
{
	json jsonThisTurn = LoadJsonFile(ThisTurnJsonLocation);
	ThisTurn = TurnData(jsonThisTurn);

	return ThisTurn;
}

SupportCardData DataManager::GetSupportCardData(int id, int limitBreak)
{
	const auto& it = SupportCardMap.find(id);
	if (it != SupportCardMap.end())
	{
		const auto& limit = it->second.find(limitBreak);
		if (limit != it->second.end())
		{
			return limit->second;
		}
	}
	throw std::runtime_error("SupportCardData not found.");
}

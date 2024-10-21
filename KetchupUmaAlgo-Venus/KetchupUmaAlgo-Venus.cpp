#include <iostream>
#include "DataManager.h"
#include "VenusSceneSimulator.h"
#include "Globals.h"
#include <future>
#include "Windows.h"
#include <conio.h>

float SimulateDecisionRun(TurnData thisTurn, DecisionSet decisionSet, int times)
{
	std::unique_ptr<VenusSceneSimulator> simulator = std::make_unique<VenusSceneSimulator>(thisTurn);
	if (decisionSet.BeforeTurnDecision && !simulator->CanActivate()) return 0.0f;
	if (!simulator->IsLegal(decisionSet.DuringTurnDecision)) return 0.0f;
	std::cout << ".";
	std::vector<float> result(times);
	for (int i = 0; i < times; ++i)
	{
		simulator->Reset(thisTurn);
		simulator->BeforeTurnEvent();
		simulator->BeforeTurnDecision(decisionSet.BeforeTurnDecision);
		simulator->DuringTurnDecision(decisionSet.DuringTurnDecision);
		simulator->AfterTurnEvent();
		simulator->AfterTurnDecision(decisionSet.AfterTurnDecision);
		simulator->Turn += 1;
		result[i] = simulator->Simulate();
	}

	float sum = std::accumulate(result.begin(), result.end(), 0.0);
	float average = sum / times;
	return average;
}

void CheckForResetKey(bool& recalculate)
{
	while (true)
	{
		if (_kbhit() && _getch() == ' ') { recalculate = true; }
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

int main()
{
	// Set cmd
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SMALL_RECT windowSize = { 0,0,39,16 };
	SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
	COORD bufferSize = { 40,1000 };
	SetConsoleScreenBufferSize(hConsole, bufferSize);

	// Init
	bool recalculate = true;
	int currentTurn = -1;
	DataManager::Instance().Initialize();

	std::thread resetKeyThread(CheckForResetKey, std::ref(recalculate));

	// Simulate
	while (1)
	{
		if (recalculate) { currentTurn = -1; recalculate = false; }
		TurnData thisTurn = DataManager::Instance().GetThisTurn();
		if (thisTurn.Turn == currentTurn) { std::this_thread::sleep_for(std::chrono::seconds(1)); continue; }
		currentTurn = thisTurn.Turn;
		std::unique_ptr<VenusSceneSimulator> simulator = std::make_unique<VenusSceneSimulator>(thisTurn);
		int showDecisionNum = DataManager::Instance().ShowDecisionNum;
		int simulateUnitTurn = DataManager::Instance().SimulateUnitTurn;
		int decisionExtraRound = DataManager::Instance().DecisionExtraRound;
		bool alwaysShowRace = DataManager::Instance().AlwaysShowRace;
		bool showScore = DataManager::Instance().ShowScore;

		std::cout << "\n\n====================================\n";
		std::cout << TurnToString[currentTurn] <<"\n";

		// simulate all once and sort score
		std::array<std::pair<int, std::future<float>>, 32> decisions;
		std::array< std::pair<int, float>, 32> decisionScores;
		for (int i = 0; i < 32; i++)
		{
			decisions[i].first = i;
			decisions[i].second = std::async(std::launch::async, SimulateDecisionRun, thisTurn, DecisionSet(i), simulateUnitTurn);
		}
		for (int i = 0; i < 32; i++) { decisions[i].second.wait(); }
		for (int i = 0; i < 32; i++)
		{
			decisionScores[i].first = i;
			decisionScores[i].second = decisions[i].second.get();
		}
		std::sort(decisionScores.begin(), decisionScores.end(), [](const auto& a, const auto& b) {return a.second > b.second; });

		// get topDecisions
		std::vector<int> topDecisionIndex;
		for (int i = 0; i < showDecisionNum; i++)
		{
			if (decisionScores[i].second != 0) topDecisionIndex.push_back(decisionScores[i].first);
		}
		int raceDecisionIndex = static_cast<int>(Decision::Race);
		if (alwaysShowRace && simulator->IsLegal(Decision::Race))
		{
			if (std::find(topDecisionIndex.begin(), topDecisionIndex.end(), raceDecisionIndex) == topDecisionIndex.end())
			{
				topDecisionIndex.pop_back();
				topDecisionIndex.push_back(raceDecisionIndex);
			}
		}

		// simulate topDecisions
		std::vector<std::pair<int, std::future<float>>> topDecisions;
		std::vector<std::pair<int, float>> topDecisionScores;
		for (int i = 0; i < topDecisionIndex.size(); i++)
		{
			DecisionSet topDecision = DecisionSet(topDecisionIndex[i]);
			topDecisions.push_back(std::make_pair(topDecisionIndex[i],
				std::async(std::launch::async, SimulateDecisionRun, thisTurn, topDecision, simulateUnitTurn * decisionExtraRound)));
		}
		for (int i = 0; i < topDecisionIndex.size(); i++) { topDecisions[i].second.wait(); }
		for (int i = 0; i < topDecisionIndex.size(); i++)
		{
			int decisionIndex = topDecisionIndex[i];
			float firstRoundScore = 0.0f;
			for (int i = 0; i < 32; i++)
			{
				if (decisionScores[i].first == decisionIndex) firstRoundScore = decisionScores[i].second;
			}
			topDecisionScores.push_back(std::make_pair(decisionIndex,
				(topDecisions[i].second.get() * decisionExtraRound + firstRoundScore) / (decisionExtraRound + 1)));
		}
		std::sort(topDecisionScores.begin(), topDecisionScores.end(), [](const auto& a, const auto& b) {return a.second > b.second; });
		std::cout << "\n";

		// print topDecisions
		if (showScore)
		{
			std::cout << "************************************\n";
			for (int i = 0; i < topDecisionScores.size(); i++)
			{
				if (topDecisionScores[i].second == 0) continue;
				DecisionSet decisionSet = DecisionSet(topDecisionScores[i].first);
				if (decisionSet.BeforeTurnDecision) std::cout << "Activate, ";
				int decisionIndex = static_cast<int>(decisionSet.DuringTurnDecision);
				std::cout << DecisionToString[decisionIndex] << ": " << topDecisionScores[i].second << "\n";
			}
			std::cout << "************************************\n";
		}

		// print result Decision
		DecisionSet result = DecisionSet(topDecisionScores[0].first);
		std::cout << "[";
		if (result.BeforeTurnDecision) std::cout << "Activate, ";
		int decisionIndex = static_cast<int>(result.DuringTurnDecision);
		std::cout << DecisionToString[decisionIndex] << "]\n";

		// check if spirit event possible
		if (!simulator->SpiritEventPossible(result.DuringTurnDecision)) { continue; }

		// simulate spirit
		std::array<std::future<float>, 3> spirits;
		std::array<float, 3> spiritScores = { 0.0f };
		for (int i = 0; i < 3; i++)
		{
			result.AfterTurnDecision = static_cast<Color>(i);
			spirits[i] = std::async(std::launch::async, SimulateDecisionRun, thisTurn, result, simulateUnitTurn * decisionExtraRound);
		}
		for (int i = 0; i < 3; i++) { spiritScores[i] = spirits[i].get(); }
		std::cout << "\n";

		if (showScore)
		{
			std::cout << "************************************\n";
			for (int i = 0; i < 3; i++) { std::cout << ColorToString[i] << ": " << spiritScores[i] << "\n"; }
			std::cout << "************************************\n";
		}

		auto maxSpirit = std::max_element(spiritScores.begin(), spiritScores.end());
		int maxSpiritIndex = std::distance(spiritScores.begin(), maxSpirit);
		std::cout << "Take [" << ColorToString[maxSpiritIndex] << "]";
	}
}
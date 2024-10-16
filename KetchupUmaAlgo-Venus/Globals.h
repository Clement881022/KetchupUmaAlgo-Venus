#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <map>
#include <memory>
#include <random>
#include <time.h>
#include "nlohmann/json.hpp"
#include <array>

using json = nlohmann::json;

enum class Train
{
	Speed,
	Stamina,
	Power,
	Will,
	Int,
	Rest,
	Hangout,
	Race,
	None
};

const std::array<std::string, 9> TrainToString
{
	"Speed",
	"Stamina",
	"Power",
	"Will",
	"Int",
	"Rest",
	"Hangout",
	"Race",
	"None"
};

enum class Status
{
	Speed,
	Stamina,
	Power,
	Will,
	Int,
	SkillPt,
	None
};

enum class Color
{
	Red,
	Blue,
	Yellow,
	Random,
	None
};

const std::array<std::string, 9> ColorToString
{
	"Red",
	"Blue",
	"Yellow",
	"Random",
	"None"
};

enum class Decision
{
	TrainSpeed,
	TrainStamina,
	TrainPower,
	TrainWill,
	TrainInt,
	Rest,
	SummerRest,
	Hangout,
	HangoutRed,
	HangoutBlue,
	HangoutYellow,
	HangoutAll,
	HangoutEnd,
	Race,
	TargetRace,
	PreferRace,
	None
};

const std::array<std::string, 17> DecisionToString
{
	"TrainSpeed",
	"TrainStamina",
	"TrainPower",
	"TrainWill",
	"TrainInt",
	"Rest",
	"SummerRest",
	"Hangout",
	"HangoutRed",
	"HangoutBlue",
	"HangoutYellow",
	"HangoutAll",
	"HangoutEnd",
	"Race",
	"TargetRace",
	"PreferRace",
	"None"
};

const std::array<std::string, 78> TurnToString =
{
	"Year 1, Month 1 -", "Year 1, Month 1 +", "Year 1, Month 2 -", "Year 1, Month 2 +",
	"Year 1, Month 3 -", "Year 1, Month 3 +", "Year 1, Month 4 -", "Year 1, Month 4 +",
	"Year 1, Month 5 -", "Year 1, Month 5 +", "Year 1, Month 6 -", "Year 1, Month 6 +",
	"Year 1, Month 7 -", "Year 1, Month 7 +", "Year 1, Month 8 -", "Year 1, Month 8 +",
	"Year 1, Month 9 -", "Year 1, Month 9 +", "Year 1, Month 10 -", "Year 1, Month 10 +",
	"Year 1, Month 11 -", "Year 1, Month 11 +", "Year 1, Month 12 -", "Year 1, Month 12 +",

	"Year 2, Month 1 -", "Year 2, Month 1 +", "Year 2, Month 2 -", "Year 2, Month 2 +",
	"Year 2, Month 3 -", "Year 2, Month 3 +", "Year 2, Month 4 -", "Year 2, Month 4 +",
	"Year 2, Month 5 -", "Year 2, Month 5 +", "Year 2, Month 6 -", "Year 2, Month 6 +",
	"Year 2, Month 7 -", "Year 2, Month 7 +", "Year 2, Month 8 -", "Year 2, Month 8 +",
	"Year 2, Month 9 -", "Year 2, Month 9 +", "Year 2, Month 10 -", "Year 2, Month 10 +",
	"Year 2, Month 11 -", "Year 2, Month 11 +", "Year 2, Month 12 -", "Year 2, Month 12 +",

	"Year 3, Month 1 -", "Year 3, Month 1 +", "Year 3, Month 2 -", "Year 3, Month 2 +",
	"Year 3, Month 3 -", "Year 3, Month 3 +", "Year 3, Month 4 -", "Year 3, Month 4 +",
	"Year 3, Month 5 -", "Year 3, Month 5 +", "Year 3, Month 6 -", "Year 3, Month 6 +",
	"Year 3, Month 7 -", "Year 3, Month 7 +", "Year 3, Month 8 -", "Year 3, Month 8 +",
	"Year 3, Month 9 -", "Year 3, Month 9 +", "Year 3, Month 10 -", "Year 3, Month 10 +",
	"Year 3, Month 11 -", "Year 3, Month 11 +", "Year 3, Month 12 -", "Year 3, Month 12 +",

	"Year 4, Month 1 -", "Year 4, Month 1 +", "Year 4, Month 2 -", "Year 4, Month 2 +",
	"Year 4, Month 3 -", "Year 4, Month 3 +"
};

const std::array<std::array<std::array<int, 7>, 6>, 5> BasicTrainValue =
{ {
	{{
		{{10, 0, 3, 0, 0, 5, -19}},
		{{11, 0, 3, 0, 0, 5, -21}},
		{{12, 0, 3, 0, 0, 5, -22}},
		{{13, 0, 4, 0, 0, 5, -24}},
		{{14, 0, 5, 0, 0, 5, -25}},
		{{15, 0, 6, 0, 0, 5, -26}}
	}},
	{{
		{{0, 8, 0, 6, 0, 5, -20}},
		{{0, 9, 0, 6, 0, 5, -22}},
		{{0, 10, 0, 7, 0, 5, -23}},
		{{0, 11, 0, 7, 0, 5, -25}},
		{{0, 12, 0, 8, 0, 5, -26}},
		{{0, 13, 0, 9, 0, 5, -27}}
	}},
	{{
		{{0, 4, 9, 0, 0, 5, -20}},
		{{0, 4, 10, 0, 0, 5, -22}},
		{{0, 4, 11, 0, 0, 5, -23}},
		{{0, 5, 12, 0, 0, 5, -25}},
		{{0, 6, 13, 0, 0, 5, -26}},
		{{0, 7, 14, 0, 0, 5, -27}}
	}},
	{{
		{{2, 0, 3, 9, 0, 5, -20}},
		{{2, 0, 3, 10, 0, 5, -22}},
		{{2, 0, 3, 11, 0, 5, -23}},
		{{3, 0, 3, 12, 0, 5, -25}},
		{{3, 0, 4, 13, 0, 5, -26}},
		{{4, 0, 5, 14, 0, 5, -27}}
	}},
	{{
		{{2, 0, 0, 0, 8, 5, 5}},
		{{2, 0, 0, 0, 9, 5, 5}},
		{{2, 0, 0, 0, 10, 5, 5}},
		{{3, 0, 0, 0, 11, 5, 5}},
		{{4, 0, 0, 0, 12, 5, 5}},
		{{5, 0, 0, 0, 13, 5, 5}}
	}}
} };

const std::array<float, 5> MotivationEffect = { 0.8f,0.9f,1.0f,1.1f,1.2f };

const std::array<std::array<int, 6>, 5> FailRateStandard =
{ {
	{520,524,528,532,536,540},
	{507,511,515,519,523,527},
	{516,520,524,528,532,536},
	{532,536,540,544,548,552},
	{320,321,322,323,324,325}
} };

const std::array<float, 6> VenusLevelToTrainBonus = { 0.0f,0.05f,0.08f,0.11f,0.13f,0.15f };
const std::array<float, 6> RedVenusLevelToVitalDiscount = { 1.0f,0.9f,0.85f,0.82f,0.8f,0.77f };
const std::array<int, 6> BlueVenusLevelToHintChance = { 0,20,25,30,33,35 };
const std::array<float, 6> YellowVenusLevelToEventBonus = { 0.0f,0.1f,0.15f,0.2f,0.23f,0.25f };

const std::array<std::array<int, 6>, 4> YearlyRaceAward =
{ {
	{10,10,10,10,10,50},
	{15,15,15,15,15,60},
	{20,20,20,20,20,70},
	{20,20,20,20,20,70}
} };

const std::array<float, 7> ZoneLastTurnChance = { 0.0f,0.0f,0.0f,0.5f,0.7f,0.8f,1.0f };

const std::array<std::array<int, 6>, 8> SpiritTypeWeight =
{ {
	{4,1,4,1,1,4},
	{1,4,1,4,1,4},
	{1,4,4,1,1,4},
	{17,6,17,17,6,17},
	{4,1,1,1,4,4},
	{1,1,1,1,1,1},
	{1,1,1,1,1,1},
	{1,1,1,1,1,1}
} };

const std::array<std::array<int, 6>, 5>  BlueVenusHintValue =
{ {
	{6,0,2,0,0,40},
	{0,6,0,2,0,40},
	{0,2,6,0,0,40},
	{1,0,1,6,0,40},
	{0,0,0,0,6,45},
} };

const std::array<float, 78> TrainStatusPercentage =
{
	0.0085f, 0.0172f, 0.0260f, 0.0349f, 0.0438f, 0.0529f, 0.0622f, 0.0715f,
	0.0809f, 0.0905f, 0.1001f, 0.1099f, 0.1198f, 0.1298f, 0.1399f, 0.1501f,
	0.1604f, 0.1708f, 0.1814f, 0.1920f, 0.2028f, 0.2137f, 0.2247f, 0.2358f,
	0.2470f, 0.2583f, 0.2697f, 0.2813f, 0.2929f, 0.3047f, 0.3166f, 0.3286f,
	0.3407f, 0.3529f, 0.3652f, 0.3776f, 0.3902f, 0.4028f, 0.4156f, 0.4285f,
	0.4414f, 0.4545f, 0.4678f, 0.4811f, 0.4945f, 0.5080f, 0.5217f, 0.5355f,
	0.5493f, 0.5633f, 0.5774f, 0.5916f, 0.6059f, 0.6204f, 0.6349f, 0.6496f,
	0.6643f, 0.6792f, 0.6942f, 0.7093f, 0.7245f, 0.7398f, 0.7552f, 0.7708f,
	0.7864f, 0.8022f, 0.8181f, 0.8341f, 0.8501f, 0.8664f, 0.8827f, 0.8991f,
	0.9156f, 0.9323f, 0.9491f, 0.9659f, 0.9829f, 1.0000f
};

const float FailValue = -24.0f;
const float CriticalFailValue = -90.0f;
const float IntFailValue = 5.0f;
const std::array<float, 6> StatusValue = { 1.0f,2.0f,1.5f,1.5f,1.0f,0.8f };

extern std::default_random_engine generator;
extern std::uniform_real_distribution<float> unif;
float GetZeroOneRandomFloat();
int WeightedRandom(const std::vector<int>& weights);

struct SpiritData
{
	Color SpiritColor;
	Status SpiritStatus;
	int StatusAmount;
	int SpiritAmount;
	bool IsEmpty();

	SpiritData();
	SpiritData(Color color, Status status, int statusAmount, int amount);
	SpiritData(int index);
};

struct SupportCardData
{
	int CardId;
	int CardLimitBreak;
	std::string Name;
	int Type;
	float FriendBonus;
	int MotivationBonus;
	float TrainBonus;
	int InitialBond;
	float MasterfulLevel;
	std::array<int, 6> StatusBonus;
	std::array<int, 6> InitialBonus;
	int VitalBonus;
	int RaceBonus;
	int HintLevel;
	int HintChance;

	SupportCardData();
	SupportCardData(const json& CardData, int LimitBreak);
};

struct UmaData {
	int Id;
	std::string Name;
	std::array<float, 6> StatusBonus;
	std::array<int, 6> StatusInitial;
	std::vector<int> TargetRaces;
	std::vector<int> PreferRaces;

	UmaData();
	UmaData(const json& jsonData);
};

struct TurnData
{
	// basic info
	int UmaId;
	int UmaLimitBreak;
	int Turn;
	int Vital;
	int MaxVital;
	int Motivation;
	std::array<int, 6> Status;
	std::array<int, 6> StatusLimit;
	std::array<int, 6> InheritStatus;
	std::array<bool, 5> IsVenusCardHangoutUsed;

	std::array<int, 6> CardId;
	std::array<int, 6> CardLimitBreak;
	std::array<int, 6> CardBond;
	std::array<bool, 6> CardHint;

	std::array<int, 5> FacilityLevelScore;
	std::array<Train, 6> CardDistribution;

	bool IsVenusCardActivated;
	bool CanVenusCardHangout;
	bool IsVenusCardZone;
	int ZoneContinueTurns;

	std::array<int, 3> VenusLevel;
	std::array<SpiritData, 15> VenusSpirits;
	std::array<SpiritData, 8> SpiritDistribution;

	TurnData();
	TurnData(const json& jsonData);
};

struct DecisionSet
{
	bool BeforeTurnDecision;
	Decision DuringTurnDecision;
	Color AfterTurnDecision;

	DecisionSet();
	DecisionSet(bool before, Decision during, Color after);
	DecisionSet(int index);
};
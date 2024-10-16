#include "Globals.h"

std::default_random_engine generator(static_cast<unsigned int>(time(NULL)));
std::uniform_real_distribution<float> unif;

// 0到1的隨機數
float GetZeroOneRandomFloat()
{
	float randomValue = unif(generator);
	return randomValue;
}

// 權重隨機
int WeightedRandom(const std::vector<int>& weights)
{
	// 計算權重的總和
	int totalWeight = 0;
	for (int weight : weights)
	{
		totalWeight += weight;
	}

	// 根據隨機數選擇結果
	int randomValue = static_cast<int>(GetZeroOneRandomFloat() * totalWeight);
	int cumulativeWeight = 0;
	for (int i = 0; i < weights.size(); ++i)
	{
		cumulativeWeight += weights[i];
		if (randomValue < cumulativeWeight)
		{
			return i;
		}
	}
	return 0;
}

SpiritData::SpiritData() {}
SpiritData::SpiritData(Color color, Status status, int statusAmount, int amount)
{
	SpiritColor = color;
	SpiritStatus = status;
	StatusAmount = statusAmount;
	SpiritAmount = amount;
}

SpiritData::SpiritData(int index)
{
	if (index == 0)
	{
		SpiritColor = Color::None;
		SpiritStatus = Status::None;
		StatusAmount = 0;
		SpiritAmount = 0;
	}
	else
	{
		SpiritAmount = index / 32 + 1;
		index %= 32;
		SpiritStatus = static_cast<Status>(index % 8 - 1);
		SpiritColor = static_cast<Color>(index / 8);
		StatusAmount = 0;
	}
}

bool SpiritData::IsEmpty()
{
	return SpiritColor == Color::None;
}

SupportCardData::SupportCardData() {}
SupportCardData::SupportCardData(const json& CardData, int LimitBreak)
{
	CardId = CardData.value("cardId", 0);
	CardLimitBreak = LimitBreak;
	Name = CardData.value("cardName", "");
	Type = CardData.value("cardType", 1);
	const auto& Detail = CardData["cardValue"][LimitBreak];
	FriendBonus = Detail.value("youQing", 0.0);
	MotivationBonus = Detail.value("ganJing", 0);
	TrainBonus = Detail.value("xunLian", 0);
	InitialBond = Detail.value("initialJiBan", 0);
	MasterfulLevel = Detail.value("deYiLv", 0.0);
	StatusBonus = Detail.value("bonus", std::array<int, 6> {});
	InitialBonus = Detail.value("initialBonus", std::array<int, 6> {});
	VitalBonus = Detail.value("wizVitalBonus", 0);
	RaceBonus = Detail.value("saiHou", 0);
	HintLevel = Detail.value("hintLevel", 0);
	HintChance = Detail.value("hintProbIncrease", 0);
}

UmaData::UmaData() {}
UmaData::UmaData(const json& jsonData)
{
	Id = jsonData.value("gameId", 0);
	Name = jsonData.value("name", std::string{});
	std::array<int, 5> StatusBonusRate = jsonData.value("fiveStatusBonus", std::array<int, 5> {});

	for (int i = 0;i < 5;i++)
	{
		float statusBonus = static_cast<float>(StatusBonusRate[i]) * 0.01f + 1.0f;
		StatusBonus[i] = statusBonus;
	}
	StatusBonus[5] = 1.0f;

	std::array<int, 5> status = jsonData.value("fiveStatusInitial", std::array<int, 5> {});
	for (int i = 0;i < 5;i++)
	{
		StatusInitial[i] = status[i];
	}
	StatusInitial[5] = 120;

	TargetRaces = jsonData.value("races", std::vector<int>{});
	TargetRaces.push_back(77);
	PreferRaces = jsonData.value("preferRaces", std::vector<int>{});
}

TurnData::TurnData() {}
TurnData::TurnData(const json& jsonData)
{
	UmaId = jsonData.value("umaId", 0);
	UmaLimitBreak = UmaId / 1000000;
	UmaId = UmaId % 1000000;
	Turn = jsonData.value("turn", 0);
	Vital = jsonData.value("vital", 0);
	MaxVital = jsonData.value("maxVital", 0);
	std::array<int, 5> status = jsonData.value("fiveStatus", std::array<int, 5> {});
	for (int i = 0;i < 5;i++)
	{
		status[i] = status[i] > 1200 ? (status[i] + 1200) / 2 : status[i];
	}

	int skillPt = jsonData.value("skillPt", 0);
	for (int i = 0;i < 5;i++)
	{
		Status[i] = status[i];
	}
	Status[5] = skillPt;


	std::array<int, 5> statusLimit = jsonData.value("fiveStatusLimit", std::array<int, 5> {});
	for (int i = 0;i < 5;i++)
	{
		StatusLimit[i] = statusLimit[i] > 1200 ? (statusLimit[i] + 1200) / 2 : statusLimit[i];
	}
	StatusLimit[5] = 9999;

	Motivation = jsonData.value("motivation", 0);
	Motivation -= 1;
	CardId = jsonData.value("cardId", std::array<int, 6> {});
	for (int i = 0; i < 6; i++) {
		CardLimitBreak[i] = CardId[i] / 100000;
		CardId[i] = CardId[i] % 100000;
	}
	CardBond = jsonData.value("cardJiBan", std::array<int, 6> {});

	FacilityLevelScore = jsonData.value("trainLevelCount", std::array<int, 5> {});
	std::array<int, 5> inheritStatus = jsonData.value("zhongMaBlueCount", std::array<int, 5> {});
	for (int i = 0; i < 5; i++) {
		InheritStatus[i] = inheritStatus[i] * 6;
	}
	InheritStatus[5] = 0;

	int venusLevelRed = jsonData.value("venusLevelRed", 0);
	int venusLevelBlue = jsonData.value("venusLevelBlue", 0);
	int venusLevelYellow = jsonData.value("venusLevelYellow", 0);
	VenusLevel[0] = venusLevelRed;
	VenusLevel[1] = venusLevelBlue;
	VenusLevel[2] = venusLevelYellow;

	std::array<int, 8> venusSpiritsBottom = jsonData.value("venusSpiritsBottom", std::array<int, 8> {});
	std::array<int, 6> venusSpiritsUpper = jsonData.value("venusSpiritsUpper", std::array<int, 6> {});
	for (int i = 0;i < 8;i++)
	{
		VenusSpirits[i] = SpiritData(venusSpiritsBottom[i]);
	}
	for (int i = 0;i < 6;i++)
	{
		VenusSpirits[i + 8] = SpiritData(venusSpiritsUpper[i]);
	}

	int venusAvailableWisdom = jsonData.value("venusAvailableWisdom", 0);
	if (venusAvailableWisdom == 0)
	{
		VenusSpirits[14] = SpiritData(Color::None, Status::None, 0, 0);
	}
	else
	{
		Color color = static_cast<Color>(venusAvailableWisdom - 1);
		VenusSpirits[14] = SpiritData(color, Status::None, 0, 0);
	}

	IsVenusCardActivated = jsonData.value("venusCardFirstClick", false);
	CanVenusCardHangout = jsonData.value("venusCardUnlockOutgoing", false);
	IsVenusCardZone = jsonData.value("venusCardIsQingRe", false);
	ZoneContinueTurns = jsonData.value("venusCardQingReContinuousTurns", 0);
	IsVenusCardHangoutUsed = jsonData.value("venusCardOutgoingUsed", std::array<bool, 5>{});

	std::array<std::array<bool, 8>, 5> cardDistribution = jsonData.value("cardDistribution", std::array<std::array<bool, 8>, 5>{{}});
	CardDistribution = { Train::None,Train::None, Train::None, Train::None, Train::None, Train::None };
	for (int facilityIndex = 0;facilityIndex < 5;facilityIndex++)
	{
		for (int cardIndex = 0;cardIndex < 6;cardIndex++)
		{
			if (cardDistribution[facilityIndex][cardIndex])
			{
				CardDistribution[cardIndex] = static_cast<Train>(facilityIndex);
			}
		}
	}

	CardHint = jsonData.value("cardHint", std::array<bool, 6>());

	std::array<int, 8>spiritDistribution = jsonData.value("spiritDistribution", std::array<int, 8>());
	for (int i = 0;i < 8;i++)
	{
		SpiritDistribution[i] = SpiritData(spiritDistribution[i]);
	}
}

DecisionSet::DecisionSet() {}
DecisionSet::DecisionSet(bool before, Decision during, Color after)
{
	BeforeTurnDecision = before;
	DuringTurnDecision = during;
	AfterTurnDecision = after;
}

DecisionSet::DecisionSet(int index)
{
	BeforeTurnDecision = index / 16 == 1;
	DuringTurnDecision = static_cast<Decision> (index % 16);
	AfterTurnDecision = Color::Random;
}

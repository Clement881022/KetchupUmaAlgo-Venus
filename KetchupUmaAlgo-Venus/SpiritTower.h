#pragma once
#include "Globals.h"

class SpiritTower
{
public:
	SpiritTower(std::array<SpiritData, 15> data, std::array<int, 3> venusLevel);
	void Reset(std::array<SpiritData, 15> data, std::array<int, 3> venusLevel);
	void Clear();
	void Update();

	bool IsFull();
	void Push(SpiritData spiritData);
	void Activate();
	std::array<int, 6> GetStatusBonus();

	bool Activated;
	std::array<std::array<SpiritData, 8>, 4> Tower;
	std::array<int, 3> VenusLevel;

private:
	void _updateNodes(int row, int column);
};
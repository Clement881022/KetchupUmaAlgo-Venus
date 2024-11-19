#include "SpiritTower.h"

SpiritTower::SpiritTower(std::array<SpiritData, 15> data, std::array<int, 3> venusLevel)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			Tower[i][j] = SpiritData(Color::None, Status::None, 0, 0);
		}
	}

	for (int i = 0;i < 8;i++)
	{
		Tower[3][i] = data[i];
	}
	for (int i = 0;i < 4;i++)
	{
		Tower[2][i] = data[i + 8];
		Tower[2][i].StatusAmount = Tower[3][i * 2].SpiritColor == Tower[3][i * 2 + 1].SpiritColor ? 2 : 3;
	}
	for (int i = 0;i < 2;i++)
	{
		Tower[1][i] = data[i + 8 + 4];
		Tower[1][i].StatusAmount = Tower[2][i * 2].SpiritColor == Tower[2][i * 2 + 1].SpiritColor ? 2 : 3;
	}
	Tower[0][0] = data[14];

	VenusLevel = venusLevel;
	Activated = false;
}

void SpiritTower::Reset(std::array<SpiritData, 15> data, std::array<int, 3> venusLevel)
{
	for (int i = 0;i < 8;i++)
	{
		Tower[3][i] = data[i];
	}
	for (int i = 0;i < 4;i++)
	{
		Tower[2][i] = data[i + 8];
		Tower[2][i].StatusAmount = Tower[3][i * 2 + 1].SpiritColor == Tower[3][i * 2 + 1].SpiritColor ? 2 : 3;
	}
	for (int i = 0;i < 2;i++)
	{
		Tower[1][i] = data[i + 8 + 4];
		Tower[1][i].StatusAmount = Tower[2][i * 2 + 1].SpiritColor == Tower[2][i * 2 + 1].SpiritColor ? 2 : 3;
	}
	Tower[0][0] = data[14];

	VenusLevel = venusLevel;
	Activated = false;
}

void SpiritTower::Clear()
{
	if (!Activated) return;
	SpiritData empty = SpiritData(Color::None, Status::None, 0, 0);
	for (int i = 0;i < 8;i++)
	{
		Tower[3][i] = empty;
	}
	for (int i = 0;i < 4;i++)
	{
		Tower[2][i] = empty;
	}
	for (int i = 0;i < 2;i++)
	{
		Tower[1][i] = empty;
	}
	Tower[0][0] = empty;
	Activated = false;
}

void SpiritTower::Update()
{
	Color left = Tower[3][0].SpiritColor;
	Color right = Tower[3][4].SpiritColor;
	int leftCount = 0;
	int rightCount = 0;

	for (int i = 0;i < 8;i++)
	{
		Tower[3][i].StatusAmount = 1;
		if (Tower[3][i].SpiritColor == left)  leftCount++;
		if (Tower[3][i].SpiritColor == right)  rightCount++;
	}

	for (int i = 0;i < 4;i++)
	{
		_updateNodes(2, i);
	}

	for (int i = 0;i < 2;i++)
	{
		_updateNodes(1, i);
	}

	if (!IsFull()) return;

	if (leftCount > rightCount)
	{
		Tower[0][0].SpiritColor = left;
	}
	else if (rightCount > leftCount)
	{
		Tower[0][0].SpiritColor = right;
	}
	else
	{
		Tower[0][0].SpiritColor = (GetZeroOneRandomFloat() < 0.5f) ? left : right;
	}
}

bool SpiritTower::IsFull()
{
	return !(Tower[3][7].SpiritColor == Color::None);
}

void SpiritTower::Push(SpiritData spiritData)
{
	int spiritAmount = spiritData.SpiritAmount;
	spiritData.SpiritAmount = 1;

	for (int i = 0;i < spiritAmount;i++)
	{
		for (int j = 0;j < 8;j++)
		{
			if (Tower[3][j].SpiritColor == Color::None)
			{
				Tower[3][j] = spiritData;
				break;
			}
		}
	}
	Update();
}

void SpiritTower::Activate()
{
	if (!IsFull()) return;
	Color color = Tower[0][0].SpiritColor;
	if (color == Color::None) return;

	Activated = true;
	int index = static_cast<int>(color);
	VenusLevel[index] += 1;
	VenusLevel[index] = std::min(5, VenusLevel[index]);
}

std::array<int, 6> SpiritTower::GetStatusBonus()
{
	std::array<int, 6> result = { 0,0,0,0,0,0 };
	for (int i = 1;i < 4;++i)
	{
		for (int j = 0;j < Tower[i].size();j++)
		{
			if (Tower[i][j].SpiritColor == Color::None) continue;
			int statusIndex = static_cast<int>(Tower[i][j].SpiritStatus);
			result[statusIndex] += Tower[i][j].StatusAmount;
		}
	}
	return result;
}

void SpiritTower::_updateNodes(int row, int column)
{
	if (Tower[row][column].IsEmpty() && !Tower[row + 1][column * 2].IsEmpty() && !Tower[row + 1][column * 2 + 1].IsEmpty())
	{
		Tower[row][column].SpiritColor = Tower[row + 1][column * 2].SpiritColor;

		Tower[row][column].SpiritStatus = GetZeroOneRandomFloat() < 0.8f
			? Tower[row + 1][column * 2].SpiritStatus
			: Tower[row + 1][column * 2 + 1].SpiritStatus;

		Tower[row][column].StatusAmount = (Tower[row + 1][column * 2].SpiritColor ==
			Tower[row + 1][column * 2 + 1].SpiritColor) ? 2 : 3;

		Tower[row][column].SpiritAmount = 1;
	}
}

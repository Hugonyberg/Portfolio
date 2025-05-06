#include "Heatmap.h"
#include <DreamEngine\utilities\UtilityFunctions.h>
#include <algorithm>
#include <random>
#include <queue>

Heatmap::Heatmap(const DE::Vector2f aPlayableAreaMinimumPoint, const DE::Vector2f aPlayableAreaMaximumPoint, const float aCellScale, const DE::Color& aColor)
{
	myGrid = new WorldGrid(aPlayableAreaMinimumPoint, aPlayableAreaMaximumPoint, aCellScale);
	auto mapDimensions = myGrid->GetIndexDimensions();
	myHeatNodes.resize(mapDimensions.x);
	for (auto& column : myHeatNodes)
	{
		column.resize(mapDimensions.y);
		for (auto& heatNode : column)
		{
			heatNode = 0.0f;
		}
	}
}

void Heatmap::Update(const float aDeltaTime, const bool aShouldDiffuse, const float aDecayOrDiffusionRate)
{
	for (int x = 0; x < myHeatNodes.size(); x++)
	{
		for (int y = 0; y < myHeatNodes[x].size(); y++)
		{
			if (aShouldDiffuse)
			{
				if (x > 0)
					myHeatNodes[x][y] += aDecayOrDiffusionRate * myHeatNodes[x - 1][y] * aDeltaTime;

				if (x < myHeatNodes.size() - 1)
					myHeatNodes[x][y] += aDecayOrDiffusionRate * myHeatNodes[x + 1][y] * aDeltaTime;

				if (y > 0)
					myHeatNodes[x][y] += aDecayOrDiffusionRate * myHeatNodes[x][y - 1] * aDeltaTime;

				if (y < myHeatNodes[x].size() - 1)
					myHeatNodes[x][y] += aDecayOrDiffusionRate * myHeatNodes[x][y + 1] * aDeltaTime;
			}
			else if (myHeatNodes[x][y] > 0.0f)
			{
				myHeatNodes[x][y] -= aDecayOrDiffusionRate * aDeltaTime;
				if (myHeatNodes[x][y] < 0.0f)
				{
					myHeatNodes[x][y] = 0.0f;
				}
			}
		}
	}
}

const float Heatmap::GetHeatAtPosition(const DE::Vector2f aPosition) const
{
	auto indexPair = myGrid->GetIndicesAtPosition(aPosition);

	if (indexPair.x >= 0 && indexPair.x < myHeatNodes.size() && indexPair.y >= 0 && indexPair.y < myHeatNodes[0].size())
	{
		return myHeatNodes[indexPair.x][indexPair.y];
	}
	return 0.0f;
}

void Heatmap::AddHeat(const DE::Vector2f aPosition, const float anAmount)
{
	auto indexPair = myGrid->GetIndicesAtPosition(aPosition);

	if (indexPair.x >= 0 && indexPair.x < myHeatNodes.size() && indexPair.y >= 0 && indexPair.y < myHeatNodes[0].size())
	{
		myHeatNodes[indexPair.x][indexPair.y] = UtilityFunctions::Min(1.0f, myHeatNodes[indexPair.x][indexPair.y] + anAmount);
	}
}

void Heatmap::ClearHeat()
{
	for (int x = 0; x < myHeatNodes.size(); x++)
	{
		for (int y = 0; y < myHeatNodes[x].size(); y++)
		{
			myHeatNodes[x][y] = 0.0f;
		}
	}
}
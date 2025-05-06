#pragma once
#include "WorldGrid.h"
#include <DreamEngine\graphics\sprite.h>

class AIPawn;

class Heatmap
{
public:
	Heatmap() = delete;
	Heatmap(const DE::Vector2f aPlayableAreaMinimumPoint, const DE::Vector2f aPlayableAreaMaximumPoint, const float aCellScale, const DE::Color& aColor);
	Heatmap(const Heatmap& anOtherHeatmap) = delete;
	Heatmap& operator=(const Heatmap& anOtherHeatmap) = delete;
	Heatmap(Heatmap&& anOtherHeatmap) = default;
	Heatmap& operator=(Heatmap&& anOtherHeatmap) = default;
	~Heatmap() = default;

	void Update(const float aDeltaTime, const bool aShouldDiffuse, const float aDecayOrDiffusionRate);
	const float GetHeatAtPosition(const DE::Vector2f aPosition) const;
	void AddHeat(const DE::Vector2f aPosition, const float anAmount);
	void ClearHeat();

private:
	WorldGrid* myGrid;
	std::vector<std::vector<float>> myHeatNodes;
	DE::Color myMainColor;
};
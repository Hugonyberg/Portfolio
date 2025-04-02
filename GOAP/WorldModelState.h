#pragma once

struct WorldState
{
	int food = 0;
	int wood = 0;
	int stone = 0;
	int gold = 0;

	int population = 0;
	int houses = 0;

	int numOfFarmers = 0;
	int maxNumOfFarmers = 0;

	int numOfSoldiers = 0;
	int maxNumOfSoldiers = 0;

	bool operator>=(const WorldState& other) const 
	{
		return (food >= other.food) &&
			(wood >= other.wood) &&
			(stone >= other.stone) &&
			(gold >= other.gold) &&
			(population >= other.population) &&
			(houses >= other.houses) &&
			(numOfSoldiers >= other.numOfSoldiers) &&
			(maxNumOfSoldiers >= other.maxNumOfSoldiers) &&
			(numOfFarmers >= other.numOfFarmers) &&
			(maxNumOfFarmers >= other.maxNumOfFarmers);
	}

	bool operator!=(const WorldState& other) const
	{
		return (food != other.food) ||
			(wood != other.wood) ||
			(stone != other.stone) ||
			(gold != other.gold) ||
			(population != other.population) ||
			(houses != other.houses) ||
			(numOfSoldiers != other.numOfSoldiers) ||
			(maxNumOfSoldiers != other.maxNumOfSoldiers) ||
			(numOfFarmers != other.numOfFarmers) ||
			(maxNumOfFarmers != other.maxNumOfFarmers);
	}

	int GetStateCost() const
	{
		int cost = 0;
		cost += food;
		cost += wood;
		cost += stone;
		cost += gold;
		cost += population;
		cost += houses;
		cost += numOfSoldiers;
		cost += maxNumOfSoldiers;
		cost += numOfFarmers;
		cost += maxNumOfFarmers;
		return cost;
	}
};
#pragma once

enum
{
	invalid_node_index = -1
};


enum class TerrainType : int
{
	Ground = 1,
	Intersection = 2,
	Mud = 3,
	// Node's with a value of over 200 000 are always isolated
	Water = 200001,
	Building = 200002
};
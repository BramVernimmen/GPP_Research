/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
#include "Car.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAstar.h"
#include "framework\EliteAI\EliteGraphs\EGridGraph.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------

namespace BT_Actions
{
	Elite::BehaviorState CreateNewPath(Elite::Blackboard* pBlackboard)
	{
		Elite::GridGraph<Elite::GridTerrainNode, Elite::GraphConnection>* pGridGraph;
		if (pBlackboard->GetData("GridGraph", pGridGraph) == false)
			return Elite::BehaviorState::Failure;

		if (pGridGraph == nullptr)
			return Elite::BehaviorState::Failure;


		Car* pCar;
		if (pBlackboard->GetData("Car", pCar) == false)
			return Elite::BehaviorState::Failure;

		if (pCar == nullptr)
			return Elite::BehaviorState::Failure;


		Elite::Heuristic heuristic;
		if (pBlackboard->GetData("Heuristic", heuristic) == false)
			return Elite::BehaviorState::Failure;

		if (heuristic == nullptr)
			return Elite::BehaviorState::Failure;



		const int amountOfNodes{ pGridGraph->GetNrOfNodes() };

		const int startNodeIdx{ pGridGraph->GetNodeIdxAtWorldPos(pCar->GetPosition()) };

		int endNodeIdx{ rand() % amountOfNodes };
		// get a random node idx
		while (endNodeIdx == startNodeIdx || pGridGraph->GetNode(endNodeIdx)->GetTerrainType() == TerrainType::Building)
		{
			//randomize another end node
			endNodeIdx = rand() % amountOfNodes;
		}

		// here we have our start and end node, now get the path

		// before calculating our path, change the cost of the intersections
		// after calculating, change it back

		// the reason why we do this is because otherwise we will always do U turns
		// making them really expensive right before calculating, prevents this

		float intersectionCost{ 250.f };
		//// change the connection costs of the 2x2 intersections
		pGridGraph->GetConnection(80, 100)->SetCost(intersectionCost); // vertical
		pGridGraph->GetConnection(119, 99)->SetCost(intersectionCost); // vertical
		pGridGraph->ChangeIntersectionConnectionCosts(104, 84, 85, 105, intersectionCost);
		pGridGraph->ChangeIntersectionConnectionCosts(109, 89, 90, 110, intersectionCost);
		pGridGraph->ChangeIntersectionConnectionCosts(114, 94, 95, 115, intersectionCost);
		//BFS Pathfinding
		//auto pathfinder = BFS<GridTerrainNode, GraphConnection>(m_pGridGraph);
		auto pathfinder = Elite::AStar<Elite::GridTerrainNode, Elite::GraphConnection>(pGridGraph, heuristic);
		auto startNode = pGridGraph->GetNode(startNodeIdx);
		auto endNode = pGridGraph->GetNode(endNodeIdx);

		std::vector<Elite::GridTerrainNode*> path{ pathfinder.FindPath(startNode, endNode) };

		float redoInterdectionCostFastLane{ 50 };
		float redoInterdectionCostSlowLane{ 30 };
		//// change the connection costs of the 2x2 intersections
		pGridGraph->ChangeIntersectionConnectionCosts(104, 84, 85, 105, redoInterdectionCostFastLane);
		pGridGraph->ChangeIntersectionConnectionCosts(109, 89, 90, 110, redoInterdectionCostFastLane);
		pGridGraph->ChangeIntersectionConnectionCosts(114, 94, 95, 115, redoInterdectionCostFastLane);
		pGridGraph->GetConnection(80, 100)->SetCost(redoInterdectionCostSlowLane); // vertical
		pGridGraph->GetConnection(119, 99)->SetCost(redoInterdectionCostSlowLane); // vertical


		// now we have the path, our Car uses a list, so we can PopFront on arrival
		// turn the path into a list of positions

		std::list<Elite::Vector2> pathPositionList;

		for (const auto& currNode : path)
		{
			pathPositionList.push_back(pGridGraph->GetNodeWorldPos(currNode->GetIndex()));
		}

		pCar->SetPath(pathPositionList);
		pCar->SetEndPos(pathPositionList.back());

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState SetToSeek(Elite::Blackboard* pBlackboard)
	{
		Car* pCar;
		if (pBlackboard->GetData("Car", pCar) == false)
			return Elite::BehaviorState::Failure;

		if (pCar == nullptr)
			return Elite::BehaviorState::Failure;


		const Elite::Vector2 nextTarget{ pCar->GetPath().front() };


		pCar->SetToSeek(nextTarget);


		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState SlowDown(Elite::Blackboard* pBlackboard)
	{
		Car* pCar;
		if (pBlackboard->GetData("Car", pCar) == false)
			return Elite::BehaviorState::Failure;

		if (pCar == nullptr)
			return Elite::BehaviorState::Failure;

		//pCar->SetMaxLinearSpeed(-20.0f);
		pCar->SetMaxLinearSpeed(0.0f);


		return Elite::BehaviorState::Success;
	}
	
	Elite::BehaviorState SyncSpeedToRoad(Elite::Blackboard* pBlackboard)
	{
		// set the speed here
		// take the connection value of the connection, divide by 3
		// do this to avoid going to fast and going off-grid

		// get current node, get the next node in our path
		// get the connectionCost between the nodes
		// divide that by 3, set as speed; done

		Car* pCar;
		if (pBlackboard->GetData("Car", pCar) == false)
			return Elite::BehaviorState::Failure;

		if (pCar == nullptr)
			return Elite::BehaviorState::Failure;


		Elite::GridGraph<Elite::GridTerrainNode, Elite::GraphConnection>* pGridGraph;
		if (pBlackboard->GetData("GridGraph", pGridGraph) == false)
			return Elite::BehaviorState::Failure;

		if (pGridGraph == nullptr)
			return Elite::BehaviorState::Failure;


		const int currentNodeIdx{ pGridGraph->GetNodeAtWorldPos(pCar->GetPosition())->GetIndex() };
		const int nextNodeIdx{ pGridGraph->GetNodeAtWorldPos(pCar->GetPath().front())->GetIndex() };

		const auto connection{ pGridGraph->GetConnection(currentNodeIdx, nextNodeIdx) }; 
		// safety check
		if (connection == nullptr) // connection doesn't exist
		{
			pCar->SetMaxLinearSpeed(5.f);
			return Elite::BehaviorState::Success;
		}
		const float cost{ pGridGraph->GetConnection(currentNodeIdx, nextNodeIdx)->GetCost() };
		//const float newSpeed{ cost / 4.f };
		const float newSpeed{ cost / 4.f };

		pCar->SetMaxLinearSpeed(newSpeed);


		return Elite::BehaviorState::Success;
	}
	
	Elite::BehaviorState CopyTemplate(Elite::Blackboard* pBlackboard)
	{
		return Elite::BehaviorState::Success;
	}

}


//-----------------------------------------------------------------
// Conditions
//-----------------------------------------------------------------

namespace BT_Conditions
{
	bool HasArrived(Elite::Blackboard* pBlackboard)
	{
		Elite::GridGraph<Elite::GridTerrainNode, Elite::GraphConnection>* pGridGraph;
		if (pBlackboard->GetData("GridGraph", pGridGraph) == false)
			return false;

		if (pGridGraph == nullptr)
			return false;


		Car* pCar;
		if (pBlackboard->GetData("Car", pCar) == false)
			return false;

		if (pCar == nullptr)
			return false;

		// if the Node Index of the position of the car is the same as the Node index as the end position
		// the car has arrived; we need to calculate a new path

		const int currentNodePosIdx{ pGridGraph->GetNodeIdxAtWorldPos(pCar->GetPosition()) };
		const int endNodeIdx{ pGridGraph->GetNodeIdxAtWorldPos(pCar->GetEndPos()) };

		if (currentNodePosIdx == endNodeIdx)
			return true;



		return false;
	}


	bool IsAtNextPos(Elite::Blackboard* pBlackboard)
	{
		Elite::GridGraph<Elite::GridTerrainNode, Elite::GraphConnection>* pGridGraph;
		if (pBlackboard->GetData("GridGraph", pGridGraph) == false)
			return false;

		if (pGridGraph == nullptr)
			return false;


		Car* pCar;
		if (pBlackboard->GetData("Car", pCar) == false)
			return false;

		if (pCar == nullptr)
			return false;

		// if the Node Index of the position of the car is the same as the Node index as the end position
		// the car has arrived; we need to calculate a new path

		const int currentNodePosIdx{ pGridGraph->GetNodeIdxAtWorldPos(pCar->GetPosition()) };
		const int nextNodeIdx{ pGridGraph->GetNodeIdxAtWorldPos(pCar->GetPath().front()) };

		if (currentNodePosIdx == nextNodeIdx)
		{
			// remove the first element from the path
			pCar->GetPath().pop_front();
			return true;
		}



		return false;
	}

	bool GiveWayToCar(Elite::Blackboard* pBlackboard)
	{

		// check all cars to see if they are in the cone of vision
		// if they are, give way to them; slow down
		Car* pCar;
		if (pBlackboard->GetData("Car", pCar) == false)
			return false;

		if (pCar == nullptr)
			return false;


		const Elite::Vector2 carPos{ pCar->GetPosition() };
		const float radius{ 40.f };
		const float baseRotation{ pCar->GetRotation() };
		float startAngle{baseRotation - Elite::ToRadians(75.f)};
		float endAngle{baseRotation + Elite::ToRadians(20.f) };

		// correct them if neccessary
		if (startAngle < -static_cast<float>(M_PI))
			startAngle = startAngle + static_cast<float>(M_PI) * 2;

		if (startAngle > static_cast<float>(M_PI))
			startAngle = startAngle - static_cast<float>(M_PI) * 2;


		if (endAngle < -static_cast<float>(M_PI))
			endAngle = endAngle + static_cast<float>(M_PI) * 2;


		if (endAngle > static_cast<float>(M_PI))
			endAngle = endAngle - static_cast<float>(M_PI) * 2;


		std::vector<Car*> *pCarsVec;
		if (pBlackboard->GetData("CarsVector", pCarsVec) == false)
			return false;


		for (const auto& currCar : *pCarsVec)
		{
			if (currCar == pCar)
				continue;
			if (Elite::IsPointInCone(currCar->GetPosition(), carPos, radius, startAngle, endAngle))
			{
				// check if they are moving, if they aren't, they could be at a red light
				if (currCar->GetMaxLinearSpeed() != 0.0f)
				{
					// check the angle between the cars, if the one in our cone is going to the right, there is no need to slow down
					// if there is one in front of use, depending on the distance, we should slow down
					const float angleBetweenCars{ Elite::AngleBetween(pCar->GetDirection(), currCar->GetDirection()) };
					
					if (angleBetweenCars <= 0.5f && angleBetweenCars >= -0.5f)
					{
						// car is in front of use
						if (Elite::DistanceSquared(carPos, currCar->GetPosition()) <= ((radius / 2.f) * (radius / 2.f)))
							return true; // too close; slow down
					}
					else if (angleBetweenCars >= 1.f && angleBetweenCars <= 2.f) // if the angle is around 90° and is positive, the car is moving towards us; we need to stop
						return true;
					//std::cout << angleBetweenCars << "\n";

				}
			}
		}

		return false;
	}
}







#endif
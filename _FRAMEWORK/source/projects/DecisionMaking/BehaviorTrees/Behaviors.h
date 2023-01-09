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
#include "projects/Shared/Agario/AgarioAgent.h"
#include "projects/Shared/Agario/AgarioFood.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------

namespace BT_Actions
{
	Elite::BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
	{
		AgarioAgent* pAgent;

		if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
			return Elite::BehaviorState::Failure;


		pAgent->SetToWander();

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToSeekFood(Elite::Blackboard* pBlackboard)
	{
		AgarioAgent* pAgent;
		Elite::Vector2 targetPos;

		if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
			return Elite::BehaviorState::Failure;
		if(pBlackboard->GetData("Target", targetPos) == false)
			return Elite::BehaviorState::Failure;


		pAgent->SetToSeek(targetPos);

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToEvadeBigger(Elite::Blackboard* pBlackboard)
	{
		AgarioAgent* pAgent;
		AgarioAgent* pEvadeTarget;

		if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
			return Elite::BehaviorState::Failure;
		if(pBlackboard->GetData("AgentFleeTarget", pEvadeTarget) == false || pEvadeTarget == nullptr)
			return Elite::BehaviorState::Failure;

		const Elite::Vector2 evadeTargetPos{ pEvadeTarget->GetPosition() };

		pAgent->SetToEvade(evadeTargetPos);
		pAgent->ChangeEvadeRadius(pAgent->GetPosition().Distance(evadeTargetPos) + 2.f); // +2 for making sure

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToPursueSmaller(Elite::Blackboard* pBlackboard)
	{
		AgarioAgent* pAgent;
		AgarioAgent* pEvadeTarget;

		if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
			return Elite::BehaviorState::Failure;
		if(pBlackboard->GetData("AgentPursueTarget", pEvadeTarget) == false || pEvadeTarget == nullptr)
			return Elite::BehaviorState::Failure;

		

		pAgent->SetToPursue(pEvadeTarget->GetPosition());

		return Elite::BehaviorState::Success;
	}
}


//-----------------------------------------------------------------
// Conditions
//-----------------------------------------------------------------

namespace BT_Conditions
{
	bool IsFoodNearby(Elite::Blackboard* pBlackboard)
	{
		AgarioAgent* pAgent;
		std::vector<AgarioFood*>* pFoodVec;
		if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
			return false;
		if(pBlackboard->GetData("FoodVec", pFoodVec) == false || pFoodVec == nullptr)
			return false;
		if (pFoodVec->empty())
			return false;
		

		const float searchRadius{ pAgent->GetRadius() + 20.f };

		float closestDistSq{searchRadius * searchRadius};
		AgarioFood* pClosestFood{ nullptr };
		const Elite::Vector2 agentPos{ pAgent->GetPosition() };
		//DEBUGRENDERER2D->DrawCircle(agentPos, searchRadius, Elite::Color{ 0,1,0 }, DEBUGRENDERER2D->NextDepthSlice());

		for (auto& pFood : *pFoodVec)
		{
			float distSq = pFood->GetPosition().DistanceSquared(agentPos);

			if (distSq < closestDistSq)
			{
				closestDistSq = distSq;
				pClosestFood = pFood;
			}
		}

		if (pClosestFood != nullptr)
		{
			pBlackboard->ChangeData("Target", pClosestFood->GetPosition());
			return true;
		}

		return false;
	}


	bool IsBiggerAgentNearby(Elite::Blackboard* pBlackboard)
	{
		AgarioAgent* pAgent;
		std::vector<AgarioAgent*>* pAgentVec;
		if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
			return false;
		if (pBlackboard->GetData("AgentsVec", pAgentVec) == false || pAgentVec == nullptr)
			return false;
		if (pAgentVec->empty())
			return false;


		const float evadeRadius{ pAgent->GetRadius() + 20.f };

		float closestDistSq{ evadeRadius };
		AgarioAgent* pClosestBiggerAgent{ nullptr };
		const Elite::Vector2 agentPos{ pAgent->GetPosition() };

		//DEBUGRENDERER2D->DrawCircle(agentPos, evadeRadius, Elite::Color{ 0,1,1 }, DEBUGRENDERER2D->NextDepthSlice());

		const float agentSize{ pAgent->GetRadius() };

		for (auto& pOtherAgent : *pAgentVec)
		{
			if (agentSize >= pOtherAgent->GetRadius())
				continue;

			float distSq = pOtherAgent->GetPosition().Distance(agentPos) - pOtherAgent->GetRadius();

			if (distSq < closestDistSq)
			{
				closestDistSq = distSq;
				pClosestBiggerAgent = pOtherAgent;
			}
		}

		if (pClosestBiggerAgent != nullptr)
		{
			pBlackboard->ChangeData("AgentFleeTarget", pClosestBiggerAgent);
			return true;
		}


		return false;
	}


	bool IsSmallerAgentNearby(Elite::Blackboard* pBlackboard)
	{
		AgarioAgent* pAgent;
		std::vector<AgarioAgent*>* pAgentVec;
		if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
			return false;
		if (pBlackboard->GetData("AgentsVec", pAgentVec) == false || pAgentVec == nullptr)
			return false;
		if (pAgentVec->empty())
			return false;


		const float pursueRadius{ pAgent->GetRadius() + 20.f };

		float closestDistSq{ pursueRadius };
		AgarioAgent* pClosestSmallerAgent{ nullptr };
		const Elite::Vector2 agentPos{ pAgent->GetPosition() };

		DEBUGRENDERER2D->DrawCircle(agentPos, pursueRadius, Elite::Color{ 1,1,0 }, DEBUGRENDERER2D->NextDepthSlice());

		const float agentSize{ pAgent->GetRadius() };

		for (auto& pOtherAgent : *pAgentVec)
		{
			if (agentSize - 1 <= pOtherAgent->GetRadius())
				continue;

			float distSq = pOtherAgent->GetPosition().Distance(agentPos) - pOtherAgent->GetRadius();

			if (distSq < closestDistSq)
			{
				closestDistSq = distSq;
				pClosestSmallerAgent = pOtherAgent;
			}
		}

		if (pClosestSmallerAgent != nullptr)
		{
			pBlackboard->ChangeData("AgentPursueTarget", pClosestSmallerAgent);
			return true;
		}


		return false;
	}
}







#endif
#include "stdafx.h"
#include "StatesAndTransitions.h"


using namespace Elite;
using namespace FSMStates;
using namespace FSMConditions;

void WanderState::OnEnter(Blackboard* pBlackboard)
{
	AgarioAgent* pAgent;
	bool isValid = pBlackboard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
		return;

	pAgent->SetToWander();
}

void SeekFoodState::OnEnter(Blackboard* pBlackboard)
{
	AgarioAgent* pAgent;
	bool isValid = pBlackboard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
		return;

	AgarioFood* pNearestFood;
	if (pBlackboard->GetData("NearestFood", pNearestFood) == false || pNearestFood == nullptr)
		return;

	pAgent->SetToSeek(pNearestFood->GetPosition());
}


void EvadeAgentState::OnEnter(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent;
	bool isValid = pBlackboard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
		return;

	AgarioAgent* pNearestBiggerAgent;
	if (pBlackboard->GetData("NearestBiggerAgent", pNearestBiggerAgent) == false || pNearestBiggerAgent == nullptr)
		return;

	pAgent->SetToEvade(pNearestBiggerAgent->GetPosition());
	//pAgent->SetToFlee(pNearestBiggerAgent->GetPosition());

	float newRadius;
	pBlackboard->GetData("EvadeRadius", newRadius);
	pAgent->ChangeEvadeRadius(newRadius);
	// to get an accurate fleeradius (default 10)
	// we change the radius depending on the pAgent size
	// radius should also take account for the biggerAgent size

}

void EvadeAgentState::Update(Blackboard* pBlackboard, float deltaTime)
{
	AgarioAgent* pAgent;
	bool isValid = pBlackboard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
		return;

	AgarioAgent* pNearestBiggerAgent;
	if (pBlackboard->GetData("NearestBiggerAgent", pNearestBiggerAgent) == false || pNearestBiggerAgent == nullptr)
		return;

	float newRadius;
	pBlackboard->GetData("EvadeRadius", newRadius);
	pAgent->ChangeEvadeRadius(newRadius);
	pAgent->SetToEvade(pNearestBiggerAgent->GetPosition());

}


void PursueAgentState::OnEnter(Blackboard* pBlackboard)
{
	AgarioAgent* pAgent;
	bool isValid = pBlackboard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
		return;

	AgarioAgent* pNearestSmallerAgent;
	if (pBlackboard->GetData("NearestSmallerAgent", pNearestSmallerAgent) == false || pNearestSmallerAgent == nullptr)
		return;

	pAgent->SetToPursue(pNearestSmallerAgent->GetPosition());
}

void PursueAgentState::Update(Blackboard* pBlackboard, float deltaTime)
{
	AgarioAgent* pAgent;
	bool isValid = pBlackboard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
		return;

	AgarioAgent* pNearestSmallerAgent;
	if (pBlackboard->GetData("NearestSmallerAgent", pNearestSmallerAgent) == false || pNearestSmallerAgent == nullptr)
		return;

	pAgent->SetToPursue(pNearestSmallerAgent->GetPosition());
}



void FleeFromWallState::OnEnter(Blackboard* pBlackboard)
{
	AgarioAgent* pAgent;
	bool isValid = pBlackboard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
		return;

	AgarioAgent* pNearestBiggerAgent;
	if (pBlackboard->GetData("NearestBiggerAgent", pNearestBiggerAgent) == false || pNearestBiggerAgent == nullptr)
		return;

	std::vector<AgarioAgent*>* pOtherAgents;
	if (pBlackboard->GetData("AgentVec", pOtherAgents) == false || pOtherAgents == nullptr)
		return; 

	// get wall evade position
	Vector2 wallEvadePos;
	if (pBlackboard->GetData("WallEvadePos", wallEvadePos) == false)
		return;


	float newRadius{ 10.f + pAgent->GetRadius() };
	//pBlackboard->GetData("EvadeRadius", newRadius);
	pAgent->ChangeEvadeRadius(newRadius);

	bool agentStillExists{ false };
	for (const auto& currAgent : *pOtherAgents)
	{
		if (currAgent == pNearestBiggerAgent)
		{
			agentStillExists = true;
			break;
		}
	}

	if (agentStillExists == true)
	{
		Vector2 nearestAgentPos{ pNearestBiggerAgent->GetPosition() };
		// check if inside radius
		if (nearestAgentPos.Distance(pAgent->GetPosition()) - pNearestBiggerAgent->GetRadius() < newRadius)
		{
			// take halfway point, use that as evade pos
			Vector2 newEvadePos{ nearestAgentPos + (wallEvadePos - nearestAgentPos) / 2.0f };
			DEBUGRENDERER2D->DrawCircle(newEvadePos, 3.f, Color{ 1.f, 1.f, 1.f }, 0.f);
			//std::cout << "BIGGER\n";
			pAgent->SetToEvade(newEvadePos);
			return;
		} // else go down
	}
	// else
	// set evade to wall position
	//std::cout << "JUST WALLPOS\n";

	pAgent->SetToEvade(wallEvadePos);
}

void FleeFromWallState::Update(Blackboard* pBlackboard, float deltaTime)
{
	OnEnter(pBlackboard);
}








bool FoodNearbyCondition::Evaluate(Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	std::vector<AgarioFood*>* pFoodVec;

	if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
		return false;
	if (pBlackboard->GetData("FoodVec", pFoodVec) == false || pFoodVec == nullptr)
		return false;

	const float radius{ 10.0f + pAgent->GetRadius()};

	Vector2 agentPos = pAgent->GetPosition();

	DEBUGRENDERER2D->DrawCircle(agentPos, radius, Color{ 1.0f, 0.f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());

	auto isCloser = [agentPos](AgarioFood* pFood1, AgarioFood* pFood2) 
	{
		float dist1 = pFood1->GetPosition().DistanceSquared(agentPos);
		float dist2 = pFood2->GetPosition().DistanceSquared(agentPos);

		return dist1 < dist2;
	};

	auto closestElementIt = std::min_element(pFoodVec->begin(), pFoodVec->end(), isCloser);

	if (closestElementIt == pFoodVec->end())
		return false;

	AgarioFood* closestFood = *closestElementIt;
	if (closestFood->GetPosition().DistanceSquared(agentPos) < radius * radius)
	{
		pBlackboard->ChangeData("NearestFood", closestFood);
		return true;
	}

	

	return false;
}

bool NearbyFoodIsGone::Evaluate(Blackboard* pBlackboard) const
{
	std::vector<AgarioFood*>* pFoodVec;
	AgarioFood* pNearestFood;

	if (pBlackboard->GetData("FoodVec", pFoodVec) == false || pFoodVec == nullptr)
		return false;
	if (pBlackboard->GetData("NearestFood", pNearestFood) == false || pNearestFood == nullptr)
		return false;

	// check if the nearestfood is still in the vector
	for (auto& currFood: *pFoodVec)
	{
		if (currFood == pNearestFood)
			return false; // still in there
	}

	return true;
}

bool EvadeBiggerAgents::Evaluate(Blackboard* pBlackboard) const
{
	// when bigger agents are in radius, evade them
	AgarioAgent* pAgent;
	std::vector<AgarioAgent*>* pOtherAgents;

	if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
		return false;
	if (pBlackboard->GetData("AgentVec", pOtherAgents) == false || pOtherAgents == nullptr)
		return false;

	const float agentRadius{ pAgent->GetRadius() }; // doesn't change better to cache instead of constantly calling
	const float radius{ 10.0f + agentRadius};
	Vector2 agentPos = pAgent->GetPosition();
	//std::cout << "CurrRadius: " << agentRadius << "\n";

	DEBUGRENDERER2D->DrawCircle(agentPos, radius, Color{ 1.0f, 0.f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());

	
	AgarioAgent* closestAgent{nullptr};
	float currentShortestDistance{FLT_MAX};

	for (const auto& currAgent : * pOtherAgents)
	{
		if (currAgent->GetRadius() <= agentRadius) // only need to evade bigger ones
			continue;

		float newDistance{ currAgent->GetPosition().Distance(agentPos) - currAgent->GetRadius()};
		if (newDistance < currentShortestDistance)
		{
			closestAgent = currAgent;
			currentShortestDistance = newDistance;
		}
	}


	if (closestAgent == nullptr)
		return false;

	if (currentShortestDistance < radius)
	{
		pBlackboard->ChangeData("NearestBiggerAgent", closestAgent);
		//std::cout << "EVADE\n";
		return true;
	}



	return false;
}

bool CanStopEvading::Evaluate(Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	AgarioAgent* pAgentToEvade;
	std::vector<AgarioAgent*>* pOtherAgents;

	if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
		return false;
	if (pBlackboard->GetData("NearestBiggerAgent", pAgentToEvade) == false || pAgentToEvade == nullptr)
		return true; // agent to evade doesn't exist / is invalid -> we don't need to evade
	if (pBlackboard->GetData("AgentVec", pOtherAgents) == false || pOtherAgents == nullptr)
		return true; // same as above

	// first loop through the vector of agents to make sure the target is in there
	bool isAgentStillAlive{ false };
	Vector2 agentPos = pAgent->GetPosition();

	for (const auto& currAgent : *pOtherAgents)
	{
		if (currAgent == pAgentToEvade)
			isAgentStillAlive = true;
	}

	if (isAgentStillAlive == false)
		return true;

	const float currDistanceToAgent{ pAgentToEvade->GetPosition().Distance(agentPos) - pAgentToEvade->GetRadius()};

	for (const auto& currAgent : *pOtherAgents)
	{
		if (currDistanceToAgent > currAgent->GetPosition().Distance(agentPos) - currAgent->GetRadius() && currAgent->GetRadius() > pAgent->GetRadius())
		{
			//std::cout << "Early Exit\n";
			return true; // new closer agent has been found
		}
		
	}



	const float agentRadius{ pAgent->GetRadius() }; // doesn't change better to cache instead of constantly calling
	
	// only need to evade when it's bigger, check if changed
	if (agentRadius > pAgentToEvade->GetRadius())
		return true;

	const float outerRadius{ 15.0f + agentRadius }; // when the agent to evade is outside this radius, we can safely stop evading
	//pBlackboard->ChangeData("EvadeRadius", outerRadius + pAgentToEvade->GetRadius() * 2 - agentRadius + agentRadius * 2);
	//pBlackboard->ChangeData("EvadeRadius", outerRadius + pAgentToEvade->GetRadius() - agentRadius + agentRadius * 2);
	pBlackboard->ChangeData("EvadeRadius", outerRadius + pAgentToEvade->GetRadius());




	DEBUGRENDERER2D->DrawCircle(agentPos, outerRadius, Color{ 1.0f, 0.0f, 0.0f }, DEBUGRENDERER2D->NextDepthSlice());
	DEBUGRENDERER2D->DrawCircle(pAgentToEvade->GetPosition(), pAgentToEvade->GetRadius() + 2, Color{ 0.f, 1.f, 0.f }, DEBUGRENDERER2D->NextDepthSlice());


	if (pAgentToEvade->GetPosition().Distance(agentPos) - pAgentToEvade->GetRadius() > outerRadius)
	{
		//std::cout << "STOP EVADE\n";

		return true;
	}

	return false;
}

bool PursueSmallerAgents::Evaluate(Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	std::vector<AgarioAgent*>* pOtherAgents;

	if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
		return false;
	if (pBlackboard->GetData("AgentVec", pOtherAgents) == false || pOtherAgents == nullptr)
		return false;

	const float agentRadius{ pAgent->GetRadius() };
	const float radius{ 10.0f + agentRadius };

	Vector2 agentPos = pAgent->GetPosition();

	DEBUGRENDERER2D->DrawCircle(agentPos, radius, Color{ 1.0f, 0.f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());



	AgarioAgent* closestAgent{ nullptr };
	float currentShortestDistance{ FLT_MAX };

	for (const auto& currAgent : *pOtherAgents)
	{
		if (currAgent->GetRadius() >= agentRadius - 1) // only need to evade bigger ones
			continue;

		float newDistance{ currAgent->GetPosition().Distance(agentPos) - currAgent->GetRadius() };
		if (newDistance < currentShortestDistance)
		{
			closestAgent = currAgent;
			currentShortestDistance = newDistance;
		}
	}


	if (closestAgent == nullptr)
		return false;

	if (currentShortestDistance < radius)
	{
		pBlackboard->ChangeData("NearestSmallerAgent", closestAgent);
		//std::cout << "PURSUE\n";
		return true;
	}



	return false;
}

bool CanStopPursuing::Evaluate(Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	AgarioAgent* pAgentToPursue;
	std::vector<AgarioAgent*>* pOtherAgents;

	if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
		return false;
	if (pBlackboard->GetData("NearestSmallerAgent", pAgentToPursue) == false || pAgentToPursue == nullptr)
		return true; // agent to pursue doesn't exist / is invalid -> we don't need to pursue
	if (pBlackboard->GetData("AgentVec", pOtherAgents) == false || pOtherAgents == nullptr)
		return true; // same as above

	// first loop through the vector of agents to make sure the target is in there
	bool isAgentStillAlive{ false };
	Vector2 agentPos = pAgent->GetPosition();

	for (const auto& currAgent : *pOtherAgents)
	{
		if (currAgent == pAgentToPursue)
			isAgentStillAlive = true;
	}

	if (isAgentStillAlive == false)
	{
		//std::cout << "AGENT DIED\n";
		return true;
	}

	const float currDistanceToAgent{ pAgentToPursue->GetPosition().Distance(agentPos) - pAgentToPursue->GetRadius() };

	for (const auto& currAgent : *pOtherAgents)
	{
		if (currDistanceToAgent > currAgent->GetPosition().Distance(agentPos) - currAgent->GetRadius() && currAgent->GetRadius() < pAgent->GetRadius() - 1)
		{
			//std::cout << "Early Exit\n";
			return true; // new closer agent has been found that can be eaten
		}

	}


	const float agentRadius{ pAgent->GetRadius() }; // doesn't change better to cache instead of constantly calling

	// only need to pursue when it's smaller, check if changed
	if (agentRadius - 1 <= pAgentToPursue->GetRadius())
	{
		//std::cout << "RADIUS CHANGED\n";
		return true;
	}

	const float radius{ 10.0f + agentRadius };

	DEBUGRENDERER2D->DrawCircle(agentPos, radius, Color{ 1.0f, 0.0f, 0.0f }, DEBUGRENDERER2D->NextDepthSlice());
	DEBUGRENDERER2D->DrawCircle(pAgentToPursue->GetPosition(), pAgentToPursue->GetRadius() + 2, Color{ 0.f, 1.f, 0.f }, DEBUGRENDERER2D->NextDepthSlice());


	if (pAgentToPursue->GetPosition().Distance(agentPos) - pAgentToPursue->GetRadius() > radius)
	{
		//std::cout << "STOP PURSUE\n";

		return true;
	}

	return false;


	
}

bool AvoidWall::Evaluate(Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	//std::vector<AgarioAgent*>* pOtherAgents;
	float worldSize;

	if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
		return false;
	//if (pBlackboard->GetData("AgentVec", pOtherAgents) == false || pOtherAgents == nullptr)
	//	return false;
	if (pBlackboard->GetData("WorldSize", worldSize) == false)
		return false;

	const float agentRadius{ pAgent->GetRadius() };
	const float radius{ 10.0f + agentRadius };

	Vector2 agentPos = pAgent->GetPosition();

	DEBUGRENDERER2D->DrawCircle(agentPos, radius, Color{ 1.0f, 0.f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());



	Vector2 wallPosition{pAgent->GetPosition()};
	if (agentPos.x + radius >= worldSize) // radius is hitting the right side
	{
		wallPosition.x = worldSize;
	}
	else if (agentPos.x - radius <= 0.0f) // radius is hitting the left side
	{
		wallPosition.x = 0.0f;
	}

	if (agentPos.y + radius >= worldSize) // radius is hitting the top
	{
		wallPosition.y = worldSize;
	}
	else if (agentPos.y - radius <= 0.0f) // radius is hitting the bottom
	{
		wallPosition.y = 0.0f;
	}

	if (wallPosition.x == 0.f || wallPosition.x == worldSize || wallPosition.y == 0.f || wallPosition.y == worldSize)
	{
		// offset coords
		bool offsetXPos{ wallPosition.y == 0.f || wallPosition.y == worldSize };
		bool offSetYPos{ wallPosition.x == 0.f || wallPosition.x == worldSize };
		Vector2 agentLinearVelocity{ pAgent->GetLinearVelocity() };
		float playerRadius{ pAgent->GetRadius() / 2.f };

		if (offsetXPos)
		{
			if (wallPosition.x == worldSize)
			{
				wallPosition.x -= playerRadius;
			}
			else
			{
				wallPosition.x += playerRadius;
			}
		}
		if (offSetYPos)
		{
			if (wallPosition.y == worldSize)
			{
				wallPosition.y -= playerRadius;
			}
			else
			{
				wallPosition.y += playerRadius;
			}
		}
	}


	if (wallPosition != pAgent->GetPosition()) // we hit a side
	{
		// set data
		pBlackboard->ChangeData("WallEvadePos", wallPosition);
		//std::cout << "WALLPOS FOUND\n";
		return true;
	}

	return false;
}

bool FSMConditions::StopAvoidingWall::Evaluate(Elite::Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	//std::vector<AgarioAgent*>* pOtherAgents;
	float worldSize;

	if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
		return false;
	//if (pBlackboard->GetData("AgentVec", pOtherAgents) == false || pOtherAgents == nullptr)
	//	return false;
	if (pBlackboard->GetData("WorldSize", worldSize) == false)
		return true;

	const float agentRadius{ pAgent->GetRadius() };
	const float radius{ 10.0f + agentRadius };

	Vector2 agentPos = pAgent->GetPosition();

	DEBUGRENDERER2D->DrawCircle(agentPos, radius, Color{ 1.0f, 0.f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());



	Vector2 wallPosition{ pAgent->GetPosition() };
	if (agentPos.x + radius >= worldSize) // radius is hitting the right side
	{
		wallPosition.x = worldSize;
	}
	else if (agentPos.x - radius <= 0.0f) // radius is hitting the left side
	{
		wallPosition.x = 0.0f;
	}

	if (agentPos.y + radius >= worldSize) // radius is hitting the top
	{
		wallPosition.y = worldSize;
	}
	else if (agentPos.y - radius <= 0.0f) // radius is hitting the bottom
	{
		wallPosition.y = 0.0f;
	}

	if (wallPosition.x == 0.f || wallPosition.x == worldSize || wallPosition.y == 0.f || wallPosition.y == worldSize)
	{
		// offset coords
		bool offsetXPos{ wallPosition.y == 0.f || wallPosition.y == worldSize };
		bool offSetYPos{ wallPosition.x == 0.f || wallPosition.x == worldSize };
		Vector2 agentLinearVelocity{ pAgent->GetLinearVelocity() };
		//float playerRadius{ pAgent->GetRadius() /2.f};
		float playerRadius{ pAgent->GetRadius() /3.f};

		if (offsetXPos)
		{
			if (wallPosition.x == worldSize)
			{
				wallPosition.x -= playerRadius;
			}
			else
			{
				wallPosition.x += playerRadius;
			}
		}
		if (offSetYPos)
		{
			if (wallPosition.y == worldSize)
			{
				wallPosition.y -= playerRadius;
			}
			else
			{
				wallPosition.y += playerRadius;
			}
		}
	}


	if (wallPosition != pAgent->GetPosition()) // we hit a side
	{
		// set data
		pBlackboard->ChangeData("WallEvadePos", wallPosition);
		//std::cout << wallPosition << "\n";
		DEBUGRENDERER2D->DrawCircle(wallPosition, 3.f, Color{ 1.0f, 0.f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());
		//std::cout << "STILL WALLPOS FOUND\n";
		return false;
	}

	return true;
}

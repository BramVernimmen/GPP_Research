/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// StatesAndTransitions.h: Implementation of the state/transition classes
/*=============================================================================*/
#ifndef ELITE_APPLICATION_FSM_STATES_TRANSITIONS
#define ELITE_APPLICATION_FSM_STATES_TRANSITIONS

#include "projects/Shared/Agario/AgarioAgent.h"
#include "projects/Shared/Agario/AgarioFood.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "framework/EliteAI/EliteData/EBlackboard.h"

//------------
//---STATES---
//------------
namespace FSMStates
{
	class WanderState : public Elite::FSMState
	{
	public:
		WanderState() : FSMState() {};
		virtual void OnEnter(Elite::Blackboard* pBlackboard) override;
	};

	class SeekFoodState : public Elite::FSMState
	{
	public:
		SeekFoodState() : FSMState() {};
		virtual void OnEnter(Elite::Blackboard* pBlackboard) override;
	};

	class EvadeAgentState : public Elite::FSMState
	{
	public:
		EvadeAgentState() : FSMState() {};
		virtual void OnEnter(Elite::Blackboard* pBlackboard) override;
		virtual void Update(Elite::Blackboard* pBlackboard, float deltaTime) override;
	};
	
	class PursueAgentState : public Elite::FSMState
	{
	public:
		PursueAgentState() : FSMState() {};
		virtual void OnEnter(Elite::Blackboard* pBlackboard) override;
		virtual void Update(Elite::Blackboard* pBlackboard, float deltaTime) override;
	};

	class FleeFromWallState : public Elite::FSMState
	{
	public:
		FleeFromWallState() : FSMState() {};
		virtual void OnEnter(Elite::Blackboard* pBlackboard) override;
		virtual void Update(Elite::Blackboard* pBlackboard, float deltaTime) override;
	};

}


//-----------------
//---TRANSITIONS---
//-----------------

namespace FSMConditions
{
	class FoodNearbyCondition : public Elite::FSMCondition
	{
	public:
		FoodNearbyCondition() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;
	};

	class NearbyFoodIsGone : public Elite::FSMCondition
	{
	public:
		NearbyFoodIsGone() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;
	};



	class EvadeBiggerAgents : public Elite::FSMCondition
	{
	public:
		EvadeBiggerAgents() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;
	};
	class CanStopEvading : public Elite::FSMCondition
	{
	public:
		CanStopEvading() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;
	};



	class PursueSmallerAgents : public Elite::FSMCondition
	{
	public:
		PursueSmallerAgents() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;
	}; 
	class CanStopPursuing : public Elite::FSMCondition
	{
	public:
		CanStopPursuing() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;
	};


	class AvoidWall : public Elite::FSMCondition
	{
	public:
		AvoidWall() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;
	};
	class StopAvoidingWall : public Elite::FSMCondition
	{
	public:
		StopAvoidingWall() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;
	};

}

#endif
#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	// just Seek the average neighborpos...
	m_Target = m_pFlock->GetAverageNeighborPos();
	return Seek::CalculateSteering(deltaT, pAgent);
}

//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	
	int nrOfNeighbors{ m_pFlock->GetNrOfNeighbors() };
	if (nrOfNeighbors > 0)
	{
		for (int i{0}; i < nrOfNeighbors; ++i)
		{
			Elite::Vector2 tempDistance{ pAgent->GetPosition() - m_pFlock->GetNeighbors()[i]->GetPosition()};
			steering.LinearVelocity += tempDistance / (tempDistance.MagnitudeSquared());
		}
	}
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	return steering;
}



//*************************
//VELOCITY MATCH (FLOCKING) (Alignment)
SteeringOutput VelocityMatch::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	// set linearvelocity as the averageneighborvelocity

	SteeringOutput steering = {};

	steering.LinearVelocity = m_pFlock->GetAverageNeighborVelocity();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	return steering;
}

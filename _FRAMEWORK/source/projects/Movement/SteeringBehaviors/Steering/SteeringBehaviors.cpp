//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "../Obstacle.h"
#include "framework\EliteMath\EMatrix2x3.h"

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, {0,1,0});
	}

	return steering;
}

//FLEE
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	//Elite::Vector2 fromTarget = pAgent->GetPosition() - m_Target.Position;
	Elite::Vector2 fromTarget = pAgent->GetPosition() - m_Target.Position;
	SteeringOutput steering = {};

	float distance = fromTarget.Magnitude();

	if (distance > m_FleeRadius)
	{
		steering.IsValid = false;
		return steering;
	}

	steering.LinearVelocity = fromTarget;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 0,1,0 });
	}

	return steering;
}

//ARRIVE
//****
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	//steering.LinearVelocity = pAgent->GetPosition() - m_Target.Position;
	//steering.LinearVelocity.Normalize();
	//steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	//
	//if (pAgent->CanRenderBehavior())
	//{
	//	DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 0,1,0 });
	//}


	const float maxSpeed = 25.f;
	const float arrivalRadius = 1.f;
	const float slowRadius = 15.f;

	Elite::Vector2 toTarget = m_Target.Position - pAgent->GetPosition();
	const float distance = toTarget.Magnitude();
	if (distance < arrivalRadius)
	{
		steering.LinearVelocity ={ 0.f, 0.f };
		//return;
	}

	Elite::Vector2 velocity = toTarget;
	velocity.Normalize();
	if (distance < slowRadius)
	{
		velocity *= maxSpeed * (distance / slowRadius);
	}
	else
	{
		velocity *= maxSpeed;
	}


	steering.LinearVelocity = velocity;

	return steering;
}

//FACE
//****
SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Elite::Vector2 toTarget{ m_Target.Position - pAgent->GetPosition() };
	const float angleBuffer = 0.1f;
	float directionAngle = atan2f(toTarget.y, toTarget.x);
	
	//std::cout << atan2f(toTarget.y, toTarget.x) << "     " << pAgent->GetRotation() << "\n";
	//std::cout << atan2f(toTarget.y, toTarget.x) - pAgent->GetRotation() << "\n";



	if (directionAngle - pAgent->GetRotation() < -static_cast<float>(M_PI))
	{
		directionAngle += 2 * static_cast<float>(M_PI);
	}
	else if (directionAngle - pAgent->GetRotation() > static_cast<float>(M_PI))
	{
		directionAngle -= 2 * static_cast<float>(M_PI);
	}


	if (pAgent->GetRotation() < directionAngle - angleBuffer)
	{
		steering.AngularVelocity = pAgent->GetMaxAngularSpeed();
	}
	else if (pAgent->GetRotation() > directionAngle + angleBuffer)
	{
		steering.AngularVelocity = -pAgent->GetMaxAngularSpeed();
	}
	

	

	return steering;
}

//WANDER
//****
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	float directionAngle = atan2f(pAgent->GetDirection().y, pAgent->GetDirection().x); // in rads
	Elite::Vector2 circlePosition = {};
	circlePosition.x = m_OffsetDistance * cosf(directionAngle) + pAgent->GetPosition().x;
	circlePosition.y = m_OffsetDistance * sinf(directionAngle) + pAgent->GetPosition().y;

	m_WanderAngle += Elite::ToRadians((rand() % static_cast<int>(Elite::ToDegrees(m_MaxAngleChange))) - Elite::ToDegrees(m_MaxAngleChange) / 2.f);

	Elite::Vector2 circleTargetPosition = {};
	circleTargetPosition.x = m_Radius * cosf(m_WanderAngle) + circlePosition.x;
	circleTargetPosition.y = m_Radius * sinf(m_WanderAngle) + circlePosition.y;


	steering.LinearVelocity = circleTargetPosition - pAgent->GetPosition();
	//steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();


	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawCircle(circlePosition, m_Radius, Elite::Color{ 1,0,0 }, 0);
		DEBUGRENDERER2D->DrawCircle(circleTargetPosition, 2, Elite::Color{ 1,0,0 }, 0);
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 0,1,0 });
	}

	return steering;
}

//PURSUIT
//****
SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = (m_Target.Position + m_Target.LinearVelocity) - pAgent->GetPosition(); // initial distance
	float t = steering.LinearVelocity.Magnitude() / (m_Target.LinearVelocity.Magnitude() + pAgent->GetMaxLinearSpeed()); // predict time

	steering.LinearVelocity = (m_Target.Position + m_Target.LinearVelocity * t) - pAgent->GetPosition(); // correct velocity

	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 0,1,0 });
		DEBUGRENDERER2D->DrawCircle((m_Target.Position + m_Target.LinearVelocity), 2.f, Elite::Color{ 1,0,0 }, 0);
	}

	return steering;
}

//EVADE
//****
SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	Elite::Vector2 fromTarget = pAgent->GetPosition() - (m_Target.Position + m_Target.LinearVelocity);
	float distance = fromTarget.Magnitude();

	//std::cout << "Distance: " << distance << " Radius: " << m_EvadeRadius << "\n";

	if (distance > m_EvadeRadius)
	{
		steering.IsValid = false;
		return steering;
	}


	steering.LinearVelocity = fromTarget; // initial distance
	float t = steering.LinearVelocity.Magnitude() / (m_Target.LinearVelocity.Magnitude() + pAgent->GetMaxLinearSpeed()); // predict time

	steering.LinearVelocity = pAgent->GetPosition() - (m_Target.Position + m_Target.LinearVelocity * t); // correct velocity

	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 10.f, { 0,1,0 });
		DEBUGRENDERER2D->DrawCircle((m_Target.Position + m_Target.LinearVelocity), 2.f, Elite::Color{ 1,0,0 }, 0);
		DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), m_EvadeRadius, Elite::Color{ 1.f, 1.f, 0.f }, 0);
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), -fromTarget, distance, Elite::Color{ 0.f, 0.f, 1.f });
	}

	return steering;
}
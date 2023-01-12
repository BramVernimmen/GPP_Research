#include "stdafx.h"
#include "Car.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"


Car::Car(const Elite::Vector2& startPos, const Elite::Vector2& endPos)
	: Car(startPos, endPos, Elite::Color{0.8f, 0.8f, 0.8f})
{
}

Car::Car(const Elite::Vector2& startPos, const Elite::Vector2& endPos, const Elite::Color& color)
	: SteeringAgent(3.0f) // default radius is now 3.0f
{
	m_BodyColor = color;
	SetPosition(startPos);
	SetMass(0.0f);

	m_pSeek = new Seek();


	SetAutoOrient(true); // we always want to look the way we are going

	m_StartPos = startPos;
	m_EndPos = endPos;

	//SetMaxLinearSpeed(25.f);
	//SetMaxLinearSpeed(16.66f);
	SetMaxLinearSpeed(10.0f);
}


Car::~Car()
{
	SAFE_DELETE(m_DecisionMaking);
	SAFE_DELETE(m_pSeek);
}

void Car::Update(float dt)
{
	if (m_DecisionMaking)
		m_DecisionMaking->Update(dt);

	SteeringAgent::Update(dt);
}

void Car::Render(float dt)
{
	SteeringAgent::Render(dt);
}

void Car::SetDecisionMaking(Elite::IDecisionMaking* decisionMakingStructure)
{
	m_DecisionMaking = decisionMakingStructure;
}

void Car::SetToSeek(const Elite::Vector2& seekTarget)
{
	m_pSeek->SetTarget(seekTarget);
	SetSteeringBehavior(m_pSeek);
}

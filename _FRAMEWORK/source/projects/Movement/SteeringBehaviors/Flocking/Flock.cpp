#include "stdafx.h"
#include "Flock.h"

#include "../SteeringAgent.h"
#include "../Steering/SteeringBehaviors.h"
#include "../CombinedSteering/CombinedSteeringBehaviors.h"
#include "../SpacePartitioning/SpacePartitioning.h"

using namespace Elite;

//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/, 
	float worldSize /*= 100.f*/, 
	SteeringAgent* pAgentToEvade /*= nullptr*/, 
	bool trimWorld /*= false*/)

	: m_WorldSize{ worldSize }
	, m_FlockSize{ flockSize }
	, m_TrimWorld { trimWorld }
	, m_pAgentToEvade{pAgentToEvade}
	, m_NeighborhoodRadius{ 5 }
	, m_NrOfNeighbors{0}
{
	m_Agents.resize(m_FlockSize);
	m_OldPos.resize(m_FlockSize);
	m_Neighbors.resize(m_FlockSize); // use m_FlockSize instead of m_FlockSize - 1, if the flocksize is 0 that would give errors

	// TODO: initialize the flock and the memory pool
	
	m_pSeekBehavior = new Seek();
	m_pWanderBehavior = new Wander();
	m_pEvadeBehavior = new Evade();
	m_pEvadeBehavior->SetEvadeRadius(m_NeighborhoodRadius);
	m_pCohesionBehavior = new Cohesion(this);
	m_pSeparationBehavior = new Separation(this);
	m_pVelMatchBehavior = new VelocityMatch(this);
	
	m_pBlendedSteering = new BlendedSteering(
		{
			{ m_pSeekBehavior		, 0.65f },
			{ m_pWanderBehavior		, 0.20f },
			{ m_pCohesionBehavior	, 0.25f },
			{ m_pSeparationBehavior	, 0.75f },
			{ m_pVelMatchBehavior	, 0.60f }
		}

	);

	m_pPrioritySteering = new PrioritySteering({ m_pEvadeBehavior, m_pBlendedSteering });

	m_pCellSpace = new CellSpace(m_WorldSize, m_WorldSize, 25, 25, m_FlockSize);

	for (auto& pAgent: m_Agents)
	{
		pAgent = new SteeringAgent();
		pAgent->SetPosition(randomVector2(0.f,m_WorldSize));
		pAgent->SetSteeringBehavior(m_pPrioritySteering);
		pAgent->SetAutoOrient(true);
		m_pCellSpace->AddAgent(pAgent);
	}
	
	for (int i{ 0 }; i < static_cast<int>(m_Agents.size()); ++i)
	{
		m_OldPos[i] = m_Agents[i]->GetPosition();
	}
	// first agent will have debug
	if (m_Agents.size() > 0) // safety check
	{
		m_Agents[0]->SetRenderBehavior(m_CanDebugRender);

	}


}

Flock::~Flock()
{
	// TODO: clean up any additional data

	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pPrioritySteering);

	for(auto pAgent: m_Agents)
	{
		SAFE_DELETE(pAgent);
	}
	m_Agents.clear();

	SAFE_DELETE(m_pSeekBehavior);
	SAFE_DELETE(m_pCohesionBehavior);
	SAFE_DELETE(m_pSeparationBehavior);
	SAFE_DELETE(m_pVelMatchBehavior);
	SAFE_DELETE(m_pWanderBehavior);
	SAFE_DELETE(m_pEvadeBehavior);

	SAFE_DELETE(m_pCellSpace);
}

void Flock::Update(float deltaT)
{
	// keep the debug render updated
	if (m_Agents.size() > 0) // safety check
	{
		m_Agents[0]->SetRenderBehavior(m_CanDebugRender);
	}

	// TODO: update the flock
	// loop over all the agents
		// register its neighbors	(-> memory pool is filled with neighbors of the currently evaluated agent)
		// update it				(-> the behaviors can use the neighbors stored in the pool, next iteration they will be the next agent's neighbors)
		// trim it to the world
	if (m_UseCellSpace == false)
	{
		for (auto pAgent: m_Agents)
		{
			RegisterNeighbors(pAgent);
			pAgent->Update(deltaT);
			pAgent->TrimToWorld(m_WorldSize, m_TrimWorld);
		}
	}
	else
	{
		for (int i{0}; i < static_cast<int>(m_Agents.size()); ++i)
		{
			SteeringAgent* pAgent{ m_Agents[i] };
			m_pCellSpace->RegisterNeighbors(pAgent, m_NeighborhoodRadius);
			m_Neighbors = m_pCellSpace->GetNeighbors();
			m_NrOfNeighbors = m_pCellSpace->GetNrOfNeighbors();
			pAgent->Update(deltaT);
			pAgent->TrimToWorld(m_WorldSize, m_TrimWorld);
			m_pCellSpace->UpdateAgentCell(pAgent, m_OldPos[i]);
			m_OldPos[i] = pAgent->GetPosition();
		}
	}
	

	// update target for evade behaviour
	TargetData evadeTarget;
	evadeTarget.AngularVelocity = m_pAgentToEvade->GetAngularVelocity();
	evadeTarget.LinearVelocity = m_pAgentToEvade->GetLinearVelocity();
	evadeTarget.Position = m_pAgentToEvade->GetPosition();
	m_pEvadeBehavior->SetTarget(evadeTarget);
}

void Flock::Render(float deltaT)
{
	// TODO: render the flock
	for (auto& pAgent : m_Agents)
	{
		//pAgent->Render(deltaT);
		if (pAgent->CanRenderBehavior())
		{
			//draw debug of the first agent
			DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), m_NeighborhoodRadius, { 0,1,0 }, 0);
			if (m_UseCellSpace)
			{
				m_pCellSpace->RegisterNeighbors(pAgent, m_NeighborhoodRadius);
				m_Neighbors = m_pCellSpace->GetNeighbors();
				m_NrOfNeighbors = m_pCellSpace->GetNrOfNeighbors();
			}
			else
			{
				RegisterNeighbors(pAgent);
			}
			for (int i{0}; i < m_NrOfNeighbors; ++i)
			{
				DEBUGRENDERER2D->DrawCircle(m_Neighbors[i]->GetPosition(), m_Neighbors[i]->GetRadius(), {0,1,0}, 0);
			}
		}
	}

	if (m_CellSpaceDebugRender && m_UseCellSpace)
	{
		m_pCellSpace->RenderCells();
		// draw bounding box, the first agent is always the debug agent...
		if (m_Agents.size() > 0) // since using the first agent, safety check for if m_Agents are empty
		{
			float agentXPos{ m_Agents[0]->GetPosition().x };
			float agentYPos{ m_Agents[0]->GetPosition().y };
			Elite::Polygon boundingBoxPolygon{ {	{ agentXPos - m_NeighborhoodRadius, agentYPos - m_NeighborhoodRadius },
													{ agentXPos - m_NeighborhoodRadius, agentYPos + m_NeighborhoodRadius },
													{ agentXPos + m_NeighborhoodRadius, agentYPos + m_NeighborhoodRadius },
													{ agentXPos + m_NeighborhoodRadius, agentYPos - m_NeighborhoodRadius } } };
			DEBUGRENDERER2D->DrawPolygon(&boundingBoxPolygon, { 0,0,1 });
		}
	
	}
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);

	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Text("LMB: place target");
	ImGui::Text("RMB: move cam.");
	ImGui::Text("Scrollwheel: zoom cam.");
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Flocking");
	ImGui::Spacing();

	// TODO: Implement checkboxes for debug rendering and weight sliders here

	// sliders for weight
	// for value on the slider use std::to_string(m_pBlendedSteering->GetWeightedBehaviorsRef()[0].weight).c_str() , no clue how to get the value shorter
	ImGui::SliderFloat("Seek", &m_pBlendedSteering->GetWeightedBehaviorsRef()[0].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Wander", &m_pBlendedSteering->GetWeightedBehaviorsRef()[1].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Cohesion", &m_pBlendedSteering->GetWeightedBehaviorsRef()[2].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Separation", &m_pBlendedSteering->GetWeightedBehaviorsRef()[3].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("VelocityMatch", &m_pBlendedSteering->GetWeightedBehaviorsRef()[4].weight, 0.f, 1.f, "%.2");

	// checkbox for debug rendering
	ImGui::Checkbox("Debug Rendering", &m_CanDebugRender);

	// checkbox for using partitioning
	ImGui::Checkbox("Using Space Partitioning", &m_UseCellSpace);
	if (m_UseCellSpace)
	{
		// checkbox for debug rendering
		ImGui::Checkbox("Draw Debug For Space Partitioning", &m_CellSpaceDebugRender);
	}


	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
	
}

void Flock::RegisterNeighbors(SteeringAgent* pAgent)
{
	// TODO: Implement
	m_NrOfNeighbors = 0;
	for (auto& pOtherAgent: m_Agents)
	{
		if (pOtherAgent != pAgent)
		{
			Vector2 toTarget{ pAgent->GetPosition() - pOtherAgent->GetPosition() };
			if (toTarget.MagnitudeSquared() <= (m_NeighborhoodRadius * m_NeighborhoodRadius)) // is in circle
			{
				m_Neighbors[m_NrOfNeighbors] = pOtherAgent;
				++m_NrOfNeighbors;
			}

		}
	}
}

Elite::Vector2 Flock::GetAverageNeighborPos() const
{
	// TODO: Implement
	Vector2 avgPos{};
	if (m_NrOfNeighbors > 0)
	{
		for (int i{0}; i < m_NrOfNeighbors; ++i)
		{
			avgPos += m_Neighbors[i]->GetPosition();
		}
		avgPos /= static_cast<float>(m_NrOfNeighbors);
	}
	return avgPos;
}

Elite::Vector2 Flock::GetAverageNeighborVelocity() const
{
	// TODO: Implement
	Vector2 avgVel{};
	if (m_NrOfNeighbors > 0)
	{
		for (int i{ 0 }; i < m_NrOfNeighbors; ++i)
		{
			avgVel += m_Neighbors[i]->GetLinearVelocity();
		}
		avgVel /= static_cast<float>(m_NrOfNeighbors);
	}
	return avgVel;
}

void Flock::SetTarget_Seek(TargetData target)
{
	// TODO: Set target for seek behavior
	m_pSeekBehavior->SetTarget(target);
}


float* Flock::GetWeight(ISteeringBehavior* pBehavior) 
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->GetWeightedBehaviorsRef();
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if(it!= weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}

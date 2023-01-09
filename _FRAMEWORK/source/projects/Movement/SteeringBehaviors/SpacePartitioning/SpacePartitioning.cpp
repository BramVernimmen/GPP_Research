#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\Movement\SteeringBehaviors\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
	boundingBox.bottomLeft = { left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	auto left = boundingBox.bottomLeft.x;
	auto bottom = boundingBox.bottomLeft.y;
	auto width = boundingBox.width;
	auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_Neighbors(maxEntities)
	, m_NrOfNeighbors(0)
{
	m_CellWidth = m_SpaceWidth / m_NrOfCols;
	m_CellHeight = m_SpaceHeight / m_NrOfRows;

	for (float i{0}; i < m_NrOfRows; ++i)
	{
		for (float j{0}; j < m_NrOfCols; ++j)
		{
			Cell newCell{(m_CellWidth * j), (m_CellHeight * i), m_CellWidth, m_CellHeight};
			m_Cells.push_back(newCell);
		}
	}
}

void CellSpace::AddAgent(SteeringAgent* agent)
{
	m_Cells[PositionToIndex(agent->GetPosition())].agents.push_back(agent);
}

void CellSpace::UpdateAgentCell(SteeringAgent* agent, Elite::Vector2 oldPos)
{
	// calculate old index and new index
	Elite::Vector2 oldTest{oldPos};
	Elite::Vector2 newTest{agent->GetPosition()};


	int oldCellIndex{ PositionToIndex(oldPos) };
	int newCellIndex{ PositionToIndex(agent->GetPosition()) };

	// check if the index changed, if so, switch the cell the agent is in
	if (oldCellIndex != newCellIndex)
	{
		m_Cells[oldCellIndex].agents.remove(agent);
		m_Cells[newCellIndex].agents.push_back(agent);
	}


}

void CellSpace::RegisterNeighbors(SteeringAgent* agent, float queryRadius)
{
	// get the rect bounding box
	Elite::Rect boundingBox{ {agent->GetPosition().x - queryRadius, agent->GetPosition().y - queryRadius}, queryRadius * 2.f, queryRadius * 2.f };

	m_NrOfNeighbors = 0;
	for (int i{0}; i < static_cast<int>(m_Cells.size()); ++i)
	{
		// check each cell if overlapping
		if (Elite::IsOverlapping(boundingBox, m_Cells[i].boundingBox))
		{
			// copy all agents from the cell to the vector.
			for (auto pAgent: m_Cells[i].agents)
			{
				if (pAgent != agent) // we don't want to include the agent given to us
				{
					// check if is in radius
					Elite::Vector2 toTarget{ agent->GetPosition() - pAgent->GetPosition() };
					if (toTarget.MagnitudeSquared() <= (queryRadius * queryRadius)) // is in circle
					{
						m_Neighbors[m_NrOfNeighbors] = pAgent;
						++m_NrOfNeighbors;
					}
				}
			}
		}
	}
}



void CellSpace::EmptyCells()
{
	for (Cell& c : m_Cells)
		c.agents.clear();
}

void CellSpace::RenderCells() const
{
	for (int i{0}; i < static_cast<int>(m_Cells.size()); ++i)
	{
		DEBUGRENDERER2D->DrawPolygon(&Elite::Polygon{ m_Cells[i].GetRectPoints() }, Elite::Color{ 1,0,0 });
		// offset the x-pos with 1 for clearer result
		Elite::Vector2 topLeftForNumber{(m_Cells[i].boundingBox.bottomLeft.x + 1.f),(m_Cells[i].boundingBox.bottomLeft.y + m_CellHeight)};
		DEBUGRENDERER2D->DrawString(topLeftForNumber, std::to_string(m_Cells[i].agents.size()).c_str());
	}
}

int CellSpace::PositionToIndex(const Elite::Vector2 pos) const
{
	int rowNumber{ static_cast<int>(pos.y / m_CellHeight) };
	int colNumber{ static_cast<int>(pos.x / m_CellWidth) };

	// fix any out of bounds
	if (pos.x == m_SpaceWidth)
	{
		colNumber = m_NrOfCols - 1;
	}
	if (pos.y == m_SpaceHeight)
	{
		rowNumber = m_NrOfRows - 1;
	}

	return (((rowNumber) * m_NrOfCols) + (colNumber));
}
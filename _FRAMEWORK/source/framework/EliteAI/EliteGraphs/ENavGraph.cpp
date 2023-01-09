#include "stdafx.h"
#include "ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

using namespace Elite;

Elite::NavGraph::NavGraph(const Polygon& contourMesh, float playerRadius = 1.0f) :
	Graph2D(false),
	m_pNavMeshPolygon(nullptr)
{
	//Create the navigation mesh (polygon of navigatable area= Contour - Static Shapes)
	m_pNavMeshPolygon = new Polygon(contourMesh); // Create copy on heap

	//Get all shapes from all static rigidbodies with NavigationCollider flag
	auto vShapes = PHYSICSWORLD->GetAllStaticShapesInWorld(PhysicsFlags::NavigationCollider);

	//Store all children
	for (auto shape : vShapes)
	{
		shape.ExpandShape(playerRadius);
		m_pNavMeshPolygon->AddChild(shape);
	}

	//Triangulate
	m_pNavMeshPolygon->Triangulate();

	//Create the actual graph (nodes & connections) from the navigation mesh
	CreateNavigationGraph();
}

Elite::NavGraph::~NavGraph()
{
	delete m_pNavMeshPolygon; 
	m_pNavMeshPolygon = nullptr;
}

int Elite::NavGraph::GetNodeIdxFromLineIdx(int lineIdx) const
{
	auto nodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(), [lineIdx](const NavGraphNode* n) { return n->GetLineIndex() == lineIdx; });
	if (nodeIt != m_Nodes.end())
	{
		return (*nodeIt)->GetIndex();
	}

	return invalid_node_index;
}

Elite::Polygon* Elite::NavGraph::GetNavMeshPolygon() const
{
	return m_pNavMeshPolygon;
}

void Elite::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigationmesh and create nodes
	for (const auto& currLine : m_pNavMeshPolygon->GetLines())
	{
		if (m_pNavMeshPolygon->GetTrianglesFromLineIndex(currLine->index).size() >= 2)
		{
			int newIndex{ this->GetNrOfNodes() };
			Vector2 newPos{ currLine->p1 + (currLine->p2 - currLine->p1) / 2.0f };
			NavGraphNode * newNode = new NavGraphNode{ newIndex, currLine->index, newPos};
			AddNode(newNode);
		}
	}

	//2. Create connections now that every node is created
	for (const auto& currTriangle: m_pNavMeshPolygon->GetTriangles())
	{
		std::vector<int> validNodeIdx{};
		for (const auto& currIndex : currTriangle->metaData.IndexLines)
		{
			const int currNodeIdx{ GetNodeIdxFromLineIdx(currIndex) };
			if (currNodeIdx != invalid_node_index)
			{
				validNodeIdx.push_back(currNodeIdx);
			}
		}

		if (validNodeIdx.size() == 2)
		{
			GraphConnection2D * newConnection = new GraphConnection2D{ validNodeIdx[0], validNodeIdx[1] };
			AddConnection(newConnection);
		}
		else if (validNodeIdx.size() == 3)
		{
			GraphConnection2D * newConnection1 = new GraphConnection2D{ validNodeIdx[0], validNodeIdx[1] };
			GraphConnection2D * newConnection2 = new GraphConnection2D{ validNodeIdx[1], validNodeIdx[2] };
			GraphConnection2D * newConnection3 = new GraphConnection2D{ validNodeIdx[2], validNodeIdx[0] };
			AddConnection(newConnection1);
			AddConnection(newConnection2);
			AddConnection(newConnection3);
		}



	}
	
	//3. Set the connections cost to the actual distance
	SetConnectionCostsToDistance();
}


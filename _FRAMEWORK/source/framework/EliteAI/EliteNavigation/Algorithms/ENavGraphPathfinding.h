#pragma once
#include <vector>
#include <iostream>
#include "framework/EliteMath/EMath.h"
#include "framework\EliteAI\EliteGraphs\ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

namespace Elite
{
	class NavMeshPathfinding
	{
	public:
		static std::vector<Vector2> FindPath(Vector2 startPos, Vector2 endPos, NavGraph* pNavGraph, std::vector<Vector2>& debugNodePositions, std::vector<Portal>& debugPortals)
		{
			//Create the path to return
			std::vector<Vector2> finalPath{};

			//Get the start and endTriangle
			const Triangle* pStartTriangle{ pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(startPos) };
			if (pStartTriangle == nullptr)
				return finalPath;
			const Triangle* pEndTriangle{ pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(endPos) };
			if (pEndTriangle == nullptr)
				return finalPath;

			if (pStartTriangle == pEndTriangle)
			{
				finalPath.push_back(endPos);
				return finalPath;
			}


			//We have valid start/end triangles and they are not the same
			//=> Start looking for a path
			//Copy the graph
			//NavGraph * pGraphClone{ pNavGraph };
			auto pGraphClone = pNavGraph->Clone();

			//Create extra node for the Start Node (Agent's position
			const int startNodeIdx{ pGraphClone->GetNrOfNodes() };
			pGraphClone->AddNode(new NavGraphNode(startNodeIdx, invalid_node_index, startPos));
			for (const auto& currLine: pStartTriangle->metaData.IndexLines)
			{
				const int nodeLineIdx{ pNavGraph->GetNodeIdxFromLineIdx(currLine) };
				if (nodeLineIdx != invalid_node_index)
				{
					const float distance{ Elite::Distance(startPos, pGraphClone->GetNode(nodeLineIdx)->GetPosition()) };
					pGraphClone->AddConnection(new GraphConnection2D(startNodeIdx, nodeLineIdx, distance));
				}
			}


			//Create extra node for the endNode
			const int endNodeIdx{ pGraphClone->GetNrOfNodes() };
			pGraphClone->AddNode(new NavGraphNode(endNodeIdx, invalid_node_index, endPos));
			for (const auto& currLine : pEndTriangle->metaData.IndexLines)
			{
				const int nodeLineIdx{ pNavGraph->GetNodeIdxFromLineIdx(currLine) };
				if (nodeLineIdx != invalid_node_index)
				{
					const float distance{ Elite::Distance(endPos, pGraphClone->GetNode(nodeLineIdx)->GetPosition()) };
					pGraphClone->AddConnection(new GraphConnection2D(endNodeIdx, nodeLineIdx, distance));
				}
			}
			//Run A star on new graph
			auto pathfinder{ AStar<NavGraphNode, GraphConnection2D>(pGraphClone.get(), HeuristicFunctions::Chebyshev)};
			auto startNode{ pGraphClone->GetNode(startNodeIdx) };
			auto endNode{ pGraphClone->GetNode(endNodeIdx) };
			auto tempPath{ pathfinder.FindPath(startNode, endNode) };

			//OPTIONAL BUT ADVICED: Debug Visualisation
			// AStar path
			/*debugNodePositions.clear();
			for (const auto& currNode: tempPath)
			{
				finalPath.push_back(currNode->GetPosition());
				debugNodePositions.push_back(currNode->GetPosition());
			}*/


			// SSFA path
			//Run optimiser on new graph, MAKE SURE the A star path is working properly before starting this section and uncommenting this!!!
			//m_Portals = SSFA::FindPortals(nodes, m_pNavGraph->GetNavMeshPolygon());
			auto portals = SSFA::FindPortals(tempPath, pNavGraph->GetNavMeshPolygon());
			finalPath = SSFA::OptimizePortals(portals);
			debugNodePositions = finalPath;
			debugPortals = portals;

			return finalPath;
		}
	};
}

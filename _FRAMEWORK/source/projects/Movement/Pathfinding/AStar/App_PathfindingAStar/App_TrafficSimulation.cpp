//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_TrafficSimulation.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAstar.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EBFS.h"

using namespace Elite;

//Destructor
App_TrafficSimulation::~App_TrafficSimulation()
{
	SAFE_DELETE(m_pGridGraph);
	SAFE_DELETE(m_pGraphRenderer);
	SAFE_DELETE(m_pGraphEditor);
}

//Functions
void App_TrafficSimulation::Start()
{
	m_pGraphEditor = new GraphEditor();
	m_pGraphRenderer = new GraphRenderer();
	//Set Camera
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(39.0f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(73.0f, 35.0f));

	//Create Graph
	MakeGridGraph();

	//Setup default start path
	startPathIdx = 44;
	endPathIdx = 88;
	CalculatePath();
}

void App_TrafficSimulation::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	//INPUT
	bool const middleMousePressed = INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft);
	if (middleMousePressed)
	{
		MouseData mouseData = { INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft) };
		Elite::Vector2 mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseData.X, (float)mouseData.Y });

		//Find closest node to click pos
		int closestNode = m_pGridGraph->GetNodeIdxAtWorldPos(mousePos);
		if (m_StartSelected)
		{
			startPathIdx = closestNode;
			CalculatePath();
		}
		else
		{
			endPathIdx = closestNode;
			CalculatePath();
		}
	}
	//IMGUI
	UpdateImGui();

	//UPDATE/CHECK GRID HAS CHANGED
	// this uses the editor, this let's u place nodes, no need to use this in this app, keep commented for now
	/*if (m_pGraphEditor->UpdateGraph(m_pGridGraph))
	{
		CalculatePath();
	}*/
}

void App_TrafficSimulation::Render(float deltaTime) const
{
	UNREFERENCED_PARAMETER(deltaTime);
	//Render grid
	m_pGraphRenderer->RenderGraph(m_pGridGraph, m_DebugSettings.DrawNodes, m_DebugSettings.DrawNodeNumbers, m_DebugSettings.DrawConnections, m_DebugSettings.DrawConnectionCosts);

	//Render start node on top if applicable
	if (startPathIdx != invalid_node_index)
	{
		m_pGraphRenderer->HighlightNodes(m_pGridGraph, { m_pGridGraph->GetNode(startPathIdx) }, START_NODE_COLOR);
	}

	//Render end node on top if applicable
	if (endPathIdx != invalid_node_index)
	{
		m_pGraphRenderer->HighlightNodes(m_pGridGraph, { m_pGridGraph->GetNode(endPathIdx) }, END_NODE_COLOR);
	}

	//render path below if applicable
	if (m_vPath.size() > 0)
	{
		m_pGraphRenderer->HighlightNodes(m_pGridGraph, m_vPath);
	}

}

void App_TrafficSimulation::MakeGridGraph()
{
	m_pGridGraph = new GridGraph<GridTerrainNode, GraphConnection>(COLUMNS, ROWS, m_SizeCell, true, false, 1.f, 1.5f);

	//Setup default terrain
	// building 1
	m_pGridGraph->GetNode(21)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(22)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(23)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(41)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(42)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(43)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(61)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(62)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(63)->SetTerrainType(TerrainType::Building);

	// building 2
	m_pGridGraph->GetNode(121)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(122)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(123)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(141)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(142)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(143)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(161)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(162)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(163)->SetTerrainType(TerrainType::Building);

	// building 3
	m_pGridGraph->GetNode(26)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(27)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(28)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(46)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(47)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(48)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(66)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(67)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(68)->SetTerrainType(TerrainType::Building);

	// building 4
	m_pGridGraph->GetNode(126)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(127)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(128)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(146)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(147)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(148)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(166)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(167)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(168)->SetTerrainType(TerrainType::Building);

	// building 5
	m_pGridGraph->GetNode(31)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(32)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(33)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(51)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(52)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(53)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(71)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(72)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(73)->SetTerrainType(TerrainType::Building);

	// building 6
	m_pGridGraph->GetNode(131)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(132)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(133)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(151)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(152)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(153)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(171)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(172)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(173)->SetTerrainType(TerrainType::Building);

	// building 7
	m_pGridGraph->GetNode(36)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(37)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(38)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(56)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(57)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(58)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(76)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(77)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(78)->SetTerrainType(TerrainType::Building);

	// building 8
	m_pGridGraph->GetNode(136)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(137)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(138)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(156)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(157)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(158)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(176)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(177)->SetTerrainType(TerrainType::Building);
	m_pGridGraph->GetNode(178)->SetTerrainType(TerrainType::Building);


	// clear all standard generated connections -> disabled this
	//m_pGridGraph->RemoveConnections();

	// add our directional connections
	// 
	// 
	// starting with the outside ring
	// vertical connections
	CreateConnectionsVertical(0, 180, 300);
	CreateConnectionsVertical(199, 19, 300);

	// now do horizontal connections
	CreateConnectionsHorizontal(180, 199, 300);
	CreateConnectionsHorizontal(19, 0, 300);


	// start creating connections inside the ring -> will cost more
	CreateConnectionsHorizontal(80, 99, 300);
	CreateConnectionsHorizontal(119, 100, 300);

	CreateConnectionsVertical(184, 4, 500.f);
	CreateConnectionsVertical(5, 185, 500.f);
	CreateConnectionsVertical(189, 9, 500.f);
	CreateConnectionsVertical(10, 190, 500.f);
	CreateConnectionsVertical(194, 14, 500.f);
	CreateConnectionsVertical(15, 195, 500.f);


	// when popping a u-ey isn't allowed, we can change the cost of using an intersection.
	// because of the directional graph, you can complete a route with using only 3 intersection connections.
	// so making it really expensive to use 4 for a u-ey should give a fix.

	float intersectionCost{ 2500.f };
	//// change the connection costs of the 2x2 intersections
	ChangeIntersectionConnectionCosts(104, 84, 85, 105, intersectionCost);
	ChangeIntersectionConnectionCosts(109, 89, 90, 110, intersectionCost);
	ChangeIntersectionConnectionCosts(114, 94, 95, 115, intersectionCost);


	m_pGridGraph->GetNode(104)->SetTerrainType(TerrainType::Intersection);
	m_pGridGraph->GetNode(84)->SetTerrainType(TerrainType::Intersection);
	m_pGridGraph->GetNode(85)->SetTerrainType(TerrainType::Intersection);
	m_pGridGraph->GetNode(105)->SetTerrainType(TerrainType::Intersection);
	m_pGridGraph->GetNode(109)->SetTerrainType(TerrainType::Intersection);
	m_pGridGraph->GetNode(89)->SetTerrainType(TerrainType::Intersection);
	m_pGridGraph->GetNode(90)->SetTerrainType(TerrainType::Intersection);
	m_pGridGraph->GetNode(110)->SetTerrainType(TerrainType::Intersection);
	m_pGridGraph->GetNode(114)->SetTerrainType(TerrainType::Intersection);
	m_pGridGraph->GetNode(94)->SetTerrainType(TerrainType::Intersection);
	m_pGridGraph->GetNode(95)->SetTerrainType(TerrainType::Intersection);
	m_pGridGraph->GetNode(115)->SetTerrainType(TerrainType::Intersection);

}

void App_TrafficSimulation::UpdateImGui()
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		int menuWidth = 200;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
		ImGui::Begin("Traffic Simulation", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::PushAllowKeyboardFocus(false);

		////Elements // old controls, no need to show
		//ImGui::Text("CONTROLS");
		//ImGui::Indent();
		//ImGui::Text("LMB: target");
		//ImGui::Text("RMB: start");
		//ImGui::Unindent();

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::Text("Using A* Pathfinding");
		ImGui::Spacing();

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::Text("Left Mouse Button");
		ImGui::Text("controls");
		std::string buttonText{ "" };
		if (m_StartSelected)
			buttonText += "Start Node";
		else
			buttonText += "End Node";

		if (ImGui::Button(buttonText.c_str()))
		{
			m_StartSelected = !m_StartSelected;
		}

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::Checkbox("Grid", &m_DebugSettings.DrawNodes);
		ImGui::Checkbox("NodeNumbers", &m_DebugSettings.DrawNodeNumbers);
		ImGui::Checkbox("Connections", &m_DebugSettings.DrawConnections);
		ImGui::Checkbox("Connections Costs", &m_DebugSettings.DrawConnectionCosts);
		// next let's us change the heuristic, we just want to use the default, so comment it
		/*if (ImGui::Combo("", &m_SelectedHeuristic, "Manhattan\0Euclidean\0SqrtEuclidean\0Octile\0Chebyshev", 4))
		{
			switch (m_SelectedHeuristic)
			{
			case 0:
				m_pHeuristicFunction = HeuristicFunctions::Manhattan;
				break;
			case 1:
				m_pHeuristicFunction = HeuristicFunctions::Euclidean;
				break;
			case 2:
				m_pHeuristicFunction = HeuristicFunctions::SqrtEuclidean;
				break;
			case 3:
				m_pHeuristicFunction = HeuristicFunctions::Octile;
				break;
			case 4:
				m_pHeuristicFunction = HeuristicFunctions::Chebyshev;
				break;
			default:
				m_pHeuristicFunction = HeuristicFunctions::Chebyshev;
				break;
			}
		}*/
		ImGui::Spacing();

		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif
}

void App_TrafficSimulation::CalculatePath()
{
	//Check if valid start and end node exist
	if (startPathIdx != invalid_node_index
		&& endPathIdx != invalid_node_index
		&& startPathIdx != endPathIdx)
	{
		//BFS Pathfinding
		//auto pathfinder = BFS<GridTerrainNode, GraphConnection>(m_pGridGraph);
		auto pathfinder = AStar<GridTerrainNode, GraphConnection>(m_pGridGraph, m_pHeuristicFunction);
		auto startNode = m_pGridGraph->GetNode(startPathIdx);
		auto endNode = m_pGridGraph->GetNode(endPathIdx);

		m_vPath = pathfinder.FindPath(startNode, endNode);


		std::cout << "New Path Calculated" << std::endl;
	}
	else
	{
		std::cout << "No valid start and end node..." << std::endl;
		m_vPath.clear();
	}
}


void App_TrafficSimulation::CreateConnectionsVertical(int startNode, int endNode, float cost)
{
	if (startNode > endNode) // we go from up to down
	{
		for (int i{ startNode }; i > endNode; i -= 20)
		{
			m_pGridGraph->AddConnection(new GraphConnection2D(i, i - 20, cost));
		}
	}
	else
	{
		for (int i{ startNode }; i < endNode; i += 20)
		{
			m_pGridGraph->AddConnection(new GraphConnection2D(i, i + 20, cost));
		}
	}


}

void App_TrafficSimulation::CreateConnectionsHorizontal(int startNode, int endNode, float cost)
{
	if (startNode > endNode) // we go from up to down
	{
		for (int i{ startNode }; i > endNode; --i)
		{
			m_pGridGraph->AddConnection(new GraphConnection2D(i, i - 1, cost));
		}
	}
	else
	{
		for (int i{ startNode }; i < endNode; ++i)
		{
			m_pGridGraph->AddConnection(new GraphConnection2D(i, i + 1, cost));
		}
	}
}

void App_TrafficSimulation::ChangeIntersectionConnectionCosts(int firstNode, int secondNode, int thirdNode, int fourthNode, float cost)
{
	//m_pGridGraph->AddConnection(new GraphConnection2D(fourthNode, thirdNode, cost));
	//m_pGridGraph->AddConnection(new GraphConnection2D(secondNode, firstNode, cost));


	m_pGridGraph->GetConnection(firstNode, secondNode)->SetCost(cost); // vertical
	m_pGridGraph->GetConnection(secondNode, thirdNode)->SetCost(cost); // horizontal
	m_pGridGraph->GetConnection(thirdNode, fourthNode)->SetCost(cost); // vertical
	m_pGridGraph->GetConnection(fourthNode, firstNode)->SetCost(cost); // horizontal
}

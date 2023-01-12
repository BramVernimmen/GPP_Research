#ifndef ASTAR_APPLICATION_H
#define ASTAR_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "framework\EliteAI\EliteGraphs\EGridGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphUtilities\EGraphEditor.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphUtilities\EGraphRenderer.h"
#include "framework\EliteAI\EliteNavigation\ENavigation.h"


//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class Car;
class NavigationColliderElement;

class App_TrafficSimulation final : public IApp
{
public:

	struct TrafficLight final 
	{
		int fromNodeIdx;
		int toNodeIdx;
		float currTime;
		bool isRed;
		const float maxTime{ 8.f };
	};

	//Constructor & Destructor
	App_TrafficSimulation() = default;
	virtual ~App_TrafficSimulation();

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:

	const int m_AmountOfCars{ 20 };
	std::vector<Car*> m_pCarsVec{};
	std::vector<int> m_SpawnIndexes{};

	std::vector<NavigationColliderElement*> m_vNavigationColliders = {};

	std::vector<TrafficLight> m_TrafficLights{};

	struct DebugSettings
	{
		bool DrawNodes{ true };
		bool DrawNodeNumbers{ false };
		bool DrawConnections{ false };
		bool DrawConnectionCosts{ false };
	};

	bool m_DrawFirstPath{ false };
	bool m_DrawDebugRadius{ false };


	//Datamembers
	const bool ALLOW_DIAGONAL_MOVEMENT = true;
	Elite::Vector2 m_StartPosition = Elite::ZeroVector2;
	Elite::Vector2 m_TargetPosition = Elite::ZeroVector2;

	//Grid datamembers
	static const int COLUMNS = 20;
	static const int ROWS = 10;
	int m_AmountOfNodes{};
	unsigned int m_SizeCell = 15;
	Elite::GridGraph<Elite::GridTerrainNode, Elite::GraphConnection>* m_pGridGraph = nullptr;

	//Pathfinding datamembers
	int startPathIdx = invalid_node_index;
	int endPathIdx = invalid_node_index;
	std::vector<Elite::GridTerrainNode*> m_vPath;

	//Editor and Visualisation
	Elite::GraphEditor* m_pGraphEditor{ nullptr};
	Elite::GraphRenderer* m_pGraphRenderer{ nullptr };

	//Debug rendering information
	DebugSettings m_DebugSettings{};
	
	bool m_StartSelected = true;
	int m_SelectedHeuristic = 4;
	Elite::Heuristic m_pHeuristicFunction = Elite::HeuristicFunctions::Chebyshev;

	//Functions
	void MakeGridGraph();
	void UpdateImGui();
	void CalculatePath();

	void CreateConnectionsVertical(int startNode, int endNode, float cost = 30.f);
	void CreateConnectionsHorizontal(int startNode, int endNode, float cost = 30.f);

	void ChangeIntersectionConnectionCosts(int firstNode, int secondNode, int thirdNode, int fourthNode, float cost = 50.f);

	Elite::Blackboard* CreateBlackboard(Car* currCar);

	//C++ make the class non-copyable
	App_TrafficSimulation(const App_TrafficSimulation&) = delete;
	App_TrafficSimulation& operator=(const App_TrafficSimulation&) = delete;
};
#endif
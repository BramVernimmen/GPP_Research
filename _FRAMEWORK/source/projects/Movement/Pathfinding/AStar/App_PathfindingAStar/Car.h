#pragma once
#include "projects/Movement/SteeringBehaviors/SteeringAgent.h"
class Car final : public SteeringAgent
{
public:
	Car(const Elite::Vector2& startPos, const Elite::Vector2& endPos);
	Car(const Elite::Vector2& startPos, const Elite::Vector2& endPos, const Elite::Color& color);
	~Car();	

	// override agent functions
	virtual void Update(float dt) override;
	virtual void Render(float dt) override;

	// personal functions
	void SetDecisionMaking(Elite::IDecisionMaking* decisionMakingStructure);

	const Elite::Vector2& GetStartPos() const { return m_StartPos; }
	const Elite::Vector2& GetEndPos() const { return m_EndPos; }
	void SetEndPos(const Elite::Vector2& newEndPos) { m_EndPos = newEndPos; }
	void SetPath(const std::list<Elite::Vector2> newPath) { m_Path = newPath; }
	std::list<Elite::Vector2>& GetPath() { return m_Path; }

	void SetToSeek(const Elite::Vector2& seekTarget);

private:
	Elite::IDecisionMaking* m_DecisionMaking = nullptr;
	ISteeringBehavior* m_pSeek = nullptr;
	Elite::Vector2 m_StartPos{};
	Elite::Vector2 m_EndPos{};

	std::list<Elite::Vector2> m_Path{}; // we keep a list, this way we can pop front if we arrived there.

	//C++ make the class non-copyable
	Car(const Car&) {};
	Car& operator=(const Car&) {};
};


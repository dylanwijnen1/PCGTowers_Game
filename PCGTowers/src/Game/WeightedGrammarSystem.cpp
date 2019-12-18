#include "WeightedGrammarSystem.h"

#include <cctype>

WeightedGrammarSystem::~WeightedGrammarSystem()
{
	delete m_pRoot;
}

WeightedGrammarSystem::RuleNode* WeightedGrammarSystem::RunGrammar(char axiom)
{
	if (m_pRoot)
		delete m_pRoot;

	m_pRoot = new RuleNode();

	ProcessNode(m_pRoot, axiom);
	return m_pRoot;
}

void WeightedGrammarSystem::ProcessNode(RuleNode* pNode, char symbol)
{
	eastl::string sentence = RunRule(symbol);
	for (size_t i = 0; i < sentence.size(); ++i)
	{
		RuleNode* pNewNode = new RuleNode(pNode, sentence[i]);
		pNode->AddChild(pNewNode);

		if (!IsTerminating(sentence[i]))
			ProcessNode(pNewNode, sentence[i]);
	}
}

eastl::string WeightedGrammarSystem::RunRule(char symbol)
{
	if (IsTerminating(symbol) || m_rules.find(symbol) == m_rules.end())
		return "";

	float sum = 0.0f;

	// Iterate over the duplicated in the map.
	auto result = m_rules.equal_range_small(symbol);
	for (auto it = result.first; it != result.second; ++it)
	{
		sum += it->second.m_weight;
	}

	// Generate a number based on weight.
	float running_distance = m_random.RandomUniform() * sum;

	// Actually find the "picked" random from weight.
	for (auto it = result.first; it != result.second; ++it)
	{
		running_distance -= it->second.m_weight;
		if (running_distance < 0.f)
			return it->second.m_successor;
	}

	// No successor found.
	return "";
}

bool WeightedGrammarSystem::IsTerminating(char symbol) const
{
	if (std::islower(symbol))
		return true;

	if (auto result = m_terminationSymbols.find(symbol); result != m_terminationSymbols.end())
	{
		return true;
	}

	return false;
}

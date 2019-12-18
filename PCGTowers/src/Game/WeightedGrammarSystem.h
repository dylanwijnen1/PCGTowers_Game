#pragma once

#include <Dragon/Generic/Random.h>

#include <EASTL/map.h>
#include <EASTL/unordered_set.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>

class WeightedGrammarSystem
{
public:
	/// <summary>
	/// Rule Tree Path
	/// </summary>
	struct RuleNode 
	{
		RuleNode* m_pParent;
		char m_symbol;

		eastl::vector<RuleNode*> m_children;

		RuleNode() : RuleNode(nullptr, 0) {}

		RuleNode(RuleNode* pParent, char symbol)
			: m_pParent(pParent)
			, m_symbol(symbol)
		{}

		~RuleNode()
		{
			for (auto pChild : m_children)
				delete pChild;
		}

		void AddChild(RuleNode* pNode) { m_children.emplace_back(pNode); }
	};

	struct Rule
	{
		eastl::string m_successor;
		float m_weight;
	};

private:

	RuleNode* m_pRoot;

	using RuleMap = eastl::multimap<char, Rule>;
	RuleMap m_rules;

	using TerminationSymbols = eastl::unordered_set<char>;
	TerminationSymbols m_terminationSymbols;

	dragon::Random m_random;

public:

	WeightedGrammarSystem() : WeightedGrammarSystem({}, TerminationSymbols{})
	{}

	WeightedGrammarSystem(RuleMap&& rules, TerminationSymbols&& terminators)
		: m_rules(eastl::move(rules))
		, m_terminationSymbols(eastl::move(terminators))
		, m_random((unsigned int)time(nullptr))
		, m_pRoot(nullptr)
	{}

	~WeightedGrammarSystem();

	void SetSeed(unsigned int seed) { m_random.Seed(seed); }

	void AddRule(char symbol, const Rule& rule) { m_rules.emplace(symbol, rule); }

	// Returns the top node of the processed nodes.
	RuleNode* RunGrammar(char axiom);
	RuleNode* RunGrammar(char axiom, size_t iterations);
	bool IsTerminating(char symbol) const;

private:

	void ProcessNode(RuleNode* pNode, char symbol);

	eastl::string RunRule(char symbol);

};

namespace eastl
{
	template<>
	struct less<WeightedGrammarSystem::Rule>
	{
		bool operator()(const WeightedGrammarSystem::Rule& left, const WeightedGrammarSystem::Rule& right) const
		{
			return left.m_weight < right.m_weight;
		}
	};
}
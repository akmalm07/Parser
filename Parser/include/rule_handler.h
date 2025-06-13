#pragma once

#include "include/config.h"
#include "include/rules.h"
#include "include/rule_operations.h"


namespace parser 
{

	class RuleHandler
	{
	public:
		RuleHandler() = default; 

		RuleHandler(std::vector<std::unique_ptr<Rule>>& rules) : _rules(std::move(rules)) {}

		std::vector<std::unique_ptr<Rule>> const& get_rules() const;

		bool check_rules(TokenizedSections files);
		
		virtual ~RuleHandler() = default;
	protected:

		std::vector<std::unique_ptr<Rule>> _rules;
	};

}
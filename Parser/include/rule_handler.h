#pragma once

#include "include/config.h"
#include "include/rules.h"
#include "include/rule_operations.h"


namespace parser 
{




	class RuleHandler //FIX THIS
	{
	public:
		RuleHandler() = default; 

		RuleHandler(std::vector<std::unique_ptr<Rule>>& rules);

		std::vector<std::unique_ptr<Rule>> const& get_rules() const;


		bool check_rules(TokenizedSectionizedCompact const& sections);
		
		virtual ~RuleHandler() = default;
	protected:

		std::vector<std::unique_ptr<GlobalRule>> _rules;
	};

}
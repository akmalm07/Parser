#pragma once

#include "include/config.h"
#include "include/rules.h"
#include "include/rule_operations.h"


namespace parser 
{




	class RuleHandler 
		// This is a class that handles rules for sections in a file.
	{
	public:
		RuleHandler() = default; 

		RuleHandler(std::vector<std::unique_ptr<Rule>>& rules);

		std::vector<std::unique_ptr<Rule>> const& get_rules() const;

		std::vector<view_ptr_non_const<Rule>> find_rules_with_identifiers(size_t id) const;

		bool check_rules(TokenizedSectionizedCompact const& sections);
		
		virtual ~RuleHandler() = default;
	protected:

		std::vector<std::unique_ptr<Rule>> _rules;
	};

}
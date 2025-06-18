#include "headers.h"
#include "include/rule_handler.h"



namespace parser
{

	RuleHandler::RuleHandler(std::vector<std::unique_ptr<Rule>>& rules)
		: _rules(std::move(rules))

	{
	}

	std::vector<std::unique_ptr<Rule>> const& RuleHandler::get_rules() const
	{
		return _rules;
	}

	std::vector<view_ptr_non_const<Rule>> RuleHandler::find_rules_with_identifiers(size_t id) const
	{
		std::vector<view_ptr_non_const<Rule>> foundRules;
		for (auto const& rule : _rules)
		{
			if (rule->get_identifier() & id)
			{
				foundRules.push_back(rule.get());
			}
		}

		return foundRules;
	}

	bool RuleHandler::check_rules(TokenizedSectionizedCompact const& sections)
	{
		for (const auto& rule : _rules)
		{
			if (!rule->check_rule(sections))
			{
				std::cerr << PARSER_LOG_ERR << "Global rule check failed." << std::endl;
				return false;
			}
		}

	}

}

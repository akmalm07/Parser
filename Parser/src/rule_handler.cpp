#include "headers.h"
#include "include/rule_handler.h"



namespace parser
{

	std::vector<std::unique_ptr<Rule>> const& RuleHandler::get_rules() const
	{
		return _rules;
	}

	bool RuleHandler::check_rules(TokenizedSections files)
	{
		return false;
	}

}

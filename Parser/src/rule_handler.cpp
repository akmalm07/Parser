#include "headers.h"
#include "include/rule_handler.h"



namespace parser
{

	std::vector<std::unique_ptr<Rule>> RuleHandler::get_rules() const
	{
		return _rules;
	}

}

#include "headers.h"
#include "include/rule_operations.h"


namespace parser
{

	bool GlobalRuleHandler::check_rule(RuleInputBase const& data)
	{
		if (TokenizedSections const* sections = data.get_sections())
		{
			return check_global_rule(*sections);
		}
		else
		{
			std::cerr << PARSER_LOG_ERR << "Invalid data type for GlobalRuleHandler." << std::endl;
			return false;
		}
	}

	bool LocalRuleHandler::check_rule(RuleInputBase const& data)
	{
		if (TokenizedSection const* section = data.get_section())
		{
			return check_local_rule(*section);
		}
		else
		{
			std::cerr << PARSER_LOG_ERR << "Invalid data type for LocalRuleHandler." << std::endl;
			return false;
		}
	}




}
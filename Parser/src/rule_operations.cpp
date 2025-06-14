#include "headers.h"
#include "include/rule_operations.h"


namespace parser
{
	GlobalRule::GlobalRule()
		: Rule(RuleType::Global)
	{
	}

	GlobalRule::GlobalRule(std::unique_ptr<BaseRuling>&& rule)
		: Rule(std::move(rule), RuleType::Global)
	{
	}


	GlobalRule::GlobalRule(std::unique_ptr<BaseRuling>&& rule, size_t identifier)
		: Rule(std::move(rule), identifier, RuleType::Global)
	{
	}

	LocalRule::LocalRule()
		: Rule(RuleType::Local)
	{
	}

	LocalRule::LocalRule(std::unique_ptr<BaseRuling>&& rule)
		: Rule(std::move(rule), RuleType::Local)
	{
	}

	LocalRule::LocalRule(std::unique_ptr<BaseRuling>&& rule, size_t identifier)
		: Rule(std::move(rule), identifier, RuleType::Local)
	{
	}


	bool GlobalRule::check_rule(RuleInputBase const& data)
	{
		if (TokenizedSections const* sections = data.get_sections())
		{
			return check_global_rule(*sections);
		}
		else
		{
			std::cerr << PARSER_LOG_ERR << "Invalid data type for GlobalRule." << std::endl;
			return false;
		}
	}

	bool LocalRule::check_rule(RuleInputBase const& data)
	{
		if (TokenizedSection const* section = data.get_section())
		{
			return check_local_rule(*section);
		}
		else
		{
			std::cerr << PARSER_LOG_ERR << "Invalid data type for LocalRule." << std::endl;
			return false;
		}
	}




	RuleType Rule::get_type() const
	{
		return _type;
	}

	ParserRule Rule::get_rule_type() const
	{
		return _rule->type;
	}


	view_ptr<BaseRuling> Rule::get_rule()
	{
		return _rule.get();
	}

	size_t Rule::get_identifier() const
	{
		return _identifier;
	}


}
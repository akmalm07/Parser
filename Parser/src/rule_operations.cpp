#include "headers.h"
#include "include/rule_operations.h"


namespace parser
{
	GlobalRule::GlobalRule()
		: Rule(RuleType::Global)
	{
	}

	GlobalRule::GlobalRule(size_t identifier)
	: Rule(RuleType::Global, identifier)
	{}



	LocalRule::LocalRule()
		: Rule(RuleType::Local)
	{
	}

	LocalRule::LocalRule(size_t identifier)
		: Rule(RuleType::Local, identifier)
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


	size_t Rule::get_identifier() const
	{
		return _identifier;
	}

	bool Rule::check_rule(TokenizedSectionizedCompact const& data)
	{
		if (data.tokens.empty())
		{
			std::cerr << PARSER_LOG_ERR << "No tokens to check." << std::endl;
			return false;
		}

		return check_rule_impl(data);
	}




	ConcreteRule<ParserRule::MustIncludeAfter> new_rule_must_include_after(std::string const& t1, std::string const& t2, size_t id)
	{
		return ConcreteRule<ParserRule::MustIncludeAfter>(RulingTwoTarget{ ParserRule::MustIncludeAfter, {t1, t2}, "Item: " + t1 + " must be included after item: " + t2 }, id);
	}


	ConcreteRule<ParserRule::MustIncludeBefore> new_rule_must_include_before(std::string const& t1, std::string const& t2, size_t id)
	{
		return ConcreteRule<ParserRule::MustIncludeBefore>(RulingTwoTarget{ ParserRule::MustIncludeBefore, {t1, t2}, "Item: " + t1 + " must be included before item: " + t2 }, id);
	}


	ConcreteRule<ParserRule::CannotInlcude> new_rule_cannot_include(std::string_view t, size_t id)
	{
		return ConcreteRule<ParserRule::CannotInlcude>(RulingOneTarget{ ParserRule::CannotInlcude, std::string(t), "Item: " + std::string(t) + " cannot be included in this section." }, id);
	}


	ConcreteRule<ParserRule::MustInclude> new_rule_must_include(std::string_view t, size_t id)
	{
		return ConcreteRule<ParserRule::MustInclude>(RulingOneTarget{ ParserRule::MustInclude, std::string(t), "Item: " + std::string(t) + " must be included in this section." }, id);
	}

	ConcreteRule<ParserRule::CannotIncludeInFile> new_rule_cannot_include_in_file(std::string_view t)
	{
		return ConcreteRule<ParserRule::CannotIncludeInFile>(RulingOneTarget{ ParserRule::CannotIncludeInFile, std::string(t), "Item: " + std::string(t) + " cannot be included in the file." });
	}

	ConcreteRule<ParserRule::MustIncludeInFile> new_rule_must_include_in_file(std::string_view t)
	{
		return ConcreteRule<ParserRule::MustIncludeInFile>(RulingOneTarget{ ParserRule::MustIncludeInFile, std::string(t), "Item: " + std::string(t) + " must be included in the file." });
	}



	std::unique_ptr<ConcreteRule<ParserRule::MustIncludeAfter>> new_rule_must_include_after_unique(std::string_view t1, std::string_view t2, size_t id)
	{
		return std::make_unique<ConcreteRule<ParserRule::MustIncludeAfter>>(RulingTwoTarget{ ParserRule::MustIncludeAfter, {std::string(t1), std::string(t2)}, "Item: " + std::string(t1) + " must be included after item: " + std::string(t2) }, id);
	}

	std::unique_ptr<ConcreteRule<ParserRule::MustIncludeBefore>> new_rule_must_include_before_unique(std::string_view t1, std::string_view t2, size_t id)
	{
		return std::make_unique<ConcreteRule<ParserRule::MustIncludeBefore>>(RulingTwoTarget{ ParserRule::MustIncludeBefore, {std::string(t1), std::string(t2)}, "Item: " + std::string(t1) + " must be included before item: " + std::string(t2) }, id);
	}


	std::unique_ptr<ConcreteRule<ParserRule::CannotInlcude>> new_rule_cannot_include_unique(std::string_view t, size_t id)
	{
		return std::make_unique<ConcreteRule<ParserRule::CannotInlcude>>(RulingOneTarget{ ParserRule::CannotInlcude, std::string(t), "Item: " + std::string(t) + " cannot be included in this section." }, id);
	}

	std::unique_ptr<ConcreteRule<ParserRule::MustInclude>> new_rule_must_include_unique(std::string_view t, size_t id)
	{
		return std::make_unique<ConcreteRule<ParserRule::MustInclude>>(RulingOneTarget{ ParserRule::MustInclude, std::string(t), "Item: " + std::string(t) + " must be included in this section." }, id);
	}


	std::unique_ptr<ConcreteRule<ParserRule::CannotIncludeInFile>> new_rule_cannot_include_in_file_unique(std::string_view t)
	{
		return std::make_unique<ConcreteRule<ParserRule::CannotIncludeInFile>>(RulingOneTarget{ ParserRule::CannotIncludeInFile, std::string(t), "Item: " + std::string(t) + " cannot be included in the file." });
	}


	std::unique_ptr<ConcreteRule<ParserRule::MustIncludeInFile>> new_rule_must_include_in_file_unique(std::string_view t)
	{
		return std::make_unique<ConcreteRule<ParserRule::MustIncludeInFile>>(RulingOneTarget{ ParserRule::MustIncludeInFile, std::string(t), "Item: " + std::string(t) + " must be included in the file." });
	}



	std::shared_ptr<ConcreteRule<ParserRule::MustIncludeAfter>> new_rule_must_include_after_shared(std::string_view t1, std::string_view t2, size_t id)
	{
		return std::make_shared<ConcreteRule<ParserRule::MustIncludeAfter>>(RulingTwoTarget{ ParserRule::MustIncludeAfter, {std::string(t1), std::string(t2)}, "Item: " + std::string(t1) + " must be included after item: " + std::string(t2) }, id);
	}

	std::shared_ptr<ConcreteRule<ParserRule::MustIncludeBefore>> new_rule_must_include_before_shared(std::string_view t1, std::string_view t2, size_t id)
	{
		return std::make_shared<ConcreteRule<ParserRule::MustIncludeBefore>>(RulingTwoTarget{ ParserRule::MustIncludeBefore, {std::string(t1), std::string(t2)}, "Item: " + std::string(t1) + " must be included before item: " + std::string(t2) }, id);
	}

	std::shared_ptr<ConcreteRule<ParserRule::CannotInlcude>> new_rule_cannot_include_shared(std::string_view t, size_t id)
	{
		return std::make_shared<ConcreteRule<ParserRule::CannotInlcude>>(RulingOneTarget{ ParserRule::CannotInlcude, std::string(t), "Item: " + std::string(t) + " cannot be included in this section." }, id);
	}

	std::shared_ptr<ConcreteRule<ParserRule::MustInclude>> new_rule_must_include_shared(std::string_view t, size_t id)
	{
		return std::make_shared<ConcreteRule<ParserRule::MustInclude>>(RulingOneTarget{ ParserRule::MustInclude, std::string(t), "Item: " + std::string(t) + " must be included in this section." }, id);
	}

	std::shared_ptr<ConcreteRule<ParserRule::CannotIncludeInFile>> new_rule_cannot_include_in_file_shared(std::string_view t)
	{
		return std::make_shared<ConcreteRule<ParserRule::CannotIncludeInFile>>(RulingOneTarget{ ParserRule::CannotIncludeInFile, std::string(t), "Item: " + std::string(t) + " cannot be included in the file." });
	}

	std::shared_ptr<ConcreteRule<ParserRule::MustIncludeInFile>> new_rule_must_include_in_file_shared(std::string_view t)
	{
		return std::make_shared<ConcreteRule<ParserRule::MustIncludeInFile>>(RulingOneTarget{ ParserRule::MustIncludeInFile, std::string(t), "Item: " + std::string(t) + " must be included in the file." });
	}





	


}
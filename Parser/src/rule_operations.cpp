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

	bool Rule::is_regex(std::string_view str)
	{
		try
		{
			std::regex reg(std::string(pattern));
			return true;
		}
		catch (const std::regex_error&)
		{
			return false;
		}
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


		bool is_regex(std::string const& str)
	{
		try 
		{
			std::regex re(str);
			return true;
		}
		catch (const std::regex_error&) 
		{
			return false;
		}
	}

		bool is_regex(std::string_view str)
		{
			try
			{
				std::regex re(std::string(str));
				return true;
			}
			catch (const std::regex_error&)
			{
				return false;
			}
		}

	ConcreteRule<ParserRule::MustIncludeAfter> new_rule_must_include_after(std::string const& t1, std::string const& t2, size_t id)
	{
		if (is_regex(t1) && is_regex(t2))
		{
			return ConcreteRule<ParserRule::MustIncludeAfter>(Ruling<2, HasRegex::Yes>{ ParserRule::MustIncludeAfter, {t1, t2}, "Item: " + t1 + " must be included after item: " + t2 }, id);
		}
		else
		{
			return ConcreteRule<ParserRule::MustIncludeAfter>(Ruling<2, HasRegex::No>{ ParserRule::MustIncludeAfter, { t1, t2 }, "Item: " + t1 + " must be included after item: " + t2 }, id);
		}
	}


	ConcreteRule<ParserRule::MustIncludeBefore> new_rule_must_include_before(std::string const& t1, std::string const& t2, size_t id)
	{
		if (is_regex(t1) && is_regex(t2))
		{
			return ConcreteRule<ParserRule::MustIncludeBefore>(Ruling<2, HasRegex::Yes>{ ParserRule::MustIncludeBefore, { t1, t2 }, "Item: " + t1 + " must be included before item: " + t2 }, id);
		}
		else
		{
			return ConcreteRule<ParserRule::MustIncludeBefore>(Ruling<2, HasRegex::No>{ ParserRule::MustIncludeBefore, { t1, t2 }, "Item: " + t1 + " must be included before item: " + t2 }, id);
		}
	}




	ConcreteRule<ParserRule::CannotInlcude> new_rule_cannot_include(std::string_view t, size_t id)
	{
		if (is_regex(t))
		{
			return ConcreteRule<ParserRule::CannotInlcude>(Ruling<1, HasRegex::Yes>{ ParserRule::CannotInlcude, std::string(t), "Item: " + std::string(t) + " cannot be included in this section." }, id);
		}
		return ConcreteRule<ParserRule::CannotInlcude>(Ruling<1, HasRegex::No>{ ParserRule::CannotInlcude, std::string(t), "Item: " + std::string(t) + " cannot be included in this section." }, id);
	}


	ConcreteRule<ParserRule::MustInclude> new_rule_must_include(std::string_view t, size_t id)
	{
		if (is_regex(t))
		{
			return ConcreteRule<ParserRule::MustInclude>(Ruling<1, HasRegex::Yes>{ ParserRule::MustInclude, std::string(t), "Item: " + std::string(t) + " must be included in this section." }, id);
		}
		return ConcreteRule<ParserRule::MustInclude>(Ruling<1, HasRegex::No>{ ParserRule::MustInclude, std::string(t), "Item: " + std::string(t) + " must be included in this section." }, id);
	}

	ConcreteRule<ParserRule::CannotIncludeInFile> new_rule_cannot_include_in_file(std::string_view t)
	{
		if (is_regex(t))
		{
			return ConcreteRule<ParserRule::CannotIncludeInFile>(Ruling<1, HasRegex::Yes>{ ParserRule::CannotIncludeInFile, std::string(t), "Item: " + std::string(t) + " cannot be included in the file." });
		}
		return ConcreteRule<ParserRule::CannotIncludeInFile>(Ruling<1, HasRegex::No>{ ParserRule::CannotIncludeInFile, std::string(t), "Item: " + std::string(t) + " cannot be included in the file." });
	}

	ConcreteRule<ParserRule::MustIncludeInFile> new_rule_must_include_in_file(std::string_view t)
	{
		if (is_regex(t))
		{
			return ConcreteRule<ParserRule::MustIncludeInFile>(Ruling<1, HasRegex::Yes>{ ParserRule::MustIncludeInFile, std::string(t), "Item: " + std::string(t) + " must be included in the file." });
		}
		return ConcreteRule<ParserRule::MustIncludeInFile>(Ruling<1, HasRegex::No>{ ParserRule::MustIncludeInFile, std::string(t), "Item: " + std::string(t) + " must be included in the file." });
	}



	std::unique_ptr<ConcreteRule<ParserRule::MustIncludeAfter>> new_rule_must_include_after_unique(std::string_view t1, std::string_view t2, size_t id)
	{
		if (is_regex(t1) && is_regex(t2))
		{
			return std::make_unique<ConcreteRule<ParserRule::MustIncludeAfter>>(Ruling<2, HasRegex::Yes>{ ParserRule::MustIncludeAfter, { std::string(t1), std::string(t2) }, "Item: " + std::string(t1) + " must be included after item: " + std::string(t2) }, id);
		}
		return std::make_unique<ConcreteRule<ParserRule::MustIncludeAfter>>(Ruling<2, HasRegex::No>{ ParserRule::MustIncludeAfter, {std::string(t1), std::string(t2)}, "Item: " + std::string(t1) + " must be included after item: " + std::string(t2) }, id);
	}

	std::unique_ptr<ConcreteRule<ParserRule::MustIncludeBefore>> new_rule_must_include_before_unique(std::string_view t1, std::string_view t2, size_t id)
	{
		if (is_regex(t1) && is_regex(t2))
		{
			return std::make_unique<ConcreteRule<ParserRule::MustIncludeBefore>>(Ruling<2, HasRegex::Yes>{ ParserRule::MustIncludeBefore, { std::string(t1), std::string(t2) }, "Item: " + std::string(t1) + " must be included before item: " + std::string(t2) }, id);
		}
		return std::make_unique<ConcreteRule<ParserRule::MustIncludeBefore>>(Ruling<2, HasRegex::No>{ ParserRule::MustIncludeBefore, { std::string(t1), std::string(t2) }, "Item: " + std::string(t1) + " must be included before item: " + std::string(t2) }, id);
	}


	std::unique_ptr<ConcreteRule<ParserRule::CannotInlcude>> new_rule_cannot_include_unique(std::string_view t, size_t id)
	{
		if (is_regex(t))
		{
			return std::make_unique<ConcreteRule<ParserRule::CannotInlcude>>(Ruling<1, HasRegex::Yes>{ ParserRule::CannotInlcude, std::string(t), "Item: " + std::string(t) + " cannot be included in this section." }, id);
		}
		return std::make_unique<ConcreteRule<ParserRule::CannotInlcude>>(Ruling<1, HasRegex::No>{ ParserRule::CannotInlcude, std::string(t), "Item: " + std::string(t) + " cannot be included in this section." }, id);

	}

	std::unique_ptr<ConcreteRule<ParserRule::MustInclude>> new_rule_must_include_unique(std::string_view t, size_t id)
	{
		if (is_regex(t))
		{
			return std::make_unique<ConcreteRule<ParserRule::MustInclude>>(Ruling<1, HasRegex::Yes>{ ParserRule::MustInclude, std::string(t), "Regex Item: " + std::string(t) + " must be included in this section." }, id);
		}
		return std::make_unique<ConcreteRule<ParserRule::MustInclude>>(Ruling<1, HasRegex::No>{ ParserRule::MustInclude, std::string(t), "Item: " + std::string(t) + " must be included in this section." }, id);
	}


	std::unique_ptr<ConcreteRule<ParserRule::CannotIncludeInFile>> new_rule_cannot_include_in_file_unique(std::string_view t)
	{
		if (is_regex(t))
		{
			return std::make_unique<ConcreteRule<ParserRule::CannotIncludeInFile>>(Ruling<1, HasRegex::Yes>{ ParserRule::CannotIncludeInFile, std::string(t), "Item: " + std::string(t) + " cannot be included in the file." });
		}
		return std::make_unique<ConcreteRule<ParserRule::CannotIncludeInFile>>(Ruling<1, HasRegex::No>{ ParserRule::CannotIncludeInFile, std::string(t), "Item: " + std::string(t) + " cannot be included in the file." });
	}


	std::unique_ptr<ConcreteRule<ParserRule::MustIncludeInFile>> new_rule_must_include_in_file_unique(std::string_view t)
	{
		if (is_regex(t))
		{
			return std::make_unique<ConcreteRule<ParserRule::MustIncludeInFile>>(Ruling<1, HasRegex::Yes>{ ParserRule::MustIncludeInFile, std::string(t), "Item: " + std::string(t) + " must be included in the file." });
		}
		return std::make_unique<ConcreteRule<ParserRule::MustIncludeInFile>>(Ruling<1, HasRegex::No>{ ParserRule::MustIncludeInFile, std::string(t), "Item: " + std::string(t) + " must be included in the file." });
	}



	std::shared_ptr<ConcreteRule<ParserRule::MustIncludeAfter>> new_rule_must_include_after_shared(std::string_view t1, std::string_view t2, size_t id)
	{
		if (is_regex(t1) && is_regex(t2))
		{
			return std::make_shared<ConcreteRule<ParserRule::MustIncludeAfter>>(Ruling<2, HasRegex::Yes>{ ParserRule::MustIncludeAfter, { std::string(t1), std::string(t2) }, "Item: " + std::string(t1) + " must be included after item: " + std::string(t2) }, id);
		}
		return std::make_shared<ConcreteRule<ParserRule::MustIncludeAfter>>(Ruling<2, HasRegex::No>{ ParserRule::MustIncludeAfter, {std::string(t1), std::string(t2)}, "Item: " + std::string(t1) + " must be included after item: " + std::string(t2) }, id);
	}

	std::shared_ptr<ConcreteRule<ParserRule::MustIncludeBefore>> new_rule_must_include_before_shared(std::string_view t1, std::string_view t2, size_t id)
	{
		if (is_regex(t1) && is_regex(t2))
		{
			return std::make_shared<ConcreteRule<ParserRule::MustIncludeBefore>>(Ruling<2, HasRegex::Yes>{ ParserRule::MustIncludeBefore, { std::string(t1), std::string(t2) }, "Item: " + std::string(t1) + " must be included before item: " + std::string(t2) }, id);
		}
		return std::make_shared<ConcreteRule<ParserRule::MustIncludeBefore>>(Ruling<2, HasRegex::No>{ ParserRule::MustIncludeBefore, {std::string(t1), std::string(t2)}, "Item: " + std::string(t1) + " must be included before item: " + std::string(t2) }, id);
	}

	std::shared_ptr<ConcreteRule<ParserRule::CannotInlcude>> new_rule_cannot_include_shared(std::string_view t, size_t id)
	{
		if (is_regex(t))
		{
			return std::make_shared<ConcreteRule<ParserRule::CannotInlcude>>(Ruling<1, HasRegex::Yes>{ ParserRule::CannotInlcude, std::string(t), "Item: " + std::string(t) + " cannot be included in this section." }, id);
		}
		return std::make_shared<ConcreteRule<ParserRule::CannotInlcude>>(Ruling<1, HasRegex::No>{ ParserRule::CannotInlcude, std::string(t), "Item: " + std::string(t) + " cannot be included in this section." }, id);
	}

	std::shared_ptr<ConcreteRule<ParserRule::MustInclude>> new_rule_must_include_shared(std::string_view t, size_t id)
	{
		if (is_regex(t))
		{
			return std::make_shared<ConcreteRule<ParserRule::MustInclude>>(Ruling<1, HasRegex::Yes>{ ParserRule::MustInclude, std::string(t), "Item: " + std::string(t) + " must be included in this section." }, id);
		}
		return std::make_shared<ConcreteRule<ParserRule::MustInclude>>(Ruling<1, HasRegex::No>{ ParserRule::MustInclude, std::string(t), "Item: " + std::string(t) + " must be included in this section." }, id);
	}

	std::shared_ptr<ConcreteRule<ParserRule::CannotIncludeInFile>> new_rule_cannot_include_in_file_shared(std::string_view t)
	{
		if (is_regex(t))
		{
			return std::make_shared<ConcreteRule<ParserRule::CannotIncludeInFile>>(Ruling<1, HasRegex::Yes>{ ParserRule::CannotIncludeInFile, std::string(t), "Item: " + std::string(t) + " cannot be included in the file." });
		}
		return std::make_shared<ConcreteRule<ParserRule::CannotIncludeInFile>>(Ruling<1, HasRegex::No>{ ParserRule::CannotIncludeInFile, std::string(t), "Item: " + std::string(t) + " cannot be included in the file." });
	}

	std::shared_ptr<ConcreteRule<ParserRule::MustIncludeInFile>> new_rule_must_include_in_file_shared(std::string_view t)
	{
		if (is_regex(t))
		{
			return std::make_shared<ConcreteRule<ParserRule::MustIncludeInFile>>(Ruling<1, HasRegex::Yes>{ ParserRule::MustIncludeInFile, std::string(t), "Item: " + std::string(t) + " must be included in the file." });
		}
		return std::make_shared<ConcreteRule<ParserRule::MustIncludeInFile>>(Ruling<1, HasRegex::No>{ ParserRule::MustIncludeInFile, std::string(t), "Item: " + std::string(t) + " must be included in the file." });
	}





	


}
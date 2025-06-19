#include "include/rule_operations.h"


namespace parser
{

	inline inline bool ConcreteRule<ParserRule::MustIncludeInFile>::check_global_rule(TokenizedSections const& sections)
	{

		bool isRegex = _rule->is_regex_based();

		if (_rule->type != ParserRule::MustIncludeInFile)
		{
			std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
			return false;
		}
			
		std::string_view name;
		bool success = false;


		for (auto const& section : sections)
		{
			for (const auto& token : section)
			{
				if (isRegex)
				{
					view_ptr<Ruling<1, HasRegex::Yes>> rule = static_cast<Ruling<1, HasRegex::Yes> const*>(_rule.get());

					if (std::regex_match(std::string(token), rule->reg.target))
					{
						name = rule->reg.pattern;
						success = true;
						break;
					}
				}
				else
				{
					view_ptr<Ruling<1, HasRegex::No>> rule = static_cast<Ruling<1, HasRegex::No> const*>(_rule.get());

					if (token == rule->target)
					{
						name = rule->target;
						success = true;
						break;
					}
				}
			}

			if (success)
			{
				std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << name << " In File" << std::endl;
				return true;
			}

		}

		std::cerr << PARSER_LOG_ERR << _rule->errMsg << std::endl;
		return false;
	}

	inline bool ConcreteRule<ParserRule::MustIncludeInFile>::check_rule_impl(TokenizedSectionizedCompact const& sections)
	{
		if (_rule->type != ParserRule::MustIncludeInFile)
		{
			std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
			return false;
		}

		bool isRegex = _rule->is_regex_based();

		std::string_view name;
		bool success = false;

		for (auto const& token : sections.tokens)
		{
			if (isRegex)
			{
				view_ptr<Ruling<1, HasRegex::Yes>> rule = static_cast<Ruling<1, HasRegex::Yes> const*>(_rule.get());
				if (std::regex_match(std::string(token), rule->reg.target))
				{
					name = rule->reg.pattern;
					success = true;
					break;
				}
			}
			else
			{
				view_ptr<Ruling<1, HasRegex::No>> rule = static_cast<Ruling<1, HasRegex::No> const*>(_rule.get());
				if (token == rule->target)
				{
					name = rule->target;
					success = true;
					break;
				}
			}
		}

		if (success)
		{
			std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << name << " In File" << std::endl;
			return true;
		}

		std::cerr << PARSER_LOG_ERR << _rule->errMsg << std::endl;
		return false;
	}
	


	inline ParserRule ConcreteRule<ParserRule::MustIncludeInFile>::get_rule_type() const
	{
		return ParserRule::MustIncludeInFile;
	}

	inline view_ptr<BaseRuling> ConcreteRule<ParserRule::MustIncludeInFile>::get_rule()
	{
		return _rule.get();
	}




	inline inline bool ConcreteRule<ParserRule::CannotInlcude>::check_local_rule(TokenizedSection const& sections)
	{
		if (_rule->type != ParserRule::CannotIncludeInFile)
		{
			std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
			return false;
		}

		bool isRegex = _rule->is_regex_based();

		std::string name;
		bool success = false;

		for (auto const& token : sections)
		{
			if (isRegex)
			{
				view_ptr<Ruling<1, HasRegex::Yes>> rule = static_cast<Ruling<1, HasRegex::Yes> const*>(_rule.get());
				if (std::regex_match(std::string(token), rule->reg.target))
				{
					name = rule->reg.pattern;
					success = true;
					break;
				}
			}
			else
			{
				view_ptr<Ruling<1, HasRegex::No>> rule = static_cast<Ruling<1, HasRegex::No> const*>(_rule.get());
				if (token == rule->target)
				{
					name = rule->target;
					success = true;
					break;
				}
			}
		}

		if (success)
		{
			std::cerr << PARSER_LOG_ERR << _rule->errMsg << std::endl;
			return false;
		}
		std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << name << " In File" << std::endl;
		return true;
	}





	inline bool ConcreteRule<ParserRule::CannotIncludeInFile>::check_global_rule(TokenizedSections const& sections)
	{
		if (_rule->type != ParserRule::CannotIncludeInFile)
		{
			std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
			return false;
		}

		bool isRegex = _rule->is_regex_based();

		std::string name;
		bool success = false;

		for (auto const& section : sections)
		{
			for (const auto& token : section)
			{
				if (isRegex)
				{
					view_ptr<Ruling<1, HasRegex::Yes>> rule = static_cast<Ruling<1, HasRegex::Yes> const*>(_rule.get());
					if (std::regex_match(std::string(token), rule->reg.target))
					{
						name = rule->reg.pattern;
						success = true;
						break;
					}
				}
				else
				{
					view_ptr<Ruling<1, HasRegex::No>> rule = static_cast<Ruling<1, HasRegex::No> const*>(_rule.get());
					if (token == rule->target)
					{
						name = rule->target;
						success = true;
						break;
					}
				}
			}
		}

		if (success)
		{
			std::cerr << PARSER_LOG_ERR << _rule->errMsg << std::endl;
			return false;
		}

		std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << name << " In File" << std::endl;
		return true;
	}

	inline bool ConcreteRule<ParserRule::CannotIncludeInFile>::check_rule_impl(TokenizedSectionizedCompact const& sections)
	{
		if (_rule->type != ParserRule::CannotIncludeInFile)
		{
			std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
			return false;
		}

		bool isRegex = _rule->is_regex_based();
		std::string name;
		bool success = false;

		for (const auto& token : sections.tokens)
		{
			if (isRegex)
			{
				view_ptr<Ruling<1, HasRegex::Yes>> rule = static_cast<Ruling<1, HasRegex::Yes> const*>(_rule.get());
				if (std::regex_match(std::string(token), rule->reg.target))
				{
					name = rule->reg.pattern;
					success = true;
					break;
				}
			}
			else
			{
				view_ptr<Ruling<1, HasRegex::No>> rule = static_cast<Ruling<1, HasRegex::No> const*>(_rule.get());
				if (token == rule->target)
				{
					name = rule->target;
					success = true;
					break;
				}
			}
		}

		if (success)
		{
			std::cerr << PARSER_LOG_ERR << _rule->errMsg << std::endl;
			return false;
		}
		std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << name << " In File" << std::endl;
		return true;
	}



	inline bool ConcreteRule<ParserRule::CannotInlcude>::check_rule_impl(TokenizedSectionizedCompact const& sections)
	{
		if (_rule->type != ParserRule::CannotInlcude)
		{
			std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
			return false;
		}

		bool isRegex = _rule->is_regex_based();
		std::string name;
		bool success = false;

		for (const auto& coord : sections.coords)
		{
			if (coord.start >= sections.tokens.size() || coord.end > sections.tokens.size())
			{
				std::cerr << PARSER_LOG_ERR << "Invalid section coordinates." << std::endl;
				return false;
			}

			if (isRegex && coord.identifier & _identifier)
			{
				view_ptr<Ruling<1, HasRegex::Yes>> rule = static_cast<Ruling<1, HasRegex::Yes> const*>(_rule.get());

				for (size_t i = coord.start; i < coord.end; i++)
				{
					if (std::regex_match(std::string(sections.tokens[i]), rule->reg.target))
					{
						name = rule->reg.pattern;
						success = true;
						break;
					}

				}
			}
			else if (!isRegex && coord.identifier & _identifier)
			{
				view_ptr<Ruling<1, HasRegex::No>> rule = static_cast<Ruling<1, HasRegex::No> const*>(_rule.get());

				for (size_t i = coord.start; i < coord.end; i++)
				{
					if (sections.tokens[i] == rule->target)
					{
						name = rule->target;
						success = true;
					}
				}
			}

		}

		if (success)
		{
			std::cerr << PARSER_LOG_ERR << _rule->errMsg << std::endl;
			return false;
		}

		std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << name << std::endl;
		return true;
	}



	inline bool ConcreteRule<ParserRule::MustInclude>::check_local_rule(TokenizedSection const& section)
	{
		if (_rule->type != ParserRule::MustInclude)
		{
			std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
			return false;
		}

		bool isRegex = _rule->is_regex_based();
		std::string name;
		bool success = false;


		for (const auto& token : section)
		{
			if (isRegex)
			{
				view_ptr<Ruling<1, HasRegex::Yes>> rule = static_cast<Ruling<1, HasRegex::Yes> const*>(_rule.get());

				if (std::regex_match(std::string(token), rule->reg.target))
				{
					name = rule->reg.pattern;
					success = true;
					break;
				}
			}
			else
			{
				view_ptr<Ruling<1, HasRegex::No>> rule = static_cast<Ruling<1, HasRegex::No> const*>(_rule.get());
				if (token == rule->target)
				{
					name = rule->target;
					success = true;
					break;
				}
			}
		}

		if (success)
		{
			std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << name << " In Section: ";
			print_tokens(section);

			return true;
		}

		std::cerr << PARSER_LOG_ERR << _rule->errMsg << std::endl;
		return false;
	}



	inline bool ConcreteRule<ParserRule::MustInclude>::check_rule_impl(TokenizedSectionizedCompact const& sections)
	{
		if (_rule->type != ParserRule::MustInclude)
		{
			std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
			return false;
		}

		bool isRegex = _rule->is_regex_based();
		std::string name;
		bool success = false;

		for (const auto& coord : sections.coords)
		{
			if (coord.start >= sections.tokens.size() || coord.end > sections.tokens.size())
			{
				std::cerr << PARSER_LOG_ERR << "Invalid section coordinates." << std::endl;
				return false;
			}

			if (isRegex && coord.identifier & _identifier)
			{
				view_ptr<Ruling<1, HasRegex::Yes>> rule = static_cast<Ruling<1, HasRegex::Yes> const*>(_rule.get());
				for (size_t i = coord.start; i < coord.end; i++)
				{
					if (std::regex_match(std::string(sections.tokens[i]), rule->reg.target))
					{
						name = rule->reg.pattern;
						success = true;
						break;
					}
				}
			}
			else if (!isRegex && coord.identifier & _identifier)
			{
				view_ptr<Ruling<1, HasRegex::No>> rule = static_cast<Ruling<1, HasRegex::No> const*>(_rule.get());
				for (size_t i = coord.start; i < coord.end; i++)
				{
					if (sections.tokens[i] == rule->target)
					{
						name = rule->target;
						success = true;
						break;
					}
				}
			}
		}

		if (success)
		{
			std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << name << " In Section: ";
			print_tokens(sections);

			return true;
		}

		std::cerr << PARSER_LOG_ERR << _rule->errMsg << std::endl;
		return false;
	}



	inline bool ConcreteRule<ParserRule::MustIncludeBefore>::check_local_rule(TokenizedSection const& section)
	{
		if (_rule->type != ParserRule::MustIncludeBefore)
		{
			std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
			return false;
		}

		bool isRegex = _rule->is_regex_based();
		std::string name1;
		std::string name2;
		bool success = false;

		for (size_t i = 1; i < section.size(); i++)
		{
			if (isRegex)
			{
				view_ptr<Ruling<2, HasRegex::Yes>> rule = static_cast<Ruling<2, HasRegex::Yes> const*>(_rule.get());

				if (std::regex_match(std::string(section[i]), rule->reg.targets[0]) && std::regex_match(std::string(section[i - 1]), rule->reg.targets[1]))
				{
					success = true;
					name1 = rule->reg.patterns[0];
					name2 = rule->reg.patterns[1];
				}
			}
			else
			{
				view_ptr<Ruling<2, HasRegex::No>> rule = static_cast<Ruling<2, HasRegex::No> const*>(_rule.get());

				if (section[i] == rule->targets[0] && section[i - 1] == rule->targets[1])
				{
					success = true;
					name1 = rule->targets[0];
					name2 = rule->targets[1];
				}
			}
		}

		if (success)
		{
			std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << name1 << " Before Item: " << name1 << std::endl;
			return true;
		}

		std::cerr << PARSER_LOG_ERR << _rule->errMsg << std::endl;
		return false;
	}


	
	inline bool ConcreteRule<ParserRule::MustIncludeBefore>::check_rule_impl(TokenizedSectionizedCompact const& sections)
	{
		if (_rule->type != ParserRule::MustIncludeBefore)
		{
			std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
			return false;
		}

		bool isRegex = _rule->is_regex_based();
		std::string name1;
		std::string name2;
		bool success = false;

		for (const auto& coord : sections.coords)
		{
			if (coord.start >= sections.tokens.size() || coord.end > sections.tokens.size())
			{
				std::cerr << PARSER_LOG_ERR << "Invalid section coordinates." << std::endl;
				return false;
			}

			for (size_t i = coord.start + 1; i < coord.end; i++)
			{

				if (isRegex && coord.identifier & _identifier && coord.end - coord.start > 0)
				{
					view_ptr<Ruling<2, HasRegex::Yes>> rule = static_cast<view_ptr<Ruling<2, HasRegex::Yes>>>(_rule.get());

					if (std::regex_match(std::string(sections.tokens[i]), rule->reg.targets[0]) &&
						std::regex_match(std::string(sections.tokens[i - 1]), rule->reg.targets[1]))
					{
						success = true;
						name1 = rule->reg.patterns[0];
						name2 = rule->reg.patterns[1];
					}

				}
				else if (!isRegex && coord.identifier & _identifier && coord.end - coord.start > 0)
				{
					view_ptr<Ruling<2, HasRegex::No>> rule = static_cast<view_ptr<Ruling<2, HasRegex::No>>>(_rule.get());

					if (sections.tokens[i] == rule->targets[0] && sections.tokens[i - 1] == rule->targets[1])
					{
						success = true;
						name1 = rule->targets[0];
						name2 = rule->targets[1];
					}
				}
			}
		}

		if (success)
		{
			std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << name1 << " Before Item: " << name2 << std::endl;
			return true;
		}

		std::cerr << PARSER_LOG_ERR << _rule->errMsg << std::endl;
		return false;
	}



	inline bool ConcreteRule<ParserRule::MustIncludeAfter>::check_rule_impl(TokenizedSectionizedCompact const& sections)
	{
		if (_rule->type != ParserRule::MustIncludeAfter)
		{
			std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
			return false;
		}

		bool isRegex = _rule->is_regex_based();
		std::string name1;
		std::string name2;
		bool success = false;


		for (const auto& coord : sections.coords)
		{
			if (coord.start >= sections.tokens.size() || coord.end > sections.tokens.size())
			{
				std::cerr << PARSER_LOG_ERR << "Invalid section coordinates." << std::endl;
				return false;
			}

			for (size_t i = coord.start; i < coord.end - 1; i++)
			{

				if (isRegex && coord.identifier & _identifier && coord.end > coord.start)
				{
					view_ptr<Ruling<2, HasRegex::Yes>> rule = static_cast<view_ptr<Ruling<2, HasRegex::Yes>>>(_rule.get());
					if (std::regex_match(std::string(sections.tokens[i]), rule->reg.targets[0]) && std::regex_match(std::string(sections.tokens[i + 1]), rule->reg.targets[1]))
					{
						name1 = rule->reg.patterns[0];
						name2 = rule->reg.patterns[1];
						success = true;
					}

				}
				else if (!isRegex && coord.identifier & _identifier && coord.end > coord.start)
				{
					view_ptr<Ruling<2, HasRegex::No>> rule = static_cast<view_ptr<Ruling<2, HasRegex::No>>>(_rule.get());

					if (sections.tokens[i] == rule->targets[0] && sections.tokens[i + 1] == rule->targets[1])
					{
						name1 = rule->targets[0];
						name2 = rule->targets[1];
						success = true;
					}
				}


			}
		}

		if (success)
		{
			std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << name1 << " After Item: " << name1 << std::endl;
			return true;
		}

		std::cerr << PARSER_LOG_ERR << _rule->errMsg << std::endl;
		return false;
	}




	inline bool ConcreteRule<ParserRule::MustIncludeAfter>::check_local_rule(TokenizedSection const& section)
	{
		if (_rule->type != ParserRule::MustIncludeAfter)
		{
			std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
			return false;
		}

		bool isRegex = _rule->is_regex_based();
		std::string name1;
		std::string name2;
		bool success = false;

		for (size_t i = 0; i < section.size() - 1; i++)
		{
			if (isRegex)
			{
				view_ptr<Ruling<2, HasRegex::Yes>> rule = static_cast<view_ptr<Ruling<2, HasRegex::Yes>>>(_rule.get());
				if (std::regex_match(std::string(section[i]), rule->reg.targets[0]) && std::regex_match(std::string(section[i + 1]), rule->reg.targets[1]))
				{
					name1 = rule->reg.patterns[0];
					name2 = rule->reg.patterns[1];
					success = true;
				}
			}
			else
			{
				view_ptr<Ruling<2, HasRegex::No>> rule = static_cast<view_ptr<Ruling<2, HasRegex::No>>>(_rule.get());

				if (section[i] == rule->targets[0] && section[i + 1] == rule->targets[1])
				{
					std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule->targets[0] << " After Item: " << rule->targets[1] << std::endl;
					return true;
				}
			}
		}

		if (success)
		{
			std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << name1 << " After Item: " << name2 << std::endl;
			return true;
		}

		std::cerr << PARSER_LOG_ERR << _rule->errMsg << std::endl;
		return false;
	}


}
#pragma once



#include "include/config.h"

#include "include/rules.h"
#include "include/tokenizer.h"



namespace parser
{
	struct RuleInputBase
	{
	public:

		virtual TokenizedSection const* get_section() const = 0;
		virtual TokenizedSections const* get_sections() const = 0;
	};

	template<bool T>
	struct RuleInput : public RuleInputBase
	{
	public:

		using Type = std::conditional_t<T, TokenizedSection, TokenizedSections>;
		
		Type data;
	
		RuleInput (Type const& d) : data(d) {}

		TokenizedSection const* get_section() const override
		{
			if constexpr (T)
			{
				return &data;
			}
			else
			{
				return nullptr;
			}
		}

		TokenizedSections const* get_sections() const override
		{
			if constexpr (!T)
			{
				return &data;
			}
			else
			{
				return nullptr;
			}
		}
	};
		
	enum class RuleType
	{
		None = 0,
		Global,
		Local
	};

	class Rule
	{
	public:

		Rule(RuleType type = RuleType::None)
			: _type(type)
		{}

		Rule(std::unique_ptr<BaseRuling>&& rule, RuleType type = RuleType::None)
			: _rule(std::move(rule)), _type(type)
		{}

		Rule(std::unique_ptr<BaseRuling>&& rule, size_t identifier, RuleType type = RuleType::None)
			: _rule(std::move(rule)), _identifier(identifier), _type(type)
		{}

		RuleType get_type() const;

		ParserRule get_rule_type() const;

		view_ptr<BaseRuling> get_rule();

		size_t get_identifier() const;

		virtual bool check_rule(RuleInputBase const& data) = 0;
		
		virtual bool check_rule(TokenizedSectionizedCompact const& data) = 0;

		virtual ~Rule() = default;
	
	protected:
		std::unique_ptr<BaseRuling> _rule;
		size_t _identifier = std::numeric_limits<size_t>::max() - ANONYMUS_SECTIONS_NOT_DEFUALT_CHECKED_BY_RULE;
		RuleType _type = RuleType::None; 

	};

	class GlobalRule : public Rule
	{
	public:

		GlobalRule();

		GlobalRule(std::unique_ptr<BaseRuling>&& rule);

		GlobalRule(std::unique_ptr<BaseRuling>&& rule, size_t identifier);

		~GlobalRule() override = default;

		bool check_rule(RuleInputBase const& data) override;

		virtual bool check_global_rule(TokenizedSections const& sections) = 0;

	};

	class LocalRule : public Rule
	{
	public:

		LocalRule();

		LocalRule(std::unique_ptr<BaseRuling>&& rule);

		LocalRule(std::unique_ptr<BaseRuling>&& rule, size_t identifier);

		~LocalRule() override = default;

		bool check_rule(RuleInputBase const& data) override;

		virtual bool check_local_rule(TokenizedSection const& section) = 0;


	};


	template<ParserRule R>
	struct ConcreteRule;

	template<>
	struct ConcreteRule<ParserRule::CannotIncludeInFile> : public GlobalRule
	{
		bool check_rule_impl(TokenizedSections const& sections, RulingOneTarget const& rule)
		{
			if (rule.type != ParserRule::CannotIncludeInFile)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (auto const& section : sections)
			{
				for (const auto& token : section)
				{
					if (token == rule.target)
					{
						std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
						return false;
					}
				}

			}

			std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << rule.target << " In File" << std::endl;
			return true;
		}

		bool check_rule_impl(TokenizedSectionizedCompact const& sections, RulingOneTarget const& rule)
		{
			if (rule.type != ParserRule::CannotIncludeInFile)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (const auto& token : sections.tokens)
			{
				if (token == rule.target)
				{
					std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
					return false;
				}	
			}
			std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << rule.target << " In File" << std::endl;
			return true;
		}


		bool check_global_rule(TokenizedSections const& sections) override
		{
			if (_rule->get_target_count() == 1)
			{
				return check_rule_impl(sections, static_cast<RulingOneTarget&>(*_rule.get()));
			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RulingOneTarget." << std::endl;
				return false;
			}
		}

		bool check_rule(TokenizedSectionizedCompact const& data) override
		{
			if (data.tokens.empty())
			{
				std::cerr << PARSER_LOG_ERR << "No tokens to check." << std::endl;
				return false;
			}
			if (_rule->get_target_count() == 1)
			{
				return check_rule_impl(data, static_cast<RulingOneTarget&>(*_rule.get()));
			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RulingOneTarget." << std::endl;
				return false;
			}
		}
	};

	template<>
	struct ConcreteRule<ParserRule::MustIncludeInFile> : public GlobalRule
	{
		bool check_rule_impl(TokenizedSections const& sections, RulingOneTarget const& rule)
		{
			if (rule.type != ParserRule::MustIncludeInFile)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (auto const& section : sections)
			{
				for (const auto& token : section)
				{
					if (token == rule.target)
					{
						std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule.target << " In File" << std::endl;
						return true;
					}
				}

			}

			std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
			return false;
		}

		bool check_rule_impl(TokenizedSectionizedCompact const& sections, RulingOneTarget const& rule)
		{
			if (rule.type != ParserRule::MustIncludeInFile)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (auto const& token : sections.tokens)
			{
				if (token == rule.target)
				{
					std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule.target << " In File" << std::endl;
					return true;
				}

			}

			std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
			return false;
		}

		bool check_global_rule(TokenizedSections const& sections) override
		{
			if (_rule->get_target_count() == 1)
			{
				return check_rule_impl(sections, static_cast<RulingOneTarget&>(*_rule.get()));
			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RulingOneTarget." << std::endl;
				return false;
			}
		}

		bool check_rule(TokenizedSectionizedCompact const& data) override
		{
			if (data.tokens.empty())
			{
				std::cerr << PARSER_LOG_ERR << "No tokens to check." << std::endl;
				return false;
			}
			if (_rule->get_target_count() == 1)
			{
				return check_rule_impl(data, static_cast<RulingOneTarget&>(*_rule.get()));
			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RulingOneTarget." << std::endl;
				return false;
			}
		}
	};

	template<>
	struct ConcreteRule<ParserRule::CannotInlcude> : public LocalRule
	{
		bool check_rule_impl(TokenizedSection const& section, RulingOneTarget const& rule)
		{
			if (rule.type != ParserRule::CannotInlcude)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (const auto& token : section)
			{
				if (token == rule.target)
				{
					std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
					return false;
				}
			}

			std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << rule.target << std::endl;
			return true;
		}
		

		bool check_rule_impl(TokenizedSectionizedCompact const& sections, RulingOneTarget const& rule)
		{
			if (rule.type != ParserRule::CannotInlcude)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (const auto& coord : sections.coords)
			{
				if (coord.start >= sections.tokens.size() || coord.end > sections.tokens.size())
				{
					std::cerr << PARSER_LOG_ERR << "Invalid section coordinates." << std::endl;
					return false;
				}

				if (coord.identifier == _identifier)
				{
	
					for (size_t i = coord.start; i < coord.end; i++)
					{
						if (sections.tokens[i] == rule.target)
						{
							std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
							return false;
						}
					}
				}

			}

			std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << rule.target << std::endl;
			return true;
		}

		bool check_local_rule(TokenizedSection const& section) override
		{
			if (_rule->get_target_count() == 1)
			{
				return check_rule_impl(section, static_cast<RulingOneTarget&>(*_rule.get()));

			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RulingOneTarget." << std::endl;
				return false;
			}
		}

		bool check_rule(TokenizedSectionizedCompact const& data) override
		{
			if (data.tokens.empty())
			{
				std::cerr << PARSER_LOG_ERR << "No tokens to check." << std::endl;
				return false;
			}
			if (_rule->get_target_count() == 1)
			{
				return check_rule_impl(data, static_cast<RulingOneTarget&>(*_rule.get()));
			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RulingOneTarget." << std::endl;
				return false;
			}
		}
	};


	template<>
	struct ConcreteRule<ParserRule::MustInclude> : public LocalRule
	{
		bool check_rule_impl(TokenizedSection section, RulingOneTarget const& rule)
		{
			if (rule.type != ParserRule::MustInclude)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (const auto& token : section)
			{
				if (token == rule.target)
				{
					std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule.target << " In Section: ";
					print_tokens(section);

					return true;
				}
			}

			std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
			return false;
		}

		bool check_rule_impl(TokenizedSectionizedCompact sections, RulingOneTarget const& rule)
		{
			if (rule.type != ParserRule::MustInclude)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (const auto& coord : sections.coords)
			{
				if (coord.start >= sections.tokens.size() || coord.end > sections.tokens.size())
				{
					std::cerr << PARSER_LOG_ERR << "Invalid section coordinates." << std::endl;
					return false;
				}

				if (coord.identifier == _identifier)
				{

					for (size_t i = coord.start; i < coord.end; i++)
					{
						if (sections.tokens[i] == rule.target)
						{
							std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule.target << " In Section: ";
							print_tokens(sections);

							return true;
						}
					}
				}
			}

			std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
			return false;
		}

		bool check_local_rule(TokenizedSection const& section) override
		{
			if (_rule->get_target_count() == 1)
			{
				return check_rule_impl(section, static_cast<RulingOneTarget&>(*_rule.get()));

			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RulingOneTarget." << std::endl;
				return false;
			}
		}

		bool check_rule(TokenizedSectionizedCompact const& data) override
		{
			if (data.tokens.empty())
			{
				std::cerr << PARSER_LOG_ERR << "No tokens to check." << std::endl;
				return false;
			}
			if (_rule->get_target_count() == 1)
			{
				return check_rule_impl(data, static_cast<RulingOneTarget&>(*_rule.get()));
			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RulingOneTarget." << std::endl;
				return false;
			}
		}
	};

	template<>
	struct ConcreteRule<ParserRule::MustIncludeBefore> : public LocalRule
	{
		bool check_rule_impl(TokenizedSection const& section, RulingTwoTarget const& rule)
		{
			if (rule.type != ParserRule::MustIncludeBefore)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}
			for (size_t i = 1; i < section.size(); i++)
			{
				if (section[i] == rule.targets[0] && section[i - 1] == rule.targets[1])
				{
					std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule.targets[0] << " Before Item: " << rule.targets[1] << std::endl;
					return true;
				}
			}
			std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
			return false;
		}


		bool check_rule_impl(TokenizedSectionizedCompact const& sections, RulingTwoTarget const& rule)
		{
			if (rule.type != ParserRule::MustIncludeBefore)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (const auto& coord : sections.coords)
			{
				if (coord.start >= sections.tokens.size() || coord.end > sections.tokens.size())
				{
					std::cerr << PARSER_LOG_ERR << "Invalid section coordinates." << std::endl;
					return false;
				}

				if (coord.identifier == _identifier && coord.end - coord.start > 0)
				{
					for (size_t i = coord.start + 1; i < coord.end; i++)
					{

						if (sections.tokens[i] == rule.targets[0] && sections.tokens[i - 1] == rule.targets[1])
						{
							std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule.targets[0] << " Before Item: " << rule.targets[1] << std::endl;
							return true;
						}
					
					}
				}
			}
			std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
			return false;
		}

		bool check_local_rule(TokenizedSection const& section) override
		{
			if (_rule->get_target_count() == 2)
			{
				return check_rule_impl(section, static_cast<RulingTwoTarget&>(*_rule));

			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RulingTwoTarget." << std::endl;
				return false;
			}
		}

		bool check_rule(TokenizedSectionizedCompact const& data) override
		{
			if (data.tokens.empty())
			{
				std::cerr << PARSER_LOG_ERR << "No tokens to check." << std::endl;
				return false;
			}
			if (_rule->get_target_count() == 2)
			{
				return check_rule_impl(data, static_cast<RulingTwoTarget&>(*_rule.get()));
			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RulingTwoTarget." << std::endl;
				return false;
			}
		}
	};

	template<>
	struct ConcreteRule<ParserRule::MustIncludeAfter> : public LocalRule
	{
		bool check_rule_impl(TokenizedSection const& section, RulingTwoTarget const& rule)
		{
			if (rule.type != ParserRule::MustIncludeAfter)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}
			for (size_t i = 0; i < section.size() - 1; i++)
			{
				if (section[i] == rule.targets[0] && section[i + 1] == rule.targets[1])
				{
					std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule.targets[0] << " After Item: " << rule.targets[1] << std::endl;
					return true;
				}
			}
			std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
			return false;
		}


		bool check_rule_impl(TokenizedSectionizedCompact const& sections, RulingTwoTarget const& rule)
		{
			if (rule.type != ParserRule::MustIncludeAfter)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (const auto& coord : sections.coords)
			{
				if (coord.start >= sections.tokens.size() || coord.end > sections.tokens.size())
				{
					std::cerr << PARSER_LOG_ERR << "Invalid section coordinates." << std::endl;
					return false;
				}

				if (coord.identifier == _identifier && coord.end > coord.start)
				{
					for (size_t i = coord.start; i < coord.end - 1; i++)
					{
						std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule.targets[0] << " After Item: " << rule.targets[1] << std::endl;
						return true;
					}
				}
			}
			
			std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
			return false;
		}

		bool check_local_rule(TokenizedSection const& section) override
		{
			{
				if (_rule->get_target_count() == 2)
				{
					return check_rule_impl(section, static_cast<RulingTwoTarget&>(*_rule));

				}
				else
				{
					std::cerr << PARSER_LOG_ERR << "Rule is not of type RulingTwoTarget." << std::endl;
					return false;
				}
			}
		}

		bool check_rule(TokenizedSectionizedCompact const& data) override
		{
			if (data.tokens.empty())
			{
				std::cerr << PARSER_LOG_ERR << "No tokens to check." << std::endl;
				return false;
			}
			if (_rule->get_target_count() == 2)
			{
				return check_rule_impl(data, static_cast<RulingTwoTarget&>(*_rule.get()));
			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RulingTwoTarget." << std::endl;
				return false;
			}
		}
	};
}




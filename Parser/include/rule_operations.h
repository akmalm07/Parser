#pragma once



#include "include/config.h"

#include "include/rules.h"


namespace parser
{
	struct RuleInputBase
	{
		virtual TokenizedSection const* get_section() const = 0;
		virtual TokenizedSections const* get_sections() const = 0;
	};

	template<bool T>
	struct RuleInput : public RuleInputBase
	{
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
		

	class RuleHandler
	{
	public:

		BaseRule* get_rule()
		{
			return _rule.get();
		}

		virtual bool check_rule(RuleInputBase const& data) = 0;

		virtual ~RuleHandler() = default;
	protected:
		RuleHandler() = default;

	protected:
		std::unique_ptr<BaseRule> _rule;
	};

	class GlobalRuleHandler : public RuleHandler
	{
	public:
		~GlobalRuleHandler() override = default;

		bool check_rule(RuleInputBase const& data) override;

		virtual bool check_global_rule(TokenizedSections const& sections) = 0;

	};

	class LocalRuleHandler : public RuleHandler
	{
	public:
		~LocalRuleHandler() override = default;

		bool check_rule(RuleInputBase const& data) override
		{
			if (std::holds_alternative<TokenizedSection>(data))
			{
				auto section = std::get<TokenizedSection>(data);
				return check_local_rule(section);
			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Invalid data type for LocalRuleHandler." << std::endl;
				return false;
			}
		}

		virtual bool check_local_rule(TokenizedSection const& section) = 0;


	};


	template<ParserRule R>
	struct ConcreteRule;

	template<>
	struct ConcreteRule<ParserRule::CannotIncludeInFile> : public GlobalRuleHandler
	{
		bool check_rule_impl(TokenizedSections const& sections, RuleOneTarget const& rule)
		{
			if (rule.type != ParserRule::CannotIncludeInFile)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (auto const& section : sections)
			{
				for (const auto& tocken : section)
				{
					if (tocken == rule.target)
					{
						std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
						return false;
					}
				}

			}

			std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << rule.target << " In File" << std::endl;
			return true;
		}

		bool check_global_rule(TokenizedSections const& sections) override
		{
			const RuleOneTarget* rule = dynamic_cast<RuleOneTarget*>(_rule.get());
			if (!rule)
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RuleOneTarget." << std::endl;
				return false;
			}

			return check_rule_impl(sections, *rule);
		}
	};

	// Specialization for Rule::CannotInclude
	template<>
	struct ConcreteRule<ParserRule::MustIncludeInFile> : public GlobalRuleHandler
	{
		bool check_rule_impl(TokenizedSections const& sections, RuleOneTarget const& rule)
		{
			if (rule.type != ParserRule::MustIncludeInFile)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (auto const& section : sections)
			{
				for (const auto& tocken : section)
				{
					if (tocken == rule.target)
					{
						std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule.target << " In File" << std::endl;
						return true;
					}
				}

			}

			std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
			return false;
		}

		bool check_global_rule(TokenizedSections const& sections) override
		{
			const RuleOneTarget* rule = dynamic_cast<RuleOneTarget*>(_rule.get());
			if (!rule)
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RuleOneTarget." << std::endl;
				return false;
			}

			return check_rule_impl(sections, *rule);
		}
	};

	template<>
	struct ConcreteRule<ParserRule::CannotInlcude> : public LocalRuleHandler
	{
		bool check_rule_impl(TokenizedSection const& section, RuleOneTarget const& rule)
		{
			if (rule.type != ParserRule::CannotInlcude)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (const auto& tocken : section)
			{
				if (tocken == rule.target)
				{
					std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
					return false;
				}
			}

			std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << rule.target << std::endl;
			return true;
		}

		bool check_local_rule(TokenizedSection const& section) override
		{
			if (_rule->get_target_count() == 1)
			{
				return check_rule_impl(section, static_cast<RuleOneTarget&>(*_rule));

			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RuleOneTarget." << std::endl;
				return false;
			}
		}
	};


	template<>
	struct ConcreteRule<ParserRule::MustInclude> : public LocalRuleHandler
	{
		bool check_rule_impl(TokenizedSection section, RuleOneTarget const& rule)
		{
			if (rule.type != ParserRule::MustInclude)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (const auto& tocken : section)
			{
				if (tocken == rule.target)
				{
					std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule.target << " In Section: ";
					print_tokens(section);

					return true;
				}
			}

			std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
			return false;
		}

		bool check_local_rule(TokenizedSection const& section) override
		{
			if (_rule->get_target_count() == 1)
			{
				return check_rule_impl(section, static_cast<RuleOneTarget&>(*_rule));

			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RuleOneTarget." << std::endl;
				return false;
			}
		}
	};

	template<>
	struct ConcreteRule<ParserRule::MustIncludeBefore> : public LocalRuleHandler
	{
		bool check_rule_impl(TokenizedSection const& section, RuleTwoTarget const& rule)
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

		bool check_local_rule(TokenizedSection const& section) override
		{
			if (_rule->get_target_count() == 2)
			{
				return check_rule_impl(section, static_cast<RuleTwoTarget&>(*_rule));

			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RuleTwoTarget." << std::endl;
				return false;
			}
		}
	};

	template<>
	struct ConcreteRule<ParserRule::MustIncludeAfter> : public LocalRuleHandler
	{
		bool check_rule_impl(TokenizedSection const& section, RuleTwoTarget const& rule)
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

		bool check_local_rule(TokenizedSection const& section) override
		{
			{
				if (_rule->get_target_count() == 2)
				{
					return check_rule_impl(section, static_cast<RuleTwoTarget&>(*_rule));

				}
				else
				{
					std::cerr << PARSER_LOG_ERR << "Rule is not of type RuleTwoTarget." << std::endl;
					return false;
				}
			}
		}
	};
}




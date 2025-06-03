#pragma once

#include "include/config.h"
#include "include/tockenization.h"


namespace parser {


	//System Implementation:
	/*
	* The RuleHandler system is designed to handle various rules that can be applied to sections of a file.
	* The system uses polymorphism to allow different types of rules to be checked against the data.
	* The RuleInput is a type alias that can hold either a single TockenizedSection or multiple TockenizedSections.
	* This is to ensure that the RuleHandler can handle both global rules that apply to multiple sections and local rules that apply to a single section.
	* The check_rule method in the RuleHandler class is a virtual method that takes a RuleInput and checks if the rule applies to the data.
	* The method is expectantly useful for global rules that need to check multiple sections at once, and allows Local rules to check single sections.
	*/
	

	//TESTING STATUS: Incomplete
	using RuleInput = std::variant<TockenizedSection, TockenizedSections>;

	class RuleHandler
	{
	public:

		Rule* get_rule()
		{
			return _rule.get();
		}

		virtual bool check_rule(RuleInput const& data) = 0;

		virtual ~RuleHandler() = default;
	protected:
		RuleHandler() = default;
	
	protected:
		std::unique_ptr<Rule> _rule;
	};

	class GlobalRuleHandler : public RuleHandler
	{
	public:
		~GlobalRuleHandler() override = default;
		
		bool check_rule(RuleInput const& data) override
		{
			if (std::holds_alternative<TockenizedSections>(data))
			{
				auto section = std::get<TockenizedSections>(data);
				return check_global_rule(section);
			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Invalid data type for GlobalRuleHandler." << std::endl;
				return false;
			}
		}

		virtual bool check_global_rule(TockenizedSections const& sections) = 0;

	};

	class LocalRuleHandler : public RuleHandler
	{
	public:
		~LocalRuleHandler() override = default;
		
		bool check_rule(RuleInput const& data) override
		{
			if (std::holds_alternative<TockenizedSection>(data))
			{
				auto section = std::get<TockenizedSection>(data);
				return check_local_rule(section);
			}
			else
			{
				std::cerr << PARSER_LOG_ERR << "Invalid data type for LocalRuleHandler." << std::endl;
				return false;
			}
		}

		virtual bool check_local_rule(TockenizedSection const& section) = 0;


	};


	template<ParserRule R>
	struct ConcreteRule;

	template<>
	struct ConcreteRule<ParserRule::CannotIncludeInFile> : public GlobalRuleHandler
	{
		bool check_rule_impl(TockenizedSections const& sections, RuleWithOneTarget const& rule)
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

		bool check_global_rule(TockenizedSections const& sections) override
		{
			const RuleWithOneTarget* rule = dynamic_cast<RuleWithOneTarget*>(_rule.get());
			if (!rule)
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RuleWithOneTarget." << std::endl;
				return false;
			}

			return check_rule_impl(sections, *rule);
		}
	};

	// Specialization for Rule::CannotInclude
	template<>
	struct ConcreteRule<ParserRule::MustIncludeInFile> : public GlobalRuleHandler
	{
		bool check_rule_impl(TockenizedSections const& sections, RuleWithOneTarget const& rule)
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

		bool check_global_rule(TockenizedSections const& sections) override
		{
			const RuleWithOneTarget* rule = dynamic_cast<RuleWithOneTarget*>(_rule.get());
			if (!rule)
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RuleWithOneTarget." << std::endl;
				return false;
			}

			return check_rule_impl(sections, *rule);
		}
	};

	template<>
	struct ConcreteRule<ParserRule::CannotInlcude> : public LocalRuleHandler
	{
		bool check_rule_impl(TockenizedSection const& section, RuleWithOneTarget const& rule)
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

		bool check_local_rule(TockenizedSection const& section) override
		{
			const RuleWithOneTarget* rule = dynamic_cast<RuleWithOneTarget*>(_rule.get());
			if (!rule)
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RuleWithOneTarget." << std::endl;
				return false;
			}

			return check_rule_impl(section, *rule);
		}
	};


	template<>
	struct ConcreteRule<ParserRule::MustInclude> : public LocalRuleHandler
	{
		bool check_rule_impl(TockenizedSection section, RuleWithOneTarget const& rule)
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
					std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule.target << " In Section: " << print_tokens(section) << std::endl;
					return true;
				}
			}

			std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
			return false;
		}

		bool check_local_rule(TockenizedSection const& section) override
		{
			const RuleWithOneTarget* rule = dynamic_cast<RuleWithOneTarget*>(_rule.get());
			if (!rule)
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RuleWithOneTarget." << std::endl;
				return false;
			}

			return check_rule_impl(section, *rule);
		}
	};

	template<>
	struct ConcreteRule<ParserRule::MustIncludeBefore> : public LocalRuleHandler
	{
		bool check_rule_impl(TockenizedSection const& section, RuleWithTwoTarget const& rule)
		{
			if (rule.type != ParserRule::MustIncludeBefore)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}
			for (size_t i = 1; i < section.size(); i++)
			{
				if (section[i] == rule.target1 && section[i - 1] == rule.target2)
				{
					std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule.target1 << " Before Item: " << rule.target2 << std::endl;
					return true;
				}
			}
			std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
			return false;
		}

		bool check_local_rule(TockenizedSection const& section) override
		{
			const RuleWithTwoTarget* rule = dynamic_cast<RuleWithTwoTarget*>(_rule.get());
			if (!rule)
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RuleWithOneTarget." << std::endl;
				return false;
			}

			return check_rule_impl(section, *rule);
		}
	};

	template<>
	struct ConcreteRule<ParserRule::MustIncludeAfter> : public LocalRuleHandler
	{
		bool check_rule_impl(TockenizedSection const& section, RuleWithTwoTarget const& rule)
		{
			if (rule.type != ParserRule::MustIncludeAfter)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}
			for (size_t i = 0; i < section.size() - 1; i++)
			{
				if (section[i] == rule.target1 && section[i + 1] == rule.target2)
				{
					std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << rule.target1 << " After Item: " << rule.target2 << std::endl;
					return true;
				}
			}
			std::cerr << PARSER_LOG_ERR << rule.errMsg << std::endl;
			return false;
		}

		bool check_local_rule(TockenizedSection const& section) override
		{
			const RuleWithTwoTarget* rule = dynamic_cast<RuleWithTwoTarget*>(_rule.get());
			if (!rule)
			{
				std::cerr << PARSER_LOG_ERR << "Rule is not of type RuleWithOneTarget." << std::endl;
				return false;
			}

			return check_rule_impl(section, *rule);
		}
	};


}
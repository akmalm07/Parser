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

		RuleInput(Type const& d) : data(d) {}

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
		{
		}

		Rule(RuleType type, size_t identifier)
			: _identifier(identifier), _type(type)
		{
		}

		RuleType get_type() const;

		virtual ParserRule get_rule_type() const = 0;

		virtual view_ptr<BaseRuling> get_rule() = 0;

		size_t get_identifier() const;

		virtual bool check_rule(RuleInputBase const& data) = 0;

		bool check_rule(TokenizedSectionizedCompact const& data);

		virtual bool check_rule_impl(TokenizedSectionizedCompact const& data) = 0;


		virtual ~Rule() = default;

	protected:
		size_t _identifier = std::numeric_limits<size_t>::max() - ANONYMUS_SECTIONS_NOT_DEFUALT_CHECKED_BY_RULE;
		RuleType _type = RuleType::None;

	};

	class GlobalRule : public Rule
	{
	public:

		GlobalRule();

		GlobalRule(size_t identifier);

		~GlobalRule() override = default;

		bool check_rule(RuleInputBase const& data) override;

		virtual bool check_global_rule(TokenizedSections const& sections) = 0;

	};

	class LocalRule : public Rule
	{
	public:

		LocalRule();

		LocalRule(size_t identifier);

		~LocalRule() override = default;

		bool check_rule(RuleInputBase const& data) override;

		virtual bool check_local_rule(TokenizedSection const& section) = 0;


	};


	template<ParserRule R>
	struct ConcreteRule;

	template<>
	struct ConcreteRule<ParserRule::CannotIncludeInFile> : public GlobalRule
	{
		ConcreteRule(RulingOneTarget const& rule, size_t identifier)
			: _rule(rule), GlobalRule(identifier)
		{}


		bool check_global_rule(TokenizedSections const& sections) override
		{
			if (_rule.type != ParserRule::CannotIncludeInFile)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (auto const& section : sections)
			{
				for (const auto& token : section)
				{
					if (token == _rule.target)
					{
						std::cerr << PARSER_LOG_ERR << _rule.errMsg << std::endl;
						return false;
					}
				}

			}

			std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << _rule.target << " In File" << std::endl;
			return true;
		}

		ParserRule get_rule_type() const override
		{
			return ParserRule::CannotIncludeInFile;
		}

		view_ptr<BaseRuling> get_rule() override
		{
			return &_rule;
		}

		bool check_rule_impl(TokenizedSectionizedCompact const& sections)
		{
			if (_rule.type != ParserRule::CannotIncludeInFile)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (const auto& token : sections.tokens)
			{
				if (token == _rule.target)
				{
					std::cerr << PARSER_LOG_ERR << _rule.errMsg << std::endl;
					return false;
				}
			}
			std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << _rule.target << " In File" << std::endl;
			return true;
		}


	private:

		RulingOneTarget _rule; 
	};


	template<>
	struct ConcreteRule<ParserRule::MustIncludeInFile> : public GlobalRule
	{

		ConcreteRule(RulingOneTarget const& rule, size_t identifier)
			: _rule(rule), GlobalRule(identifier)
		{
		}

		bool check_global_rule(TokenizedSections const& sections) override
		{
			if (_rule.type != ParserRule::MustIncludeInFile)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (auto const& section : sections)
			{
				for (const auto& token : section)
				{
					if (token == _rule.target)
					{
						std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << _rule.target << " In File" << std::endl;
						return true;
					}
				}

			}

			std::cerr << PARSER_LOG_ERR << _rule.errMsg << std::endl;
			return false;
		}

		ParserRule get_rule_type() const override
		{
			return ParserRule::MustIncludeInFile;
		}

		view_ptr<BaseRuling> get_rule() override
		{
			return &_rule;
		}

		bool check_rule_impl(TokenizedSectionizedCompact const& sections) override
		{
			if (_rule.type != ParserRule::MustIncludeInFile)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (auto const& token : sections.tokens)
			{
				if (token == _rule.target)
				{
					std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << _rule.target << " In File" << std::endl;
					return true;
				}

			}

			std::cerr << PARSER_LOG_ERR << _rule.errMsg << std::endl;
			return false;
		}

	private:
		RulingOneTarget _rule;
	};

	template<>
	struct ConcreteRule<ParserRule::CannotInlcude> : public LocalRule
	{
		ConcreteRule(RulingOneTarget const& rule, size_t identifier)
			: _rule(rule), LocalRule(identifier)
		{
		}
		bool check_local_rule(TokenizedSection const& section) override
		{
			if (_rule.type != ParserRule::CannotInlcude)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (const auto& token : section)
			{
				if (token == _rule.target)
				{
					std::cerr << PARSER_LOG_ERR << _rule.errMsg << std::endl;
					return false;
				}
			}

			std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << _rule.target << std::endl;
			return true;
		}


		ParserRule get_rule_type() const override
		{
			return ParserRule::CannotInlcude;
		}

		view_ptr<BaseRuling> get_rule() override
		{
			return &_rule;
		}


		bool check_rule_impl(TokenizedSectionizedCompact const& sections) override
		{
			if (_rule.type != ParserRule::CannotInlcude)
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
						if (sections.tokens[i] == _rule.target)
						{
							std::cerr << PARSER_LOG_ERR << _rule.errMsg << std::endl;
							return false;
						}
					}
				}

			}

			std::cout << PARSER_LOG_INFO << "Rule: Did Not Find Item: " << _rule.target << std::endl;
			return true;
		}
	private:
		RulingOneTarget _rule;
	};


	template<>
	struct ConcreteRule<ParserRule::MustInclude> : public LocalRule
	{

		ConcreteRule(RulingOneTarget const& rule, size_t identifier)
			: _rule(rule), LocalRule(identifier)
		{
		}

		bool check_local_rule(TokenizedSection const& section) override
		{
			if (_rule.type != ParserRule::MustInclude)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}

			for (const auto& token : section)
			{
				if (token == _rule.target)
				{
					std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << _rule.target << " In Section: ";
					print_tokens(section);

					return true;
				}
			}

			std::cerr << PARSER_LOG_ERR << _rule.errMsg << std::endl;
			return false;
		}


		ParserRule get_rule_type() const override
		{
			return ParserRule::MustInclude;
		}

		view_ptr<BaseRuling> get_rule() override
		{
			return &_rule;
		}


		bool check_rule_impl(TokenizedSectionizedCompact const& sections) override
		{
			if (_rule.type != ParserRule::MustInclude)
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
						if (sections.tokens[i] == _rule.target)
						{
							std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << _rule.target << " In Section: ";
							print_tokens(sections);

							return true;
						}
					}
				}
			}

			std::cerr << PARSER_LOG_ERR << _rule.errMsg << std::endl;
			return false;
		}

	private:
		RulingOneTarget _rule;
	};

	template<>
	struct ConcreteRule<ParserRule::MustIncludeBefore> : public LocalRule
	{
		ConcreteRule(RulingTwoTarget const& rule, size_t identifier)
			: _rule(rule), LocalRule(identifier)
		{
		}


		bool check_local_rule(TokenizedSection const& section)
		{
			if (_rule.type != ParserRule::MustIncludeBefore)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}
			for (size_t i = 1; i < section.size(); i++)
			{
				if (section[i] == _rule.targets[0] && section[i - 1] == _rule.targets[1])
				{
					std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << _rule.targets[0] << " Before Item: " << _rule.targets[1] << std::endl;
					return true;
				}
			}
			std::cerr << PARSER_LOG_ERR << _rule.errMsg << std::endl;
			return false;
		}


		ParserRule get_rule_type() const override
		{
			return ParserRule::MustIncludeBefore;
		}

		view_ptr<BaseRuling> get_rule() override
		{
			return &_rule;
		}

		bool check_rule_impl(TokenizedSectionizedCompact const& sections)
		{
			if (_rule.type != ParserRule::MustIncludeBefore)
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

						if (sections.tokens[i] == _rule.targets[0] && sections.tokens[i - 1] == _rule.targets[1])
						{
							std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << _rule.targets[0] << " Before Item: " << _rule.targets[1] << std::endl;
							return true;
						}

					}
				}
			}
			std::cerr << PARSER_LOG_ERR << _rule.errMsg << std::endl;
			return false;
		}
	private:
		RulingTwoTarget _rule;
	};

	template<>
	struct ConcreteRule<ParserRule::MustIncludeAfter> : public LocalRule
	{

		ConcreteRule(RulingTwoTarget const& rule, size_t identifier)
			: _rule(rule), LocalRule(identifier)
		{
		}

		bool check_local_rule(TokenizedSection const& section) override
		{
			if (_rule.type != ParserRule::MustIncludeAfter)
			{
				std::cerr << PARSER_LOG_ERR << "Rule type mismatch." << std::endl;
				return false;
			}
			for (size_t i = 0; i < section.size() - 1; i++)
			{
				if (section[i] == _rule.targets[0] && section[i + 1] == _rule.targets[1])
				{
					std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << _rule.targets[0] << " After Item: " << _rule.targets[1] << std::endl;
					return true;
				}
			}
			std::cerr << PARSER_LOG_ERR << _rule.errMsg << std::endl;
			return false;
		}

		ParserRule get_rule_type() const override
		{
			return ParserRule::MustIncludeAfter;
		}

		view_ptr<BaseRuling> get_rule() override
		{
			return &_rule;
		}

		bool check_rule_impl(TokenizedSectionizedCompact const& sections) override
		{
			if (_rule.type != ParserRule::MustIncludeAfter)
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
						std::cout << PARSER_LOG_INFO << "Rule: Found Item: " << _rule.targets[0] << " After Item: " << _rule.targets[1] << std::endl;
						return true;
					}
				}
			}

			std::cerr << PARSER_LOG_ERR << _rule.errMsg << std::endl;
			return false;
		}
	private:
		RulingTwoTarget _rule;
	};






	ConcreteRule<ParserRule::MustIncludeAfter> new_rule_must_include_after(std::string_view t1, std::string_view t2, size_t id);

	ConcreteRule<ParserRule::MustIncludeBefore> new_rule_must_include_before(std::string_view t1, std::string_view t2, size_t id);

	ConcreteRule<ParserRule::CannotIncludeInFile> new_rule_cannot_include_in_file(std::string_view t, size_t id);

	ConcreteRule<ParserRule::MustIncludeInFile> new_rule_must_include_in_file(std::string_view t, size_t id);

	ConcreteRule<ParserRule::CannotInlcude> new_rule_cannot_include(std::string_view t, size_t id);

	ConcreteRule<ParserRule::MustInclude> new_rule_must_include(std::string_view t, size_t id);



	std::unique_ptr<ConcreteRule<ParserRule::MustIncludeAfter>> new_rule_must_include_after_unique(std::string_view t1, std::string_view t2, size_t id);

	std::unique_ptr<ConcreteRule<ParserRule::MustIncludeBefore>> new_rule_must_include_before_unique(std::string_view t1, std::string_view t2, size_t id);

	std::unique_ptr<ConcreteRule<ParserRule::CannotIncludeInFile>> new_rule_cannot_include_in_file_unique(std::string_view t, size_t id);

	std::unique_ptr<ConcreteRule<ParserRule::MustIncludeInFile>> new_rule_must_include_in_file_unique(std::string_view t, size_t id);

	std::unique_ptr<ConcreteRule<ParserRule::CannotInlcude>> new_rule_cannot_include_unique(std::string_view t, size_t id);

	std::unique_ptr<ConcreteRule<ParserRule::MustInclude>> new_rule_must_include_unique(std::string_view t, size_t id);


	std::shared_ptr<ConcreteRule<ParserRule::MustIncludeAfter>> new_rule_must_include_after_shared(std::string_view t1, std::string_view t2, size_t id);

	std::shared_ptr<ConcreteRule<ParserRule::MustIncludeBefore>> new_rule_must_include_before_shared(std::string_view t1, std::string_view t2, size_t id);

	std::shared_ptr<ConcreteRule<ParserRule::CannotIncludeInFile>> new_rule_cannot_include_in_file_shared(std::string_view t, size_t id);

	std::shared_ptr<ConcreteRule<ParserRule::MustIncludeInFile>> new_rule_must_include_in_file_shared(std::string_view t, size_t id);

	std::shared_ptr<ConcreteRule<ParserRule::CannotInlcude>> new_rule_cannot_include_shared(std::string_view t, size_t id);

	std::shared_ptr<ConcreteRule<ParserRule::MustInclude>> new_rule_must_include_shared(std::string_view t, size_t id);






	//ConcreteRule<ParserRule::MustIncludeBetween> new_rule_must_include_between(std::string_view between1, std::string_view between2, std::string_view target,  size_t id);





}
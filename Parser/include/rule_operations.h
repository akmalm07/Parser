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


	bool is_regex(std::string const& str);
	bool is_regex(std::string_view str);


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


	struct RuleResult
	{
		std::string_view name = "";
		bool success = false;
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

		Rule(std::string_view str, RuleType type, size_t identifier);
		
		RuleType get_type() const;

		virtual ParserRule get_rule_type() const = 0;

		virtual view_ptr<BaseRuling> get_rule() = 0;

		size_t get_identifier() const;

		bool is_regex(std::string_view str);

		virtual bool check_rule(RuleInputBase const& data) = 0;

		bool check_rule(TokenizedSectionizedCompact const& data);

		virtual bool check_rule_impl(TokenizedSectionizedCompact const& data) = 0;


		virtual ~Rule() = default;

	protected:
		size_t _identifier = 0b0000;
		RuleType _type = RuleType::None;

	};

	class GlobalRule : public Rule
	{
	public:

		GlobalRule();
		
		GlobalRule(size_t identifier);

		virtual ~GlobalRule() override = default;

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
		template<HasRegex T>
		ConcreteRule(Ruling<1, T> const& rule)
			: _rule(std::make_unique<Ruling<1, T>>(rule)), GlobalRule()
		{}


		bool check_global_rule(TokenizedSections const& sections) override;

		ParserRule get_rule_type() const override
		{
			return ParserRule::CannotIncludeInFile;
		}

		view_ptr<BaseRuling> get_rule() override
		{
			return _rule.get();
		}

		bool check_rule_impl(TokenizedSectionizedCompact const& sections);


	private:

		std::unique_ptr<RulingOneTarget> _rule;
	};


	template<>
	struct ConcreteRule<ParserRule::MustIncludeInFile> : public GlobalRule
	{
		template<HasRegex T>
		ConcreteRule(Ruling<1, T> const& rule)
			: _rule(std::make_unique<Ruling<1, T>>(rule)), GlobalRule()
		{}

		bool check_global_rule(TokenizedSections const& sections) override;

		ParserRule get_rule_type() const override;

		view_ptr<BaseRuling> get_rule() override;

		bool check_rule_impl(TokenizedSectionizedCompact const& sections) override;

	private:
		std::unique_ptr<RulingOneTarget> _rule;
	};

	template<>
	struct ConcreteRule<ParserRule::CannotInlcude> : public LocalRule
	{
		template<HasRegex T>
		ConcreteRule(Ruling<1, T> const& rule, size_t identifier)
			: _rule(std::make_unique<Ruling<1, T>>(rule)), LocalRule(identifier)
		{
		}
		bool check_local_rule(TokenizedSection const& section) override;


		ParserRule get_rule_type() const override
		{
			return ParserRule::CannotInlcude;
		}

		view_ptr<BaseRuling> get_rule() override
		{
			return _rule.get();
		}


		bool check_rule_impl(TokenizedSectionizedCompact const& sections) override;

	private:
		std::unique_ptr<RulingOneTarget> _rule;
	};


	template<>
	struct ConcreteRule<ParserRule::MustInclude> : public LocalRule
	{
		template<HasRegex T>
		ConcreteRule(Ruling<1, T> const& rule, size_t identifier)
			: _rule(std::make_unique<Ruling<1, T>>(rule)), LocalRule(identifier)
		{
		}

		bool check_local_rule(TokenizedSection const& section) override;


		ParserRule get_rule_type() const override
		{
			return ParserRule::MustInclude;
		}

		view_ptr<BaseRuling> get_rule() override
		{
			return _rule.get();
		}


		bool check_rule_impl(TokenizedSectionizedCompact const& sections) override;



	private:
		std::unique_ptr<RulingOneTarget> _rule;
	};

	template<>
	struct ConcreteRule<ParserRule::MustIncludeBefore> : public LocalRule
	{
		template<HasRegex T>
		ConcreteRule(Ruling<2, T> const& rule, size_t identifier)
			: _rule(std::make_unique<Ruling<2, T>>(rule)), LocalRule(identifier)
		{
		}


		bool check_local_rule(TokenizedSection const& section);


		ParserRule get_rule_type() const override
		{
			return ParserRule::MustIncludeBefore;
		}

		view_ptr<BaseRuling> get_rule() override
		{
			return _rule.get();
		}

		bool check_rule_impl(TokenizedSectionizedCompact const& sections);

	private:
		std::unique_ptr<RulingTwoTarget> _rule;
	};

	template<>
	struct ConcreteRule<ParserRule::MustIncludeAfter> : public LocalRule
	{
		template<HasRegex T>
		ConcreteRule(Ruling<2, T> const& rule, size_t identifier)
			: _rule(std::make_unique<Ruling<2, T>>(rule)), LocalRule(identifier)
		{
		}

		bool check_local_rule(TokenizedSection const& section) override;

		ParserRule get_rule_type() const override
		{
			return ParserRule::MustIncludeAfter;
		}

		view_ptr<BaseRuling> get_rule() override
		{
			return _rule.get();
		}

		bool check_rule_impl(TokenizedSectionizedCompact const& sections) override;

	private:
		std::unique_ptr<RulingTwoTarget> _rule;
	};






	ConcreteRule<ParserRule::MustIncludeAfter> new_rule_must_include_after(std::string_view t1, std::string_view t2);

	ConcreteRule<ParserRule::MustIncludeBefore> new_rule_must_include_before(std::string_view t1, std::string_view t2);

	ConcreteRule<ParserRule::CannotIncludeInFile> new_rule_cannot_include_in_file(std::string_view t, size_t id);

	ConcreteRule<ParserRule::MustIncludeInFile> new_rule_must_include_in_file(std::string_view t, size_t id);

	ConcreteRule<ParserRule::CannotInlcude> new_rule_cannot_include(std::string_view t, size_t id);

	ConcreteRule<ParserRule::MustInclude> new_rule_must_include(std::string_view t, size_t id);



	std::unique_ptr<ConcreteRule<ParserRule::MustIncludeAfter>> new_rule_must_include_after_unique(std::string_view t1, std::string_view t2);

	std::unique_ptr<ConcreteRule<ParserRule::MustIncludeBefore>> new_rule_must_include_before_unique(std::string_view t1, std::string_view t2);

	std::unique_ptr<ConcreteRule<ParserRule::CannotIncludeInFile>> new_rule_cannot_include_in_file_unique(std::string_view t);

	std::unique_ptr<ConcreteRule<ParserRule::MustIncludeInFile>> new_rule_must_include_in_file_unique(std::string_view t);

	std::unique_ptr<ConcreteRule<ParserRule::CannotInlcude>> new_rule_cannot_include_unique(std::string_view t, size_t id);

	std::unique_ptr<ConcreteRule<ParserRule::MustInclude>> new_rule_must_include_unique(std::string_view t, size_t id);


	std::shared_ptr<ConcreteRule<ParserRule::MustIncludeAfter>> new_rule_must_include_after_shared(std::string_view t1, std::string_view t2);

	std::shared_ptr<ConcreteRule<ParserRule::MustIncludeBefore>> new_rule_must_include_before_shared(std::string_view t1, std::string_view t2);

	std::shared_ptr<ConcreteRule<ParserRule::CannotIncludeInFile>> new_rule_cannot_include_in_file_shared(std::string_view t);

	std::shared_ptr<ConcreteRule<ParserRule::MustIncludeInFile>> new_rule_must_include_in_file_shared(std::string_view t);

	std::shared_ptr<ConcreteRule<ParserRule::CannotInlcude>> new_rule_cannot_include_shared(std::string_view t, size_t id);

	std::shared_ptr<ConcreteRule<ParserRule::MustInclude>> new_rule_must_include_shared(std::string_view t, size_t id);






	//ConcreteRule<ParserRule::MustIncludeBetween> new_rule_must_include_between(std::string_view between1, std::string_view between2, std::string_view target,  size_t id);




}

#include "include/rule_operations.inl"
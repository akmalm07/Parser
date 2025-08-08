#pragma once

#include "config.h"

namespace parser
{
	enum class IdentifierType // TEST THIS OUT PLEASE
	{
		None = 0,
		Identify,
		Ignore,
		IgnoreUntilFound,
		IdentifyIfElse
	};

	struct BaseIdentifier
	{
		virtual ~BaseIdentifier() = default;
		virtual IdentifierType get_type() const = 0;
	};


	template<IdentifierType T>
	struct Identifier : public BaseIdentifier {};

	template<>
	struct Identifier<IdentifierType::Identify> : public BaseIdentifier
	{
		std::string target;
		size_t identifier = std::numeric_limits<size_t>::max();
		Identifier(std::string_view target, size_t identifier = std::numeric_limits<size_t>::max())
			: target(target), identifier(identifier)
		{
		}

		IdentifierType get_type() const override { return IdentifierType::Identify; }
	};

	template<>
	struct Identifier<IdentifierType::Ignore> : public BaseIdentifier
	{
		std::string target;
		size_t identifier = std::numeric_limits<size_t>::max();
		Identifier(std::string_view target, size_t identifier = std::numeric_limits<size_t>::max())
			: target(target), identifier(identifier) 
		{
		}

		IdentifierType get_type() const override { return IdentifierType::Ignore; }
	};

	template<>
	struct Identifier<IdentifierType::IgnoreUntilFound> : public BaseIdentifier
	{
		std::string target;
		std::string untilFound;
		size_t identifier = std::numeric_limits<size_t>::max();
		Identifier(std::string_view target, std::string untilFound, size_t identifier = std::numeric_limits<size_t>::max())
			: target(target), identifier(identifier) , untilFound(untilFound)
		{
		}

		IdentifierType get_type() const override { return IdentifierType::IgnoreUntilFound; }
	};

	template<>
	struct Identifier<IdentifierType::None> : public BaseIdentifier
	{
		Identifier() = default;
	};

	template<>
	struct Identifier<IdentifierType::IdentifyIfElse> : public BaseIdentifier
	{
		std::function<size_t(std::vector<std::string_view>)> condition;

		size_t traceBackTokens = 2; // How many tokens to look back for context
		size_t traceForwardTokens = 2; // How many tokens to look back for context

		Identifier(std::function<size_t(std::vector<std::string_view>)> condition)
			: condition(condition)
		{}

		IdentifierType get_type() const override { return IdentifierType::IdentifyIfElse; }
	};

}

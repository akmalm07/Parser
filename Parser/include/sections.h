#pragma once

#include "include/config.h"


namespace parser
{

	template<typename T>
	concept Charaters = std::same_as<T, char> || std::same_as<T, std::string> || std::same_as<T, const char*>;

	/*
	enum class SectionIdentifier
	{
		None = 0,
	};
	*/

	enum class ParserSectioning : uint32_t
	{
		//Creating Local spaces
		NewSectionWhenFound, // 1 param
		NewSectionWhenBetween, // 2 param
		NewSectionWhenAfter, // 2 param
		NewSectionWhenBefore, // 2 param

	};


	enum class HasIdentifier : bool
	{
		No = false,
		Yes = true
	};



	struct BaseSectioning
	{
	public:
		ParserSectioning type;

		BaseSectioning() = default;
		BaseSectioning(ParserSectioning type) : type(type) {}
		
		BaseSectioning(ParserSectioning type, size_t numOfParams) : type(type) 
		{
			if (type == ParserSectioning::NewSectionWhenFound && numOfParams != 1)
			{
				std::cerr << PARSER_LOG_ERR << "NewSectionWhenFound requires exactly one target." << std::endl;
			}
			else if ((type == ParserSectioning::NewSectionWhenBetween || type == ParserSectioning::NewSectionWhenAfter || type == ParserSectioning::NewSectionWhenBefore) && numOfParams != 2)
			{
				std::cerr << PARSER_LOG_ERR << "NewSectionWhenBetween, NewSectionWhenAfter, or NewSectionWhenBefore require exactly two targets." << std::endl;
			}
		}


		constexpr virtual size_t get_target_count() const = 0;
		constexpr virtual bool is_identifible() const = 0;
	
		constexpr virtual ~BaseSectioning() = default;
	};
	

	template<size_t N, HasIdentifier Identifier>
		requires (N > 0)
	struct Sectioning;


	template<size_t N>
		requires (N > 0)
	struct Sectioning<N, HasIdentifier::No> : public BaseSectioning
	{
	public:

		Sectioning(std::array<std::string, N> const& targets, ParserSectioning type = ParserSectioning::NewSectionWhenBetween)
			: targets(targets), BaseSectioning(type, N)
		{
		}

		std::array<std::string, N> targets;

		constexpr size_t get_target_count() const override { return N; }
		
		constexpr bool is_identifible() const override { return false; }
	};



	template<>
	struct Sectioning<1, HasIdentifier::No> : public BaseSectioning
	{
	public:
		std::string target;

		Sectioning(std::string_view target, ParserSectioning type = ParserSectioning::NewSectionWhenFound)
			: target(target), BaseSectioning(type, 1)
		{
		}

		constexpr size_t get_target_count() const override { return 1; }
	
		constexpr bool is_identifible() const override { return false; }
	};


	template<size_t N>
		requires (N > 0)
	struct Sectioning<N, HasIdentifier::Yes> : public BaseSectioning
	{
	public:

		Sectioning(std::array<std::string, N> const& targets, size_t identifier = 0, ParserSectioning type = ParserSectioning::NewSectionWhenFound)
			: targets(targets), integerIdentifier(identifier), BaseSectioning(type, N)
		{
		}

		std::array<std::string, N> targets;
		
		size_t integerIdentifier;

		constexpr size_t get_target_count() const override { return N; }

		constexpr bool is_identifible() const override { return true; }
	};


	template<>
	struct Sectioning<1, HasIdentifier::Yes> : public BaseSectioning
	{
	public:

		Sectioning(std::string_view target, size_t identifier = 0, ParserSectioning type = ParserSectioning::NewSectionWhenFound)
			: target(target), integerIdentifier(identifier), BaseSectioning(type, 1)
		{
		}

		std::string target;
		size_t integerIdentifier;

		constexpr size_t get_target_count() const override { return 1; }

		constexpr bool is_identifible() const override { return true; }
	};

	template<size_t N, Charaters T>
		requires (N > 0)
	Sectioning(std::array<T, N> const&,
		ParserSectioning) -> Sectioning<N, HasIdentifier::No>;
	template<size_t N, Charaters T>
		requires (N > 0)
	Sectioning(std::array<T, N> const&) -> Sectioning<N, HasIdentifier::No>;


	template<Charaters T>
	Sectioning(T const&, 
		ParserSectioning) -> Sectioning<1, HasIdentifier::No>;
	template<Charaters T>
	Sectioning(T const&) -> Sectioning<1, HasIdentifier::No>;

	Sectioning(std::string_view) -> Sectioning<1, HasIdentifier::No>;
	Sectioning(std::string_view,
		ParserSectioning) -> Sectioning<1, HasIdentifier::No>;
	

	/// <summary>
	/// 
	/// </summary>

	template<size_t N, Charaters T>
		requires (N > 0)
	Sectioning(std::array<T, N> const&, 
		size_t,
		ParserSectioning) -> Sectioning<N, HasIdentifier::Yes>;
	template<size_t N, Charaters T>
		requires (N > 0)
	Sectioning(std::array<T, N> const&, 
		size_t) -> Sectioning<N, HasIdentifier::Yes>;
	
	
	template<Charaters T>
	Sectioning(T const&,
		size_t,
		ParserSectioning) -> Sectioning<1, HasIdentifier::Yes>;
	template<Charaters T>
	Sectioning(T const&, 
		size_t) -> Sectioning<1, HasIdentifier::Yes>;


	Sectioning(std::string_view,
		size_t)->Sectioning<1, HasIdentifier::Yes>;
	Sectioning(std::string_view,
		size_t,
		ParserSectioning)->Sectioning<1, HasIdentifier::Yes>;




	using SectioningOneTarget = Sectioning<1, HasIdentifier::No>;
	using SectioningTwoTarget = Sectioning<2, HasIdentifier::No>;
	

	using SectioningOneTargetIdentifier = Sectioning<1, HasIdentifier::Yes>;
	using SectioningTwoTargetIdentifier = Sectioning<2, HasIdentifier::Yes>;

	template <size_t N>
	requires (N > 0)
	using SectioningIdentifier = Sectioning<N, HasIdentifier::Yes>;

	template <size_t N>
	requires (N > 0)
	using SectioningUnidentifier = Sectioning<N, HasIdentifier::No>;






	//Definitions

	template<HasIdentifier N>
	Sectioning<1, N> NewSectionWhenFoundSectioning(std::string_view target, std::optional<size_t> identifier)
	{
		if constexpr (N == HasIdentifier::No)
		{
			return Sectioning<1, HasIdentifier::No>(target, ParserSectioning::NewSectionWhenFound);
		}
		else
		{
			return Sectioning<1, HasIdentifier::Yes>(target, identifier.value(0), ParserSectioning::NewSectionWhenFound);
		}
	}

	template<HasIdentifier N>
	std::shared_ptr<Sectioning<1, N>> NewSectionWhenFoundSectioningShared(std::string_view target, std::optional<size_t> identifier = std::nullopt)
	{
		if constexpr (N == HasIdentifier::No)
		{
			return std::make_shared<Sectioning<1, HasIdentifier::No>>(target, ParserSectioning::NewSectionWhenFound);
		}
		else
		{
			return std::make_shared<Sectioning<1, HasIdentifier::Yes>>(target, identifier.value_or(0), ParserSectioning::NewSectionWhenFound);
		}
	}

	template<HasIdentifier N>
	std::shared_ptr<Sectioning<1, HasIdentifier::No>> NewSectionWhenFoundSectioningShared(std::string_view target, std::optional<size_t> identifier)
	{

		if constexpr (N == HasIdentifier::No)
		{
			return std::make_shared<Sectioning<1, HasIdentifier::No>>(target, ParserSectioning::NewSectionWhenFound);
		}
		else
		{
			return std::make_shared<Sectioning<1, HasIdentifier::Yes>>(target, identifier.value(), ParserSectioning::NewSectionWhenFound);
		}
	}

	template<HasIdentifier N>
	Sectioning<2, N> NewSectionWhenBetweenSectioning(std::string_view target1, std::string_view target2, std::optional<size_t> identifier)
	{
		if constexpr (N == HasIdentifier::No)
		{
			return Sectioning<2, HasIdentifier::No>({ target1, target2 }, ParserSectioning::NewSectionWhenBetween);
		}
		else
		{
			return Sectioning<2, HasIdentifier::Yes>({ target1, target2 }, identifier.value(0), ParserSectioning::NewSectionWhenBetween);
		}
	}

	template<HasIdentifier N>
	std::shared_ptr<Sectioning<2, N>> NewSectionWhenBetweenSectioningShared(std::string_view target1, std::string_view target2, std::optional<size_t> identifier)
	{
		if constexpr (N == HasIdentifier::No)
		{
			return std::make_shared<Sectioning<2, HasIdentifier::No>>({ target1, target2 }, ParserSectioning::NewSectionWhenBetween);
		}
		else
		{
			return std::make_shared<Sectioning<2, HasIdentifier::Yes>>({ target1, target2 }, identifier.value(), ParserSectioning::NewSectionWhenBetween);
		}
	}

	template<HasIdentifier N>
	Sectioning<2, N> NewSectionWhenFoundAfterSectioning(std::string_view target1, std::string_view target2, std::optional<size_t> identifier)
	{
		if constexpr (N == HasIdentifier::No)
		{
			return Sectioning<2, HasIdentifier::No>({ target1, target2 }, ParserSectioning::NewSectionWhenAfter);
		}
		else
		{
			return Sectioning<2, HasIdentifier::Yes>({ target1, target2 }, identifier.value(0), ParserSectioning::NewSectionWhenAfter);
		}
	}

	template<HasIdentifier N>
	std::shared_ptr<Sectioning<2, N>> NewSectionWhenFoundAfterSectioningShared(std::string_view target1, std::string_view target2, std::optional<size_t> identifier)
	{
		if constexpr (N == HasIdentifier::No)
		{
			return std::make_shared<Sectioning<2, HasIdentifier::No>>({ target1, target2 }, ParserSectioning::NewSectionWhenAfter);
		}
		else
		{
			return std::make_shared<Sectioning<2, HasIdentifier::Yes>>({ target1, target2 }, identifier.value(), ParserSectioning::NewSectionWhenAfter);
		}
	}

	template<HasIdentifier N>
	Sectioning<2, N> NewSectionWhenFoundBeforeSectioning(std::string_view target1, std::string_view target2, std::optional<size_t> identifier)
	{
		if constexpr (N == HasIdentifier::No)
		{
			return Sectioning<2, HasIdentifier::No>({ target1, target2 }, ParserSectioning::NewSectionWhenBefore);
		}
		else
		{
			return Sectioning<2, HasIdentifier::Yes>({ target1, target2 }, identifier.value(0), ParserSectioning::NewSectionWhenBefore);
		}
	}

	template<HasIdentifier N>
	std::shared_ptr<Sectioning<2, N>> NewSectionWhenFoundBeforeSectioningShared(std::string_view target1, std::string_view target2, std::optional<size_t> identifier)
	{
		if constexpr (N == HasIdentifier::No)
		{
			return std::make_shared<Sectioning<2, HasIdentifier::No>>({ target1, target2 }, ParserSectioning::NewSectionWhenBefore);
		}
		else
		{
			return std::make_shared<Sectioning<2, HasIdentifier::Yes>>({ target1, target2 }, identifier.value(), ParserSectioning::NewSectionWhenBefore);
		}
	}



}
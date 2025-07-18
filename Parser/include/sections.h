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
	struct Sectioning<N, HasIdentifier::No> : public BaseSectioning // TODO: add more target detectors!
	{
	public:

		Sectioning(std::array<std::string, N> const& targets, ParserSectioning type = ParserSectioning::NewSectionWhenBetween)
			: targets(targets), BaseSectioning(type, N)
		{
		}
		Sectioning(std::array<std::string_view, N> const& targets, ParserSectioning type = ParserSectioning::NewSectionWhenBetween)
			: BaseSectioning(type, N)
		{
			for (size_t i = 0; i < N; ++i)
			{
				this->targets[i] = std::string(targets[i]);
			}
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

		Sectioning(std::array<std::string, N> const& targets, size_t identifier = std::numeric_limits<size_t>::max(), ParserSectioning type = ParserSectioning::NewSectionWhenFound)
			: targets(targets), integerIdentifier(identifier), BaseSectioning(type, N)
		{
		}
		Sectioning(std::array<std::string_view, N> const& targets, size_t identifier = std::numeric_limits<size_t>::max(), ParserSectioning type = ParserSectioning::NewSectionWhenBetween)
			: integerIdentifier(identifier), BaseSectioning(type, N) {
			for (size_t i = 0; i < N; ++i)
			{
				this->targets[i] = std::string(targets[i]);
			}
		} 
		std::array<std::string, N> targets;
		
		size_t integerIdentifier = std::numeric_limits<size_t>::max();

		constexpr size_t get_target_count() const override { return N; }

		constexpr bool is_identifible() const override { return true; }
	};


	template<>
	struct Sectioning<1, HasIdentifier::Yes> : public BaseSectioning
	{
	public:

		Sectioning(std::string_view target, size_t identifier = std::numeric_limits<size_t>::max(), ParserSectioning type = ParserSectioning::NewSectionWhenFound)
			: target(target), integerIdentifier(identifier), BaseSectioning(type, 1)
		{
		}

		std::string target;
		size_t integerIdentifier = std::numeric_limits<size_t>::max();

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





	template<size_t Dim, HasIdentifier N>
	Sectioning<Dim, N> make_sectioning_value(std::array<std::string, Dim> const& targets,
		size_t identifier,
		ParserSectioning type)
	{
		using S = Sectioning<Dim, N>;

		if constexpr (Dim == 1)
		{
			if constexpr (N == HasIdentifier::Yes)
				return S(targets[0], identifier, type);
			else
				return S(targets[0], type);
		}
		else if constexpr (Dim == 2)
		{
			if constexpr (N == HasIdentifier::Yes)
				return S(targets, identifier, type);
			else
				return S(targets, type);
		}
	}

	// Unique pointer version
	template<size_t Dim, HasIdentifier N>
	std::unique_ptr<Sectioning<Dim, N>> make_sectioning_unique(std::array<std::string, Dim> const& targets,
		size_t identifier,
		ParserSectioning type)
	{
		using S = Sectioning<Dim, N>;

		if constexpr (Dim == 1)
		{
			if constexpr (N == HasIdentifier::Yes)
				return std::make_unique<S>(targets[0], identifier, type);
			else
				return std::make_unique<S>(targets[0], type);
		}
		else if constexpr (Dim == 2)
		{
			if constexpr (N == HasIdentifier::Yes)
				return std::make_unique<S>(targets, identifier, type);
			else
				return std::make_unique<S>(targets, type);
		}
	}

	// Shared pointer version
	template<size_t Dim, HasIdentifier N>
	std::shared_ptr<Sectioning<Dim, N>> make_sectioning_shared(std::array<std::string, Dim> const& targets,
		size_t identifier,
		ParserSectioning type)
	{
		using S = Sectioning<Dim, N>;

		if constexpr (Dim == 1)
		{
			if constexpr (N == HasIdentifier::Yes)
				return std::make_shared<S>(targets[0], identifier, type);
			else
				return std::make_shared<S>(targets[0], type);
		}
		else if constexpr (Dim == 2)
		{
			if constexpr (N == HasIdentifier::Yes)
				return std::make_shared<S>(targets, identifier, type);
			else
				return std::make_shared<S>(targets, type);
		}
	}



	Sectioning<1, HasIdentifier::Yes> new_section_when_found(std::string const& target, size_t id = std::numeric_limits<size_t>::max());


	std::shared_ptr<Sectioning<1, HasIdentifier::Yes>> new_section_when_found_shared(std::string const& target, size_t id = std::numeric_limits<size_t>::max());


	std::unique_ptr<Sectioning<1, HasIdentifier::Yes>> new_section_when_found_unique(std::string const& target, size_t id = std::numeric_limits<size_t>::max());


	Sectioning<2, HasIdentifier::Yes> new_section_when_between(std::string const& t1, std::string const& t2, size_t id = std::numeric_limits<size_t>::max());


	std::shared_ptr<Sectioning<2, HasIdentifier::Yes>> new_section_when_between_shared(std::string const& t1, std::string const& t2, size_t id = std::numeric_limits<size_t>::max());


	std::unique_ptr<Sectioning<2, HasIdentifier::Yes>> new_section_when_between_unique(std::string const& t1, std::string const& t2, size_t id = std::numeric_limits<size_t>::max());


	Sectioning<2, HasIdentifier::Yes> new_section_when_after(std::string const& t1, std::string const& t2, size_t id = std::numeric_limits<size_t>::max());


	std::shared_ptr<Sectioning<2, HasIdentifier::Yes>> new_section_when_after_shared(std::string const& t1, std::string const& t2, size_t id = std::numeric_limits<size_t>::max());



	std::unique_ptr<Sectioning<2, HasIdentifier::Yes>>  new_section_when_after_unique(std::string const& t1, std::string const& t2, size_t id = std::numeric_limits<size_t>::max());


	Sectioning<2, HasIdentifier::Yes> new_section_when_before(std::string const& t1, std::string const& t2, size_t id = std::numeric_limits<size_t>::max());


	std::shared_ptr<Sectioning<2, HasIdentifier::Yes>> new_section_when_before_shared(std::string const& t1, std::string const& t2, size_t id = std::numeric_limits<size_t>::max());


	std::unique_ptr<Sectioning<2, HasIdentifier::Yes>>  new_section_when_before_unique(std::string const& t1, std::string const& t2, size_t id = std::numeric_limits<size_t>::max());

}
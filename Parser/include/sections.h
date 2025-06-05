#pragma once

#include "include/config.h"


namespace parser
{
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

		constexpr virtual size_t get_target_count() const = 0;
		
		constexpr virtual bool is_identifible() const = 0;
	};
	

	template<size_t N, bool Identifier>
		requires (N > 0)
	struct Sectioning;


	template<size_t N>
		requires (N > 0)
	struct Sectioning<N, false> : public BaseSectioning
	{
	public:

		std::array<std::string, N> targets;

		constexpr size_t get_target_count() const override { return N; }
		
		constexpr bool is_identifible() const override { return false; }
	};



	template<>
	struct Sectioning<1, false> : public BaseSectioning
	{
	public:
		std::string target;

		constexpr size_t get_target_count() const override { return 1; }
	
		constexpr bool is_identifible() const override { return false; }
	};


	template<size_t N>
		requires (N > 0)
	struct Sectioning<N, true> : public BaseSectioning
	{
	public:

		std::array<std::string, N> targets;
		
		size_t integerIdentifier;

		constexpr size_t get_target_count() const override { return N; }

		constexpr bool is_identifible() const override { return true; }
	};


	template<>
	struct Sectioning<1, true> : public BaseSectioning
	{
	public:
		std::string target;
		size_t integerIdentifier;

		constexpr size_t get_target_count() const override { return 1; }

		constexpr bool is_identifible() const override { return true; }
	};


	using SectioningOneTarget = Sectioning<1, false>;
	using SectioningTwoTarget = Sectioning<2, false>;
	

	using SectioningOneTargetIdentifier = Sectioning<1, true>;
	using SectioningTwoTargetIdentifier = Sectioning<2, true>;

	template <size_t N>
	requires (N > 0)
	using SectioningIdentifier = Sectioning<N, true>;

	template <size_t N>
	requires (N > 0)
	using SectioningUnidentifier = Sectioning<N, false>;



}
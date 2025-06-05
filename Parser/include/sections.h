#pragma once

#include "include/config.h"


namespace parser
{

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

		virtual size_t get_target_count() const = 0;
	};

	template<size_t N>
		requires (N > 0)
	struct Sectioning : public BaseSectioning
	{
	public:

		std::array<std::string, N> targets;

		size_t get_target_count() const override { return N; }
	};

	template<>
	struct Sectioning<1> : BaseSectioning
	{
	public:

		std::string target;
		size_t get_target_count() const override { return 1; }
	};

	using SectioningOneTarget = Sectioning<1>;
	using SectioningTwoTarget = Sectioning<2>;

}
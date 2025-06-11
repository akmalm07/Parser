#pragma once


#include "include/config.h"


namespace parser
{

	enum class ParserRule : uint32_t
	{
		//Global
		CannotIncludeInFile,
		MustIncludeInFile,

		//Local (Section Oriented)
		CannotInlcude,
		MustInclude,

		//Local (Section Oriented) with two targets
		MustIncludeBefore,
		MustIncludeAfter,
		MustIncludeBetween,
	};


	struct BaseRuling
	{
	public:
		ParserRule type;
		std::string errMsg;

		virtual size_t get_target_count() const = 0;
	};

	template<size_t N>
		requires (N > 0)
	struct Ruling : public BaseRuling
	{
	public:

		std::array<std::string, N> targets; // This is a fixed size array of strings that contains the targets of the rule. The size of the array is determined by the template parameter N.

		size_t get_target_count() const override { return N; }

		Ruling(ParserRule r, std::array<std::string, N> const& t, std::string_view e)
		{
			type = r;
			targets = t;
			errMsg = e;
		}

		~Ruling() = default;

	};

	template<>
	struct Ruling<1> : BaseRuling
	{
	public:
		std::string target;

		size_t get_target_count() const override { return 1; }

		Ruling(ParserRule r, std::string_view t, std::string_view e)
		{
			type = r;
			target = t;
			errMsg = e;
		}

		~Ruling() = default;
	};

	using RulingOneTarget = Ruling<1>;
	using RulingTwoTarget = Ruling<2>;

	template<size_t N>
	using RulingMultiTarget = Ruling<N>;

}
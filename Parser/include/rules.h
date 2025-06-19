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

	enum class HasRegex : bool
	{
		No = false,
		Yes = true
	};


	struct BaseRuling
	{
	public:
		ParserRule type;
		std::string errMsg;

		BaseRuling(ParserRule type, std::string_view errMsg)
			: type(type), errMsg(errMsg) {}

		virtual bool is_regex_based() const = 0;

		virtual size_t get_target_count() const = 0;
	};


	template<size_t N, HasRegex R>
		requires (N > 0)
	struct Ruling;



	template<size_t N>
		requires (N > 0)
	struct RulingMultiTarget : public BaseRuling
	{
	public:
		RulingMultiTarget(ParserRule type, std::string_view errMsg)
			: BaseRuling(type, errMsg) {}

		virtual ~RulingMultiTarget() = default;

		virtual bool is_regex_based() const override = 0;

		size_t get_target_count() const override final
		{
			return N;
		}
	};

	template<size_t N>
		requires (N > 0)
	struct Ruling<N, HasRegex::No> : public RulingMultiTarget<N>
	{
	public:

		std::array<std::string, N> targets; // This is a fixed size array of strings that contains the targets of the rule. The size of the array is determined by the template parameter N.

		Ruling(ParserRule r, std::array<std::string, N> const& t, std::string_view e)
			: RulingMultiTarget<N>(r, e), targets(t)
		{}
		 

		bool is_regex_based() const override
		{
			return false;
		}

		~Ruling() = default;

	};

	template<size_t N>
		requires (N > 0)
	struct Ruling<N, HasRegex::Yes> : public RulingMultiTarget<N>
	{
	public:

		struct
		{
			std::array<std::string, N> patterns;
			std::array<std::regex, N> targets;
		}reg;


		Ruling(ParserRule r, std::array<std::string, N> const& t, std::string_view e)
			: RulingMultiTarget<N>(r, e)
		{
			for (size_t i = 0; i < N; ++i)
			{
				reg.patterns[i] = t[i];
				try
				{
					reg.targets[i] = std::regex(t[i]);
				}
				catch (const std::regex_error& e)
				{
					std::cerr << PARSER_LOG_ERR << "Invalid regex pattern: " << t[i] << ". Error: " << e.what() << std::endl;
					throw;
				}
			}
		}

		bool is_regex_based() const override
		{
			return true;
		}

		~Ruling() = default;

	};

	
	struct RulingOneTarget : public BaseRuling
	{
	public:

		RulingOneTarget(ParserRule type, std::string_view errMsg)
			: BaseRuling(type, errMsg)
		{}

		virtual ~RulingOneTarget() = default;

		virtual bool is_regex_based() const override = 0;

		size_t get_target_count() const override final
		{
			return 1;
		}
	};


	template<>
	struct Ruling<1, HasRegex::No> : public RulingOneTarget
	{
	public:
		std::string target;

		Ruling(ParserRule r, std::string_view t, std::string_view e)
			: RulingOneTarget(r, e), target(t)
		{
		}

		bool is_regex_based() const override
		{
			return false;
		}

		~Ruling() = default;
	};



	template<>
	struct Ruling<1, HasRegex::Yes> : public RulingOneTarget
	{
	public:
		struct
		{
			std::string pattern;
			std::regex target;
		}reg;

		Ruling(ParserRule r, std::string const& t, std::string_view e)
			: RulingOneTarget(r, e)
		{
			reg.pattern = t;
			try
			{
				reg.target = std::regex(t);
			}
			catch (const std::regex_error& e)
			{
				std::cerr << PARSER_LOG_ERR << "Invalid regex pattern: " << t << ". Error: " << e.what() << std::endl;
				throw;
			}
		}


		bool is_regex_based() const override
		{
			return true;
		}

		~Ruling() = default;
	};

	using RulingTwoTarget = RulingMultiTarget<2>;

}
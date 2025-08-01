#pragma once


#include "config.h"
#include "compact_map.h"


namespace parser
{
	enum class LexerType
	{
		None = 0,
		ChangeInto,
		Ignore,
	};

	class LexerInput
	{
	public:
		virtual ~LexerInput() = default;

		virtual std::compact_map<> execute(EntireUntokenizedFile& code, size_t identification) = 0;
	};


	template<LexerType T>
	class LexerInputConcrete : public LexerInput {};

	template<>
	class LexerInputConcrete<LexerType::ChangeInto> : public LexerInput
	{
	public:

		LexerInputConcrete(std::string_view target, std::string_view changeInto, size_t identifier)
			: target(target), changeInto(changeInto), identifier(identifier)
		{
		}

		execute(EntireUntokenizedFile& code, size_t identification) override
		{

			size_t index = 0;

			for (size_t i = 0; i < code.size(); ++i)
			{
				if (index == target.size() && code.substr(i, target.size()) == target)
				{
					code.replace(i, target.size(), changeInto);
					i += changeInto.size() - 1;
					index = 0; 
					continue;
				}

				index++;
			}
		}



	private:
		std::string_view target;
		std::string_view changeInto;
		size_t identifier = std::numeric_limits<size_t>::max();
	};

} // namespace parser
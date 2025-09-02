#include "headers.h"
#include "include/regex.h"

namespace parser
{
	
	struct ContainerVecs
	{
		std::vector<Container*> containers;

		struct
		{
			Container* ptr = nullptr;
			bool lastVal = false;

			void reset()
			{
				if (ptr)
				{
					if (lastVal == false)
					{
						lastVal = true;
					}
					else
					{
						ptr = nullptr;
						lastVal = false;
					}
				}
			}
		}peren, brack, curlBrak;
	

		void replace_back_pointer()
		{
			if (containers.empty())
				return;
			Container* temp = nullptr;
			switch (containers.back()->type())
			{
			case RegType::Parentheses:
				temp = peren.ptr;
			case RegType::Bracket:
				temp = brack.ptr;
			case RegType::CurlyBracket:
				temp = curlBrak.ptr;
			}

			temp = static_cast<ParenContainer*>(containers.back());
			containers.pop_back();
		}

		bool check_if_proper_closing(char c) const
		{
			if (containers.empty())
			{
				return false;
			}

			switch (containers.back()->type())
			{
				case RegType::Parentheses:
					return c == ')' ? true : false;
				case RegType::Bracket:
					return c == ']' ? true : false;
				case RegType::CurlyBracket:
					return c == '}' ? true : false;
				default:
					return false;
			}
		}

		void reset()
		{
			peren.reset();
			brack.reset();
			curlBrak.reset();
		}
	};


	std::unique_ptr<BasePart> valid_reg_operator(bool& wasLastOperator, std::vector<std::unique_ptr<BasePart>>& parts, std::string_view pattern, size_t const index)
	{
		switch (pattern[index])
		{
			case '*':
			{
				wasLastOperator = false;
				auto lastPart = std::move(parts.back());
				parts.pop_back();

				return std::make_unique<StarOp>(lastPart);
			}
			case '+':
			{
				wasLastOperator = false;
				auto lastPart = std::move(parts.back());
				parts.pop_back();

				return std::make_unique<PlusOp>(lastPart);
			}
			case '?':
			{
				wasLastOperator = false;
				auto lastPart = std::move(parts.back());
				parts.pop_back();

				return std::make_unique<OneOrZeroOp>(lastPart);
			}
			case '|':
			{
				wasLastOperator = true;
				auto lastPart = std::move(parts.back());
				parts.pop_back();

				return std::make_unique<OrOp>(lastPart);
			}
			case '-':
				wasLastOperator = true;
				auto lastPart = std::move(parts.back());
				parts.pop_back();

				return std::make_unique<DashOp>(lastPart);
		}
	} 

	std::unique_ptr<BasePart> valid_reg_placeholders(std::string_view pattern, size_t index)
	{
		char c = pattern[index];

		if (pattern[index] == '.')
		{
			return std::make_unique<DotPH>();
		}
		else if (pattern[index - 1] != '\\')
		{
			return nullptr;
		}

		switch (c)
		{
		case 'd':
			index++;
			return std::make_unique<NumberPH>();
			break;
		case 'D':
			index++;
			return std::make_unique<NotNumberPH>();
		case 'w':
			index++;
			return std::make_unique<WordPH>();
		case 'W':
			index++;
			return std::make_unique<NotWordPH>();
		case 's':
			index++;
			return std::make_unique<WhitespacePH>();
		case 'S':
			index++;
			return std::make_unique<NotWhitespacePH>();
		default:
			return nullptr;
		}
	}

	bool is_container_close(char c)
	{
		return c == ')' || c == ']' || c == '}';
	}

	char* container_open(char c)
	{
		return (c == '(' || c == '[' || c == '{' ? &c : nullptr);
	}

	template<typename T>
	std::expected<void, ErrCode> regex_parser_assistent(
		std::vector<std::unique_ptr<BasePart>>& parts,
		ContainerVecs& current,
		bool& wasLastOperator,
		std::string_view pattern,
		size_t index)
	{

	}


	std::expected<std::vector<std::unique_ptr<BasePart>>, ErrCode> regex_parser(const std::string& pattern)
	{
		std::vector<std::unique_ptr<BasePart>> parts;

		ContainerVecs current;	

		bool wasLastOperator = false;	

		for (size_t i = 0; i < pattern.size(); i++)
		{
			current.reset();

			std::unique_ptr<BasePart> part;

			if ((pattern[i] >= 'A' && pattern[i] <= 'Z') || (pattern[i] >= 'a' && pattern[i] <= 'z'))
			{
				part = std::make_unique<CharacterPart>(pattern[i]);
			}
			else if (pattern[i] >= '0' && pattern[i] <= '9')
			{
				size_t start = i;
				while (i < pattern.size() && pattern[i] >= '0' && pattern[i] <= '9')
				{
					i++;
				}
				i--;
				part = std::make_unique<NumberPart>(pattern.substr(start, i - start + 1));
			}
		
			char* containerOpen = container_open(pattern[i]);

			if (containerOpen)
			{
	
				switch (*containerOpen)
				{
				case '(':
					parts.push_back(std::make_unique<ParenContainer>());
					current.containers.push_back(static_cast<ParenContainer*>(parts.back().get()));
					break;
				case '[':
					parts.push_back(std::make_unique<BracketContainer>());
					current.containers.push_back(static_cast<BracketContainer*>(parts.back().get()));
					break;
				case '{':
					parts.push_back(std::make_unique<CurlyBracketContainer>());
					current.containers.push_back(static_cast<CurlyBracketContainer*>(parts.back().get()));
					break;
				}
				continue;
			}
			else if (is_container_close(pattern[i]))
			{
				if (current.check_if_proper_closing(pattern[i]))
				{
					current.replace_back_pointer();
				}
				else
					return std::unexpected(ErrCode::UnmatchedClosingParenthesis);
				continue;
			}

			auto insert = [&wasLastOperator, &part,&parts, &pattern, &i]
			(BasePart* lastOperatorRetrivedVal, std::function<void(std::unique_ptr<BasePart>&)> insertionMethod) {
				if (wasLastOperator)
				{
					auto item = lastOperatorRetrivedVal;
					switch (item->type())
					{
					case RegType::Or:
						static_cast<OrOp*>(item)->add_branch(part);
						wasLastOperator = false;
						break;
					case RegType::Dash:
						static_cast<DashOp*>(item)->set_right(part);
						wasLastOperator = false;
						break;
					}
					return;
				}
				if (!part)
				{
					if (auto placeholder = valid_reg_placeholders(pattern, i))
					{
						insertionMethod(placeholder);
					}
					else if (auto oper = valid_reg_operator(wasLastOperator, parts, pattern, i))
					{
						insertionMethod(oper);
					}
				}
				else
					insertionMethod(part);
			};

			if (current.containers.size() > 0)
			{
				insert(current.containers.back()->part_back_ptr(),
					[&current](std::unique_ptr<BasePart>& t) { current.containers.back()->add_part(t); });
			}
			else
			{
				insert(current.containers.back()->part_back_ptr(),
					[&parts](std::unique_ptr<BasePart>& t) { parts.push_back(t); });

			}
		}

		if (current.containers.size() > 0)
		{
			return std::unexpected(ErrCode::UnmatchedOpeningParenthesis);
		}

		return parts;
	}

}
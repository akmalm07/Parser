#include "headers.h"
#include "include/regex.h"

namespace parser
{

	std::optional<uint16_t> to_uint16(std::string_view s)
	{
		uint16_t value{};
		auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);

		if (ec != std::errc{} || ptr != s.data() + s.size())
			return std::nullopt;

		return value;
	}

	std::string Regex::compile(std::string_view givenPattern)
	{
		_parts.clear();

		std::string_view pattern;
		bool pushEndOfString = false;

		if (givenPattern.size() < 1)
			return "";
		
		// Trimming the starting ^ and ending $ if they exist and diagnosing 
		{
			size_t start = 0;
			size_t end = givenPattern.size();

			if (givenPattern.front() == '^') 
			{
				_parts.push_back(std::make_unique<StartOfString>());
				start++;
			}

			if (end > start && givenPattern.back() == '$')
			{
				_parts.push_back(std::make_unique<EndOfString>());
				end--;
			}

			pattern = givenPattern.substr(start, end - start);

		}


		std::vector<std::unique_ptr<ParenContainer>> perens;
		std::unique_ptr<BracketContainer> brak;
		std::unique_ptr<OrOp> orOp;
		struct {
			std::vector<ParenContainer*> list;
			std::vector<ParenContainer*> tempList; // This is to first process the inner nodes of the perens before the outer ones, so that the inner nodes can be added to the outer nodes when they are processed
		} captureGroups;

		auto emit_item = [&](std::unique_ptr<BasePart> item, int& i, int incVal = 0) {


			if (perens.size() != 0)
			{
				
				if (orOp && (perens.size() == 1 || perens[perens.size() - 2]->get_back()->type() != RegType::PerenToAddHere))
				{
					//assert(basePtr->type() == RegType::Or);
					//perens.back()->pop_back(); // this is deleted pointer!
					orOp->set_right(std::move(item));
					perens.back()->add_part(std::move(orOp));
					i += incVal;
					return;
				}
				perens.back()->add_part(std::move(item));
			}
			else if (orOp)
			{
				orOp->set_right(std::move(item));
				_parts.push_back(std::move(orOp));
				orOp.reset();				
			}
			else
				_parts.push_back(std::move(item));
			i += incVal;
		};

		auto emit_qual_item = [&](std::unique_ptr<RepeatPartsBase> item, int& i) {

			if (perens.size() != 0)
			{
				//charactersSinceLastPush += item->charater_length();
				if (perens.back()->size() != 0)
					item->add(std::move(perens.back()->pop_back_and_transfer()));
				else
				{
					perens.back()->add_part(std::make_unique<CharacterPart>('*'));
					return;
				}
				if (orOp)
				{
					orOp->set_right(std::move(item));
					perens.back()->add_part(std::move(orOp));
					orOp.reset();
					return;
				}
				perens.back()->add_part(std::move(item));
			}
			else
			{
				if (_parts.size() == 0)
					_parts.push_back(std::make_unique<CharacterPart>('*')); // Defaulting value for quantifier
				
				item->add(std::move(_parts.back()));
				_parts.pop_back(); 
				if (orOp)
				{
					orOp->set_right(std::move(item));
					_parts.push_back(std::move(orOp));
					orOp.reset();
					return;
				}
				_parts.push_back(std::move(item));
			}
			};

		auto emit_backreference_item = [&](uint16_t num, bool explicitCall, int & i, int incVal = 0) -> bool {
			if (num > captureGroups.list.size())
			{
				if (explicitCall)
				{
					emit_item(std::make_unique<CharacterPart>('<', true), i);
					std::string s = std::to_string(num);
					for (char c : s)
						emit_item(std::make_unique<CharacterPart>(c), i);
					emit_item(std::make_unique<CharacterPart>('>'), i);
				}
				else
					emit_item(std::make_unique<CharacterPart>(std::to_string(num)[0], true), i);
				i += incVal;
				return false;
			}
			ParenContainer& ref = *captureGroups.list[num - 1];

			_parts.push_back(std::make_unique<Backreference>(ref, num));
			
			i += incVal;
			return true;
		};

		for (int i = 0; i < pattern.size(); i++)
		{
			switch (pattern[i])
			{
			case '.':
				emit_item(std::make_unique<DotPH>(), i);
				continue;
			case '*':
				emit_qual_item(std::make_unique<StarOp>(), i);
				continue;
			case '+':
				emit_qual_item(std::make_unique<PlusOp>(), i);
				continue;
			case '?':
				emit_qual_item(std::make_unique<OneOrZeroOp>(), i);
				continue;
			case '|':
			{
				if (perens.size() != 0)
				{
					// If inside parentheses, attach to the last part inside the parens
					if (perens.back()->size() == 0)
					{
						emit_item(std::make_unique<CharacterPart>(pattern[i]), i);
						continue;
					}
					auto ptr = perens.back()->pop_back_and_transfer();
					orOp = std::make_unique<OrOp>(std::move(ptr));
				}
				else
				{
					// Otherwise attach to the last item in _parts
					orOp = std::make_unique<OrOp>(std::move(_parts.back()));
					_parts.pop_back();
				}

				continue;
			}
			case '{':
			{
				std::string number1, number2;
				const size_t prevI = i;
				i++;
				while (i < pattern.size() && pattern[i] >= '0' && pattern[i] <= '9')
				{
					number1 += pattern[i];
					i++;
				}

				if (prevI == i && pattern[i] != ',')
				{
					emit_item(std::make_unique<CharacterPart>(pattern[i]), i);
					continue;
				}
				else if (prevI != i && pattern[i] == '}')
				{
					if (auto num = to_uint16(number1))
					{
						emit_qual_item(std::make_unique<RepeatNumberedTimes>(num.value()), i);
						continue;
					}
					for (int j = i - prevI; j <= i; j++)
						emit_item(std::make_unique<CharacterPart>(pattern[prevI + j]), i);
					continue;
				}
				else if (pattern[i] == ',')
				{
					i++;
					while (i < pattern.size() && pattern[i] >= '0' && pattern[i] <= '9')
					{
						number2 += pattern[i];
						i++;
					}
					if (i < pattern.size() && pattern[i] == '}')
					{
						std::optional<uint16_t> min = to_uint16(number1);
						std::optional<uint16_t> max = to_uint16(number2);
						if (min || max)
						{
							emit_qual_item(std::make_unique<RepeatRangedTimes>(min, max), i);
							continue;
						}
						for (int j = prevI; j <= i; j++)
							emit_item(std::make_unique<CharacterPart>(pattern[j]), i);
						continue;
					}
				}
				continue;
			}

			case '[':
			{
				NegetiveFlag flag = POSITIVE;
				const size_t prevI = i;

				if (i + 1 < pattern.size() && pattern[i + 1] == '^') 
				{
					flag = NEGETIVE;
					i++;
				}

				brak = std::make_unique<BracketContainer>(flag);

				for (i = i + 1; i < pattern.size(); i++)
				{
					if (pattern[i] == ']')
					{
						emit_item(std::move(brak), i);
						brak.reset();
						break;
					}

					if (i + 1 == pattern.size())
						break;

					if (pattern[i] == '\\')
					{
						brak->add_char(pattern[i + 1], true);
						i++;
						continue;
					}

					if (pattern[i] != '-' && pattern[i + 1] == '-')
					{
						if (i + 2 < pattern.size())
						{
							brak->add_dash(pattern[i], pattern[i + 2]);
							i += 2;
							continue;
						}
					}

					brak->add_char(pattern[i]);
				}

				if (brak)
				{
					size_t brakLen = i - prevI;
					std::vector<std::unique_ptr<CharacterPart>> charPartsVec;
					charPartsVec.reserve(brakLen);

					for (size_t j = prevI + 1; j < i; j++) 
						charPartsVec.push_back(std::make_unique<CharacterPart>(pattern[j]));


					_parts.insert(_parts.end(),
						std::make_move_iterator(charPartsVec.begin()),
						std::make_move_iterator(charPartsVec.end())
					);

					brak.reset();
				}
				continue;
			}
			
			case '\\':
			{
				if (i + 1 >= pattern.size()) break;
				else if (pattern[i + 1] > '0' && pattern[i + 1] <= '9')
				{
					// Backreferences
					emit_backreference_item(to_uint16(std::string_view(&pattern[i + 1], 1)).value(), false, i, 1);
					continue;
				}
				else if (pattern[i + 1] == '<')
				{
					std::string numStr = "";
					bool successFlag = true;
					
					int j = i + 2;
					for (; pattern[j] != '>'; j++)
					{
						if (j >= pattern.size()) 
						{
							successFlag = false;
							break;
						}
						else if (pattern[j] >= '0' && pattern[j] <= '9')
							numStr += pattern[j];
						else
						{
							successFlag = false;
							break;
						}
					}
					if (successFlag && !numStr.empty())
					{
						i = j;
						emit_backreference_item(to_uint16(numStr).value(), true, i);
						continue;
					}
				}
				switch (pattern[i + 1])
				{
				case 'w':
					emit_item(std::make_unique<WordPH>(), i, 1);
					continue;
				case 'W':
					emit_item(std::make_unique<NotWordPH>(), i, 1);
					continue;
				case 's':
					emit_item(std::make_unique<WhitespacePH>(), i, 1);
					continue;
				case 'S':
					emit_item(std::make_unique<NotWhitespacePH>(), i, 1);
					continue;
				case 'd':
					emit_item(std::make_unique<NumberPH>(), i, 1);
					continue;
				case 'D':
					emit_item(std::make_unique<NotNumberPH>(), i, 1);
					continue;
				case 'b':
					emit_item(std::make_unique<WordBoundary>(), i, 1);
					continue;
				case 'B':
					emit_item(std::make_unique<NonWordBoundary>(), i, 1);
					continue;
				default:
					emit_item(std::make_unique<CharacterPart>(pattern[i + 1], true), i, 1);
					continue;
				}
			}
			case '(':
			{
				if (perens.size() == 0)
					_parts.push_back(std::make_unique<PerenMarker>(i));
				else
					perens.back()->add_part(std::make_unique<PerenMarker>(i));
				perens.push_back(std::make_unique<ParenContainer>());

				break;
			}
			case ')':
			{
				if (perens.size() == 0)
				{
					emit_item(std::make_unique<CharacterPart>(pattern[i]), i);
					continue;
				}	// Make so that it identified the sequace and jsut makes each one into a charater
				if (perens.size() == 1)
				{
					_parts.pop_back(); // Remove the PerenMarker

					auto ptr = std::move(perens.back());
					perens.pop_back();

					captureGroups.list.push_back(ptr.get());
					if (captureGroups.tempList.size() != 0)
					{
						for (auto& item : std::views::reverse(captureGroups.tempList))
							captureGroups.list.push_back(item);
						captureGroups.tempList.clear();
					}
					
					emit_item(std::move(ptr), i);
					continue;
				}
				auto ptr = std::move(perens.back());

				captureGroups.tempList.push_back(ptr.get());

				perens.pop_back();
				perens[perens.size() - 1]->pop_back(); // Remove the PerenMarker
				perens[perens.size() - 1]->add_part(std::move(ptr));

				continue;
			}
			default:
				emit_item(std::make_unique<CharacterPart>(pattern[i]), i);
			}
		}

		if (_parts.back()->type() != RegType::PerenToAddHere)
		{
			if (pushEndOfString)
				_parts.push_back(std::make_unique<EndOfString>());
			return print_regex_compilation();
		}

		// Replace the parentheses sequnace as literals because the closing parenthesis was not provided
		const size_t prevI = static_cast<PerenMarker*>(_parts.back().get())->get_prev_index();
		_parts.pop_back(); // Remove the PerenMarker

		for (size_t i = prevI + 1; i < pattern.size(); i++)
			_parts.push_back(std::make_unique<CharacterPart>(pattern[i]));

		return print_regex_compilation();
	}


	bool Regex::execute(std::string_view test)
	{
		if (_parts.size() == 0)
			return false;
		if (_parts[0]->type() == RegType::StartOfString)
		{
			int idx = 0;
			for (const auto& part : _parts)
			{
				if (!part->execute(test, idx))
					return false;
			}
		}
	}

	std::string Regex::print_regex_compilation() const
	{
		std::string output;		
		for (const auto& part : _parts)
		{
			output += part->print() + " ";
		}
		return output;
	}





	/*
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
	*/
}

#include "headers.h"
#include "include/regex_ast_parts.h"


namespace parser {

	size_t digits_in_number(unsigned int number)
	{
		size_t digits = 0;
		while (number != 0)
		{
			number /= 10;
			digits++;
		}
		return digits;
	}


	size_t RepeatRangedTimes::charater_length() const
	{
		return 3 + (_min ? digits_in_number(_min.value()) : 0) + 3 + (_max ? digits_in_number(_max.value()) : 0);
	}

	std::string NotNumberPH::print() const
	{
		return "NotNum";
	}

	bool NotNumberPH::execute(std::string_view substr, size_t& i) 
	{
		if (substr[i] < '0' || substr[i] > '9')
		{
			_matchedString = substr[i];
			i += 1;
			return true;
		}
		return false;
	}

	std::string NumberPH::print() const
	{
		return "Num";
	}

	bool NumberPH::execute(std::string_view substr, size_t& i) 
	{
		if (substr[i] >= '0' && substr[i] <= '9')
		{
			_matchedString = substr[i];
			i += 1;
			return true;
		}
		return false;
	}

	std::string WordPH::print() const
	{
		return "Word";
	}

	bool WordPH::execute(std::string_view substr, size_t& i) 
	{
		if ((substr[i] >= 'A' && substr[i] <= 'Z') ||
			(substr[i] >= 'a' && substr[i] <= 'z') ||
			(substr[i] >= '0' && substr[i] <= '9') ||
			(substr[i] == '_'))
		{
			_matchedString = substr[i];
			i += 1;
			return true;
		}
		return false;
	}

	std::string NotWordPH::print() const
	{
		return "NotNum";
	}

	bool NotWordPH::execute(std::string_view substr, size_t& i) 
	{
		if (!((substr[i] >= 'A' && substr[i] <= 'Z') ||
			(substr[i] >= 'a' && substr[i] <= 'z') ||
			(substr[i] >= '0' && substr[i] <= '9') ||
			(substr[i] == '_')))
		{
			_matchedString = substr[i];
			i += 1;
			return true;
		}
		return false;
	}

	std::string WhitespacePH::print() const
	{
		return "WhiteSpace";
	}

	bool WhitespacePH::execute(std::string_view substr, size_t& i) 
	{
		if (substr[i] == ' ' || substr[i] == '\t' || substr[i] == '\n' || substr[i] == '\r' || substr[i] == '\v' || substr[i] == '\f')
		{
			_matchedString = substr[i];
			i += 1;
			return true;
		}
		return false;
	}

	std::string NotWhitespacePH::print() const
	{
		return "NotWhiteSpace";
	}

	bool NotWhitespacePH::execute(std::string_view substr, size_t& i) 
	{
		if (!(substr[i] == ' ' || substr[i] == '\t' || substr[i] == '\n' || substr[i] == '\r' || substr[i] == '\v' || substr[i] == '\f'))
		{
			_matchedString = substr[i];
			i += 1;
			return true;
		}
		return false;
	}

	std::string DotPH::print() const
	{
		return "AnyCharater";
	}

	bool DotPH::execute(std::string_view substr, size_t& i) 
	{
		if (substr.size() > 0)
		{
			_matchedString = substr[i];
			i += 1;
			return true;
		}
		return false;
	}

	void ParenContainer::add_part(std::unique_ptr<BasePart> part)
	{
		_parts.push_back(std::move(part));
	}

	size_t ParenContainer::size() const
	{
		return _parts.size();
	}

	std::unique_ptr<BasePart> ParenContainer::pop_back_and_transfer()
	{
		auto ptr = std::move(_parts.back());
		_parts.pop_back();
		return ptr;
	}

	std::unique_ptr<BasePart>& ParenContainer::get_back()
	{
		return _parts.back();
	}

	std::string ParenContainer::print() const
	{
		if (_parts.size() == 1)
			return std::format("Cont{{ {} }}", _parts[0]->print());
		std::string str = "Cont{ ";
		int i = 0;
		for (const auto& part : _parts)
		{
			str += part->print();
			i++;
			if (i < _parts.size())
				str += ", ";
		}
		return str + " }";
	}

	void ParenContainer::pop_back()
	{
		_parts.pop_back();
	}

	void ParenContainer::replace_back_part(std::unique_ptr<BasePart> part)
	{
		_parts.back() = std::move(part);
	}

	// Homework, add the get matched case function, which is used for backreferences, and it should return the string that was matched by the parentheses container, so that it can be used for backreferences

	bool ParenContainer::execute(std::string_view substr, size_t& i) 
	{
		size_t localIndex = 0;
		for (const auto& part : _parts)
		{
			if (!part->execute(substr.substr(localIndex), localIndex))
			{
				_matchedString = "";
				return false;
			}
			_matchedString += part->get_matched_string();
		}
		i += localIndex;
		return true;
	}

	size_t ParenContainer::charater_length() const
	{
		size_t length = 2;
		for (const auto& part : _parts)
			length += part->charater_length();
		return length;
	}

	std::string BracketContainer::print() const
	{
		std::string str;
		if (_not == POSITIVE)
			str = "Class { ";
		else
			str = "NotClass { ";
		int i = 0;
		for (const auto& part : _parts)
		{
			str += part->print();
			i++;
			if (i < _parts.size())
				str += ", ";
		}
		return str + " }";
	}

	bool BracketContainer::execute(std::string_view substr, size_t& i) 
	{
		size_t localIndex = i;
		bool matched = false;
		for (const auto& part : _parts)
		{
			int before = localIndex;
			if (part->execute(substr, localIndex))
			{
				matched = true;
				break;
			}
			localIndex = before;
		}
		if (_not == NEGETIVE)
		{
			if (matched)
				return false;
			else
			{
				_matchedString = std::string(1, substr[i]);
				i += 1;
				return true;
			}
		}

		if (matched)
		{
			_matchedString = substr.substr(i, localIndex - i);
			i = localIndex;
			return true;
		}
		else
			return false;
	}

	size_t BracketContainer::charater_length() const
	{
		size_t length = (_not == NEGETIVE ? 3 : 2);
		for (const auto& part : _parts)
			length += part->charater_length();
		return length;
	}

	void BracketContainer::add_char(char part, bool usesBackslash)
	{
		_parts.push_back(std::make_unique<CharacterPart>(part, usesBackslash));
	}

	void BracketContainer::add_dash(char left, char right)
	{
		if (left > right)
			std::swap(left, right);
		_parts.push_back(std::make_unique<DashOp>(left, right));
	}

	CharacterPart::CharacterPart(char c, bool createdWithBackslash)
		: _char(c), _backslash(createdWithBackslash) 
	{}

	bool CharacterPart::execute(std::string_view substr, size_t& i)
	{
		if (substr.size() > 0 && substr[i] == _char)
		{
			i += 1;
			_matchedString = std::string(1, _char);
			return true;
		}
		return false;
	}

	std::string CharacterPart::print() const
	{
		return std::format("'{}'", _char);
	}

	size_t OrOp::charater_length() const
	{
		return _left->charater_length() + _right->charater_length() + 1;
	}

	bool OrOp::execute(std::string_view substr, size_t& i) 
	{
		size_t localIndex = i;
		if (_left->execute(substr, localIndex))
		{
			i = localIndex;
			_matchedString = _left->get_matched_string();
			return true;
		}
		localIndex = i;
		if (_right->execute(substr, localIndex))
		{
			i = localIndex;
			_matchedString = _left->get_matched_string();
			return true;
		}
		return false;
	}

	std::string OrOp::print() const
	{
		return std::format("OR{{ {}, {} }}", _left->print(), _right->print());
	}

	void OrOp::set_right(std::unique_ptr<BasePart> part)
	{
		_right = std::move(part); // There is an error that makes this give the absolute persensisis of a serise. Fix this immidiately
	}

	RepeatRangedTimes::RepeatRangedTimes(std::optional<uint16_t> min, std::optional<uint16_t> max)
		: _min(min), _max(max) {}

	bool RepeatRangedTimes::execute(std::string_view substr, size_t& i) 
	{
		const int start = i;
		uint16_t count = 0;

		while (i < substr.size())
		{
			int before = i;

			if (!_repetition->execute(substr, i))
				break;

			if (i == before)
				break;

			count++;

			if (_max && count >= _max.value())
				break;
		}

		if (_min && count < _min.value())
		{
			i = start;
			return false;
		}
		_matchedString = substr.substr(start, i - start);
		i = start + _matchedString.size();
		return true;
	}

	std::string RepeatRangedTimes::print() const
	{
		return std::format("RepeatRangedQual{{ {}, {} }}", _min.value_or(0), _max.value_or(std::numeric_limits<uint16_t>::max()));
	}

	RepeatNumberedTimes::RepeatNumberedTimes(uint16_t times)
		: _times(times) {}

	std::string RepeatNumberedTimes::print() const
	{
		return std::format("RepeatNumQual{{ {} }}", _times);

	}

	bool RepeatNumberedTimes::execute(std::string_view substr, size_t& i) 
	{
		const int start = i;
		uint16_t count = 0;
		while (i < substr.size() && count < _times)
		{
			int before = i;
			if (!_repetition->execute(substr, i))
				break;
			if (i == before)
				break;
			count++;
		}
		if (count < _times)
		{
			i = start;
			return false;
		}
		_matchedString = substr.substr(start, i - start);
		i = start + _matchedString.size();
		return true;
	}

	std::string DashOp::get_matched_string() const
	{
		return _matchedString;
	}

	bool DashOp::execute(std::string_view substr, size_t& i)
	{
		if (substr.size() > 0 && substr[i] >= _left->_char && substr[i] <= _right->_char)
		{
			i += 1;
			_matchedString = std::string(1, substr[i - 1]);
			return true;
		}
		return false;
	}
	
	std::string DashOp::print() const
	{
		return std::format("Dash{{ {}, {} }}", _left->_char, _right->_char);
	}


	bool WordBoundary::execute(std::string_view substr, size_t& i) 
	{
		bool isAtStart = (i == 0);
		bool isAtEnd = (i == substr.size());
		bool beforeIsWordChar = false;
		bool afterIsWordChar = false;
		if (!isAtStart)
		{
			char beforeChar = substr[i - 1];
			beforeIsWordChar = ( (beforeChar >= 'A' && beforeChar <= 'Z') ||
								 (beforeChar >= 'a' && beforeChar <= 'z') ||
								 (beforeChar >= '0' && beforeChar <= '9') ||
								 (beforeChar == '_') );
		}
		if (!isAtEnd)
		{
			char afterChar = substr[i];
			afterIsWordChar = ( (afterChar >= 'A' && afterChar <= 'Z') ||
								(afterChar >= 'a' && afterChar <= 'z') ||
								(afterChar >= '0' && afterChar <= '9') ||
								(afterChar == '_') );
		}
		if (beforeIsWordChar != afterIsWordChar)
		{
			_matchedString = substr.substr(i, 0);
			return true;
		}

		return false;
	}

	size_t WordBoundary::charater_length() const
	{
		return 2;
	}

	std::string WordBoundary::print() const
	{
		return "WordBoundry";
	}

	bool NonWordBoundary::execute(std::string_view substr, size_t& i) 
	{
		bool isAtStart = (i == 0);
		bool isAtEnd = (i == substr.size());
		bool beforeIsWordChar = false;
		bool afterIsWordChar = false;
		if (!isAtStart)
		{
			char beforeChar = substr[i - 1];
			beforeIsWordChar = ( (beforeChar >= 'A' && beforeChar <= 'Z') ||
								 (beforeChar >= 'a' && beforeChar <= 'z') ||
								 (beforeChar >= '0' && beforeChar <= '9') ||
								 (beforeChar == '_') );
		}
		if (!isAtEnd)
		{
			char afterChar = substr[i];
			afterIsWordChar = ( (afterChar >= 'A' && afterChar <= 'Z') ||
								(afterChar >= 'a' && afterChar <= 'z') ||
								(afterChar >= '0' && afterChar <= '9') ||
								(afterChar == '_') );
		}
		if (beforeIsWordChar == afterIsWordChar)
		{
			_matchedString = substr.substr(i, 0);
			return true;
		}
		return false;
	}

	size_t NonWordBoundary::charater_length() const
	{
		return 2;
	}

	std::string NonWordBoundary::print() const
	{
		return "NotWordBoundry";
	}



	std::string OneOrZeroOp::print() const
	{
		return std::format("OneOrZero{{ {} }}", _repetition->print());
	}

	bool OneOrZeroOp::execute(std::string_view substr, size_t& i) 
	{
		int before = i;
		if (_repetition->execute(substr, i))
		{
			_matchedString += _repetition->get_matched_string();
			return true;
		}
		i = before;
		return true;
	}

	std::string StarOp::print() const
	{
		return std::format("ZeroOrMore{{ {} }}", _repetition->print());
	}

	bool StarOp::execute(std::string_view substr, size_t& i) 
	{
		const int start = i;
		while (i < substr.size())
		{
			int before = i;
			if (!_repetition->execute(substr, i))
				break;
			if (i == before)
				break;
		}
		_matchedString = substr.substr(start, i - start);
		return true;
	}

	bool PlusOp::execute(std::string_view substr, size_t& i) 
	{
		const int start = i;
		int matchCount = 0;
		while (i < substr.size())
		{
			int before = i;
			if (!_repetition->execute(substr, i))
				break;
			if (i == before)
				break;
			matchCount++;
		}
		_matchedString = substr.substr(start, i - start);
		return matchCount > 0;
	}

	std::string PlusOp::print() const
	{
		return std::format("OneOrMore{{ {} }}", _repetition->print());
	}

	void RepeatPartsBase::add(std::unique_ptr<BasePart> part)
	{
		_repetition = std::move(part);
	}

	size_t RepeatPartsBase::charater_length() const
	{
		return _repetition->charater_length() + 1;
	}

	std::string EndOfString::print() const
	{
		return "EndOfString";
	}

	size_t EndOfString::charater_length() const
	{
		return 1;
	}

	std::string StartOfString::print() const
	{
		return "StartOfString";
	}

	bool StartOfString::execute(std::string_view, size_t& i)
	{
		return i == 0;
	}

	bool EndOfString::execute(std::string_view substr, size_t& i)
	{
		return i == substr.size();
	}

	size_t StartOfString::charater_length() const
	{
		return 1;
	}

	std::string BasePart::print() const
	{
		return "";
	}


	PerenMarker::PerenMarker(size_t prevI) : _prevI(prevI)
	{}

	const size_t PerenMarker::get_prev_index() const
	{
		return _prevI;
	}


	Backreference::Backreference(ParenContainer& referencedCont, uint16_t num)
		: _referencedContainer(referencedCont), _num(num)
	{}

	bool Backreference::execute(std::string_view substr, size_t& i) 
	{
		if (substr.substr(i).starts_with(_referencedContainer.get_matched_string()))
		{
			i += _referencedContainer.get_matched_string().size();
			return true;
		}
		return false;
	}

	size_t Backreference::charater_length() const
	{
		return _referencedContainer.get_matched_string().size();
	}

	std::string Backreference::print() const
	{
		return "Backref{ " + std::to_string(_num) + " }";
	}

} // namespace parser
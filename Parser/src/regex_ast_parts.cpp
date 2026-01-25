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

	bool NotNumberPH::execute(std::string_view substr, int& i) const
	{
		if (substr[i] < '0' || substr[i] > '9')
		{
			i += 1;
			return true;
		}
		return false;
	}

	bool NumberPH::execute(std::string_view substr, int& i) const
	{
		if (substr[i] >= '0' && substr[i] <= '9')
		{
			i += 1;
			return true;
		}
		return false;
	}

	bool WordPH::execute(std::string_view substr, int& i) const
	{
		if ((substr[i] >= 'A' && substr[i] <= 'Z') ||
			(substr[i] >= 'a' && substr[i] <= 'z') ||
			(substr[i] >= '0' && substr[i] <= '9') ||
			(substr[i] == '_'))
		{
			i += 1;
			return true;
		}
		return false;
	}

	bool NotWordPH::execute(std::string_view substr, int& i) const
	{
		if (!((substr[i] >= 'A' && substr[i] <= 'Z') ||
			(substr[i] >= 'a' && substr[i] <= 'z') ||
			(substr[i] >= '0' && substr[i] <= '9') ||
			(substr[i] == '_')))
		{
			i += 1;
			return true;
		}
		return false;
	}

	bool WhitespacePH::execute(std::string_view substr, int& i) const
	{
		if (substr[i] == ' ' || substr[i] == '\t' || substr[i] == '\n' || substr[i] == '\r' || substr[i] == '\v' || substr[i] == '\f')
		{
			i += 1;
			return true;
		}
		return false;
	}

	bool NotWhitespacePH::execute(std::string_view substr, int& i) const
	{
		if (!(substr[i] == ' ' || substr[i] == '\t' || substr[i] == '\n' || substr[i] == '\r' || substr[i] == '\v' || substr[i] == '\f'))
		{
			i += 1;
			return true;
		}
		return false;
	}

	bool DotPH::execute(std::string_view substr, int& i) const
	{
		if (substr.size() > 0)
		{
			i += 1;
			return true;
		}
		return false;
	}

	void ParenContainer::add_part(std::unique_ptr<BasePart> part)
	{
		_parts.push_back(std::move(part));
	}

	std::unique_ptr<BasePart>& ParenContainer::get_back_part_ref()
	{
		return _parts.back();
	}

	void ParenContainer::delete_back()
	{
		_parts.pop_back();
	}

	void ParenContainer::replace_back_part(std::unique_ptr<BasePart> part)
	{
		_parts.back() = std::move(part);
	}

	bool ParenContainer::execute(std::string_view substr, int& i) const
	{
		int localIndex = 0;
		for (const auto& part : _parts)
		{
			if (!part->execute(substr.substr(localIndex), localIndex))
				return false;
		}
		return false;
	}

	size_t ParenContainer::charater_length() const
	{
		size_t length = 2;
		for (const auto& part : _parts)
			length += part->charater_length();
		return length;
	}

	bool BracketContainer::execute(std::string_view substr, int& i) const
	{
		int localIndex = i;
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
				i += 1;
				return true;
			}
		}

		if (matched)
		{
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

	bool CharacterPart::execute(std::string_view substr, int& i) const
	{
		if (substr.size() > 0 && substr[i] == _char)
		{
			i += 1;
			return true;
		}
		return false;
	}

	size_t OrOp::charater_length() const
	{
		return _left->charater_length() + _right->charater_length() + 1;
	}

	bool OrOp::execute(std::string_view substr, int& i) const
	{
		int tempIndex = i;
		if (_left->execute(substr, tempIndex))
		{
			i = tempIndex;
			return true;
		}
		tempIndex = i;
		if (_right->execute(substr, tempIndex))
		{
			i = tempIndex;
			return true;
		}
		return false;
	}

	void OrOp::set_right(std::unique_ptr<BasePart> part)
	{
		_right = std::move(part);
	}

	RepeatRangedTimes::RepeatRangedTimes(std::optional<uint16_t> min, std::optional<uint16_t> max)
		: _min(min), _max(max) {}

	bool RepeatRangedTimes::execute(std::string_view substr, int& i) const
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
		
		return true;
	}

	RepeatNumberedTimes::RepeatNumberedTimes(uint16_t times)
		: _times(times) {}

	bool RepeatNumberedTimes::execute(std::string_view substr, int& i) const
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
		return true;
	}

	bool DashOp::execute(std::string_view substr, int& i) const
	{
		if (substr.size() > 0 && substr[i] >= _left->_char && substr[i] <= _right->_char)
		{
			i += 1;
			return true;
		}
		return false;
	}	

	bool WordBoundary::execute(std::string_view substr, int& i) const
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
			return true;
		}
		return false;
	}

	size_t WordBoundary::charater_length() const
	{
		return 2;
	}

	bool NonWordBoundary::execute(std::string_view substr, int& i) const
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
			return true;
		}
		return false;
	}

	size_t NonWordBoundary::charater_length() const
	{
		return 2;
	}



	bool OneOrZeroOp::execute(std::string_view substr, int& i) const
	{
		int before = i;
		if (_repetition->execute(substr, i))
		{
			return true;
		}
		i = before;
		return true;
	}



	bool StarOp::execute(std::string_view substr, int& i) const
	{
		while (i < substr.size())
		{
			int before = i;
			if (!_repetition->execute(substr, i))
				break;
			if (i == before)
				break;
		}
		return true;
	}

	bool PlusOp::execute(std::string_view substr, int& i) const
	{
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
		return matchCount > 0;
	}

	void RepeatPartsBase::add(std::unique_ptr<BasePart> part)
	{
		_repetition = std::move(part);
	}

	size_t RepeatPartsBase::charater_length() const
	{
		return _repetition->charater_length() + 1;
	}

	size_t EndOfString::charater_length() const
	{
		return 1;
	}

	size_t StartOfString::charater_length() const
	{
		return 1;
	}

} // namespace parser
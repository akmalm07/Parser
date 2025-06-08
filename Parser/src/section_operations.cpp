#include "headers.h"
#include "include/section_operations.h"

namespace parser 
{

	SectioningInput<true> const* parser::SectioningInputBase::get_specialized() const
	{
		return nullptr;
	}

	SectioningInput<true> const* SectioningInput<true>::get_specialized() const
	{
		return this;
	}

	size_t BaseSection::get_section_level() const
	{
		return _sectionLevel;
	}

	std::shared_ptr<BaseSection> BaseSection::get_section_above() const
	{
		return _sectionAbove;
	}

	std::vector<std::string_view> BaseSection::get_content() const
	{
		return _content;
	}

	void BaseSection::add_token(std::string_view token)
	{
		_content.push_back(token);

	}

	size_t BaseSection::get_section_identifier() const
	{
		return _sectionIdentifier;
	}

}

#pragma once

#include "include/config.h"
#include "include/sections.h"



namespace parser {


	enum class SectionIdentifier
	{
		None = 0,
	};

	struct SectionKey
	{
	public:
		SectionIdentifier id;
		size_t depth;
		size_t index;
	};

	class Section;


	class BaseSection
	{
	public:

		BaseSection() = default;
		BaseSection(size_t sectionLevel, BaseSection* sectionAbove, SectionIdentifier sectionIdentifier)
			: _sectionLevel(sectionLevel), _sectionAbove(sectionAbove), _sectionIdentifier(sectionIdentifier) {
		}

		BaseSection(BaseSection* sectionAbove, SectionIdentifier sectionIdentifier)
			: _sectionAbove(sectionAbove), _sectionIdentifier(sectionIdentifier) 
		{
			determine_section_level();
		}

		void determine_section_level()
		{
			if (_sectionAbove)
			{
				_sectionLevel = (_sectionAbove)->get_section_level() + 1;
			}
			else
			{
				_sectionLevel = 0;
			}
		}

		size_t get_section_level() const { return _sectionLevel; }

		BaseSection* get_section_above() const { return _sectionAbove; }

		SectionIdentifier get_section_identifier() const { return _sectionIdentifier; }

		virtual ~BaseSection() = default;

	protected:
		size_t _sectionLevel = 0;
		BaseSection* _sectionAbove = nullptr;
		SectionIdentifier _sectionIdentifier = SectionIdentifier::None;
	};


	class Section : public BaseSection
	{
	public:

		Section(Section* sectionAbove = nullptr, SectionIdentifier sectionIdentifier = SectionIdentifier::None)
			: BaseSection(0, sectionAbove, sectionIdentifier)
		{
			BaseSection::determine_section_level();
		}

	private:
		std::vector <BaseSection*> _subSections;
		std::vector<std::string_view> _tokens;
	};


	class SectionHandler
	{
	public:
		SectionHandler() = default;

		SectionHandler(TockenizedSections const& tokenizedSections, std::vector<BaseSectioning> const& criteria)
		{
			 
		}

		void add_section(std::shared_ptr<BaseSection>& section)
		{
			_sectionValues.push_back(section);
		}

		void remove_section(std::shared_ptr<BaseSection>& section)
		{

		}



	private:

		void section_tokens(TockenizedSections const& tokenizedSections, std::vector<BaseSectioning> const& criteria);

	private:
		std::vector<SectionKey> _sectionKeys;
		std::vector<std::shared_ptr<BaseSection>> _sectionValues;
	};
}
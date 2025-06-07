#pragma once

#include "include/config.h"
#include "include/sections.h"
#include "include/section_operations.h"



namespace parser {



	struct SectionKey
	{
	public:
		size_t id;
		size_t depth;
		size_t index;
	};


	class Section : public BaseSection
	{
	public:

		Section(std::shared_ptr<Section> sectionAbove = nullptr, size_t sectionIdentifier = -1)
			: BaseSection(0, sectionAbove, sectionIdentifier)
		{
			BaseSection::determine_section_level();
		}

		void add_subsection(std::unique_ptr<BaseSection> subSection)
		{
			_subSections.push_back(std::move(subSection));
		}

	private:
		std::vector <std::unique_ptr<BaseSection>> _subSections;
		std::vector<std::string_view> _tokens;
	};



	struct CriteriaProcesserOutput
	{
		std::vector<ParserSectioning> sectioningType;
		std::vector<std::array<std::string_view, 2>> triggers;
		std::vector< std::function<std::shared_ptr<BaseSection>(SectioningInput)> > execute;

		CriteriaProcesserOutput() = default;

		CriteriaProcesserOutput(std::vector<ParserSectioning>& s,
			std::vector<std::array<std::string_view, 2>>& t,
			std::vector<std::function<std::shared_ptr<BaseSection>(SectioningInput)>>& e)
		{
			sectioningType = std::move(s);
			triggers = std::move(t);
			execute = std::move(e);
		}
	};






	class SectionHandler
	{
	public:
		SectionHandler() = default;

		SectionHandler(TockenizedUnsectionedFile const& tokenizedSections, std::vector<BaseSectioning> const& criteria)
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

		CriteriaProcesserOutput user_criteria_processer(TockenizedUnsectionedFile const& file, std::vector<std::unique_ptr<BaseSectioning>> const& criteria);

		void process_sectioning(TockenizedUnsectionedFile const& file, std::vector<std::unique_ptr<BaseSectioning>> const& criteria);

	private:
		std::vector<SectionKey> _sectionKeys;
		std::vector<std::shared_ptr<BaseSection>> _sectionValues;
	};
}



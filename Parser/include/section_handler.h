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

		Section(std::vector<std::string_view> const& content, std::shared_ptr<Section> sectionAbove = nullptr, size_t sectionIdentifier = -1)
			: BaseSection(content, sectionAbove, sectionIdentifier)
		{
		}

		Section(size_t sectionLvl, std::vector<std::string_view> const& content, std::shared_ptr<Section> sectionAbove = nullptr, size_t sectionIdentifier = -1)
			: BaseSection(sectionLvl, content, sectionAbove, sectionIdentifier)
		{
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
		std::vector<std::unique_ptr<ExecuteFunctorBase>> execute;

		CriteriaProcesserOutput() = default;

		CriteriaProcesserOutput(std::vector<ParserSectioning>& s,
			std::vector<std::array<std::string_view, 2>>& t,
			std::vector<std::unique_ptr<ExecuteFunctorBase>>& e)
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

		SectionHandler(TockenizedUnsectionedFile const& tokenizedSections, std::vector<std::shared_ptr<BaseSectioning>> const& criteria)
		{
			process_sectioning(tokenizedSections, criteria);
		}

		void add_section(std::shared_ptr<BaseSection>& section)
		{
			_sectionValues.push_back(section);
		}

		void debug_print_sections() const
		{
			for (const auto& section : _sectionValues)
			{
				std::cout << "Section Level: " << section->get_section_level() << ", Content: ";
				for (const auto& token : section->get_content())
				{
					std::cout << token << " ";
				}
				std::cout << std::endl;
			}
		}

		void remove_section(std::shared_ptr<BaseSection>& section)
		{

		}

	private:

		CriteriaProcesserOutput user_criteria_processer(TockenizedUnsectionedFile const& file, std::vector<std::shared_ptr<BaseSectioning>> const& criteria);

		void process_sectioning(TockenizedUnsectionedFile const& file, std::vector<std::shared_ptr<BaseSectioning>> const& criteria);

	private:
		std::vector<SectionKey> _sectionKeys;
		std::vector<std::shared_ptr<BaseSection>> _sectionValues;
	};
}



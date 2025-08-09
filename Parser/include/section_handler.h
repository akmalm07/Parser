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
		SectionCoords coords;
		SectionKey(size_t sectionLevel, size_t identifier = std::numeric_limits<size_t>::max(), size_t sectionIndex = 0, SectionCoords coords = { 0, 0 })
			: id(identifier), depth(sectionLevel), coords(coords), index(sectionIndex) {
		}

		bool operator==(const SectionKey& other) const
		{
			return id == other.id && depth == other.depth;
		}
	};


	class Section : public BaseSection
	{
	public:

		Section(std::vector<std::string_view> const& content, view_ptr<Section> sectionAbove = nullptr, size_t sectionIdentifier = -1)
			: BaseSection(content, sectionAbove, sectionIdentifier)
		{
		}

		Section(size_t sectionLvl, std::vector<std::string_view> const& content, view_ptr<Section> sectionAbove = nullptr, size_t sectionIdentifier = -1)
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
		// This class is responsible for handling/creating sections in a tokenized file.
	{
	public:
		SectionHandler() = default;

		SectionHandler(TockenizedUnsectionedFile const& tokenizedSections, std::vector<std::unique_ptr<BaseSectioning>> const& criteria)
		{
			process_sectioning(tokenizedSections, criteria);
		}

		void add_section(SectionKey const& key, std::unique_ptr<BaseSection>&& section);

		void debug_print_sections() const;

		TokenizedSectionizedCompact get_compressed_sections() const;

		void remove_section(SectionKey const& key);
	private:

		CriteriaProcesserOutput user_criteria_processer(TockenizedUnsectionedFile const& file, std::vector<std::unique_ptr<BaseSectioning>> const& criteria);

		void process_sectioning(TockenizedUnsectionedFile const& file, std::vector<std::unique_ptr<BaseSectioning>> const& criteria);


	private:
		std::vector<SectionKey> _sectionKeys;
		std::vector<std::unique_ptr<BaseSection>> _sectionValues;
	
		TokenizedSectionizedCompact _compressedSections; // A compact representation of the sections, which includes the tokens and their coordinates in the original file.
	
		constexpr static std::array<std::string_view, 5> REGEX_KEYWORD_LIST = { "|***|", "|*|", "|<<|", "|||", "|>>|" };
	
	private:

		bool check_find_until_regex(std::string& trigger, TockenizedUnsectionedFile const& file, size_t currentIndex) const;

		bool check_skip_one_regex(std::string& trigger, TockenizedUnsectionedFile const& file, size_t currentIndex) const;

		bool check_or_regex(std::string& trigger, TockenizedUnsectionedFile const& file, size_t currentIndex) const;
	};
}



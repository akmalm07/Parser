#pragma once

#include "include/config.h"
#include "include/sections.h"



namespace parser {



	struct SectionKey
	{
	public:
		size_t id;
		size_t depth;
		size_t index;
	};

	class Section;


	class BaseSection
	{
	public:

		BaseSection() = default;
		BaseSection(size_t sectionLevel, std::shared_ptr<BaseSection> sectionAbove, size_t sectionIdentifier)
			: _sectionLevel(sectionLevel), _sectionAbove(std::move(sectionAbove)), _sectionIdentifier(sectionIdentifier) {
		}

		BaseSection(BaseSection* sectionAbove, size_t sectionIdentifier)
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

		std::shared_ptr<BaseSection> get_section_above() const { return _sectionAbove; }

		size_t get_section_identifier() const { return _sectionIdentifier; }

		virtual ~BaseSection() = default;

	protected:
		size_t _sectionLevel = 0;
		std::shared_ptr<BaseSection> _sectionAbove = nullptr;
		size_t _sectionIdentifier = -1;
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






	struct SectioningInput
	{
		TockenizedUnsectionedFileIterator const placement;
		TockenizedUnsectionedFileIterator const endOfSection;
		std::shared_ptr<BaseSection> sectionAbove;
	};


	class ExecuteFunctorBase
	{
	public:
		virtual std::shared_ptr<BaseSection> operator() (SectioningInput const& input) = 0;
		virtual ~ExecuteFunctorBase() = default;
	};

	template<size_t N, bool CriteriaNeeded, bool EndInteratorNeeded>
	class ExecuteFunctor;

	template<size_t N>
	class ExecuteFunctor<N, false, false> : public ExecuteFunctorBase
	{
	public:

		ExecuteFunctor() = default;
		ExecuteFunctor(std::unique_ptr<BaseSectioning>&& criteria,
			std::function<std::vector<std::string_view>(size_t, TockenizedUnsectionedFileIterator)>&& execute)
			: _criteria(std::move(criteria)), _execute(std::move(execute))
		{
		}

		std::shared_ptr<BaseSection> operator() (SectioningInput const& input) override;

	private:
		std::unique_ptr<BaseSectioning> _criteria;
		std::function<std::vector<std::string_view>(size_t placementNum, TockenizedUnsectionedFileIterator placement)> _execute;
	};

	template<size_t N>
	class ExecuteFunctor < N, true, false > : public ExecuteFunctorBase
	{
	public:

		ExecuteFunctor() = default;
		ExecuteFunctor(std::unique_ptr<BaseSectioning>&& criteria, 
			std::function<std::vector<std::string_view>(size_t, TockenizedUnsectionedFileIterator, BaseSectioning*)>&& execute)
			: _criteria(std::move(criteria)), _execute(std::move(execute))
					{}
					
				std::shared_ptr<BaseSection> operator() (SectioningInput const& input) override;

	private:
		std::unique_ptr<BaseSectioning> _criteria;
		std::function<std::vector<std::string_view>(size_t placementNum, TockenizedUnsectionedFileIterator placement, BaseSectioning* criteria)> _execute;
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


#include "section_handler.inl"

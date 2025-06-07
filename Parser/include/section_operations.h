#pragma once



#include "include/config.h"
#include "include/sections.h"

namespace parser {


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

		std::shared_ptr<BaseSection> operator() (SectioningInput const& input) override final;

	protected:
		std::unique_ptr<BaseSectioning> _criteria;

		std::function<std::vector<std::string_view>(size_t placementNum, 
			TockenizedUnsectionedFileIterator placement)> _execute;
	};

	template<size_t N>
	class ExecuteFunctor < N, true, false> : public ExecuteFunctorBase// Add the EndInteratorNeeded modifier to the template
	{
	public:

		ExecuteFunctor() = default;
		ExecuteFunctor(std::unique_ptr<BaseSectioning>&& criteria,
			std::function<std::vector<std::string_view>(size_t, TockenizedUnsectionedFileIterator, BaseSectioning*)>&& execute)
			: _criteria(std::move(criteria)), _execute(std::move(execute))
		{
		}

		std::shared_ptr<BaseSection> operator() (SectioningInput const& input) override final;

	private:
		std::unique_ptr<BaseSectioning> _criteria;

		std::function<std::vector<std::string_view>(size_t placementNum, TockenizedUnsectionedFileIterator placement, 
			BaseSectioning* criteria)> _execute;
	};

	template<size_t N>
	class ExecuteFunctor < N, true, true> : public ExecuteFunctorBase// Add the EndInteratorNeeded modifier to the template
	{
	public:

		ExecuteFunctor() = default;
		ExecuteFunctor(std::unique_ptr<BaseSectioning>&& criteria,
			std::function<std::vector<std::string_view>(size_t, TockenizedUnsectionedFileIterator, TockenizedUnsectionedFileIterator, BaseSectioning*)>&& execute)
			: _criteria(std::move(criteria)), _execute(std::move(execute))
		{
		}

		std::shared_ptr<BaseSection> operator() (SectioningInput const& input) override final;

	private:
		std::unique_ptr<BaseSectioning> _criteria;

		std::function<std::vector<std::string_view>(size_t placementNum, TockenizedUnsectionedFileIterator placement, 
			TockenizedUnsectionedFileIterator endOfList, BaseSectioning* criteria)> _execute;
	};

	template<size_t N>
	class ExecuteFunctor< N, false, true> : public ExecuteFunctor<N, false, false>
	{ 
		ExecuteFunctor() = default;
		ExecuteFunctor(std::unique_ptr<BaseSectioning>&& criteria,
			std::function<std::vector<std::string_view>(size_t, TockenizedUnsectionedFileIterator)>&& execute)
			: ExecuteFunctor<N, false, false>(std::move(criteria), std::move(execute))
		{
		}

	};

	template<size_t N>
	using ExecuteFunc = ExecuteFunctor<N, false, false>;
	

	template<size_t N>
	using ExecuteFuncIterator = ExecuteFunctor<N, true, false>;

	template<size_t N>
	using ExecuteFuncTwoIterators = ExecuteFunctor<N, true, true>;


}

#include "section_operations.inl"

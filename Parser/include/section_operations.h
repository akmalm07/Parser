#pragma once



#include "include/config.h"
#include "include/sections.h"

namespace parser {


	class Section;

	class BaseSection
	{
	public:

		BaseSection() = default;
		BaseSection(size_t sectionLevel, std::vector<std::string_view> const& content, view_ptr<BaseSection> sectionAbove, size_t sectionIdentifier = 0)
			: _sectionLevel(sectionLevel), _content(content), _sectionAbove(sectionAbove), _sectionIdentifier(sectionIdentifier) 
		{
		}

		BaseSection(std::vector<std::string_view> const& content, view_ptr<BaseSection> sectionAbove, size_t sectionIdentifier = 0)
			: _sectionAbove(sectionAbove), _content(content), _sectionIdentifier(sectionIdentifier)
		{
			determine_section_level();
		}


		//Without Content Ctors
		BaseSection(size_t sectionLevel, view_ptr<BaseSection> sectionAbove, size_t sectionIdentifier = 0)
			: _sectionLevel(sectionLevel), _sectionAbove(std::move(sectionAbove)), _sectionIdentifier(sectionIdentifier) {
		}

		BaseSection(view_ptr<BaseSection> sectionAbove, size_t sectionIdentifier = 0)
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

		size_t get_section_level() const;

		view_ptr<BaseSection> get_section_above() const;

		std::vector<std::string_view> get_content() const;
		
		void add_token(std::string_view token);

		size_t get_section_identifier() const;

		virtual ~BaseSection() = default;

	private:
		size_t _sectionLevel = 0;
		std::vector<std::string_view> _content;
		view_ptr<BaseSection> _sectionAbove = nullptr;
		size_t _sectionIdentifier = -1;
	};


	template<bool IsSpecialized>
	struct SectioningInput;

	struct SectioningInputBase
	{
	public:

		TockenizedUnsectionedFileIteratorConst placement;
		view_ptr<BaseSection> sectionAbove = nullptr;
		
		SectioningInputBase() = default;
		SectioningInputBase(TockenizedUnsectionedFileIteratorConst const& placement, view_ptr<BaseSection> above)
			: placement(placement), sectionAbove(above) {
		}

		virtual view_ptr<SectioningInput<true>> get_specialized() const;
	};

	template<bool IsSpecialized> // TODO: make it so that the template is size_t N, so the number of targets could be infinite!
	struct SectioningInput : public SectioningInputBase
	{
		SectioningInput(TockenizedUnsectionedFileIterator const& placement, view_ptr<BaseSection> above)
			: SectioningInputBase(placement, above) {
		}

		SectioningInput(TockenizedUnsectionedFileIteratorConst const& placement, view_ptr<BaseSection> above)
			: SectioningInputBase(placement, above) {
		}
	};

	template<>
	struct SectioningInput<true> : public SectioningInputBase
	{
	public:

		TockenizedUnsectionedFileIteratorConst endOfSection;

		SectioningInput(TockenizedUnsectionedFileIterator const& placement,
			TockenizedUnsectionedFileIterator const& endOfSection,
			view_ptr<BaseSection> above)
			: SectioningInputBase(placement, above), endOfSection(endOfSection) {
		}
		SectioningInput(TockenizedUnsectionedFileIteratorConst const& placement,
			TockenizedUnsectionedFileIteratorConst const& endOfSection,
			view_ptr<BaseSection> above)
			: SectioningInputBase(placement, above), endOfSection(endOfSection) {
		}

		view_ptr<SectioningInput<true>> get_specialized() const override;

	};

	SectioningInput(
		TockenizedUnsectionedFileIterator const&,
		TockenizedUnsectionedFileIterator const&,
		view_ptr<BaseSection>)
		-> SectioningInput<true>;

	SectioningInput(
		TockenizedUnsectionedFileIteratorConst,
		TockenizedUnsectionedFileIteratorConst,
		view_ptr<BaseSection>)
		-> SectioningInput<true>;
	
	

	SectioningInput(
		TockenizedUnsectionedFileIterator const&,
		view_ptr<BaseSection>)
		-> SectioningInput<false>;


	SectioningInput(
		TockenizedUnsectionedFileIteratorConst,
		view_ptr<BaseSection>)
		-> SectioningInput<false>;





	// Output structure for the sectioning process
	struct SectioningOutput
	{
	public:

		TockenizedUnsectionedFileIteratorConst endOfSection;
		std::unique_ptr<BaseSection> section;

		SectioningOutput() = default;
		SectioningOutput(TockenizedUnsectionedFileIteratorConst end, std::unique_ptr<BaseSection>&& section)
			: endOfSection(end), section(std::move(section)) {
		}
		SectioningOutput(TockenizedUnsectionedFile file, std::unique_ptr<BaseSection>&& section)
			: section(std::move(section)), endOfSection(file.end()) {
		}
	};

	struct ExecutionOutput
	{
	public:

		TockenizedUnsectionedFileIteratorConst endOfSection;
		std::vector<std::string_view> content;

		ExecutionOutput() = default;
		ExecutionOutput(TockenizedUnsectionedFileIteratorConst end, std::vector<std::string_view> const& content)
			: endOfSection(end), content(content) {
		}
		ExecutionOutput(TockenizedUnsectionedFileIteratorConst end)
			: endOfSection(end) {
		}
	};


	class ExecuteFunctorBase
	{
	public:

		ExecuteFunctorBase() = default;

		virtual SectioningOutput execute(SectioningInputBase const& input) = 0;
		virtual SectioningOutput operator() (SectioningInputBase const& input) = 0;
		virtual ~ExecuteFunctorBase() = default;
	};

	enum class CriteriaNeeded
	{
		No = false,
		Yes = true
	};

	enum class EndOfIteratorNeeded
	{
		No = false,
		Yes = true
	};

	template<size_t N, CriteriaNeeded C, EndOfIteratorNeeded I>
	class ExecuteFunctor;

	/*
	template<size_t N>
		requires (N > 0)
	class ExecuteFunctor<N, CriteriaNeeded::No, EndOfIteratorNeeded::No> : public ExecuteFunctorBase
	{
	public:

		ExecuteFunctor() = default;
		ExecuteFunctor(BaseSectioning const* criteria,
			std::function<ExecutionOutput(size_t, TockenizedUnsectionedFileIteratorConst)>&& execute)
			: _criteria(criteria), _execute(std::move(execute))
		{
		}

		SectioningOutput execute(SectioningInputBase const& input) override final;

		SectioningOutput operator() (SectioningInputBase const& input) override final;

	protected:
		BaseSectioning const* _criteria;;

		std::function<ExecutionOutput(size_t placementNum, 
			TockenizedUnsectionedFileIteratorConst placement)> _execute;
	};
	*/
	template<size_t N>
		requires (N > 0)
	class ExecuteFunctor < N, CriteriaNeeded::Yes, EndOfIteratorNeeded::No> : public ExecuteFunctorBase// Add the CriteriaNeeded modifier to the template
	{
	public:

		ExecuteFunctor() = default;
		ExecuteFunctor(BaseSectioning const* criteria,
			std::function<ExecutionOutput(size_t, TockenizedUnsectionedFileIteratorConst, BaseSectioning*)>&& execute)
			: _criteria(criteria), _execute(std::move(execute))
		{
		}

		SectioningOutput execute(SectioningInputBase const& input) override final;

		SectioningOutput operator() (SectioningInputBase const& input) override final;

	protected:
		BaseSectioning const* _criteria;

		std::function<ExecutionOutput(size_t placementNum, 
			TockenizedUnsectionedFileIteratorConst placement, BaseSectioning* criteria)> _execute;
	};

	template<size_t N>
		requires (N > 0)
	class ExecuteFunctor < N, CriteriaNeeded::Yes, EndOfIteratorNeeded::Yes> : public ExecuteFunctorBase// Add the CriteriaNeeded modifier to the template
	{
	public:

		ExecuteFunctor() = default;
		ExecuteFunctor(BaseSectioning const* criteria,
			std::function<ExecutionOutput(size_t, TockenizedUnsectionedFileIteratorConst, TockenizedUnsectionedFileIteratorConst, BaseSectioning*)>&& execute)
			: _criteria(criteria), _execute(std::move(execute))
		{
		}

		SectioningOutput execute(SectioningInputBase const& input) override final;

		SectioningOutput operator() (SectioningInputBase const& input) override final;

	private:
		BaseSectioning const* _criteria;;

		std::function<ExecutionOutput(size_t placementNum, 
			TockenizedUnsectionedFileIteratorConst placement, 
			TockenizedUnsectionedFileIteratorConst endOfList,
			BaseSectioning* criteria)> _execute;
	};

	template<size_t N>
		requires (N > 0)
	class ExecuteFunctor< N, CriteriaNeeded::No, EndOfIteratorNeeded::Yes> : public ExecuteFunctorBase
	{ 
	public:
		ExecuteFunctor() = default;
		ExecuteFunctor(BaseSectioning const* criteria,
			std::function<ExecutionOutput(size_t, TockenizedUnsectionedFileIteratorConst, TockenizedUnsectionedFileIteratorConst)>&& execute)
			: _criteria(criteria), _execute(std::move(execute))
		{
		}

		SectioningOutput execute(SectioningInputBase const& input) override final;

		SectioningOutput operator() (SectioningInputBase const& input) override final;

	private:
		BaseSectioning const* _criteria;;

		std::function<ExecutionOutput(size_t placementNum,
			TockenizedUnsectionedFileIteratorConst placement,
			TockenizedUnsectionedFileIteratorConst endOfSection)> _execute;

	};

	template<size_t N>
	using ExecuteFunc = ExecuteFunctor<N, CriteriaNeeded::No, EndOfIteratorNeeded::Yes>;
	
	/*
	template<size_t N>
	using ExecuteFuncOneIterator = ExecuteFunctor<N, CriteriaNeeded::Yes, EndOfIteratorNeeded::Yes>;

	template<size_t N>
	using ExecuteFuncTwoIterators = ExecuteFunctor<N, CriteriaNeeded::Yes, EndOfIteratorNeeded::Yes>;

	*/

	template<size_t N>
	using ExecuteFuncWithCriteria = ExecuteFunctor<N, CriteriaNeeded::Yes, EndOfIteratorNeeded::Yes>;

}

#include "section_operations.inl"
#pragma once



#include "include/config.h"
#include "include/sections.h"

namespace parser {


	class Section;

	class BaseSection
	{
	public:

		BaseSection() = default;
		BaseSection(size_t sectionLevel, std::vector<std::string_view> const& content, std::shared_ptr<BaseSection> sectionAbove, size_t sectionIdentifier)
			: _sectionLevel(sectionLevel), _content(std::move(content)), _sectionAbove(std::move(sectionAbove)), _sectionIdentifier(sectionIdentifier) {
		}

		BaseSection(std::vector<std::string_view> const& content, std::shared_ptr<BaseSection> sectionAbove, size_t sectionIdentifier)
			: _sectionAbove(sectionAbove), _content(std::move(content)), _sectionIdentifier(sectionIdentifier)
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

		std::shared_ptr<BaseSection> get_section_above() const;

		std::vector<std::string_view> get_content() const;
		
		void add_token(std::string_view token);

		size_t get_section_identifier() const;

		virtual ~BaseSection() = default;

	private:
		size_t _sectionLevel = 0;
		std::vector<std::string_view> _content;
		std::shared_ptr<BaseSection> _sectionAbove = nullptr;
		size_t _sectionIdentifier = -1;
	};


	template<bool OneIterator>
	struct SectioningInput;

	struct SectioningInputBase
	{
		virtual SectioningInput<false> const* get_one_iter() const;
		
		virtual SectioningInput<true> const* get_two_iter() const;
	};

	template<bool OneIterator>
	struct SectioningInput : public SectioningInputBase
	{};

	template<>
	struct SectioningInput<false> : public SectioningInputBase
	{
		TockenizedUnsectionedFileIteratorConst placement;
		std::shared_ptr<BaseSection> sectionAbove;

		SectioningInput(TockenizedUnsectionedFileIteratorConst placement,
			std::shared_ptr<BaseSection> above)
			: placement(placement), sectionAbove(std::move(above)) {
		}

		SectioningInput(TockenizedUnsectionedFileIterator const placement,
			std::shared_ptr<BaseSection> above)
			: placement(placement), sectionAbove(std::move(above)) {
		}

		SectioningInput<false> const* get_one_iter() const override;


	};

	template<>
	struct SectioningInput<true> : public SectioningInputBase
	{
		TockenizedUnsectionedFileIteratorConst placement;
		TockenizedUnsectionedFileIteratorConst endOfSection;
		std::shared_ptr<BaseSection> sectionAbove;

		SectioningInput(TockenizedUnsectionedFileIterator const placement,
			TockenizedUnsectionedFileIterator const endOfSection,
			std::shared_ptr<BaseSection> above)
			: placement(placement), endOfSection(endOfSection), sectionAbove(std::move(above)) {
		}
		SectioningInput(TockenizedUnsectionedFileIteratorConst placement,
			TockenizedUnsectionedFileIteratorConst endOfSection,
			std::shared_ptr<BaseSection> above)
			: placement(placement), endOfSection(endOfSection), sectionAbove(std::move(above)) {
		}

		SectioningInput<true> const* get_two_iter() const override;

	};

	SectioningInput(
		TockenizedUnsectionedFileIterator const,
		TockenizedUnsectionedFileIterator const,
		std::shared_ptr<BaseSection>)
		-> SectioningInput<true>;

	SectioningInput(
		TockenizedUnsectionedFileIteratorConst,
		TockenizedUnsectionedFileIteratorConst,
		std::shared_ptr<BaseSection>)
		-> SectioningInput<true>;
	

	SectioningInput(
		TockenizedUnsectionedFileIteratorConst,
		std::shared_ptr<BaseSection>)
		-> SectioningInput<false>;

	SectioningInput(
		TockenizedUnsectionedFileIteratorConst const,
		std::shared_ptr<BaseSection>)
		-> SectioningInput<false>;




	// Output structure for the sectioning process
	struct SectioningOutput
	{
		TockenizedUnsectionedFileIteratorConst endOfSection;
		std::shared_ptr<BaseSection> section;
	};

	struct ExecutionOutput
	{
		TockenizedUnsectionedFileIteratorConst endOfSection;
		std::vector<std::string_view> content;
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
		ExecuteFunctor(std::unique_ptr<BaseSectioning>&& criteria,
			std::function<ExecutionOutput(size_t, TockenizedUnsectionedFileIterator)>&& execute)
			: _criteria(std::move(criteria)), _execute(std::move(execute))
		{
		}

		SectioningOutput execute(SectioningInputBase const& input) override final;

		SectioningOutput operator() (SectioningInputBase const& input) override final;

	protected:
		std::unique_ptr<BaseSectioning> _criteria;

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
		ExecuteFunctor(std::unique_ptr<BaseSectioning>&& criteria,
			std::function<ExecutionOutput(size_t, TockenizedUnsectionedFileIterator, BaseSectioning*)>&& execute)
			: _criteria(std::move(criteria)), _execute(std::move(execute))
		{
		}

		SectioningOutput execute(SectioningInputBase const& input) override final;

		SectioningOutput operator() (SectioningInputBase const& input) override final;

	protected:
		std::unique_ptr<BaseSectioning> _criteria;

		std::function<ExecutionOutput(size_t placementNum, 
			TockenizedUnsectionedFileIteratorConst placement, BaseSectioning* criteria)> _execute;
	};

	template<size_t N>
		requires (N > 0)
	class ExecuteFunctor < N, CriteriaNeeded::Yes, EndOfIteratorNeeded::Yes> : public ExecuteFunctorBase// Add the CriteriaNeeded modifier to the template
	{
	public:

		ExecuteFunctor() = default;
		ExecuteFunctor(std::unique_ptr<BaseSectioning>&& criteria,
			std::function<ExecutionOutput(size_t, TockenizedUnsectionedFileIterator, TockenizedUnsectionedFileIterator, BaseSectioning*)>&& execute)
			: _criteria(std::move(criteria)), _execute(std::move(execute))
		{
		}

		SectioningOutput execute(SectioningInputBase const& input) override final;

		SectioningOutput operator() (SectioningInputBase const& input) override final;

	private:
		std::unique_ptr<BaseSectioning> _criteria;

		std::function<ExecutionOutput(size_t placementNum, 
			TockenizedUnsectionedFileIterator placement, 
			TockenizedUnsectionedFileIteratorConst endOfList,
			BaseSectioning* criteria)> _execute;
	};

	template<size_t N>
		requires (N > 0)
	class ExecuteFunctor< N, CriteriaNeeded::No, EndOfIteratorNeeded::Yes> : public ExecuteFunctorBase
	{ 
		ExecuteFunctor() = default;
		ExecuteFunctor(std::unique_ptr<BaseSectioning>&& criteria,
			std::function<ExecutionOutput(size_t, TockenizedUnsectionedFileIterator)>&& execute)
			: _criteria(std::move(criteria)), _execute(std::move(execute))
		{
		}

		SectioningOutput execute(SectioningInputBase const& input) override final;

		SectioningOutput operator() (SectioningInputBase const& input) override final;

	private:
		std::unique_ptr<BaseSectioning> _criteria;

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

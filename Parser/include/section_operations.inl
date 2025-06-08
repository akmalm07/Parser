#pragma once

#include "include/config.h"
#include "include/section_operations.h"

namespace parser
{
	/*
	template<size_t N>
		requires (N > 0)
	SectioningOutput ExecuteFunctor<N, CriteriaNeeded::No, EndOfIteratorNeeded::No>::execute(SectioningInputBase const& input)
	{
		return operator()(input);
	}
	*/

	template<size_t N>
		requires (N > 0)
	SectioningOutput ExecuteFunctor<N, CriteriaNeeded::Yes, EndOfIteratorNeeded::No>::execute(SectioningInputBase const& input)
	{
		return operator()(input);
	}
	

	template<size_t N>
		requires (N > 0)
	SectioningOutput ExecuteFunctor<N, CriteriaNeeded::Yes, EndOfIteratorNeeded::Yes>::execute(SectioningInputBase const& input)
	{
		return operator()(input);
	}
	

	template<size_t N>
		requires (N > 0)
	SectioningOutput ExecuteFunctor<N, CriteriaNeeded::No, EndOfIteratorNeeded::Yes>::execute(SectioningInputBase const& input)
	{
		return operator()(input);
	}

	/*
	template<size_t N>
		requires (N > 0)
	SectioningOutput ExecuteFunctor<N, CriteriaNeeded::No, EndOfIteratorNeeded::No>::operator()(SectioningInputBase const& input)
	{
		SectioningInput<true>* realInput = input.get_two_iter();

		if (!realInput)
		{
			std::cerr << PARSER_LOG_ERR << "Invalid SectioningInputBase provided to ExecuteFunctor." << std::endl;
			return nullptr;
		}

		size_t placementNum = realInput->endOfSection - realInput->placement;


		ExecutionOutput output = _execute(placementNum, realInput->placement);

		size_t identifier = std::numeric_limits<size_t>::max();
		if (_criteria->is_identifible())
		{
			identifier = static_cast<SectioningIdentifier<N>*>(_criteria.get())->integerIdentifier;
		}

		return { output.endOfSection, std::make_shared<BaseSection>(output.content, realInput->sectionAbove, identifier) };
	}

	*/

	template<size_t N>
		requires (N > 0)
	SectioningOutput ExecuteFunctor<N, CriteriaNeeded::Yes, EndOfIteratorNeeded::No>::operator()(SectioningInputBase const& input)
	{
		SectioningInput<true>* realInput = input.get_two_iter();

		if (!realInput)
		{
			std::cerr << PARSER_LOG_ERR << "Invalid SectioningInputBase provided to ExecuteFunctor." << std::endl;
			return nullptr;
		}

		size_t placementNum = realInput->endOfSection - realInput->placement;

		ExecutionOutput output = _execute(placementNum, realInput->placement, _criteria.get());

		size_t identifier = std::numeric_limits<size_t>::max();
		if (_criteria->is_identifible())
		{
			identifier = static_cast<SectioningIdentifier<N>*>(_criteria.get())->integerIdentifier;
		}

		return { output.endOfSection, std::make_shared<BaseSection>(output.content, realInput->sectionAbove, identifier) };
	}



	template<size_t N>
		requires (N > 0)
	SectioningOutput ExecuteFunctor<N, CriteriaNeeded::Yes, EndOfIteratorNeeded::Yes>::operator()(SectioningInputBase const& input)
	{
		SectioningInput<true>* realInput = input.get_two_iter();
		
		if (!realInput)
		{
			std::cerr << PARSER_LOG_ERR << "Invalid SectioningInputBase provided to ExecuteFunctor." << std::endl;
			return nullptr;
		}

		size_t placementNum = realInput->endOfSection - realInput->placement;

		ExecutionOutput output = _execute(placementNum, realInput->placement, realInput->endOfSection, _criteria.get());

		size_t identifier = std::numeric_limits<size_t>::max();
		if (_criteria->is_identifible())
		{
			identifier = static_cast<SectioningIdentifier<N>*>(_criteria.get())->integerIdentifier;
		}

		return { output.endOfSection, std::make_shared<BaseSection>(output.content, realInput->sectionAbove, identifier) };
	}


	template<size_t N>
		requires (N > 0)
	SectioningOutput ExecuteFunctor<N, CriteriaNeeded::No, EndOfIteratorNeeded::Yes>::operator()(SectioningInputBase const& input)
	{
		SectioningInput<true>* realInput = input.get_two_iter();
		
		if (!realInput)
		{
			std::cerr << PARSER_LOG_ERR << "Invalid SectioningInputBase provided to ExecuteFunctor." << std::endl;
			return nullptr;
		}

		size_t placementNum = realInput->endOfSection - realInput->placement;

		ExecutionOutput output = _execute(placementNum, realInput->endOfSection, _criteria.get());

		size_t identifier = std::numeric_limits<size_t>::max();
		if (_criteria->is_identifible())
		{
			identifier = static_cast<SectioningIdentifier<N>*>(_criteria.get())->integerIdentifier;
		}

		return { output.endOfSection, std::make_shared<BaseSection>(output.content, realInput->sectionAbove, identifier) };
	}
}
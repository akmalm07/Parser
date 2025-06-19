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
		view_ptr<SectioningInput<true>> realInput = input.get_specialized();

		if (!realInput)
		{
			std::cerr << PARSER_LOG_ERR << "Invalid SectioningInputBase provided to ExecuteFunctor." << std::endl;
			return { input.placement, nullptr };
		}

		size_t placementNum = realInput->endOfSection - realInput->placement;


		ExecutionOutput output = _execute(placementNum, realInput->placement);

		if (auto _criteria = __criteria.lock())
		{
			size_t identifier = std::numeric_limits<size_t>::max();
			if (_criteria->is_identifible())
			{
				identifier = static_cast<SectioningIdentifier<N> const*>(_criteria)->integerIdentifier;
			}

			return { output.endOfSection, std::make_shared<BaseSection>(output.content, realInput->sectionAbove, identifier) };

		}
		else
		{
			std::cerr << PARSER_LOG_ERR << "Invalid or Expired Pointer for Executing the Sectioning Output!\n";
			return { realInput->placement, nullptr };
		}
	}

	*/

	template<size_t N>
		requires (N > 0)
	SectioningOutput ExecuteFunctor<N, CriteriaNeeded::Yes, EndOfIteratorNeeded::No>::operator()(SectioningInputBase const& input)
	{
		view_ptr<SectioningInput<true>> realInput = input.get_specialized();

		if (!realInput)
		{
			std::cerr << PARSER_LOG_ERR << "Invalid SectioningInputBase provided to ExecuteFunctor." << std::endl;
			return { input.placement, nullptr };
		}

		size_t placementNum = realInput->endOfSection - realInput->placement;


		ExecutionOutput output = _execute(placementNum, realInput->placement, const_cast<BaseSectioning*>(_criteria));
			
		size_t identifier = std::numeric_limits<size_t>::max();
		if (_criteria->is_identifible())
		{
			identifier = static_cast<SectioningIdentifier<N> const*>(_criteria)->integerIdentifier;
		}

		return { output.endOfSection, std::make_unique<BaseSection>(output.content, realInput->sectionAbove, identifier) };

		
	}



	template<size_t N>
		requires (N > 0)
	SectioningOutput ExecuteFunctor<N, CriteriaNeeded::Yes, EndOfIteratorNeeded::Yes>::operator()(SectioningInputBase const& input)
	{
		view_ptr<SectioningInput<true>> realInput = input.get_specialized();
		
		if (!realInput)
		{
			std::cerr << PARSER_LOG_ERR << "Invalid SectioningInputBase provided to ExecuteFunctor." << std::endl;
			return { input.placement, nullptr };
		}

		size_t placementNum = realInput->endOfSection - realInput->placement;



		ExecutionOutput output = _execute(placementNum, realInput->placement, realInput->endOfSection, const_cast<BaseSectioning*>(_criteria));
			
		size_t identifier = std::numeric_limits<size_t>::max();
		if (_criteria->is_identifible())
		{
			identifier = static_cast<SectioningIdentifier<N> const*>(_criteria)->integerIdentifier;
		}

		return { output.endOfSection, std::make_unique<BaseSection>(output.content, realInput->sectionAbove, identifier) };

	}


	template<size_t N>
		requires (N > 0)
	SectioningOutput ExecuteFunctor<N, CriteriaNeeded::No, EndOfIteratorNeeded::Yes>::operator()(SectioningInputBase const& input)
	{
		view_ptr<SectioningInput<true>> realInput = input.get_specialized();
		
		if (!realInput)
		{
			std::cerr << PARSER_LOG_ERR << "Invalid SectioningInputBase provided to ExecuteFunctor." << std::endl;
			return { input.placement, nullptr };
		}

		size_t placementNum = realInput->endOfSection - realInput->placement;

		ExecutionOutput output = _execute(placementNum, realInput->placement, realInput->endOfSection); // in instances that WhenFound is inside Between, a crash occurs

		size_t identifier = std::numeric_limits<size_t>::max();
		if (_criteria->is_identifible())
		{
			identifier = static_cast<SectioningIdentifier<N> const*>(_criteria)->integerIdentifier;
		}

		return { output.endOfSection, std::make_unique<BaseSection>(output.content, realInput->sectionAbove, identifier) };

	}
}
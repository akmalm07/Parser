#pragma once

#include "include/config.h"
#include "include/section_operations.h"

namespace parser
{

	template<size_t N>
	std::shared_ptr<BaseSection> ExecuteFunctor<N, true, false>::operator()(SectioningInput const& input)
	{
		size_t placementNum = input.endOfSection - input.placement;

		std::vector<std::string_view> content = _execute(placementNum, input.placement, _criteria.get());

		size_t identifier = std::numeric_limits<size_t>::max();
		if (_criteria->is_identifible())
		{
			identifier = static_cast<SectioningIdentifier<N>*>(_criteria.get())->integerIdentifier;
		}

		return std::make_shared<BaseSection>(input.sectionAbove, identifier);
	}

	template<size_t N>
	std::shared_ptr<BaseSection> ExecuteFunctor<N, false, false>::operator()(SectioningInput const& input)
	{

		size_t placementNum = input.endOfSection - input.placement;


		std::vector<std::string_view> content = _execute(placementNum, input.placement);

		/*
		std::vector<std::string_view> content;
		std::string_view stopTrigger;
		if (_criteria->is_identifible())
		{
			stopTrigger = static_cast<SectioningIdentifier<N>*>(_criteria.get())->targets[1];
		}
		else
		{
			stopTrigger = static_cast<SectioningUnidentifier<N>*>(_criteria.get())->targets[1];
		}

		content.reserve(placementNum);
		for (size_t i = 0; i < placementNum; i++)
		{
			content.push_back(*(input.placement + i));
		}
		///*/

		size_t identifier = std::numeric_limits<size_t>::max();
		if (_criteria->is_identifible())
		{
			identifier = static_cast<SectioningIdentifier<N>*>(_criteria.get())->integerIdentifier;
		}

		return std::make_shared<BaseSection>(input.sectionAbove, identifier);
	}

}
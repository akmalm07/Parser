#pragma once


#include "config.h"

#include "identifier.h"


namespace parser
{

	class IdentiferHandler
	{
	public:
		IdentiferHandler() = default;
		
		IdentiferHandler(std::vector<std::unique_ptr<BaseIdentifier>>& identifiers);

		ItemsIdentified identify_items(EntireTokenizedFile& tokens) const;

		void add_identifier(BaseIdentifier const& identifier);

		std::vector<std::unique_ptr<BaseIdentifier>> const& get_identifiers() const;

		~IdentiferHandler() = default;

	private:
		std::vector<std::unique_ptr<BaseIdentifier>> _identifier;
	};

	template <IdentifierType T>
	std::unique_ptr<Identifier<T>> new_identifier(Identifier<T> const& item)
	{
		return std::make_unique<Identifier<T>>(item);
	}

} // namespace parser
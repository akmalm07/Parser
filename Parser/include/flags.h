#pragma once


#include "include/config.h"


namespace parser
{


	template <typename BitType>
	class Flags
	{
	public:
		using MaskType = typename std::underlying_type<BitType>::type;

		constexpr Flags() : _mask(0) {}
		constexpr Flags(BitType bit) : _mask(static_cast<MaskType>(bit)) {}
		constexpr Flags(MaskType rawMask) : _mask(rawMask) {}

		constexpr Flags operator|(BitType bit) const
		{
			return Flags(_mask | static_cast<MaskType>(bit));
		}

		constexpr Flags& operator|=(BitType bit)
		{
			_mask |= static_cast<MaskType>(bit);
			return *this;
		}

		constexpr bool operator&(BitType bit) const
		{
			return (_mask & static_cast<MaskType>(bit)) != 0;
		}

		constexpr MaskType raw() const { return _mask; }

	private:
		MaskType _mask;
	};



	enum class StringChecksBitFlags : uint32_t
	{
		None = 0,
		NoSpaceCheck = 1 << 0,
		NoNewLineCheck = 1 << 1,
		NoTabCheck = 1 << 2,
		NoAtoZCheck = 1 << 3, // No a-z or A-Z check
		NoDigitCheck = 1 << 4, // No 0-9 check
		NoSpecialCharCheck = 1 << 5, // No special characters check
		NoWhitespaceCheck = NoSpaceCheck | NoNewLineCheck | NoTabCheck,
	};

	using StringChecksFlag = Flags<StringChecksBitFlags>;

	inline StringChecksFlag operator|(StringChecksBitFlags lhs, StringChecksBitFlags rhs)
	{
		return StringChecksFlag(lhs | rhs);
	}

}
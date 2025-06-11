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

		constexpr bool has(BitType bit) const
		{
			return (_mask & static_cast<MaskType>(bit)) != 0;
		}

		constexpr MaskType raw() const { return _mask; }

	private:
		MaskType _mask;
	};


	// --StringChecksFlags--
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
		return StringChecksFlag(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
	}

	// --TokenizationSeperationFlags--

	enum class TokenizationSeperationBitFlags : uint64_t
	{
		None = 0,
		TockeizeSpace = 1ULL << 0, 
		TockeizeNewLine = 1ULL << 1, 
		TockeizeTab = 1ULL << 2, 

		TockeizeWhitespace = (TockeizeSpace | TockeizeNewLine | TockeizeTab), 

		TockeizeAtoZ = 1ULL << 3, 
		TockeizeDigit = 1ULL << 4, 

		TockeizeExplemation = 1ULL << 5, 
		TockeizeQuestion = 1ULL << 6, 
		TockeizeColon = 1ULL << 7, 
		TockeizeSemicolon = 1ULL << 8, 
		TockeizeDot = 1ULL << 9,
		TockeizeComma = 1ULL << 10,

		TockenizePunctuation = (TockeizeExplemation | TockeizeQuestion | TockeizeColon | TockeizeSemicolon| TockeizeDot | TockeizeComma),


		TokenizeAt = 1ULL << 11, 
		TokenizeHash = 1ULL << 12, 
		TokenizeDollar = 1ULL << 13, 
		TokenizePercent = 1ULL << 14, 
		TokenizeCaret = 1ULL << 15, 
		TokenizeAmpersand = 1ULL << 16, 
		TokenizeStar = 1ULL << 17, 
		TokenizeDash = 1ULL << 18, 
		TokenizePlus = 1ULL << 19, 
		TokenizeEqual = 1ULL << 20, 
		TokenizeSlash = 1ULL << 21, 
		TokenizeBackslash = 1ULL << 22, 
		TokenizeTilde = 1ULL << 23, 
		TokenizePipe = 1ULL << 24, 
		TokenizeGrave = 1ULL << 25, 
		TokenizeUnderscore = 1ULL << 26, 

		TokenizeOpenAngleBracket = 1ULL << 27,   
		TokenizeClosedAngleBracket = 1ULL << 28, 
		TokenizeAngleBrackets = (TokenizeOpenAngleBracket | TokenizeClosedAngleBracket),
		TokenizeOpenCurlyBracket = 1ULL << 29,   
		TokenizeClosedCurlyBracket = 1ULL << 30, 
		TokenizeCurlyBrackets = (TokenizeOpenCurlyBracket | TokenizeClosedCurlyBracket),
		TokenizeOpenBracket = 1ULL << 31,   
		TokenizeClosedBracket = 1ULL << 32, 
		TokenizeBrackets = (TokenizeOpenBracket | TokenizeClosedBracket),

		TokenizeOpenSquareBracket = 1ULL << 33,   
		TokenizeClosedSquareBracket = 1ULL << 34,
		TokenizeSquareBrackets = (TokenizeOpenSquareBracket | TokenizeClosedSquareBracket),
		
		TokenizeSpecialChars = (TokenizeAt |TokenizeHash |TokenizeDollar | TokenizePercent |TokenizeCaret |TokenizeAmpersand |TokenizeStar |TokenizeDash |TokenizePlus |TokenizeEqual |TokenizeSlash |TokenizeBackslash |TokenizeTilde |TokenizePipe |TokenizeGrave |TokenizeUnderscore |TokenizeAngleBrackets |TokenizeBrackets),
	};

	using TokenizationSeperationFlag = Flags<TokenizationSeperationBitFlags>;

	inline TokenizationSeperationFlag operator|(TokenizationSeperationBitFlags lhs, TokenizationSeperationBitFlags rhs)
	{
		return TokenizationSeperationFlag(static_cast<uint64_t>(lhs) | static_cast<uint64_t>(rhs));
	}


	// --WhiteSpaceDissolveFlags--

	enum class WhiteSpaceDissolveBitFlags : uint32_t
	{
		None = 0,
		DissolveSpace = 1 << 0, // Dissolve spaces
		DissolveNewLine = 1 << 1, // Dissolve new lines
		DissolveTab = 1 << 2, // Dissolve tabs
		DissolveAll = DissolveSpace | DissolveNewLine | DissolveTab, // Dissolve all whitespace
	};

	using WhiteSpaceDissolveFlag = Flags<WhiteSpaceDissolveBitFlags>;

	inline WhiteSpaceDissolveFlag operator|(WhiteSpaceDissolveBitFlags lhs, WhiteSpaceDissolveBitFlags rhs)
	{
		return WhiteSpaceDissolveFlag(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
	}


	// --SectioningFlags--

	enum class SectioningBitFlags : uint32_t
	{
		None = 0,
		NewSectionWhenFound = 1 << 0, 
		NewSectionWhenBetween = 1 << 1, 
		NewSectionWhenAfter = 1 << 2, 
		NewSectionWhenBefore = 1 << 3,
		NewSectionWhenAfterAndBefore = NewSectionWhenAfter | NewSectionWhenBefore, // Create a new section when the target is found after and before another target	
	};

	using SectioningFlag = Flags<SectioningBitFlags>;

	inline SectioningFlag operator|(SectioningBitFlags lhs, SectioningBitFlags rhs)
	{
		return SectioningFlag(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
	}
}
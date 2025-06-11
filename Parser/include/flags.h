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
		return StringChecksFlag(lhs | rhs);
	}

	// --TokenizationSeperationFlags--

	enum class TokenizationSeperationBitFlags : uint64_t
	{
		None = 0,
		TockeizeSpace = 1 << 0, 
		TockeizeNewLine = 1 << 1, 
		TockeizeTab = 1 << 2, 

		TockeizeWhitespace = (TockeizeSpace | TockeizeNewLine | TockeizeTab), 

		TockeizeAtoZ = 1 << 3, 
		TockeizeDigit = 1 << 4, 

		TockeizeExplemation = 1 << 5, 
		TockeizeQuestion = 1 << 6, 
		TockeizeColon = 1 << 7, 
		TockeizeSemicolon = 1 << 8, 
		TockeizeDot = 1 << 9,
		TockeizeComma = 1 << 10,

		TockenizePunctuation = (TockeizeExplemation | TockeizeQuestion | TockeizeColon | TockeizeSemicolon| TockeizeDot | TockeizeComma),


		TokenizeAt = 1 << 11, 
		TokenizeHash = 1 << 12, 
		TokenizeDollar = 1 << 13, 
		TokenizePercent = 1 << 14, 
		TokenizeCaret = 1 << 15, 
		TokenizeAmpersand = 1 << 16, 
		TokenizeStar = 1 << 17, 
		TokenizeDash = 1 << 18, 
		TokenizePlus = 1 << 19, 
		TokenizeEqual = 1 << 20, 
		TokenizeSlash = 1 << 21, 
		TokenizeBackslash = 1 << 22, 
		TokenizeTilde = 1 << 23, 
		TokenizePipe = 1 << 24, 
		TokenizeGrave = 1 << 25, 
		TokenizeUnderscore = 1 << 26, 

		TokenizeOpenAngleBracket = 1 << 27,   
		TokenizeClosedAngleBracket = 1 << 28, 
		TokenizeAngleBrackets = (TokenizeOpenAngleBracket | TokenizeClosedAngleBracket),
		TokenizeOpenCurlyBracket = 1 << 29,   
		TokenizeClosedCurlyBracket = 1 << 30, 
		TokenizeBrackets = (TokenizeOpenCurlyBracket | TokenizeClosedCurlyBracket),
		TokenizeOpenBracket = 1 << 31,   
		TokenizeClosedBracket = 1 << 32, 
		TokenizeCurlyBrackets = (TokenizeOpenBracket | TokenizeClosedBracket), 

		TokenizeOpenSquareBracket = 1 << 33,   
		TokenizeClosedSquareBracket = 1 << 34,
		TokenizeSquareBrackets = (TokenizeOpenSquareBracket | TokenizeClosedSquareBracket),
		
		TokenizeSpecialChars = (TokenizeAt |TokenizeHash |TokenizeDollar | TokenizePercent |TokenizeCaret |TokenizeAmpersand |TokenizeStar |TokenizeDash |TokenizePlus |TokenizeEqual |TokenizeSlash |TokenizeBackslash |TokenizeTilde |TokenizePipe |TokenizeGrave |TokenizeUnderscore |TokenizeAngleBrackets |TokenizeBrackets),
	};

	using TokenizationSeperationFlag = Flags<TokenizationSeperationBitFlags>;

	inline TokenizationSeperationFlag operator|(TokenizationSeperationBitFlags lhs, TokenizationSeperationBitFlags rhs)
	{
		return TokenizationSeperationFlag(lhs | rhs);
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
		return WhiteSpaceDissolveFlag(lhs | rhs);
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
		return SectioningFlag(lhs | rhs);
	}
}
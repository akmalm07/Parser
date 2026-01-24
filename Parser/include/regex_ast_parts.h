#pragma once


#include "config.h"


// -------------------------------------------
// ------ PARTS OF ABSTRACT SYNTAX TREE ------
// -------------------------------------------

namespace parser
{

	enum class RegType
	{
		
		// Root / Structural
		
		Base,               // Root or default node

		
		// Grouping & Containers
		
		Parentheses,        // () Capture / grouping
		Bracket,            // [] Character class

		
		// Anchors & Zero-Width Assertions
		
		StartOfString,      // ^
		EndOfString,        // $
		WordBoundary,       // \b (\bcat\b == " cat ")
		NonWordBoundary,    // \B (\Bcat\B == "cat")

		
		// Predefined Character Classes (Placeholders)
		
		NumberPH,           // \d
		NotNumberPH,        // \D
		WhitespacePH,       // \s
		NotWhitespacePH,    // \S
		DotPH,              // . (any character)
		WordPH,               // \w ([A-Za-z0-9_])
		NotWordPH,               // \W ([^A-Za-z0-9_])


		
		// Literal / Concrete Match Values
		
		Character,          // Literal character
		Number,             // Numeric literal (used in {n,m}, \1)
		
		// Comma,              // , literal (used in {n,m})

		
		// Character-Class Operators (Inside [])
		
		Dash,               // a-z range operator

		
		// Logical / Alternation Operators
		
		Or,                 // |

		
		// Quantifiers
		
		Star,               // *
		Plus,               // +
		OneOrZero,          // ?
		RepeatRangedTimes,  // {n,m} && {n,} && {,m}
		RepeatNumberedTimes,// {n} // Make this a seperate class!!!
		
		// Backreferences
		
		Backreference,      // \1, \2, etc.

		// Markers (To make compiling easier)
		PerenToAddHere
	};

	struct FitsResult
	{
		std::unique_ptr<BasePart> part = nullptr;
		size_t length = 0;
	};

	class BasePart
	{
	public:

		virtual constexpr RegType type() const = 0;

		virtual size_t charater_length() const = 0;

		// Figure out to execute the checks

		//virtual FitsResult fits_start(std::string_view substr) = 0;

		virtual ~BasePart() = default;
	};



	class PlaceHolder : public BasePart
	{
	public: 
		size_t charater_length() const override { return 2; }
	};

	class NotNumberPH : public PlaceHolder
	{
		NotNumberPH() = default;
		constexpr RegType type() const override { return RegType::NotNumberPH; }		
		~NotNumberPH() override = default;
	};

	class NumberPH : public PlaceHolder
	{
    public:
		NumberPH() = default;
		constexpr RegType type() const override { return RegType::NumberPH; }
		~NumberPH() override = default;
	};

	class WordPH : public PlaceHolder
	{
    public:
		WordPH() = default;
		constexpr RegType type() const override { return RegType::WordPH; }
		~WordPH() override = default;
	};

	class NotWordPH : public PlaceHolder
	{
    public:
		NotWordPH() = default;
		constexpr RegType type() const override { return RegType::NotWordPH; }
		~NotWordPH() override = default;
	};


	class WhitespacePH : public PlaceHolder
	{
    public:
		WhitespacePH() = default;
		constexpr RegType type() const override { return RegType::WhitespacePH; }
		~WhitespacePH() override = default;
	};
	class NotWhitespacePH : public PlaceHolder
	{
    public:
		NotWhitespacePH() = default;
		constexpr RegType type() const override { return RegType::NotWhitespacePH; }
		~NotWhitespacePH() override = default;
	};

	class DotPH : public PlaceHolder
	{
    public:
		DotPH() = default;
		constexpr RegType type() const override { return RegType::DotPH; }
		~DotPH() override = default;
	};


	/* --- CONTAINER REGS ---*/

	class Container : public BasePart
	{
	public:
		Container() = default;
		 
		//BasePart* part_back_ptr()
		//{
		//	return _parts.back().get();
		//}

		~Container() override = default;
	protected:
		//std::vector<std::unique_ptr<BasePart>> _parts;
	};


	class ParenContainer : public Container
	{
	public:
		ParenContainer() = default;

		void add_part(std::unique_ptr<BasePart> part) { _parts.emplace_back(std::move(part)); }

		std::unique_ptr<BasePart>& get_back_part_ref() { return _parts.back(); }

		void replace_back_part(std::unique_ptr<BasePart> part) { _parts.pop_back(); _parts.push_back(std::move(part)); }

		constexpr RegType type() const override { return RegType::Parentheses; }

		size_t charater_length() const override 
		{ 
			size_t length = 2;
			for (const auto& part : _parts)
				length += part->charater_length();
			return length;
		}
		~ParenContainer() override = default;
	private:
		std::vector<std::unique_ptr<BasePart>> _parts;
	};

	enum NegetiveFlag
	{
		NEGETIVE = 0,
		POSITIVE = 1,
	};

	class BracketAcceptedPart
	{
	public:
		virtual size_t charater_length() const = 0;
		virtual ~BracketAcceptedPart() = default;

	};

	class BracketContainer : public Container
	{
	public:
		BracketContainer() = default;
		BracketContainer(NegetiveFlag negetive) : _not(negetive) {}

		constexpr RegType type() const override { return RegType::Bracket; }

		size_t charater_length() const override 
		{ 
			size_t length = (_not == NEGETIVE ? 3 : 2);
			for (const auto& part : _parts)
				length += part->charater_length();
			return length;
		}

		void add_char(char part, bool usesBackslash = false) { _parts.push_back(std::make_unique<CharacterPart>(part, usesBackslash)); }
		void add_dash(char left, char right) 
		{
			if (left > right)
				std::swap(left, right);
			_parts.push_back(std::make_unique<DashOp>(left, right));
		}
		~BracketContainer() override = default;
	private:
		NegetiveFlag _not = POSITIVE;
		std::vector<std::unique_ptr<BracketAcceptedPart>> _parts;
		//std::unique_ptr<BasePart> _quantifier;
	};


	// This is a quantifier container
	class RepeatRangedTimes : public RepeatPartsBase
	{
	public:
		RepeatRangedTimes(std::unique_ptr<BasePart> part, std::optional<uint16_t> min, std::optional<uint16_t> max) : RepeatPartsBase(std::move(part)), _min(min), _max(max)
		{};

		constexpr RegType type() const override { return RegType::RepeatRangedTimes; }

		size_t charater_length() const override;

		~RepeatRangedTimes() override = default;
	private:
			std::optional<uint16_t> _min = std::nullopt;
			std::optional<uint16_t> _max = std::nullopt;

	};

	class RepeatNumberedTimes : public RepeatPartsBase
	{
	public:
		RepeatNumberedTimes(std::unique_ptr<BasePart> part, int times) : RepeatPartsBase(std::move(part)), _times(times) {};

		constexpr RegType type() const override { return RegType::RepeatNumberedTimes; }

		~RepeatNumberedTimes() override = default;
	private:
		int _times;
	};

	class RealValue : public BasePart
	{};

	class CharacterPart : public RealValue, public BracketAcceptedPart
	{
	public:
		CharacterPart(char c, bool createdWithBackslash = false) : _char(c), _backslash(createdWithBackslash) {}

		size_t charater_length() const override { return 1 + (_backslash ? 1 : 0); }

		constexpr RegType type() const override { return RegType::Character; }

		~CharacterPart() override = default;
	private:
		char _char;
		bool _backslash = false;
	};

	//class NumberPart : public CharacterPart
	//{
	//public:
	//	NumberPart(std::string_view number);

	//	size_t charater_length() const override { return digits_in_number(_number); }

	//	constexpr RegType type() const override { return RegType::Number; }
	//	
	//	int get_number() const { return _number; }
	//	
	//	~NumberPart() override = default;
	//private:
	//	unsigned int _number = 0;
	//};




	class OrOp : public BasePart
	{
	public:
		OrOp(std::unique_ptr<BasePart> left) { _left = std::move(left); };

		size_t charater_length() const override { return _left->charater_length() + _right->charater_length() + 1; }

		void set_right(std::unique_ptr<BasePart> part) 
		{
			_right = std::move(part);
		}

		constexpr RegType type() const override { return RegType::Or; }

		~OrOp() override = default;
	private:
			std::unique_ptr<BasePart> _left;
			std::unique_ptr<BasePart> _right;
	};


	class RepeatPartsBase : public BasePart
	{
	public:
		RepeatPartsBase() = default;
		
		void add(std::unique_ptr<BasePart> part)
		{
			_repetition = std::move(part);
		}
		//RepeatPartsBase(std::unique_ptr<BasePart> part) : _repetition(std::move(part)) {};

		virtual ~RepeatPartsBase() = default;
	protected:
		std::unique_ptr<BasePart> _repetition;
	};



	class StarOp : public RepeatPartsBase
	{
	public:
		StarOp(std::unique_ptr<BasePart>& part) : RepeatPartsBase(std::move(part)) {};

		constexpr RegType type() const override { return RegType::Star; }

		~StarOp() override = default;
	};


	class PlusOp : public RepeatPartsBase
	{
	public:
		PlusOp(std::unique_ptr<BasePart>& part) : RepeatPartsBase(std::move(part)) {};

		constexpr RegType type() const override { return RegType::Plus; }

		~PlusOp() override = default;
	private:
		std::shared_ptr<BasePart> _repetition;
	};


	class OneOrZeroOp : public RepeatPartsBase
	{
    public:
		OneOrZeroOp(std::unique_ptr<BasePart>& part) : RepeatPartsBase(std::move(part)) {};
		constexpr RegType type() const override { return RegType::OneOrZero; }
		~OneOrZeroOp() override = default;
	private:
		std::shared_ptr<BasePart> _repetition;
	};

	// template<typename T>
	// concept CharOrNumber = std::is_same_v<T, CharacterPart> || std::is_same_v<T, NumberPart>;

	class DashOp : public BracketAcceptedPart, public BasePart
		// This is so that the bracket container can accept dashes in the inheritance tree
	{
    public:
		DashOp(char left, char right) : _left(std::make_unique<CharacterPart>(left)), _right(std::make_unique<CharacterPart>(right)) {};

		size_t charater_length() const override { return 3; }

		constexpr RegType type() const override { return RegType::Dash; }
		
		~DashOp() override = default;
	private:
		std::unique_ptr<CharacterPart> _left;
		std::unique_ptr<CharacterPart> _right;
	};




	enum class MinOrMaxVal
	{
		Min = 0,
		Max,
	};


	//class BackreferencePart : public BasePart
	//{
	//public:
	//	BackreferencePart(int groupNumber) : _groupNumber(groupNumber) {}

	//	constexpr RegType type() const override { return RegType::Backreference; }

	//	~BackreferencePart() override = default;
	//private:
	//	int _groupNumber;
	//};

	class StringPlacementPartsBase : public BasePart
	{
	public:
		StringPlacementPartsBase() = default;
		virtual ~StringPlacementPartsBase() = default;
	protected:
	};

	class StartOfString : public StringPlacementPartsBase
	{
	public:
		StartOfString() = default;

		constexpr RegType type() const override { return RegType::StartOfString; }

		~StartOfString() override = default;
	private:
	};

	class EndOfString : public StringPlacementPartsBase
	{
	public:
		EndOfString() = default;

		constexpr RegType type() const override { return RegType::EndOfString; }

		~EndOfString() override = default;
	private:
	};

	class WordBoundary : public StringPlacementPartsBase
	{
	public:
		WordBoundary() = default;

		constexpr RegType type() const override { return RegType::WordBoundary; }

		~WordBoundary() override = default;
	private:

	};

	class NonWordBoundary : public StringPlacementPartsBase
	{
	public:
		NonWordBoundary() = default;

		constexpr RegType type() const override { return RegType::NonWordBoundary; }

		~NonWordBoundary() override = default;
	private:
	};

	//using CharOrNumber = std::variant<CharacterPart, NumberPart>;

	class PerenMarker : public BasePart
	{
	public:
		PerenMarker() = default;
		constexpr RegType type() const override { return RegType::PerenToAddHere; }
		size_t charater_length() const override { return 0; }
		~PerenMarker() override = default;
	};
}

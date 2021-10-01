/******************************************************************************
 StylingScannerBase.h

	Copyright © 2021 by John Lindal.

 *****************************************************************************/

#ifndef _H_StylingScannerBase
#define _H_StylingScannerBase

#include <reflex/abslexer.h>
#include <reflex/matcher.h>
#include <jx-af/jcore/JStyledText.h>

class StylingScannerBase : public reflex::AbstractLexer<reflex::Matcher>
{
public:

	enum BaseTokenType
	{
		kEOF = 258
	};

	struct Token
	{
		int						type;
		JStyledText::TextRange	range;
		JStyledText::TextRange	docCommentRange;	// preceding comment range for DOC comment tags
		const JString*			scriptLanguage;

		Token()
			:
			type(kEOF), scriptLanguage(nullptr)
			{ };

		Token(const int t, const JStyledText::TextRange& r)
			:
			type(t), range(r), scriptLanguage(nullptr)
			{ };

		Token(const int t, const JStyledText::TextRange& r, const JString* l)
			:
			type(t), range(r), scriptLanguage(l)
			{ };
	};

public:

	StylingScannerBase(const reflex::Input& input, std::ostream& os);

	virtual void	BeginScan(const JStyledText* text,
							  const JStyledText::TextIndex& startIndex,
							  std::istream& input);

	const JStyledText::TextRange&	GetCurrentRange() const;
	const JStyledText::TextRange&	GetPPNameRange() const;

protected:

	void	InitToken();
	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const int type);
	Token	DocToken(const int type);

	void	Undo(const JStyledText::TextRange& range,
				 const JSize prevCharByteCount, const JString& text);

	void	SetCurrentRange(const JStyledText::TextRange& r);

	void	SavePPNameRange();
	JString	GetPPCommand(const JString& text) const;

	bool	SlurpQuoted(const JSize count, const JUtf8Byte* suffixList);

	bool	IsQuote(const JUtf8Character& c);
private:

	const JStyledText*		itsCurrentText;
	JStyledText::TextRange	itsCurrentRange;
	JStyledText::TextRange	itsPPNameRange;

private:

	bool	ReadCharacter(JStyledText::TextIndex* index, JUtf8Character* ch);

	// not allowed

	StylingScannerBase(const StylingScannerBase&) = delete;
	StylingScannerBase& operator=(const StylingScannerBase&) = delete;
};


/******************************************************************************
 InitToken (protected)

 *****************************************************************************/

inline void
StylingScannerBase::InitToken()
{
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
}

/******************************************************************************
 StartToken (protected)

 *****************************************************************************/

inline void
StylingScannerBase::StartToken()
{
	InitToken();
	ContinueToken();
}

/******************************************************************************
 ContinueToken (protected)

 *****************************************************************************/

inline void
StylingScannerBase::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(text(), size());
	itsCurrentRange.byteRange.last += size();
}

/******************************************************************************
 ThisToken (protected)

 *****************************************************************************/

inline StylingScannerBase::Token
StylingScannerBase::ThisToken
	(
	const int type
	)
{
	return Token(type, itsCurrentRange);
}

/******************************************************************************
 GetCurrentRange

 *****************************************************************************/

inline const JStyledText::TextRange&
StylingScannerBase::GetCurrentRange()
	const
{
	return itsCurrentRange;
}

/******************************************************************************
 SetCurrentRange (protected)

 *****************************************************************************/

inline void
StylingScannerBase::SetCurrentRange
	(
	const JStyledText::TextRange& r
	)
{
	itsCurrentRange = r;
}

/******************************************************************************
 GetPPNameRange

 *****************************************************************************/

inline const JStyledText::TextRange&
StylingScannerBase::GetPPNameRange()
	const
{
	return itsPPNameRange;
}

/******************************************************************************
 GetPPNameRange (protected)

 *****************************************************************************/

inline bool
StylingScannerBase::IsQuote
	(
	const JUtf8Character& c
	)
{
	return c == '\'' || c == '"' || c == '`';
}

/******************************************************************************
 Comparisons

	These compare the type and *range*, not the contents, since ranges do not
	know where they came from.  Still useful when the tokens come from the same
	string.

 *****************************************************************************/

inline int
operator==
	(
	const StylingScannerBase::Token& t1,
	const StylingScannerBase::Token& t2
	)
{
	return (t1.type == t2.type &&
			(t1.range.charRange == t2.range.charRange || t1.type == StylingScannerBase::kEOF));
}

inline int
operator!=
	(
	const StylingScannerBase::Token& t1,
	const StylingScannerBase::Token& t2
	)
{
	return !(t1 == t2);
}

#endif

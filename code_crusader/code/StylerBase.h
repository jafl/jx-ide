/******************************************************************************
 StylerBase.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_StylerBase
#define _H_StylerBase

#include <jx-af/jcore/JSTStyler.h>
#include <jx-af/jcore/JPrefObject.h>
#include <jx-af/jcore/JStringMap.h>
#include "TextFileType.h"

class EditStylerDialog;

class StylerBase : public JSTStyler, public JPrefObject, virtual public JBroadcaster
{
public:

	typedef JStrValue<JFontStyle>	WordStyle;

public:

	StylerBase(const JSize typeVersion,
				 const JSize typeCount, const JUtf8Byte** typeNames,
				 const JString& editDialogTitle, const JPrefID& prefID,
				 const TextFileType fileType);

	virtual ~StylerBase();

	void	EditStyles();

	const JStringMap<JFontStyle>&	GetWordList() const;

	void	ReadFromSharedPrefs(std::istream& input);
	void	WriteForSharedPrefs(std::ostream& output) const;

protected:

	JFontStyle	GetStyle(const JIndex typeIndex, const JString& word);

	JFontStyle	GetTypeStyle(const JIndex index) const;
	void		SetTypeStyle(const JIndex index, const JFontStyle& style);

	bool	GetWordStyle(const JString& word, JFontStyle* style) const;
	void	SetWordStyle(const JString& word, const JFontStyle& style);
	void	RemoveWordStyle(const JString& word);

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) = 0;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JColorID				itsDefColor;

	const JFileVersion		itsTypeNameVersion;
	const JSize				itsTypeNameCount;
	const JUtf8Byte**		itsTypeNames;			// not owned
	JArray<JFontStyle>*		itsTypeStyles;
	const TextFileType	itsFileType;			// only use for IsCharInWord() !!

	JStringMap<JFontStyle>*	itsWordStyles;

	const JString&			itsDialogTitle;
	EditStylerDialog*		itsEditDialog;			// can be nullptr
	JString					itsDialogGeom;

private:

	void	GetWordList(const JStringMap<JFontStyle>& wordStyles,
						JArray<WordStyle>* wordList,
						const bool sort) const;

	JFontStyle	ReadStyle(std::istream& input);
	void		WriteStyle(std::ostream& output, const JFontStyle& style) const;

	void	ExtractStyles();
	bool	TypeStylesChanged(const JArray<JFontStyle>& newTypeStyles) const;
	bool	WordStylesChanged(const JStringMap<JFontStyle>& newWordStyles) const;

	void	SetDefaultFontColor(const JColorID color);

	static JListT::CompareResult
		CompareWords(const WordStyle& w1, const WordStyle& w2);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kWordListChanged;

	class WordListChanged : public JBroadcaster::Message
		{
		public:

			WordListChanged(const JStringMap<JFontStyle>& wordList)
				:
				JBroadcaster::Message(kWordListChanged),
				itsWordList(wordList)
				{ };

			const JStringMap<JFontStyle>&
			GetWordList() const
			{
				return itsWordList;
			};

		private:

			const JStringMap<JFontStyle>&	itsWordList;
		};
};


/******************************************************************************
 GetWordList

 ******************************************************************************/

inline const JStringMap<JFontStyle>&
StylerBase::GetWordList()
	const
{
	return *itsWordStyles;
}

/******************************************************************************
 Type style (protected)

 ******************************************************************************/

inline JFontStyle
StylerBase::GetTypeStyle
	(
	const JIndex index
	)
	const
{
	return itsTypeStyles->GetElement(index);
}

inline void
StylerBase::SetTypeStyle
	(
	const JIndex		index,
	const JFontStyle&	style
	)
{
	itsTypeStyles->SetElement(index, style);
}

/******************************************************************************
 Word style (protected)

 ******************************************************************************/

inline bool
StylerBase::GetWordStyle
	(
	const JString&	word,
	JFontStyle*		style
	)
	const
{
	return itsWordStyles->GetElement(word, style);
}

inline void
StylerBase::SetWordStyle
	(
	const JString&		word,
	const JFontStyle&	style
	)
{
	itsWordStyles->SetElement(word, style);
}

inline void
StylerBase::RemoveWordStyle
	(
	const JString& word
	)
{
	itsWordStyles->RemoveElement(word);
}

/******************************************************************************
 Shared prefs

 ******************************************************************************/

inline void
StylerBase::ReadFromSharedPrefs
	(
	std::istream& input
	)
{
	ReadPrefs(input);
}

inline void
StylerBase::WriteForSharedPrefs
	(
	std::ostream& output
	)
	const
{
	WritePrefs(output);
}

#endif

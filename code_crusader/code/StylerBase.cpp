/******************************************************************************
 StylerBase.cpp

	Base class for all Code Crusader styler classes.  This stores an
	associative array of styles for arbitrary strings and an array of
	styles for token types.  It also handles reading, writing, and the
	dialog window for editing both these data structures.

	GetStyle() takes a token type and the text and looks up the appropriate
	style.

	Derived classes must override the following function:

		UpgradeTypeList
			Upgrade the given list of type styles from the given version
			to the latest version, so it matches the type names passed
			to the constructor.

	BASE CLASS = JSTStyler, JPrefObject, virtual JBroadcaster

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "StylerBase.h"
#include "EditStylerDialog.h"
#include "sharedUtil.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

// setup information -- remember to increment shared prefs file version

const JFileVersion kCurrentSetupVersion = 0;

// JBroadcaster message types

const JUtf8Byte* StylerBase::kWordListChanged = "WordListChanged::StylerBase";

/******************************************************************************
 Constructor

 ******************************************************************************/

StylerBase::StylerBase
	(
	const JSize				typeVersion,
	const JSize				typeCount,
	const JUtf8Byte**		typeNames,
	const JString&			editDialogTitle,
	const JPrefID&			prefID,
	const TextFileType	fileType
	)
	:
#ifndef CODE_CRUSADER_UNIT_TEST
	JPrefObject(GetPrefsManager(), prefID),
#else
	JPrefObject(nullptr, prefID),
#endif
	itsTypeNameVersion(typeVersion),
	itsTypeNameCount(typeCount),
	itsTypeNames(typeNames),
	itsFileType(fileType),
	itsDialogTitle(editDialogTitle)
{
	itsTypeStyles = jnew JArray<JFontStyle>(64);
	assert( itsTypeStyles != nullptr );

#ifndef CODE_CRUSADER_UNIT_TEST
	itsDefColor = GetPrefsManager()->GetColor(PrefsManager::kTextColorIndex);
#else
	itsDefColor = JColorManager::GetBlackColor();
#endif

	JFontStyle style(itsDefColor);
	for (JIndex i=1; i<=typeCount; i++)
	{
		itsTypeStyles->AppendItem(style);
	}

	itsWordStyles = jnew JStringMap<JFontStyle>(64);
	assert( itsWordStyles != nullptr );

#ifndef CODE_CRUSADER_UNIT_TEST
	ListenTo(GetPrefsManager());
#endif
}

/******************************************************************************
 Destructor

 ******************************************************************************/

StylerBase::~StylerBase()
{
	jdelete itsTypeStyles;
	jdelete itsWordStyles;
}

/******************************************************************************
 GetStyle (protected)

	If word is not found in our associative array, we use the typeIndex.

 ******************************************************************************/

JFontStyle
StylerBase::GetStyle
	(
	const JIndex	typeIndex,
	const JString&	word
	)
{
	JFontStyle style;
	if (!GetWordStyle(word, &style))
	{
		style = GetTypeStyle(typeIndex);
	}

	return style;
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
StylerBase::ReadPrefs
	(
	std::istream& input
	)
{
JIndex i;

	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
	{
		return;
	}

	bool active;
	input >> JBoolFromString(active);

	// type styles

	JArray<JFontStyle> typeStyles;

	JSize typeCount;
	input >> typeCount;

	for (i=1; i<=typeCount; i++)
	{
		typeStyles.AppendItem(ReadStyle(input));
	}

	JFileVersion typeListVers;
	input >> typeListVers;
	if (typeListVers > itsTypeNameVersion)
	{
		return;
	}

	SetActive(active);
	*itsTypeStyles = typeStyles;
	UpgradeTypeList(typeListVers, itsTypeStyles);
	assert( itsTypeStyles->GetItemCount() == itsTypeNameCount );

	// word styles

	itsWordStyles->RemoveAll();

	JSize wordCount;
	input >> wordCount;

	JString s;
	for (i=1; i<=wordCount; i++)
	{
		input >> s;
		itsWordStyles->SetItem(s, ReadStyle(input));
	}

	Broadcast(WordListChanged(*itsWordStyles));

	// dialog geometry

	input >> itsDialogGeom;
}

/******************************************************************************
 ReadStyle (private)

 ******************************************************************************/

JFontStyle
StylerBase::ReadStyle
	(
	std::istream& input
	)
{
	JFontStyle style;

	input >> JBoolFromString(style.bold);
	input >> JBoolFromString(style.italic);
	input >> style.underlineCount;
	input >> JBoolFromString(style.strike);

	JRGB color;
	input >> color;
	style.color = JColorManager::GetColorID(color);

	return style;
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
StylerBase::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;
	output << ' ' << JBoolToString(IsActive());

	// type styles

	const JSize typeCount = itsTypeStyles->GetItemCount();
	output << ' ' << typeCount;

	for (JIndex i=1; i<=typeCount; i++)
	{
		WriteStyle(output, itsTypeStyles->GetItem(i));
	}

	output << ' ' << itsTypeNameVersion;

	// word styles

	output << ' ' << itsWordStyles->GetItemCount();

	JStringMapCursor<JFontStyle> cursor(itsWordStyles);
	while (cursor.Next())
	{
		output << ' ' << cursor.GetKey();
		WriteStyle(output, cursor.GetValue());
	}

	// dialog geometry

	output << ' ' << itsDialogGeom;
}

/******************************************************************************
 WriteStyle (private)

 ******************************************************************************/

void
StylerBase::WriteStyle
	(
	std::ostream&		output,
	const JFontStyle&	style
	)
	const
{
	output << ' ' << JBoolToString(style.bold);
	output << ' ' << JBoolToString(style.italic);
	output << ' ' << style.underlineCount;
	output << ' ' << JBoolToString(style.strike);

	output << ' ' << JColorManager::GetRGB(style.color);
}

/******************************************************************************
 GetWordList (private)

	Turns wordStyles into an array.

 ******************************************************************************/

void
StylerBase::GetWordList
	(
	const JStringMap<JFontStyle>&	wordStyles,
	JArray<WordStyle>*				wordList,
	const bool						sort
	)
	const
{
	wordList->RemoveAll();
	wordList->SetSortOrder(JListT::kSortAscending);
	wordList->SetCompareFunction(CompareWords);

	JStringMapCursor<JFontStyle> cursor(&wordStyles);
	while (cursor.Next())
	{
		const WordStyle data(jnew JString(cursor.GetKey()), cursor.GetValue());
		assert( data.key != nullptr );
		if (sort)
		{
			wordList->InsertSorted(data);
		}
		else
		{
			wordList->AppendItem(data);
		}
	}
}

/******************************************************************************
 CompareWords (static private)

 ******************************************************************************/

std::weak_ordering
StylerBase::CompareWords
	(
	const WordStyle& w1,
	const WordStyle& w2
	)
{
	return JIntToWeakOrdering(JString::Compare(*w1.key, *w2.key, JString::kIgnoreCase));
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
StylerBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
#if defined CODE_CRUSADER && ! defined CODE_CRUSADER_UNIT_TEST

	if (sender == GetPrefsManager() &&
		message.Is(PrefsManager::kTextColorChanged))
	{
		SetDefaultFontColor(GetPrefsManager()->GetColor(PrefsManager::kTextColorIndex));
	}

	else
	{
		JBroadcaster::Receive(sender, message);
	}

#endif
}

/******************************************************************************
 SetDefaultFontColor (private)

	Called when user changes default font color.  We change all styles
	that have the same color as itsDefColor.

 ******************************************************************************/

void
StylerBase::SetDefaultFontColor
	(
	const JColorID color
	)
{
	if (color != itsDefColor)
	{
		for (JIndex i=1; i<=itsTypeNameCount; i++)
		{
			JFontStyle style = itsTypeStyles->GetItem(i);
			if (style.color == itsDefColor)
			{
				style.color = color;
				itsTypeStyles->SetItem(i, style);
			}
		}

		JStringMapCursor<JFontStyle> cursor(itsWordStyles);
		while (cursor.Next())
		{
			JFontStyle style = cursor.GetValue();
			if (style.color == itsDefColor)
			{
				style.color = color;
				itsWordStyles->SetOldElement(cursor.GetKey(), style);
			}
		}

		itsDefColor = color;
	}
}

/******************************************************************************
 EditStyles

 ******************************************************************************/

#if defined CODE_CRUSADER && ! defined CODE_CRUSADER_UNIT_TEST

void
StylerBase::EditStyles()
{
	JArray<WordStyle> wordList;
	GetWordList(*itsWordStyles, &wordList, true);

	auto* dlog = jnew EditStylerDialog(itsDialogTitle, IsActive(),
									   itsTypeNames, *itsTypeStyles,
									   wordList, itsFileType);
	assert( dlog != nullptr );

	for (auto& ws : wordList)
	{
		jdelete ws.key;
	}

	JXWindow* window = dlog->GetWindow();
	window->ReadGeometry(itsDialogGeom);
	window->Deiconify();

	if (dlog->DoDialog())
	{
		bool active;
		JArray<JFontStyle> newTypeStyles = *itsTypeStyles;
		JStringMap<JFontStyle> newWordStyles;
		dlog->GetData(&active, &newTypeStyles, &newWordStyles);
		if (active != IsActive() ||
			TypeStylesChanged(newTypeStyles) ||
			WordStylesChanged(newWordStyles))
		{
			dlog->GetData(&active, itsTypeStyles, itsWordStyles);

			SetActive(active);
			GetDocumentManager()->StylerChanged(this);

			Broadcast(WordListChanged(*itsWordStyles));
		}
	}

	dlog->GetWindow()->WriteGeometry(&itsDialogGeom);
}

#endif

/******************************************************************************
 TypeStylesChanged (private)

 ******************************************************************************/

bool
StylerBase::TypeStylesChanged
	(
	const JArray<JFontStyle>& newTypeStyles
	)
	const
{
	const JSize count = itsTypeStyles->GetItemCount();
	assert( newTypeStyles.GetItemCount() == count );

	for (JIndex i=1; i<=count; i++)
	{
		if (itsTypeStyles->GetItem(i) != newTypeStyles.GetItem(i))
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 WordStylesChanged (private)

 ******************************************************************************/

bool
StylerBase::WordStylesChanged
	(
	const JStringMap<JFontStyle>& newWordStyles
	)
	const
{
	const JSize count = itsWordStyles->GetItemCount();
	if (newWordStyles.GetItemCount() != count)
	{
		return true;
	}
	else if (count == 0)
	{
		return false;
	}

	JStringMapCursor<JFontStyle> cursor(itsWordStyles);
	JFontStyle newStyle;
	while (cursor.Next())
	{
		if (!newWordStyles.GetItem(cursor.GetKey(), &newStyle) ||
			newStyle != cursor.GetValue())
		{
			return true;
		}
	}

	return false;
}

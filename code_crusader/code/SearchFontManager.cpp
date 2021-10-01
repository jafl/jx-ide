/******************************************************************************
 SearchFontManager.cpp

	BASE CLASS = JFontManager

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "SearchFontManager.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SearchFontManager::SearchFontManager()
	:
	JFontManager()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SearchFontManager::~SearchFontManager()
{
}

/******************************************************************************
 GetFontNames (virtual)

 ******************************************************************************/

void
SearchFontManager::GetFontNames
	(
	JPtrArray<JString>* fontNames
	)
{
	fontNames->CleanOut();
}

/******************************************************************************
 GetMonospaceFontNames (virtual)

 ******************************************************************************/

void
SearchFontManager::GetMonospaceFontNames
	(
	JPtrArray<JString>* fontNames
	)
{
	fontNames->CleanOut();
}

/******************************************************************************
 GetFontSizes (virtual)

 ******************************************************************************/

bool
SearchFontManager::GetFontSizes
	(
	const JString&	name,
	JSize*			minSize,
	JSize*			maxSize,
	JArray<JSize>*	sizeList
	)
{
	*minSize = *maxSize = 0;
	sizeList->RemoveAll();
	return false;
}

/******************************************************************************
 GetLineHeight (virtual)

 ******************************************************************************/

JSize
SearchFontManager::GetLineHeight
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,

	JCoordinate*		ascent,
	JCoordinate*		descent
	)
{
	*ascent  = 1;
	*descent = 1;
	return (*ascent + *descent);
}

/******************************************************************************
 GetCharWidth (virtual)

 ******************************************************************************/

JSize
SearchFontManager::GetCharWidth
	(
	const JFontID			fontID,
	const JUtf8Character&	c
	)
{
	return 1;
}

/******************************************************************************
 GetStringWidth (virtual)

 ******************************************************************************/

JSize
SearchFontManager::GetStringWidth
	(
	const JFontID	fontID,
	const JString&	str
	)
{
	return str.GetCharacterCount();
}

/******************************************************************************
 IsExact (virtual)

 ******************************************************************************/

bool
SearchFontManager::IsExact
	(
	const JFontID id
	)
{
	return true;
}

/******************************************************************************
 HasGlyphForCharacter (virtual)

 ******************************************************************************/

bool
SearchFontManager::HasGlyphForCharacter
	(
	const JFontID			id,
	const JUtf8Character&	c
	)
{
	return true;
}

/******************************************************************************
 GetSubstituteFontName (virtual protected)

 ******************************************************************************/

bool
SearchFontManager::GetSubstituteFontName
	(
	const JFont&			f,
	const JUtf8Character&	c,
	JString*				name
	)
{
	return false;
}

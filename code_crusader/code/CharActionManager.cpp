/******************************************************************************
 CharActionManager.cpp

	Stores a list of characters and associated actions.  The action is
	performed immediately after the character is typed.

	BASE CLASS = none

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CharActionManager.h"
#include "MacroManager.h"
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion   = 1;
const unsigned char kEndOfSetupListMarker = '\1';

	// version 1 no longer stores dialog geometry or column width

/******************************************************************************
 Constructor

 ******************************************************************************/

CharActionManager::CharActionManager()
{
	itsActionMap = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
	assert( itsActionMap != nullptr );
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

CharActionManager::CharActionManager
	(
	const CharActionManager& source
	)
{
	itsActionMap = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
	assert( itsActionMap != nullptr );

	JStringMapCursor cursor(source.itsActionMap);
	while (cursor.Next())
	{
		itsActionMap->SetNewElement(
			cursor.GetKey().GetFirstCharacter(),
			*cursor.GetValue());
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CharActionManager::~CharActionManager()
{
	jdelete itsActionMap;
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CharActionManager::Perform
	(
	const JUtf8Character&	c,
	TextDocument*			doc
	)
{
	JString* script;
	if (itsActionMap->GetItem(JString(c), &script))
	{
		MacroManager::Perform(*script, doc);
	}
}

/******************************************************************************
 SetAction

 ******************************************************************************/

void
CharActionManager::SetAction
	(
	const JUtf8Character&	c,
	const JString&			script
	)
{
	if (script.IsEmpty())
	{
		ClearAction(c);
	}
	else
	{
		itsActionMap->SetItem(JString(c), script, JPtrArrayT::kDelete);
	}
}

/******************************************************************************
 ClearAction

 ******************************************************************************/

void
CharActionManager::ClearAction
	(
	const JUtf8Character& c
	)
{
	itsActionMap->DeleteItem(JString(c));
}

/******************************************************************************
 ClearAllActions

 ******************************************************************************/

void
CharActionManager::ClearAllActions()
{
	itsActionMap->CleanOut();
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CharActionManager::ReadSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
	{
		return;
	}

	ClearAllActions();

	input.ignore(1);

	JUtf8Character c;
	JString script;
	while (true)
	{
		input >> c;
		if (c == kEndOfSetupListMarker)
		{
			break;
		}
		input >> script;
		SetAction(c, script);
	}

	if (vers == 0)
	{
		JString geom;
		JCoordinate width;
		input >> geom >> width;
	}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CharActionManager::WriteSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion << ' ';

	JStringMapCursor cursor(itsActionMap);
	while (cursor.Next())
	{
		output << cursor.GetKey().GetFirstCharacter();
		output << ' ' << *cursor.GetValue();
	}

	output << kEndOfSetupListMarker;
}

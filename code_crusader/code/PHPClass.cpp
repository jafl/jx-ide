/******************************************************************************
 PHPClass.cpp

	BASE CLASS = Class

	Copyright © 2014 John Lindal.

 ******************************************************************************/

#include "PHPClass.h"
#include "Tree.h"
#include "TextDocument.h"
#include "TextEditor.h"
#include "globals.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PHPClass::PHPClass
	(
	const JString&		name,
	const DeclareType	declType,
	const JFAID_t		fileID,
	Tree*				tree,
	const bool		isFinal
	)
	:
	Class(name, declType, fileID, tree),
	itsIsFinalFlag(isFinal)
{
}

PHPClass::PHPClass
	(
	std::istream&			input,
	const JFileVersion	vers,
	Tree*				tree
	)
	:
	Class(input, vers, tree)
{
	input >> JBoolFromString(itsIsFinalFlag);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PHPClass::~PHPClass()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
PHPClass::StreamOut
	(
	std::ostream& output
	)
	const
{
	Class::StreamOut(output);

	output << ' ' << JBoolToString(itsIsFinalFlag);
	output << ' ';
}

/******************************************************************************
 ViewSource (virtual)

 ******************************************************************************/

void
PHPClass::ViewSource()
	const
{
#ifndef CODE_CRUSADER_UNIT_TEST

	JString fileName;
	if (GetFileName(&fileName))
	{
		GetDocumentManager()->OpenTextDocument(fileName);
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("NoGhostFile::TreeWidget"));
	}

#endif
}

/******************************************************************************
 ViewHeader (virtual)

 ******************************************************************************/

void
PHPClass::ViewHeader()
	const
{
}

/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost PHPClass.

 ******************************************************************************/

Class*
PHPClass::NewGhost
	(
	const JString&	name,
	Tree*			tree
	)
{
	auto* newClass = jnew PHPClass(name, kGhostType, JFAID::kInvalidID, tree,
										  false);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 AdjustNameStyle (virtual protected)

 ******************************************************************************/

void
PHPClass::AdjustNameStyle
	(
	JFontStyle* style
	)
	const
{
	Class::AdjustNameStyle(style);

	if (GetDeclareType() != kGhostType && itsIsFinalFlag)
	{
		style->bold = true;
	}
}

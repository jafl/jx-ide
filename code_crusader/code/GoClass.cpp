/******************************************************************************
 GoClass.cpp

	BASE CLASS = Class

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#include "GoClass.h"
#include "Tree.h"
#include "TextDocument.h"
#include "TextEditor.h"
#include "globals.h"
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GoClass::GoClass
	(
	const JString&		name,
	const DeclareType	declType,
	const JFAID_t		fileID,
	Tree*				tree,
	const bool			isPublic
	)
	:
	Class(name, declType, fileID, tree),
	itsIsPublicFlag(isPublic)
{
}

GoClass::GoClass
	(
	std::istream&		input,
	const JFileVersion	vers,
	Tree*				tree
	)
	:
	Class(input, vers, tree),
	itsIsPublicFlag(true)
{
	if (vers >= 88)
	{
		input >> JBoolFromString(itsIsPublicFlag);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GoClass::~GoClass()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
GoClass::StreamOut
	(
	std::ostream& output
	)
	const
{
	Class::StreamOut(output);

	output << ' ' << JBoolToString(itsIsPublicFlag);
	output << ' ';
}

/******************************************************************************
 ViewSource (virtual)

 ******************************************************************************/

void
GoClass::ViewSource()
	const
{
#ifndef CODE_CRUSADER_UNIT_TEST

	JString fileName;
	if (GetFileName(&fileName))
	{
		DocumentManager* docMgr = GetDocumentManager();

		TextDocument* doc = nullptr;
		if (docMgr->OpenTextDocument(fileName, 0, &doc))
		{
			JString p("struct[ \t\n]*");
			p += GetName();
			p += "\\b";
			const JRegex r(p);

			TextEditor* te = doc->GetTextEditor();
			te->SetCaretLocation(1);

			const JStyledText::TextIndex start(1,1);
			bool wrapped;
			const JStringMatch m =
				te->GetText()->SearchForward(start, r, false, false, &wrapped);
			if (!m.IsEmpty())
			{
				te->SelectLine(te->GetLineForChar(m.GetCharacterRange().first));
				te->ScrollForDefinition(kGoLang);
			}
			else
			{
				const JUtf8Byte* map[] =
			{
					"name", GetName().GetBytes()
			};
				const JString msg = JGetString("NoDefinition::Class", map, sizeof(map));
				JGetUserNotification()->ReportError(msg);
			}
		}
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
GoClass::ViewHeader()
	const
{
}

/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost GoClass.

 ******************************************************************************/

Class*
GoClass::NewGhost
	(
	const JString&	name,
	Tree*			tree
	)
{
	auto* newClass = jnew GoClass(name, kGhostType, JFAID::kInvalidID, tree,
										 true);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 AdjustNameStyle (virtual protected)

 ******************************************************************************/

void
GoClass::AdjustNameStyle
	(
	JFontStyle* style
	)
	const
{
	Class::AdjustNameStyle(style);

	if (GetDeclareType() != kGhostType && !itsIsPublicFlag)
	{
		style->color = JColorManager::GetGrayColor(50);
	}
}

/******************************************************************************
 DClass.cpp

	BASE CLASS = Class

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#include "DClass.h"
#include "Tree.h"
#include "TextDocument.h"
#include "TextEditor.h"
#include "globals.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kNamespaceOperator = ".";

/******************************************************************************
 Constructor

 ******************************************************************************/

DClass::DClass
	(
	const JString&		name,
	const DeclareType	declType,
	const JFAID_t		fileID,
	Tree*				tree,
	const bool			isFinal
	)
	:
	Class(name, declType, fileID, tree, kNamespaceOperator),
	itsIsFinalFlag(isFinal)
{
}

DClass::DClass
	(
	std::istream&		input,
	const JFileVersion	vers,
	Tree*				tree
	)
	:
	Class(input, vers, tree, kNamespaceOperator),
	itsIsFinalFlag(false)
{
	if (vers >= 88)
	{
		input >> JBoolFromString(itsIsFinalFlag);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DClass::~DClass()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
DClass::StreamOut
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
DClass::ViewSource()
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
			JString p("(class|interface)[ \t\n]*");
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
				te->ScrollForDefinition(kDLang);
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
DClass::ViewHeader()
	const
{
}


/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost DClass.

 ******************************************************************************/

Class*
DClass::NewGhost
	(
	const JString&	name,
	Tree*			tree
	)
{
	auto* newClass = jnew DClass(name, kGhostType, JFAID::kInvalidID, tree, false);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 AdjustNameStyle (virtual protected)

 ******************************************************************************/

void
DClass::AdjustNameStyle
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

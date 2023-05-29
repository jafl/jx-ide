/******************************************************************************
 JavaClass.cpp

	BASE CLASS = Class

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#include "JavaClass.h"
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

JavaClass::JavaClass
	(
	const JString&		name,
	const DeclareType	declType,
	const JFAID_t		fileID,
	Tree*				tree,
	const bool			isPublic,
	const bool			isFinal
	)
	:
	Class(name, declType, fileID, tree),
	itsIsPublicFlag(isPublic),
	itsIsFinalFlag(isFinal)
{
}

JavaClass::JavaClass
	(
	std::istream&		input,
	const JFileVersion	vers,
	Tree*				tree
	)
	:
	Class(input, vers, tree),
	itsIsPublicFlag(true),
	itsIsFinalFlag(false)
{
	if (vers >= 52)
	{
		input >> JBoolFromString(itsIsPublicFlag)
			  >> JBoolFromString(itsIsFinalFlag);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JavaClass::~JavaClass()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
JavaClass::StreamOut
	(
	std::ostream& output
	)
	const
{
	Class::StreamOut(output);

	output << ' ' << JBoolToString(itsIsPublicFlag)
				  << JBoolToString(itsIsFinalFlag);
	output << ' ';
}

/******************************************************************************
 ViewSource (virtual)

 ******************************************************************************/

void
JavaClass::ViewSource()
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
				te->ScrollForDefinition(kJavaLang);
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
JavaClass::ViewHeader()
	const
{
}

/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost JavaClass.

 ******************************************************************************/

Class*
JavaClass::NewGhost
	(
	const JString&	name,
	Tree*			tree
	)
{
	auto* newClass = jnew JavaClass(name, kGhostType, JFAID::kInvalidID, tree,
											true, false);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 AdjustNameStyle (virtual protected)

 ******************************************************************************/

void
JavaClass::AdjustNameStyle
	(
	JFontStyle* style
	)
	const
{
	Class::AdjustNameStyle(style);

	if (GetDeclareType() != kGhostType)
	{
		if (!itsIsPublicFlag)
		{
			style->color = JColorManager::GetGrayColor(50);
		}

		if (itsIsFinalFlag)
		{
			style->bold = true;
		}
	}
}

/******************************************************************************
 GetAncestorList (virtual)

	We must not include the package.

 ******************************************************************************/

void
JavaClass::GetAncestorList
	(
	JPtrArray<JString>* list
	)
	const
{
	for (const auto* s : *list)
	{
		if (*s == GetName())
		{
			return;
		}
	}

	list->Append(GetName());

	const JSize count = GetParentCount();
	const Class* parent;
	for (JIndex i=1; i<=count; i++)
	{
		if (GetParent(i, &parent))
		{
			parent->GetAncestorList(list);
		}
	}
}

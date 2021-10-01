/******************************************************************************
 CClass.cpp

	BASE CLASS = Class

	Copyright © 1996-99 John Lindal.

 ******************************************************************************/

#include "CClass.h"
#include "Tree.h"
#include "TextDocument.h"
#include "TextEditor.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kNamespaceOperator = "::";

/******************************************************************************
 Constructor

 ******************************************************************************/

CClass::CClass
	(
	const JString&		name,
	const DeclareType	declType,
	const JFAID_t		fileID,
	Tree*				tree
	)
	:
	Class(name, declType, fileID, tree, kNamespaceOperator)
{
}

CClass::CClass
	(
	std::istream&			input,
	const JFileVersion	vers,
	Tree*				tree
	)
	:
	Class(input, vers, tree, kNamespaceOperator)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CClass::~CClass()
{
}

/******************************************************************************
 ViewSource (virtual)

 ******************************************************************************/

void
CClass::ViewSource()
	const
{
#ifndef CODE_CRUSADER_UNIT_TEST

	DocumentManager* docMgr = GetDocumentManager();

	JString headerName;
	if (!GetFileName(&headerName))
	{
		JGetUserNotification()->ReportError(JGetString("NoGhostFile::TreeWidget"));
	}
	else
	{
		docMgr->OpenComplementFile(headerName, kCHeaderFT,
								   GetTree()->GetProjectDoc());
	}

#endif
}

/******************************************************************************
 ViewHeader (virtual)

 ******************************************************************************/

void
CClass::ViewHeader()
	const
{
#ifndef CODE_CRUSADER_UNIT_TEST

	JString headerName;
	if (GetFileName(&headerName))
	{
		GetDocumentManager()->OpenTextDocument(headerName);
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("NoGhostFile::TreeWidget"));
	}

#endif
}


/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost CClass.

 ******************************************************************************/

Class*
CClass::NewGhost
	(
	const JString&	name,
	Tree*			tree
	)
{
	auto* newClass = jnew CClass(name, kGhostType, JFAID::kInvalidID, tree);
	assert( newClass != nullptr );
	return newClass;
}

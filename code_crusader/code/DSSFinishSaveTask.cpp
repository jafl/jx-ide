/******************************************************************************
 DSSFinishSaveTask.cpp

	BASE CLASS = JXDirectSaveAction

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "DSSFinishSaveTask.h"
#include "TextDocument.h"
#include "TextEditor.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DSSFinishSaveTask::DSSFinishSaveTask
	(
	TextDocument* doc
	)
	:
	JXDirectSaveAction(),
	itsDoc(doc)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DSSFinishSaveTask::~DSSFinishSaveTask()
{
}

/******************************************************************************
 Save (virtual)

 ******************************************************************************/

void
DSSFinishSaveTask::Save
	(
	const JString& fullName
	)
{
	(itsDoc->GetTextEditor())->Focus();		// revert file display before save
	itsDoc->SaveInNewFile(fullName);
}

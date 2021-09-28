/******************************************************************************
 CBDSSFinishSaveTask.cpp

	BASE CLASS = JXDirectSaveAction

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "CBDSSFinishSaveTask.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBDSSFinishSaveTask::CBDSSFinishSaveTask
	(
	CBTextDocument* doc
	)
	:
	JXDirectSaveAction(),
	itsDoc(doc)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDSSFinishSaveTask::~CBDSSFinishSaveTask()
{
}

/******************************************************************************
 Save (virtual)

 ******************************************************************************/

void
CBDSSFinishSaveTask::Save
	(
	const JString& fullName
	)
{
	(itsDoc->GetTextEditor())->Focus();		// revert file display before save
	itsDoc->SaveInNewFile(fullName);
}

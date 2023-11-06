/******************************************************************************
 CommandOutputDocument.cpp

	BASE CLASS = TextDocument

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#include "CommandOutputDocument.h"
#include "TextEditor.h"
#include "CommandOutputPostFTCTask.h"
#include "globals.h"
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* CommandOutputDocument::kFinished = "Finished::CommandOutputDocument";

/******************************************************************************
 Constructor

 ******************************************************************************/

CommandOutputDocument::CommandOutputDocument
	(
	const TextFileType	fileType,
	const JUtf8Byte*	helpSectionName,
	const JString&		dontCloseMessage
	)
	:
	TextDocument(fileType, helpSectionName, false, ConstructTextEditor),
	itsUseCount(0),
	itsDontCloseMsg(dontCloseMessage)
{
	JXUrgentTask* task = jnew CommandOutputPostFTCTask(this);
	task->Go();

	JXGetDocumentManager()->DocumentMustStayOpen(this, true);
}

// static private

TextEditor*
CommandOutputDocument::ConstructTextEditor
	(
	TextDocument*		document,
	const JString&		fileName,
	JXMenuBar*			menuBar,
	TELineIndexInput*	lineInput,
	TEColIndexInput*	colInput,
	JXScrollbarSet*		scrollbarSet
	)
{
	auto* te =
		jnew TextEditor(document, fileName, menuBar, lineInput, colInput, true,
						scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( te != nullptr );

	return te;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CommandOutputDocument::~CommandOutputDocument()
{
}

/******************************************************************************
 OKToClose (virtual protected)

 ******************************************************************************/

bool
CommandOutputDocument::OKToClose()
{
	if (itsUseCount > 0 || CommandRunning())
	{
		Activate();
		JGetUserNotification()->ReportError(itsDontCloseMsg);
		return false;
	}
	else
	{
		return TextDocument::OKToClose();
	}
}

/******************************************************************************
 DecrementUseCount

	Not inline because we use assert().

 ******************************************************************************/

void
CommandOutputDocument::DecrementUseCount()
{
	assert( itsUseCount > 0 );
	itsUseCount--;
	UseCountUpdated(itsUseCount);
}

/******************************************************************************
 UseCountUpdated (virtual protected)

 ******************************************************************************/

void
CommandOutputDocument::UseCountUpdated
	(
	const JSize count
	)
{
}

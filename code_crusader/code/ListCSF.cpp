/******************************************************************************
 ListCSF.cpp

	BASE CLASS = JXChooseSaveFile

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "ListCSF.h"
#include "ListChooseFileDialog.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ListCSF::ListCSF
	(
	const JString& replaceListStr,
	const JString& appendToListStr
	)
	:
	JXChooseSaveFile()
{
	itsChooseDialog        = nullptr;
	itsReplaceExistingFlag = false;
	itsReplaceListStr      = replaceListStr;
	itsAppendToListStr     = appendToListStr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ListCSF::~ListCSF()
{
}

/******************************************************************************
 CreateChooseFileDialog (virtual protected)

 ******************************************************************************/

JXChooseFileDialog*
ListCSF::CreateChooseFileDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const bool	allowSelectMultiple,
	const JString&	origName,
	const JString&	message
	)
{
	itsChooseDialog =
		ListChooseFileDialog::Create(supervisor, dirInfo, fileFilter,
									   allowSelectMultiple,
									   itsReplaceListStr, itsAppendToListStr,
									   origName, message);
	return itsChooseDialog;
}

/******************************************************************************
 Receive (protected)

	Listen for response from itsChooseDialog.

 ******************************************************************************/

void
ListCSF::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsChooseDialog && message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			itsReplaceExistingFlag = itsChooseDialog->ReplaceExisting();
		}
		itsChooseDialog = nullptr;
	}

	JXChooseSaveFile::Receive(sender, message);
}

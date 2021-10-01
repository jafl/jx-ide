/******************************************************************************
 util.cpp

	Copyright © 2006 John Lindal.

 ******************************************************************************/

#include "DocumentManager.h"
#include "util.h"
#include "globals.h"
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 CleanUpAfterRename

 ******************************************************************************/

void
CleanUpAfterRename
	(
	const JString& origFullName,
	const JString& newFullName
	)
{
	JString backupName = origFullName;
	backupName        += "~";
	JRemoveFile(backupName);

	JString path, name;
	JSplitPathAndName(origFullName, &path, &name);
	const JString safetyName = path + "#" + name + "#";
	JRemoveFile(safetyName);
	const JString crashName = path + "##" + name + "##";
	JRemoveFile(crashName);

	if (!newFullName.IsEmpty())
	{
		GetDocumentManager()->FileRenamed(origFullName, newFullName);
	}
}

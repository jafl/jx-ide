/******************************************************************************
 GetSourceFileListCmd.cpp

	BASE CLASS = GetSourceFileListCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jvm/GetSourceFileListCmd.h"
#include "FileListDir.h"
#include "jvm/Link.h"
#include "globals.h"
#include <jx-af/jx/JXFileListTable.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetSourceFileListCmd::GetSourceFileListCmd
	(
	FileListDir* fileList
	)
	:
	::GetSourceFileListCmd("NOP", fileList)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetSourceFileListCmd::~GetSourceFileListCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
jvm::GetSourceFileListCmd::Starting()
{
	::GetSourceFileListCmd::Starting();

	dynamic_cast<Link*>(GetLink())->FlushClassList();

	JXFileListTable* table = GetFileList()->GetTable();
	table->RemoveAllFiles();

	const JPtrArray<JString>& list = dynamic_cast<Link*>(GetLink())->GetSourcePathList();
	const JSize count              = list.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		ScanDirectory(*(list.GetItem(i)));
	}
}

/******************************************************************************
 ScanDirectory (private)

 *****************************************************************************/

void
jvm::GetSourceFileListCmd::ScanDirectory
	(
	const JString& path
	)
{
	JDirInfo* info;
	if (!JDirInfo::Create(path, &info))
	{
		return;
	}

	JXFileListTable* table = GetFileList()->GetTable();

	const JSize count = info->GetEntryCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JDirEntry& e = info->GetEntry(i);
		if (e.GetType() == JDirEntry::kFile)
		{
			const TextFileType fileType =
				GetPrefsManager()->GetFileType(e.GetName());
			if (fileType == kJavaSourceFT)
			{
				table->AddFile(e.GetFullName());
			}
		}
		else if (e.GetType() == JDirEntry::kDir)
		{
			ScanDirectory(e.GetFullName());
		}
	}

	jdelete info;
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetSourceFileListCmd::HandleSuccess
	(
	const JString& origData
	)
{
}

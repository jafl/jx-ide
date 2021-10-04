/******************************************************************************
 XDGetSourceFileList.cpp

	BASE CLASS = GetSourceFileListCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetSourceFileList.h"
#include "FileListDir.h"
#include "XDLink.h"
#include "globals.h"
#include <jx-af/jx/JXFileListTable.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetSourceFileList::XDGetSourceFileList
	(
	FileListDir* fileList
	)
	:
	GetSourceFileListCmd(JString("status", JString::kNoCopy), fileList)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetSourceFileList::~XDGetSourceFileList()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
XDGetSourceFileList::Starting()
{
	GetSourceFileListCmd::Starting();

	JXFileListTable* table = GetFileList()->GetTable();
	table->RemoveAllFiles();

	const JPtrArray<JString>& list = dynamic_cast<XDLink*>(GetLink())->GetSourcePathList();
	const JSize count              = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		ScanDirectory(*(list.GetElement(i)));
	}
}

/******************************************************************************
 ScanDirectory (private)

 *****************************************************************************/

void
XDGetSourceFileList::ScanDirectory
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
			if (fileType == kPHPFT)
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
XDGetSourceFileList::HandleSuccess
	(
	const JString& origData
	)
{
}

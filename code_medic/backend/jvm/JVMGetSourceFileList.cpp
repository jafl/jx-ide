/******************************************************************************
 JVMGetSourceFileList.cpp

	BASE CLASS = GetSourceFileList

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetSourceFileList.h"
#include "FileListDir.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jx/JXFileListTable.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetSourceFileList::JVMGetSourceFileList
	(
	FileListDir* fileList
	)
	:
	GetSourceFileList(JString("NOP", JString::kNoCopy), fileList)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetSourceFileList::~JVMGetSourceFileList()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
JVMGetSourceFileList::Starting()
{
	GetSourceFileList::Starting();

	dynamic_cast<JVMLink*>(GetLink())->FlushClassList();

	JXFileListTable* table = GetFileList()->GetTable();
	table->RemoveAllFiles();

	const JPtrArray<JString>& list = dynamic_cast<JVMLink*>(GetLink())->GetSourcePathList();
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
JVMGetSourceFileList::ScanDirectory
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
JVMGetSourceFileList::HandleSuccess
	(
	const JString& origData
	)
{
}

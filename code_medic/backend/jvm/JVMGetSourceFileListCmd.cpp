/******************************************************************************
 JVMGetSourceFileListCmd.cpp

	BASE CLASS = GetSourceFileListCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetSourceFileListCmd.h"
#include "FileListDir.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jx/JXFileListTable.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetSourceFileListCmd::JVMGetSourceFileListCmd
	(
	FileListDir* fileList
	)
	:
	GetSourceFileListCmd(JString("NOP", JString::kNoCopy), fileList)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetSourceFileListCmd::~JVMGetSourceFileListCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
JVMGetSourceFileListCmd::Starting()
{
	GetSourceFileListCmd::Starting();

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
JVMGetSourceFileListCmd::ScanDirectory
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
JVMGetSourceFileListCmd::HandleSuccess
	(
	const JString& origData
	)
{
}

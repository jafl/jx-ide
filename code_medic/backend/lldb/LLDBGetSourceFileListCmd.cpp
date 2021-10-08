/******************************************************************************
 LLDBGetSourceFileListCmd.cpp

	BASE CLASS = GetSourceFileListCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetSourceFileListCmd.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBModule.h"
#include "lldb/API/SBCompileUnit.h"
#include "FileListDir.h"
#include "LLDBLink.h"
#include "globals.h"
#include <jx-af/jx/JXFileListTable.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::GetSourceFileListCmd::GetSourceFileListCmd
	(
	FileListDir* fileList
	)
	:
	::GetSourceFileListCmd(JString::empty, fileList)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::GetSourceFileListCmd::~GetSourceFileListCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
lldb::GetSourceFileListCmd::HandleSuccess
	(
	const JString& origData
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link == nullptr)
	{
		return;
	}

	SBTarget t = link->GetDebugger()->GetSelectedTarget();
	if (t.IsValid())
	{
		JXGetApplication()->DisplayBusyCursor();

		JXFileListTable* table = GetFileList()->GetTable();
		table->RemoveAllFiles();

		JString fullName;

		const JSize mCount = t.GetNumModules();
		for (JUnsignedOffset i=0; i<mCount; i++)
		{
			SBModule m   = t.GetModuleAtIndex(i);
			const JSize uCount = m.GetNumCompileUnits();
			for (JUnsignedOffset j=0; j<uCount; j++)
			{
				SBCompileUnit u = m.GetCompileUnitAtIndex(j);
				SBFileSpec f    = u.GetFileSpec();

				if (f.GetDirectory() != nullptr && f.GetFilename() != nullptr)
				{
					fullName = JCombinePathAndName(
						JString(f.GetDirectory(), JString::kNoCopy),
						JString(f.GetFilename(), JString::kNoCopy));

					table->AddFile(fullName);
				}
			}
		}
	}
}

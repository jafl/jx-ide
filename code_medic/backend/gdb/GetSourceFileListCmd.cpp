/******************************************************************************
 GetSourceFileListCmd.cpp

	BASE CLASS = GetSourceFileListCmd

	We do not use -file-list-exec-source-files because it generates much
	more data and the output takes too long to parse.

	Copyright (C) 2001-10 by John Lindal.

 ******************************************************************************/

#include "gdb/GetSourceFileListCmd.h"
#include "FileListDir.h"
#include "globals.h"
#include <jx-af/jx/JXFileListTable.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetSourceFileListCmd::GetSourceFileListCmd
	(
	FileListDir* fileList
	)
	:
	::GetSourceFileListCmd(JString("info sources", JString::kNoCopy), fileList)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetSourceFileListCmd::~GetSourceFileListCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
gdb::GetSourceFileListCmd::HandleSuccess
	(
	const JString& data
	)
{
	JStringIterator iter(data);
	if (iter.Next("\n\n"))
	{
		JXGetApplication()->DisplayBusyCursor();

		JXFileListTable* table = GetFileList()->GetTable();
		table->RemoveAllFiles();

		const JIndex offset = iter.GetPrevByteIndex();
		icharbufstream input(data.GetRawBytes() + offset, data.GetByteCount() - offset);
		JString fullName, path, name, s;
		bool foundDelimiter;
		do
		{
			input >> std::ws;
			fullName = JReadUntil(input, ',', &foundDelimiter);
			fullName.TrimWhitespace();
			if (!fullName.IsEmpty())
			{
				JSplitPathAndName(fullName, &path, &name);
				table->AddFile(name);
			}
		}
			while (foundDelimiter);
	}
	else
	{
		std::ostringstream log;
		log << "GDBGetSourceFileListCmd failed to match";
		Link::Log(log);
	}
}

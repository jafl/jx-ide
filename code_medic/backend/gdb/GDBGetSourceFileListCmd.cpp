/******************************************************************************
 GDBGetSourceFileListCmd.cpp

	BASE CLASS = GetSourceFileListCmd

	We do not use -file-list-exec-source-files because it generates much
	more data and the output takes too long to parse.

	Copyright (C) 2001-10 by John Lindal.

 ******************************************************************************/

#include "GDBGetSourceFileListCmd.h"
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

static const JRegex textPattern = "Source files for which symbols[^:]+(:|$)";

void
gdb::GetSourceFileListCmd::HandleSuccess
	(
	const JString& origData
	)
{
	if (origData.BeginsWith("Source files for which symbols have been read in:"))
	{
		JXGetApplication()->DisplayBusyCursor();

		JXFileListTable* table = GetFileList()->GetTable();
		table->RemoveAllFiles();

		JString data = origData;

		JStringIterator iter(&data);
		while (iter.Next(textPattern))
		{
			iter.ReplaceLastMatch(",");
		}
		data.TrimWhitespace();		// no comma after last file

		icharbufstream input(data.GetRawBytes(), data.GetByteCount());
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
		GetLink()->Log(log);
	}
}

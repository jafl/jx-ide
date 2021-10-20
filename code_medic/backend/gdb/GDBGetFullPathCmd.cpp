/******************************************************************************
 GDBGetFullPathCmd.cpp

	Each file has to be checked separately.  The compilation directory does
	not necessarily have anything to do with the file's path.  Only
	"Located in file_name" printed by "info source" is reliable.

	BASE CLASS = GetFullPathCmd

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBGetFullPathCmd.h"
#include "globals.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetFullPathCmd::GetFullPathCmd
	(
	const JString&	fileName,
	const JIndex	lineIndex	// for convenience
	)
	:
	::GetFullPathCmd(BuildCommand(fileName), fileName, lineIndex)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetFullPathCmd::~GetFullPathCmd()
{
}

/******************************************************************************
 BuildCommand (static private)

 ******************************************************************************/

JString
gdb::GetFullPathCmd::BuildCommand
	(
	const JString& fileName
	)
{
	JString cmd("list ");
	cmd += fileName;
	cmd += ":1\n-file-list-exec-source-file";
	return cmd;
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex redirectPattern = "file: \\\"([^\"]+)\\\", line number: 1\\n";
static const JRegex pathPattern     = ",fullname=\"([^\"]+)\"";

void
gdb::GetFullPathCmd::HandleSuccess
	(
	const JString& cmdData
	)
{
	const JStringMatch m1 = redirectPattern.Match(cmdData, JRegex::kIncludeSubmatches);
	if (!m1.IsEmpty())
	{
		const JString fullName = m1.GetSubstring(1);
		if (fullName == GetFileName())
		{
			Broadcast(FileNotFound(GetFileName()));
			GetLink()->RememberFile(GetFileName(), JString::empty);
		}
		else if (JIsAbsolutePath(fullName))
		{
			GetLink()->RememberFile(GetFileName(), fullName);
			Broadcast(FileFound(fullName, GetLineIndex()));
		}
		else
		{
			auto* cmd = jnew GetFullPathCmd(fullName, GetLineIndex());
			assert( cmd != nullptr );
			Broadcast(NewCommand(cmd));
		}
		return;
	}

	const JString& data = GetLastResult();
	bool fileOK     = false;

	const JPtrArray<JString>& resultList = GetResults();
	JString data1;
	if (resultList.GetElementCount() == 2)
	{
		data1 = *(resultList.GetFirstElement());
	}

	const JStringMatch m2 = pathPattern.Match(data, JRegex::kIncludeSubmatches);
	if (!data1.Contains("No source file") && !m2.IsEmpty())
	{
		const JString fullName = m2.GetSubstring(1);
		if (JFileReadable(fullName))
		{
			fileOK = true;
			GetLink()->RememberFile(GetFileName(), fullName);
			Broadcast(FileFound(fullName, GetLineIndex()));
		}
	}
	else
	{
		Link::Log("GDBGetFullPathCmd failed to match");
	}

	if (!fileOK)
	{
		Broadcast(FileNotFound(GetFileName()));
		GetLink()->RememberFile(GetFileName(), JString::empty);
	}
}

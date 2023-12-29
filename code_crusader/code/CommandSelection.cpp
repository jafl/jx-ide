/******************************************************************************
 CommandSelection.cpp

	text/x-jcc-command with DELETE support for DND.

	BASE CLASS = JXSelectionData

	Copyright © 2003 by John Lindal.

 ******************************************************************************/

#include "CommandSelection.h"
#include "CommandTable.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kCommandXAtomName = "text/x-jcc-command";

/******************************************************************************
 Constructor

 ******************************************************************************/

CommandSelection::CommandSelection
	(
	JXDisplay*						display,
	CommandTable*					table,
	const CommandManager::CmdInfo&	cmdInfo
	)
	:
	JXSelectionData(display),
	itsTable(table)
{
	assert( itsTable != nullptr );

	JPoint cell;
	const bool hasSelection =
		itsTable->GetTableSelection().GetSingleSelectedCell(&cell);
	assert( hasSelection );
	itsSrcRowIndex = cell.y;

	std::ostringstream data;
	CommandManager::WriteCmdInfo(data, cmdInfo);
	data << std::ends;
	itsData = data.str();

	ClearWhenGoingAway(itsTable, &itsTable);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CommandSelection::~CommandSelection()
{
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
CommandSelection::AddTypes
	(
	const Atom selectionName
	)
{
	itsCommandXAtom = AddType(kCommandXAtomName);
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

bool
CommandSelection::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	*bitsPerBlock = 8;

	JXSelectionManager* selMgr = GetSelectionManager();
	JPoint cell;

	if (requestType == itsCommandXAtom)
	{
		*returnType = itsCommandXAtom;
		*dataLength = itsData.GetByteCount();
		*data       = jnew_allow_null unsigned char[ *dataLength ];
		if (*data != nullptr)
		{
			memcpy(*data, itsData.GetRawBytes(), *dataLength);
			return true;
		}
	}

	else if (requestType == selMgr->GetDeleteSelectionXAtom() &&
			 itsTable != nullptr && IsCurrent() &&
			 itsTable->GetTableSelection().GetSingleSelectedCell(&cell) &&
			 JIndex(cell.y) == itsSrcRowIndex)
	{
		itsTable->RemoveCommand();

		*data       = jnew unsigned char[1];
		*dataLength = 0;
		*returnType = selMgr->GetNULLXAtom();
		return true;
	}

	*data       = nullptr;
	*dataLength = 0;
	*returnType = None;
	return false;
}

/******************************************************************************
 GetCommandXAtomName (static)

 ******************************************************************************/

const JUtf8Byte*
CommandSelection::GetCommandXAtomName()
{
	return kCommandXAtomName;
}

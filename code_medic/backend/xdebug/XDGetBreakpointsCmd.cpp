/******************************************************************************
 XDGetBreakpointsCmd.cpp

	This is the only way to get all the relevant information about each
	breakpoint.  gdb does not print enough information when "break" is
	used.

	BASE CLASS = GetBreakpointsCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetBreakpointsCmd.h"
#include "BreakpointManager.h"
#include "XDLink.h"
#include "globals.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::GetBreakpointsCmd::GetBreakpointsCmd()
	:
	::GetBreakpointsCmd(JString("breakpoint_list", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::GetBreakpointsCmd::~GetBreakpointsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

void
xdebug::GetBreakpointsCmd::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
	xmlNode* root;
	if (link == nullptr || !link->GetParsedData(&root))
	{
		return;
	}

	(GetLink()->GetBreakpointManager())->SetUpdateWhenStop(false);

	JPtrArray<Breakpoint> bpList(JPtrArrayT::kForgetAll);	// ownership taken by BreakpointManager
	bpList.SetCompareFunction(::BreakpointManager::CompareBreakpointLocations);
	bpList.SetSortOrder(JListT::kSortAscending);

	JPtrArray<Breakpoint> otherList(JPtrArrayT::kForgetAll);	// ownership taken by BreakpointManager

	xmlNode* node = root->children;
	JString type, idStr, fileName, lineStr, state;
	while (node != nullptr)
	{
		type = JGetXMLNodeAttr(node, "type");
		if (type == "line")
		{
			idStr    = JGetXMLNodeAttr(node, "id");
			fileName = JGetXMLNodeAttr(node, "filename");
			lineStr  = JGetXMLNodeAttr(node, "lineno");
			state    = JGetXMLNodeAttr(node, "state");

			JIndex bpIndex;
			bool ok = idStr.ConvertToUInt(&bpIndex);
			assert( ok );

			if (fileName.BeginsWith("file://"))
			{
				JStringIterator iter(&fileName);
				iter.RemoveNext(7);
			}

			JIndex lineNumber;
			ok = lineStr.ConvertToUInt(&lineNumber);
			assert( ok );

			const bool enabled = state == "enabled";

			auto* bp =
				jnew Breakpoint(bpIndex, fileName, lineNumber, JString::empty, JString::empty,
								 enabled, Breakpoint::kKeepBreakpoint, JString::empty, 0);
			assert( bp != nullptr );
			bpList.InsertSorted(bp);

			if (true)	// no way to know if it is temporary -- may be deleted or other status change
			{
				(GetLink()->GetBreakpointManager())->SetUpdateWhenStop(true);
			}
		}

		node = node->next;
	}

	(GetLink()->GetBreakpointManager())->UpdateBreakpoints(bpList, otherList);
}

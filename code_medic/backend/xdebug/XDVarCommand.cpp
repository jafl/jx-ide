/******************************************************************************
 XDVarCommand.cpp

	BASE CLASS = VarCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDVarCommand.h"
#include "XDGetContextVars.h"
#include "XDLink.h"
#include "VarNode.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDVarCommand::XDVarCommand
	(
	const JString& cmd
	)
	:
	VarCmd()
{
	SetCommand(cmd);

	itsRootNode = GetLink()->CreateVarNode(false);
	assert( itsRootNode != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDVarCommand::~XDVarCommand()
{
	jdelete itsRootNode;
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDVarCommand::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<XDLink*>(GetLink());
	xmlNode* root;
	if (link == nullptr || !link->GetParsedData(&root))
	{
		return;
	}

	XDGetContextVars::BuildTree(1, root, itsRootNode);
	if (itsRootNode->HasChildren())
	{
		Broadcast(ValueMessage(kValueUpdated, itsRootNode->GetVarChild(1)));
	}
	else
	{
		SetData(JString("<error reading value>", JString::kNoCopy));
		Broadcast(ValueMessage(kValueFailed, nullptr));
	}
	itsRootNode->DeleteAllChildren();
}

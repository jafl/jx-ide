/******************************************************************************
 XDVarCmd.cpp

	BASE CLASS = VarCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDVarCmd.h"
#include "XDGetContextVarsCmd.h"
#include "XDLink.h"
#include "VarNode.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::VarCmd::VarCmd
	(
	const JString& cmd
	)
	:
	::VarCmd()
{
	SetCommand(cmd);

	itsRootNode = GetLink()->CreateVarNode(false);
	assert( itsRootNode != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::VarCmd::~VarCmd()
{
	jdelete itsRootNode;
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
xdebug::VarCmd::HandleSuccess
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

	GetContextVarsCmd::BuildTree(1, root, itsRootNode);
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

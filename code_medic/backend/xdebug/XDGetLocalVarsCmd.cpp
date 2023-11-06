/******************************************************************************
 XDGetLocalVarsCmd.cpp

	BASE CLASS = GetLocalVarsCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetLocalVarsCmd.h"
#include "XDGetContextVarsCmd.h"
#include "XDLink.h"
#include "VarNode.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::GetLocalVarsCmd::GetLocalVarsCmd
	(
	VarNode* rootNode
	)
	:
	::GetLocalVarsCmd(JString("context_names", JString::kNoCopy)),
	itsRootNode(rootNode)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::GetLocalVarsCmd::~GetLocalVarsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
xdebug::GetLocalVarsCmd::HandleSuccess
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

	itsRootNode->DeleteAllChildren();

	xmlNode* node = root->children;
	JString id;
	while (node != nullptr)
	{
		if (JGetXMLNodeAttr(node, "name") != "Globals")
		{
			id = JGetXMLNodeAttr(node, "id");

			auto* cmd = jnew GetContextVarsCmd(itsRootNode, id);
			cmd->Send();
		}

		node = node->next;
	}
}

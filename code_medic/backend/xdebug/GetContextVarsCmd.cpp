/******************************************************************************
 GetContextVarsCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "xdebug/GetContextVarsCmd.h"
#include "xdebug/Link.h"
#include "VarNode.h"
#include "globals.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::GetContextVarsCmd::GetContextVarsCmd
	(
	::VarNode*		rootNode,
	const JString&	contextID
	)
	:
	Command("", true, true),
	itsRootNode(rootNode)
{
	JString cmd("context_get -c ");
	cmd += contextID;
	cmd += " -d ";
	cmd += JString((JUInt64) dynamic_cast<Link*>(GetLink())->GetStackFrameIndex());
	SetCommand(cmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::GetContextVarsCmd::~GetContextVarsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
xdebug::GetContextVarsCmd::HandleSuccess
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

	BuildTree(1, root, itsRootNode);
}

/******************************************************************************
 BuildTree (static)

 ******************************************************************************/

void
xdebug::GetContextVarsCmd::BuildTree
	(
	const JSize	depth,
	xmlNode*	root,
	::VarNode*	varRoot
	)
{
	xmlNode* node = root->children;
	JString name, fullName, type, value;
	while (node != nullptr)
	{
		name     = JGetXMLNodeAttr(node, (depth == 1 ? "fullname" : "name"));
		fullName = JGetXMLNodeAttr(node, "fullname");
		type     = JGetXMLNodeAttr(node, "type");

		::VarNode* n;
		if (node->children != nullptr && node->children->type == XML_TEXT_NODE)
		{
			value = (char*) node->children->content;

			const JString encoding = JGetXMLNodeAttr(node, "encoding");
			if (encoding == "base64")
			{
				value.DecodeBase64(&value);
			}

			if (type == "string")
			{
				JStringIterator iter(&value, JStringIterator::kStartAtEnd);
				JUtf8Character c;
				while (iter.Prev(&c))
				{
					if (c == '"' || c == '\\')
					{
						iter.Insert("\\");
					}
				}

				value.Prepend("\"");
				value.Append("\"");
			}

			n = GetLink()->CreateVarNode(nullptr, name, fullName, value);
			assert( n != nullptr );
		}
		else
		{
			n = GetLink()->CreateVarNode(nullptr, name, fullName, JString::empty);
			assert( n != nullptr );

			if (type == "array" && JGetXMLNodeAttr(node, "children") == "1")
			{
				n->MakePointer(true);
			}

			if (node->children != nullptr && node->children->type == XML_ELEMENT_NODE)
			{
				BuildTree(depth+1, node, n);
			}
		}

		varRoot->Append(n);	// avoid automatic update

		node = node->next;
	}
}

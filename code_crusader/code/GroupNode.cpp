/******************************************************************************
 GroupNode.cpp

	BASE CLASS = ProjectNode

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#include "GroupNode.h"
#include "ProjectTree.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

	The second version requires the parent to be specified because
	the tree hasn't been fully constructed yet.

 ******************************************************************************/

GroupNode::GroupNode
	(
	ProjectTree*	tree,
	const bool		append
	)
	:
	ProjectNode(tree, kGroupNT, JGetString("NewGroupName::GroupNode"), true)
{
	if (tree != nullptr && append)
	{
		tree->GetRoot()->Append(this);
	}
}

GroupNode::GroupNode
	(
	std::istream&		input,
	const JFileVersion	vers,
	ProjectNode*		parent
	)
	:
	ProjectNode(input, vers, parent, kGroupNT, true)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GroupNode::~GroupNode()
{
}

/******************************************************************************
 Print (virtual)

 ******************************************************************************/

void
GroupNode::Print
	(
	JString* text
	)
	const
{
	*text += "\n";
	*text += GetName();
	*text += "\n\n";

	ProjectNode::Print(text);
}

/******************************************************************************
 ShowFileLocation (virtual)

 ******************************************************************************/

void
GroupNode::ShowFileLocation()
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		(GetProjectChild(i))->ShowFileLocation();
	}
}

/******************************************************************************
 ViewPlainDiffs (virtual)

 ******************************************************************************/

void
GroupNode::ViewPlainDiffs
	(
	const bool origSilent
	)
	const
{
	const JSize count     = GetChildCount();
	const bool silent = origSilent || count > 1;
	for (JIndex i=1; i<=count; i++)
	{
		(GetProjectChild(i))->ViewPlainDiffs(silent);
	}
}

/******************************************************************************
 ViewVCSDiffs (virtual)

 ******************************************************************************/

void
GroupNode::ViewVCSDiffs
	(
	const bool origSilent
	)
	const
{
	const JSize count     = GetChildCount();
	const bool silent = origSilent || count > 1;
	for (JIndex i=1; i<=count; i++)
	{
		(GetProjectChild(i))->ViewVCSDiffs(silent);
	}
}

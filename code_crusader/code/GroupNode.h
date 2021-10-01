/******************************************************************************
 GroupNode.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_GroupNode
#define _H_GroupNode

#include "ProjectNode.h"

class GroupNode : public ProjectNode
{
public:

	GroupNode(ProjectTree* tree, const bool append = true);
	GroupNode(std::istream& input, const JFileVersion vers,
				ProjectNode* parent);

	virtual ~GroupNode();

	virtual void	Print(JString* text) const;
	virtual void	ShowFileLocation() const;
	virtual void	ViewPlainDiffs(const bool silent) const;
	virtual void	ViewVCSDiffs(const bool silent) const;
};

#endif

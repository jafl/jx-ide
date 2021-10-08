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

	~GroupNode() override;

	void	Print(JString* text) const override;
	void	ShowFileLocation() const override;
	void	ViewPlainDiffs(const bool silent) const override;
	void	ViewVCSDiffs(const bool silent) const override;
};

#endif

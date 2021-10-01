/******************************************************************************
 DTreeDirector.h

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#ifndef _H_DTreeDirector
#define _H_DTreeDirector

#include "TreeDirector.h"

class DTree;

class DTreeDirector : public TreeDirector
{
public:

	DTreeDirector(ProjectDocument* supervisor);
	DTreeDirector(std::istream& projInput, const JFileVersion projVers,
					std::istream* setInput, const JFileVersion setVers,
					std::istream* symInput, const JFileVersion symVers,
					ProjectDocument* supervisor, const bool subProject);

	virtual ~DTreeDirector();

	DTree*	GetDTree() const;

protected:

	virtual void	UpdateTreeMenu() override;
	virtual void	HandleTreeMenu(const JIndex index) override;

private:

	DTree*	itsDTree;			// not owned

private:

	void	DTreeDirectorX();

	static Tree*	NewDTree(TreeDirector* director, const JSize marginWidth);
	static Tree*	StreamInDTree(std::istream& projInput, const JFileVersion projVers,
								  std::istream* setInput, const JFileVersion setVers,
								  std::istream* symInput, const JFileVersion symVers,
								  TreeDirector* director,
								  const JSize marginWidth, DirList* dirList);
	static void		InitDTreeToolBar(JXToolBar* toolBar, JXTextMenu* treeMenu);

	// not allowed

	DTreeDirector(const DTreeDirector& source);
	const DTreeDirector& operator=(const DTreeDirector& source);
};


/******************************************************************************
 GetDTree

 ******************************************************************************/

inline DTree*
DTreeDirector::GetDTree()
	const
{
	return itsDTree;
}

#endif

/******************************************************************************
 PHPTreeDirector.h

	Copyright © 2014 by John Lindal.

 ******************************************************************************/

#ifndef _H_PHPTreeDirector
#define _H_PHPTreeDirector

#include "TreeDirector.h"

class PHPTree;

class PHPTreeDirector : public TreeDirector
{
public:

	PHPTreeDirector(ProjectDocument* supervisor);
	PHPTreeDirector(std::istream& projInput, const JFileVersion projVers,
					  std::istream* setInput, const JFileVersion setVers,
					  std::istream* symInput, const JFileVersion symVers,
					  ProjectDocument* supervisor, const bool subProject);

	virtual ~PHPTreeDirector();

	PHPTree*	GetPHPTree() const;

protected:

	virtual void	UpdateTreeMenu();
	virtual void	HandleTreeMenu(const JIndex index);

private:

	PHPTree*	itsPHPTree;	// not owned

private:

	void	PHPTreeDirectorX();

	static Tree*	NewPHPTree(TreeDirector* director, const JSize marginWidth);
	static Tree*	StreamInPHPTree(std::istream& projInput, const JFileVersion projVers,
									std::istream* setInput, const JFileVersion setVers,
									std::istream* symInput, const JFileVersion symVers,
									TreeDirector* director,
									const JSize marginWidth, DirList* dirList);
	static void		InitPHPTreeToolBar(JXToolBar* toolBar, JXTextMenu* treeMenu);

	// not allowed

	PHPTreeDirector(const PHPTreeDirector& source);
	const PHPTreeDirector& operator=(const PHPTreeDirector& source);
};


/******************************************************************************
 GetPHPTree

 ******************************************************************************/

inline PHPTree*
PHPTreeDirector::GetPHPTree()
	const
{
	return itsPHPTree;
}

#endif

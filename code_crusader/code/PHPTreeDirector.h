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

	~PHPTreeDirector();

	PHPTree*	GetPHPTree() const;

protected:

	void	UpdateTreeMenu() override;
	void	HandleTreeMenu(const JIndex index) override;

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

/******************************************************************************
 CTreeDirector.h

	Copyright © 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_CTreeDirector
#define _H_CTreeDirector

#include "TreeDirector.h"

class CTree;

class CTreeDirector : public TreeDirector
{
public:

	CTreeDirector(ProjectDocument* supervisor);
	CTreeDirector(std::istream& projInput, const JFileVersion projVers,
					std::istream* setInput, const JFileVersion setVers,
					std::istream* symInput, const JFileVersion symVers,
					ProjectDocument* supervisor, const bool subProject,
					DirList* dirList);

	~CTreeDirector() override;

	CTree*	GetCTree() const;

protected:

	void	UpdateTreeMenu() override;
	void	HandleTreeMenu(const JIndex index) override;
	bool	UpgradeTreeMenuToolBarID(JString* s) override;

private:

	CTree*	itsCTree;	// not owned

private:

	void	CTreeDirectorX();

	static Tree*	NewCTree(TreeDirector* director, const JSize marginWidth);
	static Tree*	StreamInCTree(std::istream& projInput, const JFileVersion projVers,
								  std::istream* setInput, const JFileVersion setVers,
								  std::istream* symInput, const JFileVersion symVers,
								  TreeDirector* director,
								  const JSize marginWidth, DirList* dirList);
	static void		InitCTreeToolBar(JXToolBar* toolBar, JXTextMenu* treeMenu);
};


/******************************************************************************
 GetCTree

 ******************************************************************************/

inline CTree*
CTreeDirector::GetCTree()
	const
{
	return itsCTree;
}

#endif

/******************************************************************************
 GoTreeDirector.h

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#ifndef _H_GoTreeDirector
#define _H_GoTreeDirector

#include "TreeDirector.h"

class GoTree;

class GoTreeDirector : public TreeDirector
{
public:

	GoTreeDirector(ProjectDocument* supervisor);
	GoTreeDirector(std::istream& projInput, const JFileVersion projVers,
					 std::istream* setInput, const JFileVersion setVers,
					 std::istream* symInput, const JFileVersion symVers,
					 ProjectDocument* supervisor, const bool subProject);

	~GoTreeDirector() override;

	GoTree*	GetGoTree() const;

protected:

	void	UpdateTreeMenu() override;
	void	HandleTreeMenu(const JIndex index) override;
	bool	UpgradeTreeMenuToolBarID(JString* s) override;

private:

	GoTree*	itsGoTree;	// not owned

private:

	void	GoTreeDirectorX();

	static Tree*	NewGoTree(TreeDirector* director, const JSize marginWidth);
	static Tree*	StreamInGoTree(std::istream& projInput, const JFileVersion projVers,
								   std::istream* setInput, const JFileVersion setVers,
								   std::istream* symInput, const JFileVersion symVers,
								   TreeDirector* director,
								   const JSize marginWidth, DirList* dirList);
	static void		InitGoTreeToolBar(JXToolBar* toolBar, JXTextMenu* treeMenu);
};


/******************************************************************************
 GetGoTree

 ******************************************************************************/

inline GoTree*
GoTreeDirector::GetGoTree()
	const
{
	return itsGoTree;
}

#endif

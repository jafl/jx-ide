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

	virtual ~GoTreeDirector();

	GoTree*	GetGoTree() const;

protected:

	virtual void	UpdateTreeMenu();
	virtual void	HandleTreeMenu(const JIndex index);

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

	// not allowed

	GoTreeDirector(const GoTreeDirector& source);
	const GoTreeDirector& operator=(const GoTreeDirector& source);
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
/******************************************************************************
 JavaTreeDirector.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JavaTreeDirector
#define _H_JavaTreeDirector

#include "TreeDirector.h"

class JavaTree;

class JavaTreeDirector : public TreeDirector
{
public:

	JavaTreeDirector(ProjectDocument* supervisor);
	JavaTreeDirector(std::istream& projInput, const JFileVersion projVers,
					   std::istream* setInput, const JFileVersion setVers,
					   std::istream* symInput, const JFileVersion symVers,
					   ProjectDocument* supervisor, const bool subProject);

	virtual ~JavaTreeDirector();

	JavaTree*	GetJavaTree() const;

protected:

	virtual void	UpdateTreeMenu();
	virtual void	HandleTreeMenu(const JIndex index);

private:

	JavaTree*	itsJavaTree;	// not owned

private:

	void	JavaTreeDirectorX();

	static Tree*	NewJavaTree(TreeDirector* director, const JSize marginWidth);
	static Tree*	StreamInJavaTree(std::istream& projInput, const JFileVersion projVers,
									 std::istream* setInput, const JFileVersion setVers,
									 std::istream* symInput, const JFileVersion symVers,
									 TreeDirector* director,
									 const JSize marginWidth, DirList* dirList);
	static void		InitJavaTreeToolBar(JXToolBar* toolBar, JXTextMenu* treeMenu);
};


/******************************************************************************
 GetJavaTree

 ******************************************************************************/

inline JavaTree*
JavaTreeDirector::GetJavaTree()
	const
{
	return itsJavaTree;
}

#endif

/******************************************************************************
 CTreeDirector.h

	Copyright © 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_CTreeDirector
#define _H_CTreeDirector

#include "TreeDirector.h"

class CTree;
class EditCPPMacroDialog;

class CTreeDirector : public TreeDirector
{
public:

	CTreeDirector(ProjectDocument* supervisor);
	CTreeDirector(std::istream& projInput, const JFileVersion projVers,
					std::istream* setInput, const JFileVersion setVers,
					std::istream* symInput, const JFileVersion symVers,
					ProjectDocument* supervisor, const bool subProject,
					DirList* dirList);

	virtual ~CTreeDirector();

	CTree*	GetCTree() const;

protected:

	virtual void	UpdateTreeMenu() override;
	virtual void	HandleTreeMenu(const JIndex index) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CTree*				itsCTree;			// not owned
	EditCPPMacroDialog*	itsEditCPPDialog;	// nullptr unless editing

private:

	void	CTreeDirectorX();

	static Tree*	NewCTree(TreeDirector* director, const JSize marginWidth);
	static Tree*	StreamInCTree(std::istream& projInput, const JFileVersion projVers,
								  std::istream* setInput, const JFileVersion setVers,
								  std::istream* symInput, const JFileVersion symVers,
								  TreeDirector* director,
								  const JSize marginWidth, DirList* dirList);
	static void		InitCTreeToolBar(JXToolBar* toolBar, JXTextMenu* treeMenu);

	// not allowed

	CTreeDirector(const CTreeDirector& source);
	const CTreeDirector& operator=(const CTreeDirector& source);
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

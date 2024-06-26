/******************************************************************************
 TreeDirector.h

	Copyright © 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_TreeDirector
#define _H_TreeDirector

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPrefObject.h>
#include <jx-af/jcore/JFAID.h>
#include "PrefsManager.h"		// for FileTypesChanged

struct JXPM;
class JXTextMenu;
class JXToolBar;
class JXScrollbarSet;
class JXWidgetSet;
class JXStaticText;
class JXTextButton;
class JXProgressIndicator;
class JXPSPrinter;
class JXEPSPrinter;
class DirList;
class Class;
class Tree;
class TreeWidget;
class EditTreePrefsDialog;
class ProjectDocument;
class CommandMenu;
class SymbolUpdatePG;

class TreeDirector;
using TreeCreateFn   = Tree* (*)(TreeDirector* director, const JSize marginWidth);
using TreeStreamInFn = Tree* (*)(std::istream& projInput, const JFileVersion projVers,
								 std::istream* setInput, const JFileVersion setVers,
								 std::istream* symInput, const JFileVersion symVers,
								 TreeDirector* director,
								 const JSize marginWidth, DirList* dirList);
using TreeInitToolBarFn = void (*)(JXToolBar* toolBar, JXTextMenu* treeMenu);

class TreeDirector : public JXWindowDirector, public JPrefObject
{
public:

	TreeDirector(ProjectDocument* supervisor, TreeCreateFn createTreeFn,
				 const JUtf8Byte* windowTitleSuffixID,
				 const JUtf8Byte* windowHelpName,
				 const JXPM& windowIcon,
				 const JUtf8Byte* treeMenuItems,
				 const JIndex toolBarPrefID, TreeInitToolBarFn initToolBarFn);
	TreeDirector(std::istream& projInput, const JFileVersion projVers,
				 std::istream* setInput, const JFileVersion setVers,
				 std::istream* symInput, const JFileVersion symVers,
				 ProjectDocument* supervisor, const bool subProject,
				 TreeStreamInFn streamInTreeFn,
				 const JUtf8Byte* windowTitleSuffixID,
				 const JUtf8Byte* windowHelpName,
				 const JXPM& windowIcon,
				 const JUtf8Byte* treeMenuItems,
				 const JIndex toolBarPrefID, TreeInitToolBarFn initToolBarFn,
				 DirList* dirList, const bool readCompileRunDialogs);

	~TreeDirector() override;

	ProjectDocument*	GetProjectDoc() const;
	TreeWidget*			GetTreeWidget() const;
	Tree*				GetTree() const;
	SymbolUpdatePG*		GetMinimizeMILinksPG() const;

	void	FindFunction();

	void	ViewFunctionList(const Class* theClass);

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const DirList* dirList) const;

	// called by ProjectDocument

	void	FileTypesChanged(const PrefsManager::FileTypesChanged& info);

	// called by FileListTable

	void	PrepareForTreeUpdate(const bool reparseAll);
	void	TreeUpdateThreadFinished(const JArray<JFAID_t>& deadFileList);
	bool	TreeUpdateFinished();

	// called by ProjectDocument

	void	SetTreePrefs(const JSize fontSize,
						 const bool autoMinMILinks, const bool drawMILinksOnTop,
						 const bool raiseWhenSingleMatch);

protected:

	JXTextMenu*		GetTreeMenu() const;
	virtual void	UpdateTreeMenu() = 0;
	virtual void	HandleTreeMenu(const JIndex index) = 0;
	virtual bool	UpgradeTreeMenuToolBarID(JString* s);

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	ProjectDocument*	itsProjDoc;
	TreeWidget*			itsTreeWidget;
	Tree*				itsTree;

	JXPSPrinter*	itsPSPrinter;
	JXEPSPrinter*	itsEPSPrinter;

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsTreeMenu;
	JXTextMenu*		itsProjectMenu;
	CommandMenu*	itsCmdMenu;
	JXTextMenu*		itsPrefsMenu;

	SymbolUpdatePG*	itsMinimizeMILinksPG;

	const JString		itsWindowTitleSuffix;
	const JUtf8Byte*	itsWindowHelpName;

// begin JXLayout

	JXToolBar*           itsToolBar;
	JXWidgetSet*         itsUpdateContainer;
	JXTextButton*        itsUpdateCancelButton;
	JXStaticText*        itsUpdateLabel;
	JXStaticText*        itsUpdateCounter;
	JXProgressIndicator* itsUpdateCleanUpIndicator;

// end JXLayout

private:

	JXScrollbarSet*	TreeDirectorX(ProjectDocument* doc, const JXPM& windowIcon,
								  const JUtf8Byte* treeMenuItems,
								  const JIndex toolBarPrefID, TreeInitToolBarFn initToolBarFn);
	JXScrollbarSet*	BuildWindow(const JXPM& windowIcon,
								const JUtf8Byte* treeMenuItems,
								const JIndex toolBarPrefID, TreeInitToolBarFn initToolBarFn);
	void			AdjustWindowTitle();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateProjectMenu();
	void	HandleProjectMenu(const JIndex index);

	void	HandlePrefsMenu(const JIndex index);
	void	EditTreePrefs();
};


/******************************************************************************
 GetTree

 ******************************************************************************/

inline Tree*
TreeDirector::GetTree()
	const
{
	return itsTree;
}

/******************************************************************************
 GetTreeWidget

 ******************************************************************************/

inline TreeWidget*
TreeDirector::GetTreeWidget()
	const
{
	return itsTreeWidget;
}

/******************************************************************************
 GetTreeMenu

 ******************************************************************************/

inline JXTextMenu*
TreeDirector::GetTreeMenu()
	const
{
	return itsTreeMenu;
}

/******************************************************************************
 GetProjectDoc

 ******************************************************************************/

inline ProjectDocument*
TreeDirector::GetProjectDoc()
	const
{
	return itsProjDoc;
}

/******************************************************************************
 GetMinimizeMILinksPG

 ******************************************************************************/

inline SymbolUpdatePG*
TreeDirector::GetMinimizeMILinksPG()
	const
{
	return itsMinimizeMILinksPG;
}

#endif

/******************************************************************************
 TreeDirector.h

	Copyright © 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_TreeDirector
#define _H_TreeDirector

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPrefObject.h>
#include <jx-af/jcore/JFAID.h>
#include "PrefsManager.h"		// need definition of FileTypesChanged

struct JXPM;
class JXTextMenu;
class JXToolBar;
class JXScrollbarSet;
class JXPSPrinter;
class JXEPSPrinter;
class JXGetStringDialog;
class DirList;
class Class;
class Tree;
class TreeWidget;
class EditTreePrefsDialog;
class ProjectDocument;
class CommandMenu;
class SymbolUpdatePG;

class TreeDirector;
typedef Tree* (TreeCreateFn)(TreeDirector* director, const JSize marginWidth);
typedef Tree* (TreeStreamInFn)(std::istream& projInput, const JFileVersion projVers,
								   std::istream* setInput, const JFileVersion setVers,
								   std::istream* symInput, const JFileVersion symVers,
								   TreeDirector* director,
								   const JSize marginWidth, DirList* dirList);
typedef void (TreeInitToolBarFn)(JXToolBar* toolBar, JXTextMenu* treeMenu);

class TreeDirector : public JXWindowDirector, public JPrefObject
{
public:

	TreeDirector(ProjectDocument* supervisor, TreeCreateFn* createTreeFn,
				   const JUtf8Byte* windowTitleSuffixID,
				   const JUtf8Byte* windowHelpName,
				   const JXPM& windowIcon,
				   const JUtf8Byte* treeMenuItems, const JUtf8Byte* treeMenuNamespace,
				   const JIndex toolBarPrefID, TreeInitToolBarFn* initToolBarFn);
	TreeDirector(std::istream& projInput, const JFileVersion projVers,
				   std::istream* setInput, const JFileVersion setVers,
				   std::istream* symInput, const JFileVersion symVers,
				   ProjectDocument* supervisor, const bool subProject,
				   TreeStreamInFn* streamInTreeFn,
				   const JUtf8Byte* windowTitleSuffixID,
				   const JUtf8Byte* windowHelpName,
				   const JXPM& windowIcon,
				   const JUtf8Byte* treeMenuItems, const JUtf8Byte* treeMenuNamespace,
				   const JIndex toolBarPrefID, TreeInitToolBarFn* initToolBarFn,
				   DirList* dirList, const bool readCompileRunDialogs);

	virtual ~TreeDirector();

	ProjectDocument*	GetProjectDoc() const;
	TreeWidget*		GetTreeWidget() const;
	Tree*				GetTree() const;

	void		AskForFunctionToFind();
	bool	ShowInheritedFns() const;

	void		ViewFunctionList(const Class* theClass);

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const DirList* dirList) const;

	// for loading updated symbols

	virtual void	ReloadSetup(std::istream& input, const JFileVersion vers);

	// called by ProjectDocument

	void	FileTypesChanged(const PrefsManager::FileTypesChanged& info);

	// called by FileListTable

	void		PrepareForTreeUpdate(const bool reparseAll);
	bool	TreeUpdateFinished(const JArray<JFAID_t>& deadFileList);

	// called by ProjectDocument

	void	SetTreePrefs(const JSize fontSize, const bool showInheritedFns,
						 const bool autoMinMILinks, const bool drawMILinksOnTop,
						 const bool raiseWhenSingleMatch);

protected:

	JXTextMenu*		GetTreeMenu() const;
	virtual void	UpdateTreeMenu() = 0;
	virtual void	HandleTreeMenu(const JIndex index) = 0;

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	ProjectDocument*	itsProjDoc;
	TreeWidget*		itsTreeWidget;
	Tree*				itsTree;
	bool			itsShowInheritedFnsFlag;

	JXPSPrinter*		itsPSPrinter;
	JXEPSPrinter*		itsEPSPrinter;
	JXPSPrinter*		itsFnListPrinter;		// shared by all FnListDirectors

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsTreeMenu;
	JXTextMenu*		itsProjectMenu;
	CommandMenu*	itsCmdMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

	JXGetStringDialog*	itsFindFnDialog;	// usually nullptr

	const JString		itsWindowTitleSuffix;
	const JUtf8Byte*	itsWindowHelpName;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

private:

	JXScrollbarSet*	TreeDirectorX(ProjectDocument* doc, const JXPM& windowIcon,
									const JUtf8Byte* treeMenuItems,
									const JUtf8Byte* treeMenuNamespace,
									const JIndex toolBarPrefID, TreeInitToolBarFn* initToolBarFn);
	JXScrollbarSet*	BuildWindow(const JXPM& windowIcon,
								const JUtf8Byte* treeMenuItems,
								const JUtf8Byte* treeMenuNamespace,
								const JIndex toolBarPrefID, TreeInitToolBarFn* initToolBarFn);
	void			AdjustWindowTitle();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateProjectMenu();
	void	HandleProjectMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);
	void	EditTreePrefs();

	// not allowed

	TreeDirector(const TreeDirector& source);
	const TreeDirector& operator=(const TreeDirector& source);
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
 ShowInheritedFns

 ******************************************************************************/

inline bool
TreeDirector::ShowInheritedFns()
	const
{
	return itsShowInheritedFnsFlag;
}

#endif

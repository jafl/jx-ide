/******************************************************************************
 GoTreeDirector.cpp

	BASE CLASS = TreeDirector

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#include "GoTreeDirector.h"
#include "GoTree.h"
#include "ProjectDocument.h"
#include "actionDefs.h"
#include "globals.h"
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jcore/jAssert.h>

#include "jcc_go_tree_window.xpm"

// Tree menu

static const JUtf8Byte* kTreeMenuStr =
	"    Add structs/interfaces...                              %i" kEditSearchPathsAction
	"  | Update                 %k Meta-U                       %i" kUpdateClassTreeAction
	"  | Minimize MI link lengths now                           %i" kMinimizeMILinkLengthAction
	"%l| Open source            %k Left-dbl-click or Return     %i" kOpenSelectedFilesAction
	"  | Open function list     %k Right-dbl-click              %i" kOpenClassFnListAction
	"%l| Collapse               %k Meta-<                       %i" kCollapseClassesAction
	"  | Expand                 %k Meta->                       %i" kExpandClassesAction
	"  | Expand all                                             %i" kExpandAllClassesAction
	"%l| Select parents                                         %i" kSelectParentClassAction
	"  | Select descendants                                     %i" kSelectDescendantClassAction
	"  | Copy selected names    %k Meta-C                       %i" kCopyClassNameAction
	"%l| Find function...       %k Meta-F                       %i" kFindFunctionAction;

enum
{
	kEditSearchPathsCmd = 1, kUpdateCurrentCmd,
	kForceMinMILinksCmd,
	kTreeOpenSourceCmd, kTreeOpenFnListCmd,
	kTreeCollapseCmd, kTreeExpandCmd, kTreeExpandAllCmd,
	kTreeSelParentsCmd, kTreeSelDescendantsCmd, kCopySelNamesCmd,
	kFindFnCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

GoTreeDirector::GoTreeDirector
	(
	ProjectDocument* supervisor
	)
	:
	TreeDirector(supervisor, NewGoTree, "WindowTitleSuffix::GoTreeDirector",
				   "GoTreeHelp", jcc_go_tree_window,
				   kTreeMenuStr, "GoTreeDirector",
				   kGoTreeToolBarID, InitGoTreeToolBar)
{
	GoTreeDirectorX();
}

GoTreeDirector::GoTreeDirector
	(
	std::istream&			projInput,
	const JFileVersion	projVers,
	std::istream*			setInput,
	const JFileVersion	setVers,
	std::istream*			symInput,
	const JFileVersion	symVers,
	ProjectDocument*	supervisor,
	const bool		subProject
	)
	:
	TreeDirector(projInput, projVers, setInput, setVers, symInput, symVers,
				   supervisor, subProject, StreamInGoTree,
				   "WindowTitleSuffix::GoTreeDirector",
				   "GoTreeHelp", jcc_go_tree_window,
				   kTreeMenuStr, "GoTreeDirector",
				   kGoTreeToolBarID, InitGoTreeToolBar,
				   nullptr, false)
{
	GoTreeDirectorX();
}

// private

void
GoTreeDirector::GoTreeDirectorX()
{
	itsGoTree = dynamic_cast<GoTree*>(GetTree());
	assert( itsGoTree != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GoTreeDirector::~GoTreeDirector()
{
}

/******************************************************************************
 UpdateTreeMenu (virtual protected)

 ******************************************************************************/

void
GoTreeDirector::UpdateTreeMenu()
{
	JXTextMenu* treeMenu = GetTreeMenu();

	treeMenu->EnableItem(kEditSearchPathsCmd);
	treeMenu->EnableItem(kUpdateCurrentCmd);

	if (!itsGoTree->IsEmpty())
	{
		treeMenu->EnableItem(kFindFnCmd);
		treeMenu->EnableItem(kTreeExpandAllCmd);

		if (itsGoTree->NeedsMinimizeMILinks())
		{
			treeMenu->EnableItem(kForceMinMILinksCmd);
		}
	}

	bool hasSelection, canCollapse, canExpand;
	itsGoTree->GetMenuInfo(&hasSelection, &canCollapse, &canExpand);
	if (hasSelection)
	{
		treeMenu->EnableItem(kTreeOpenSourceCmd);
		treeMenu->EnableItem(kTreeOpenFnListCmd);
		treeMenu->EnableItem(kTreeSelParentsCmd);
		treeMenu->EnableItem(kTreeSelDescendantsCmd);
		treeMenu->EnableItem(kCopySelNamesCmd);
	}
	if (canCollapse)
	{
		treeMenu->EnableItem(kTreeCollapseCmd);
	}
	if (canExpand)
	{
		treeMenu->EnableItem(kTreeExpandCmd);
	}
}

/******************************************************************************
 HandleTreeMenu (virtual protected)

 ******************************************************************************/

void
GoTreeDirector::HandleTreeMenu
	(
	const JIndex index
	)
{
	GetDocumentManager()->SetActiveProjectDocument(GetProjectDoc());

	if (index == kEditSearchPathsCmd)
	{
		GetProjectDoc()->EditSearchPaths();
	}
	else if (index == kUpdateCurrentCmd)
	{
		GetProjectDoc()->UpdateSymbolDatabase();
	}
	else if (index == kForceMinMILinksCmd)
	{
		itsGoTree->ForceMinimizeMILinks();
	}

	else if (index == kTreeOpenSourceCmd)
	{
		itsGoTree->ViewSelectedSources();
	}
	else if (index == kTreeOpenFnListCmd)
	{
		itsGoTree->ViewSelectedFunctionLists();
	}

	else if (index == kTreeCollapseCmd)
	{
		itsGoTree->CollapseExpandSelectedClasses(true);
	}
	else if (index == kTreeExpandCmd)
	{
		itsGoTree->CollapseExpandSelectedClasses(false);
	}
	else if (index == kTreeExpandAllCmd)
	{
		itsGoTree->ExpandAllClasses();
	}

	else if (index == kTreeSelParentsCmd)
	{
		itsGoTree->SelectParents();
	}
	else if (index == kTreeSelDescendantsCmd)
	{
		itsGoTree->SelectDescendants();
	}
	else if (index == kCopySelNamesCmd)
	{
		itsGoTree->CopySelectedClassNames();
	}

	else if (index == kFindFnCmd)
	{
		AskForFunctionToFind();
	}
}

/******************************************************************************
 NewGoTree (static private)

 ******************************************************************************/

Tree*
GoTreeDirector::NewGoTree
	(
	TreeDirector*	director,
	const JSize		marginWidth
	)
{
	// dynamic_cast<> doesn't work because object is not fully constructed

	auto* goTreeDir = static_cast<GoTreeDirector*>(director);
	assert( goTreeDir != nullptr );

	auto* tree = jnew GoTree(goTreeDir, marginWidth);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 StreamInGoTree (static private)

 ******************************************************************************/

Tree*
GoTreeDirector::StreamInGoTree
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	TreeDirector*		director,
	const JSize			marginWidth,
	DirList*			dirList
	)
{
	assert( dirList == nullptr );

	// dynamic_cast<> doesn't work because object is not fully constructed

	auto* goTreeDir = static_cast<GoTreeDirector*>(director);
	assert( goTreeDir != nullptr );

	auto* tree = jnew GoTree(projInput, projVers,
								   setInput, setVers, symInput, symVers,
								   goTreeDir, marginWidth, dirList);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 InitGoTreeToolBar (static private)

 ******************************************************************************/

void
GoTreeDirector::InitGoTreeToolBar
	(
	JXToolBar*	toolBar,
	JXTextMenu*	treeMenu
	)
{
	toolBar->AppendButton(treeMenu, kEditSearchPathsCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(treeMenu, kTreeCollapseCmd);
	toolBar->AppendButton(treeMenu, kTreeExpandCmd);
}

/******************************************************************************
 DTreeDirector.cpp

	BASE CLASS = TreeDirector

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#include "DTreeDirector.h"
#include "DTree.h"
#include "ProjectDocument.h"
#include "actionDefs.h"
#include "globals.h"
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jcore/jAssert.h>

#include "jcc_d_tree_window.xpm"

// Tree menu

static const JUtf8Byte* kTreeMenuStr =
	"    Add classes...                                         %i" kEditSearchPathsAction
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

DTreeDirector::DTreeDirector
	(
	ProjectDocument* supervisor
	)
	:
	TreeDirector(supervisor, NewDTree, "WindowTitleSuffix::DTreeDirector",
				   "DTreeHelp", jcc_d_tree_window,
				   kTreeMenuStr, "DTreeDirector",
				   kDTreeToolBarID, InitDTreeToolBar)
{
	DTreeDirectorX();
}

DTreeDirector::DTreeDirector
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	ProjectDocument*	supervisor,
	const bool		subProject
	)
	:
	TreeDirector(projInput, projVers, setInput, setVers, symInput, symVers,
				   supervisor, subProject, StreamInDTree,
				   "WindowTitleSuffix::DTreeDirector",
				   "DTreeHelp", jcc_d_tree_window,
				   kTreeMenuStr, "DTreeDirector",
				   kDTreeToolBarID, InitDTreeToolBar,
				   nullptr, false)
{
	DTreeDirectorX();
}

// private

void
DTreeDirector::DTreeDirectorX()
{
	itsDTree = dynamic_cast<DTree*>(GetTree());
	assert( itsDTree != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DTreeDirector::~DTreeDirector()
{
}

/******************************************************************************
 UpdateTreeMenu (virtual protected)

 ******************************************************************************/

void
DTreeDirector::UpdateTreeMenu()
{
	JXTextMenu* treeMenu = GetTreeMenu();

	treeMenu->EnableItem(kEditSearchPathsCmd);
	treeMenu->EnableItem(kUpdateCurrentCmd);

	if (!itsDTree->IsEmpty())
	{
		treeMenu->EnableItem(kFindFnCmd);
		treeMenu->EnableItem(kTreeExpandAllCmd);

		if (itsDTree->NeedsMinimizeMILinks())
		{
			treeMenu->EnableItem(kForceMinMILinksCmd);
		}
	}

	bool hasSelection, canCollapse, canExpand;
	itsDTree->GetMenuInfo(&hasSelection, &canCollapse, &canExpand);
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
DTreeDirector::HandleTreeMenu
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
		itsDTree->ForceMinimizeMILinks();
	}

	else if (index == kTreeOpenSourceCmd)
	{
		itsDTree->ViewSelectedSources();
	}
	else if (index == kTreeOpenFnListCmd)
	{
		itsDTree->ViewSelectedFunctionLists();
	}

	else if (index == kTreeCollapseCmd)
	{
		itsDTree->CollapseExpandSelectedClasses(true);
	}
	else if (index == kTreeExpandCmd)
	{
		itsDTree->CollapseExpandSelectedClasses(false);
	}
	else if (index == kTreeExpandAllCmd)
	{
		itsDTree->ExpandAllClasses();
	}

	else if (index == kTreeSelParentsCmd)
	{
		itsDTree->SelectParents();
	}
	else if (index == kTreeSelDescendantsCmd)
	{
		itsDTree->SelectDescendants();
	}
	else if (index == kCopySelNamesCmd)
	{
		itsDTree->CopySelectedClassNames();
	}

	else if (index == kFindFnCmd)
	{
		AskForFunctionToFind();
	}
}

/******************************************************************************
 NewDTree (static private)

 ******************************************************************************/

Tree*
DTreeDirector::NewDTree
	(
	TreeDirector*	director,
	const JSize		marginWidth
	)
{
	// dynamic_cast<> doesn't work because object is not fully constructed

	auto* dTreeDir = static_cast<DTreeDirector*>(director);
	assert( dTreeDir != nullptr );

	auto* tree = jnew DTree(dTreeDir, marginWidth);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 StreamInDTree (static private)

 ******************************************************************************/

Tree*
DTreeDirector::StreamInDTree
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
	// dynamic_cast<> doesn't work because object is not fully constructed

	auto* dTreeDir = static_cast<DTreeDirector*>(director);
	assert( dTreeDir != nullptr );

	auto* tree = jnew DTree(projInput, projVers,
								setInput, setVers, symInput, symVers,
								dTreeDir, marginWidth, dirList);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 InitDTreeToolBar (static private)

 ******************************************************************************/

void
DTreeDirector::InitDTreeToolBar
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

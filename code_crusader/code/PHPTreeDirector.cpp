/******************************************************************************
 PHPTreeDirector.cpp

	BASE CLASS = TreeDirector

	Copyright © 2014 by John Lindal.

 ******************************************************************************/

#include "PHPTreeDirector.h"
#include "PHPTree.h"
#include "ProjectDocument.h"
#include "globals.h"
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jcore/jAssert.h>

#include "TreeDirector-Tree.h"
#include "jcc_php_tree_window.xpm"

/******************************************************************************
 Constructor

 ******************************************************************************/

PHPTreeDirector::PHPTreeDirector
	(
	ProjectDocument* supervisor
	)
	:
	TreeDirector(supervisor, NewPHPTree, "WindowTitleSuffix::PHPTreeDirector",
				   "PHPTreeHelp", jcc_php_tree_window,
				   kTreeMenuStr, kPHPTreeToolBarID, InitPHPTreeToolBar)
{
	PHPTreeDirectorX();
}

PHPTreeDirector::PHPTreeDirector
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
				   supervisor, subProject, StreamInPHPTree,
				   "WindowTitleSuffix::PHPTreeDirector",
				   "PHPTreeHelp", jcc_php_tree_window,
				   kTreeMenuStr, kPHPTreeToolBarID, InitPHPTreeToolBar,
				   nullptr, false)
{
	PHPTreeDirectorX();
}

// private

void
PHPTreeDirector::PHPTreeDirectorX()
{
	itsPHPTree = dynamic_cast<PHPTree*>(GetTree());
	assert( itsPHPTree != nullptr );

	ConfigureTreeMenu(GetTreeMenu());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PHPTreeDirector::~PHPTreeDirector()
{
}

/******************************************************************************
 UpdateTreeMenu (virtual protected)

 ******************************************************************************/

void
PHPTreeDirector::UpdateTreeMenu()
{
	JXTextMenu* treeMenu = GetTreeMenu();

	treeMenu->EnableItem(kEditSearchPathsCmd);
	treeMenu->EnableItem(kUpdateCurrentCmd);

	if (!itsPHPTree->IsEmpty())
	{
		treeMenu->EnableItem(kFindFnCmd);
		treeMenu->EnableItem(kTreeExpandAllCmd);

		if (itsPHPTree->NeedsMinimizeMILinks())
		{
			treeMenu->EnableItem(kForceMinMILinksCmd);
		}
	}

	bool hasSelection, canCollapse, canExpand;
	itsPHPTree->GetMenuInfo(&hasSelection, &canCollapse, &canExpand);
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
PHPTreeDirector::HandleTreeMenu
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
		itsPHPTree->ForceMinimizeMILinks();
	}

	else if (index == kTreeOpenSourceCmd)
	{
		itsPHPTree->ViewSelectedSources();
	}
	else if (index == kTreeOpenFnListCmd)
	{
		itsPHPTree->ViewSelectedFunctionLists();
	}

	else if (index == kTreeCollapseCmd)
	{
		itsPHPTree->CollapseExpandSelectedClasses(true);
	}
	else if (index == kTreeExpandCmd)
	{
		itsPHPTree->CollapseExpandSelectedClasses(false);
	}
	else if (index == kTreeExpandAllCmd)
	{
		itsPHPTree->ExpandAllClasses();
	}

	else if (index == kTreeSelParentsCmd)
	{
		itsPHPTree->SelectParents();
	}
	else if (index == kTreeSelDescendantsCmd)
	{
		itsPHPTree->SelectDescendants();
	}
	else if (index == kCopySelNamesCmd)
	{
		itsPHPTree->CopySelectedClassNames();
	}

	else if (index == kFindFnCmd)
	{
		FindFunction();
	}
}

/******************************************************************************
 NewPHPTree (static private)

 ******************************************************************************/

Tree*
PHPTreeDirector::NewPHPTree
	(
	TreeDirector*	director,
	const JSize		marginWidth
	)
{
	// dynamic_cast<> doesn't work because object is not fully constructed

	auto* phpTreeDir = static_cast<PHPTreeDirector*>(director);
	assert( phpTreeDir != nullptr );

	auto* tree = jnew PHPTree(phpTreeDir, marginWidth);
	return tree;
}

/******************************************************************************
 StreamInPHPTree (static private)

 ******************************************************************************/

Tree*
PHPTreeDirector::StreamInPHPTree
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

	auto* phpTreeDir = static_cast<PHPTreeDirector*>(director);
	assert( phpTreeDir != nullptr );

	auto* tree = jnew PHPTree(projInput, projVers,
							  setInput, setVers, symInput, symVers,
							  phpTreeDir, marginWidth, dirList);
	return tree;
}

/******************************************************************************
 InitPHPTreeToolBar (static private)

 ******************************************************************************/

void
PHPTreeDirector::InitPHPTreeToolBar
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

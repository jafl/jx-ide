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

#include "GoTreeDirector-Tree.h"
#include "jcc_go_tree_window.xpm"

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
				   kTreeMenuStr, kGoTreeToolBarID, InitGoTreeToolBar)
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
				   kTreeMenuStr, kGoTreeToolBarID, InitGoTreeToolBar,
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

	ConfigureTreeMenu(GetTreeMenu());
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
		FindFunction();
	}
}

/******************************************************************************
 UpgradeTreeMenuToolBarID (virtual protected)

 ******************************************************************************/

static const JUtf8Byte* kToolbarIDMap[] =
{
	"CBEditSearchPaths",		"EditSearchPaths::GoTreeDirector",
	"CBUpdateClassTree",		"UpdateClassTree::GoTreeDirector",
	"CBMinimizeMILinkLength",	"MinimizeMILinkLength::GoTreeDirector",
	"CBOpenSelectedFiles",		"OpenSelectedFiles::GoTreeDirector",
	"CBOpenClassFnList",		"OpenClassFnList::GoTreeDirector",
	"CBCollapseClasses",		"CollapseClasses::GoTreeDirector",
	"CBExpandClasses",			"ExpandClasses::GoTreeDirector",
	"CBExpandAllClasses",		"ExpandAllClasses::GoTreeDirector",
	"CBSelectParentClass",		"SelectParentClass::GoTreeDirector",
	"CBSelectDescendantClass",	"SelectDescendantClass::GoTreeDirector",
	"CBCopyClassName"			"CopyClassName::GoTreeDirector",
	"CBFindFunction",			"FindFunction::GoTreeDirector",
};

const JSize kToolbarIDMapCount = sizeof(kToolbarIDMap) / sizeof(JUtf8Byte*);

bool
GoTreeDirector::UpgradeTreeMenuToolBarID
	(
	JString* s
	)
{
	if (!s->StartsWith("CB"))
	{
		return false;
	}

	for (JUnsignedOffset i=0; i<kToolbarIDMapCount; i+=2)
	{
		if (*s == kToolbarIDMap[i])
		{
			*s = kToolbarIDMap[i+1];
			return true;
		}
	}

	return false;
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

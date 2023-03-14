/******************************************************************************
 CTreeDirector.cpp

	BASE CLASS = TreeDirector

	Copyright © 1996-99 by John Lindal.

 ******************************************************************************/

#include "CTreeDirector.h"
#include "CTree.h"
#include "EditCPPMacroDialog.h"
#include "ProjectDocument.h"
#include "actionDefs.h"
#include "globals.h"
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jcore/jAssert.h>

#include "jcc_c_tree_window.xpm"

// Tree menu

static const JUtf8Byte* kTreeMenuStr =
	"    Configure C preprocessor...                            %i" kConfigureCTreeCPPAction
	"  | Add classes...                                         %i" kEditSearchPathsAction
	"  | Update                 %k Meta-U                       %i" kUpdateClassTreeAction
	"  | Minimize MI link lengths now                           %i" kMinimizeMILinkLengthAction
	"%l| Open source            %k Left-dbl-click or Return     %i" kOpenSelectedFilesAction
	"  | Open header            %k Middle-dbl-click or Control-Tab %i" kOpenComplFilesAction
	"  | Open function list     %k Right-dbl-click              %i" kOpenClassFnListAction
	"%l| Create derived class...                                %i" kCreateDerivedClassAction
	"%l| Collapse               %k Meta-<                       %i" kCollapseClassesAction
	"  | Expand                 %k Meta->                       %i" kExpandClassesAction
	"  | Expand all                                             %i" kExpandAllClassesAction
	"%l| Select parents                                         %i" kSelectParentClassAction
	"  | Select descendants                                     %i" kSelectDescendantClassAction
	"  | Copy selected names    %k Meta-C                       %i" kCopyClassNameAction
	"%l| Find function...       %k Meta-F                       %i" kFindFunctionAction;

enum
{
	kEditCPPMacrosCmd = 1, kEditSearchPathsCmd, kUpdateCurrentCmd,
	kForceMinMILinksCmd,
	kTreeOpenSourceCmd, kTreeOpenHeaderCmd, kTreeOpenFnListCmd,
	kCreateDerivedClassCmd,
	kTreeCollapseCmd, kTreeExpandCmd, kTreeExpandAllCmd,
	kTreeSelParentsCmd, kTreeSelDescendantsCmd, kCopySelNamesCmd,
	kFindFnCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

CTreeDirector::CTreeDirector
	(
	ProjectDocument* supervisor
	)
	:
	TreeDirector(supervisor, NewCTree, "WindowTitleSuffix::CTreeDirector",
				 "CTreeHelp", jcc_c_tree_window,
				 kTreeMenuStr, "CTreeDirector",
				 kCTreeToolBarID, InitCTreeToolBar)
{
	CTreeDirectorX();
}

CTreeDirector::CTreeDirector
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	ProjectDocument*	supervisor,
	const bool			subProject,
	DirList*			dirList
	)
	:
	TreeDirector(projInput, projVers, setInput, setVers, symInput, symVers,
				 supervisor, subProject, StreamInCTree,
				 "WindowTitleSuffix::CTreeDirector",
				 "CTreeHelp", jcc_c_tree_window,
				 kTreeMenuStr, "CTreeDirector",
				 kCTreeToolBarID, InitCTreeToolBar,
				 dirList, true)
{
	CTreeDirectorX();
}

// private

void
CTreeDirector::CTreeDirectorX()
{
	itsCTree = dynamic_cast<CTree*>(GetTree());
	assert( itsCTree != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CTreeDirector::~CTreeDirector()
{
}

/******************************************************************************
 UpdateTreeMenu (virtual protected)

 ******************************************************************************/

void
CTreeDirector::UpdateTreeMenu()
{
	JXTextMenu* treeMenu = GetTreeMenu();

	treeMenu->EnableItem(kEditCPPMacrosCmd);
	treeMenu->EnableItem(kEditSearchPathsCmd);
	treeMenu->EnableItem(kUpdateCurrentCmd);

	if (!itsCTree->IsEmpty())
	{
		treeMenu->EnableItem(kFindFnCmd);
		treeMenu->EnableItem(kTreeExpandAllCmd);

		if (itsCTree->NeedsMinimizeMILinks())
		{
			treeMenu->EnableItem(kForceMinMILinksCmd);
		}
	}

	bool hasSelection, canCollapse, canExpand;
	itsCTree->GetMenuInfo(&hasSelection, &canCollapse, &canExpand);
	if (hasSelection)
	{
		treeMenu->EnableItem(kTreeOpenSourceCmd);
		treeMenu->EnableItem(kTreeOpenHeaderCmd);
		treeMenu->EnableItem(kTreeOpenFnListCmd);
		treeMenu->EnableItem(kCreateDerivedClassCmd);
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
CTreeDirector::HandleTreeMenu
	(
	const JIndex index
	)
{
	GetDocumentManager()->SetActiveProjectDocument(GetProjectDoc());

	if (index == kEditCPPMacrosCmd)
	{
		auto* dlog = jnew EditCPPMacroDialog(*itsCTree->GetCPreprocessor());
		assert( dlog != nullptr );
		if (dlog->DoDialog() &&
			dlog->UpdateMacros(itsCTree->GetCPreprocessor()))
		{
			itsCTree->NextUpdateMustReparseAll();
			GetProjectDoc()->UpdateSymbolDatabase();
		}
	}
	else if (index == kEditSearchPathsCmd)
	{
		GetProjectDoc()->EditSearchPaths();
	}
	else if (index == kUpdateCurrentCmd)
	{
		GetProjectDoc()->UpdateSymbolDatabase();
	}
	else if (index == kForceMinMILinksCmd)
	{
		itsCTree->ForceMinimizeMILinks();
	}

	else if (index == kTreeOpenSourceCmd)
	{
		itsCTree->ViewSelectedSources();
	}
	else if (index == kTreeOpenHeaderCmd)
	{
		itsCTree->ViewSelectedHeaders();
	}
	else if (index == kTreeOpenFnListCmd)
	{
		itsCTree->ViewSelectedFunctionLists();
	}

	else if (index == kCreateDerivedClassCmd)
	{
		itsCTree->DeriveFromSelected();
	}

	else if (index == kTreeCollapseCmd)
	{
		itsCTree->CollapseExpandSelectedClasses(true);
	}
	else if (index == kTreeExpandCmd)
	{
		itsCTree->CollapseExpandSelectedClasses(false);
	}
	else if (index == kTreeExpandAllCmd)
	{
		itsCTree->ExpandAllClasses();
	}

	else if (index == kTreeSelParentsCmd)
	{
		itsCTree->SelectParents();
	}
	else if (index == kTreeSelDescendantsCmd)
	{
		itsCTree->SelectDescendants();
	}
	else if (index == kCopySelNamesCmd)
	{
		itsCTree->CopySelectedClassNames();
	}

	else if (index == kFindFnCmd)
	{
		FindFunction();
	}
}

/******************************************************************************
 NewCTree (static private)

 ******************************************************************************/

Tree*
CTreeDirector::NewCTree
	(
	TreeDirector*	director,
	const JSize		marginWidth
	)
{
	// dynamic_cast<> doesn't work because object is not fully constructed

	auto* cTreeDir = static_cast<CTreeDirector*>(director);
	assert( cTreeDir != nullptr );

	auto* tree = jnew CTree(cTreeDir, marginWidth);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 StreamInCTree (static private)

 ******************************************************************************/

Tree*
CTreeDirector::StreamInCTree
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

	auto* cTreeDir = static_cast<CTreeDirector*>(director);
	assert( cTreeDir != nullptr );

	auto* tree = jnew CTree(projInput, projVers,
								setInput, setVers, symInput, symVers,
								cTreeDir, marginWidth, dirList);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 InitCTreeToolBar (static private)

 ******************************************************************************/

void
CTreeDirector::InitCTreeToolBar
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

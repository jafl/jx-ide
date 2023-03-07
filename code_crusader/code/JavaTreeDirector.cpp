/******************************************************************************
 JavaTreeDirector.cpp

	BASE CLASS = TreeDirector

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "JavaTreeDirector.h"
#include "JavaTree.h"
#include "ProjectDocument.h"
#include "actionDefs.h"
#include "globals.h"
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jcore/jAssert.h>

#include "jcc_java_tree_window.xpm"

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

JavaTreeDirector::JavaTreeDirector
	(
	ProjectDocument* supervisor
	)
	:
	TreeDirector(supervisor, NewJavaTree, "WindowTitleSuffix::JavaTreeDirector",
				   "JavaTreeHelp", jcc_java_tree_window,
				   kTreeMenuStr, "JavaTreeDirector",
				   kJavaTreeToolBarID, InitJavaTreeToolBar)
{
	JavaTreeDirectorX();
}

JavaTreeDirector::JavaTreeDirector
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
				   supervisor, subProject, StreamInJavaTree,
				   "WindowTitleSuffix::JavaTreeDirector",
				   "JavaTreeHelp", jcc_java_tree_window,
				   kTreeMenuStr, "JavaTreeDirector",
				   kJavaTreeToolBarID, InitJavaTreeToolBar,
				   nullptr, false)
{
	JavaTreeDirectorX();
}

// private

void
JavaTreeDirector::JavaTreeDirectorX()
{
	itsJavaTree = dynamic_cast<JavaTree*>(GetTree());
	assert( itsJavaTree != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JavaTreeDirector::~JavaTreeDirector()
{
}

/******************************************************************************
 UpdateTreeMenu (virtual protected)

 ******************************************************************************/

void
JavaTreeDirector::UpdateTreeMenu()
{
	JXTextMenu* treeMenu = GetTreeMenu();

	treeMenu->EnableItem(kEditSearchPathsCmd);
	treeMenu->EnableItem(kUpdateCurrentCmd);

	if (!itsJavaTree->IsEmpty())
	{
		treeMenu->EnableItem(kFindFnCmd);
		treeMenu->EnableItem(kTreeExpandAllCmd);

		if (itsJavaTree->NeedsMinimizeMILinks())
		{
			treeMenu->EnableItem(kForceMinMILinksCmd);
		}
	}

	bool hasSelection, canCollapse, canExpand;
	itsJavaTree->GetMenuInfo(&hasSelection, &canCollapse, &canExpand);
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
JavaTreeDirector::HandleTreeMenu
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
		itsJavaTree->ForceMinimizeMILinks();
	}

	else if (index == kTreeOpenSourceCmd)
	{
		itsJavaTree->ViewSelectedSources();
	}
	else if (index == kTreeOpenFnListCmd)
	{
		itsJavaTree->ViewSelectedFunctionLists();
	}

	else if (index == kTreeCollapseCmd)
	{
		itsJavaTree->CollapseExpandSelectedClasses(true);
	}
	else if (index == kTreeExpandCmd)
	{
		itsJavaTree->CollapseExpandSelectedClasses(false);
	}
	else if (index == kTreeExpandAllCmd)
	{
		itsJavaTree->ExpandAllClasses();
	}

	else if (index == kTreeSelParentsCmd)
	{
		itsJavaTree->SelectParents();
	}
	else if (index == kTreeSelDescendantsCmd)
	{
		itsJavaTree->SelectDescendants();
	}
	else if (index == kCopySelNamesCmd)
	{
		itsJavaTree->CopySelectedClassNames();
	}

	else if (index == kFindFnCmd)
	{
		AskForFunctionToFind();
	}
}

/******************************************************************************
 NewJavaTree (static private)

 ******************************************************************************/

Tree*
JavaTreeDirector::NewJavaTree
	(
	TreeDirector*	director,
	const JSize		marginWidth
	)
{
	// dynamic_cast<> doesn't work because object is not fully constructed

	auto* javaTreeDir = static_cast<JavaTreeDirector*>(director);
	assert( javaTreeDir != nullptr );

	auto* tree = jnew JavaTree(javaTreeDir, marginWidth);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 StreamInJavaTree (static private)

 ******************************************************************************/

Tree*
JavaTreeDirector::StreamInJavaTree
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

	auto* javaTreeDir = static_cast<JavaTreeDirector*>(director);
	assert( javaTreeDir != nullptr );

	auto* tree = jnew JavaTree(projInput, projVers,
									  setInput, setVers, symInput, symVers,
									  javaTreeDir, marginWidth, dirList);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 InitJavaTreeToolBar (static private)

 ******************************************************************************/

void
JavaTreeDirector::InitJavaTreeToolBar
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

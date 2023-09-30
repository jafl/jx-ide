/******************************************************************************
 DockManager.cpp

	BASE CLASS = JXDockManager

	Copyright © 2003 by John Lindal.

 ******************************************************************************/

#include "DockManager.h"
#include "ProjectDocument.h"
#include "SymbolDirector.h"
#include "FileListDirector.h"
#include "CTreeDirector.h"
#include "JavaTreeDirector.h"
#include "PHPTreeDirector.h"
#include "TextDocument.h"
#include "globals.h"
#include <jx-af/jx/JXDockDirector.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXHorizDockPartition.h>
#include <jx-af/jx/JXVertDockPartition.h>
#include <jx-af/jx/JXDockWidget.h>
#include <jx-af/jx/JXTabGroup.h>
#include <jx-af/jx/JXImage.h>
#include "jcc_project_window.xpm"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DockManager::DockManager()
	:
	JXDockManager(JXGetApplication()->GetCurrentDisplay(), JGetString("Name"),
				  GetPrefsManager(), kDockPrefID)
{
	JXDisplay* display = JXGetApplication()->GetCurrentDisplay();
	auto* icon      = jnew JXImage(display, jcc_project_window);
	assert( icon != nullptr );
	SetIcon(icon);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DockManager::~DockManager()
{
}

/******************************************************************************
 CanDockAll (virtual)

 ******************************************************************************/

bool
DockManager::CanDockAll()
	const
{
	return GetDockList()->IsEmpty();
}

/******************************************************************************
 DockAll (virtual)

	Dock all the windows and set docking positions for all window types.

 ******************************************************************************/

void
DockManager::DockAll()
{
	if (!GetDockList()->IsEmpty())
	{
		return;
	}

	JXDockDirector* dir = CreateDock(false);

	JXVertDockPartition* v;
	bool ok = dir->GetVertPartition(&v);
	assert( ok );

	JXHorizDockPartition* h;
	ok = v->SplitHoriz(1, &h);
	assert( ok );

	v->SetElasticIndex(1);
	h->SetElasticIndex(2);

	(dir->GetWindow())->AdjustSize(200, 0);
	v->SetCompartmentSize(2, 2*v->GetCompartmentSize(2)/3);

	JXDockWidget* dock1 = h->GetDockWidget(1);
	assert( dock1 != nullptr );
	SetDefaultDock(GetProjectWindowClass(),  dock1);
	SetDefaultDock(GetSymbolWindowClass(),   dock1);
	SetDefaultDock(GetFileListWindowClass(), dock1);

	JXDockWidget* dock2 = h->GetDockWidget(2);
	assert( dock2 != nullptr );
	SetDefaultDock(GetEditorWindowClass(), dock2);
	SetDefaultDock(GetTreeWindowClass(),   dock2);

	JXDockWidget* dock3 = v->GetDockWidget(2);
	assert( dock3 != nullptr );
	SetDefaultDock(GetExecOutputWindowClass(),    dock3);
	SetDefaultDock(GetCompileOutputWindowClass(), dock3);
	SetDefaultDock(GetSearchOutputWindowClass(),  dock3);

	(dock3->GetTabGroup())->SetTabEdge(JXTabGroup::kBottom);

	JPtrArray<ProjectDocument>* projList =
		GetDocumentManager()->GetProjectDocList();

	JSize count = projList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		ProjectDocument* projDoc = projList->GetElement(i);

		dock1->Dock(projDoc);
		dock1->Dock(projDoc->GetSymbolDirector());
		dock1->Dock(projDoc->GetFileListDirector());

		dock2->Dock(projDoc->GetCTreeDirector());
		dock2->Dock(projDoc->GetJavaTreeDirector());
		dock2->Dock(projDoc->GetPHPTreeDirector());
	}

	JPtrArray<TextDocument>* textList =
		GetDocumentManager()->GetTextDocList();

	count = textList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		TextDocument* textDoc = textList->GetElement(i);

		const TextFileType type = textDoc->GetFileType();
		if (type == kExecOutputFT ||
			type == kSearchOutputFT)
		{
			dock3->Dock(textDoc);
		}
		else
		{
			dock2->Dock(textDoc);
		}
	}
}

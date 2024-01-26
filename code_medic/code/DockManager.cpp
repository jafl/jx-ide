/******************************************************************************
 DockManager.cpp

	BASE CLASS = JXDockManager

	Copyright (C) 2003 by John Lindal.

 ******************************************************************************/

#include "DockManager.h"
#include "CommandDirector.h"
#include "globals.h"
#include <jx-af/jx/JXDockDirector.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXHorizDockPartition.h>
#include <jx-af/jx/JXVertDockPartition.h>
#include <jx-af/jx/JXDockWidget.h>
#include <jx-af/jx/JXImage.h>
#include "medic_command_window.xpm"
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
	auto* icon      = jnew JXImage(display, medic_command_window);
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

	JXDockDirector* dir = CreateDock(true);

	JXHorizDockPartition* h;
	bool ok = dir->GetHorizPartition(&h);
	assert( ok );

	JXVertDockPartition* v1;
	ok = h->SplitVert(1, &v1);
	assert( ok );

	JXVertDockPartition* v2;
	ok = h->SplitVert(2, &v2);
	assert( ok );

	h->SetElasticIndex(1);
	v1->SetElasticIndex(1);
	v2->SetElasticIndex(2);

	(dir->GetWindow())->AdjustSize(200, 0);

	JXDockWidget* dock1 = v1->GetDockWidget(1);
	assert( dock1 != nullptr );
	SetDefaultDock("Code_Medic_Source_Main",       dock1);
	SetDefaultDock("Code_Medic_Disassembly_Main",  dock1);
	SetDefaultDock("Code_Medic_Source",            dock1);
	SetDefaultDock("Code_Medic_Disassembly",       dock1);
	SetDefaultDock("Code_Medic_Variables_Plot_2D", dock1);

	JXDockWidget* dock2 = v1->GetDockWidget(2);
	assert( dock2 != nullptr );
	SetDefaultDock("Code_Medic_Command_Line", dock2);
	SetDefaultDock("Code_Medic_Breakpoints",  dock2);

	JXDockWidget* dock3 = v2->GetDockWidget(1);
	assert( dock3 != nullptr );
	SetDefaultDock("Code_Medic_Threads",   dock3);
	SetDefaultDock("Code_Medic_Stack",     dock3);
	SetDefaultDock("Code_Medic_File_List", dock3);

	JXDockWidget* dock4 = v2->GetDockWidget(2);
	assert( dock4 != nullptr );
	SetDefaultDock("Code_Medic_Variables",          dock4);
	SetDefaultDock("Code_Medic_Variables_Local",    dock4);
	SetDefaultDock("Code_Medic_Variables_Array_1D", dock4);
	SetDefaultDock("Code_Medic_Variables_Array_2D", dock4);
	SetDefaultDock("Code_Medic_Registers",          dock4);
	SetDefaultDock("Code_Medic_Memory",             dock4);

	CommandDirector* cmdDir = GetCommandDirector();
	dock2->Dock(cmdDir->GetWindow());
	cmdDir->DockAll(dock1, dock3, dock4);
}

/******************************************************************************
 MacroTable.cpp

	BASE CLASS = KeyScriptTableBase

	Copyright © 1998-2008 by John Lindal.

 ******************************************************************************/

#include "MacroTable.h"
#include "MacroManager.h"
#include "EditMacroDialog.h"
#include "ListChooseFileDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXSaveFileDialog.h>
#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

MacroTable::MacroTable
	(
	EditMacroDialog*	dialog,
	JXTextButton*		addRowButton,
	JXTextButton*		removeRowButton,
	JXTextButton*		loadButton,
	JXTextButton*		saveButton,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	KeyScriptTableBase(dialog, addRowButton, removeRowButton,
						 scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsLoadButton = loadButton;
	ListenTo(itsLoadButton);

	itsSaveButton = saveButton;
	ListenTo(itsSaveButton);

	SetColWidth(kMacroColumn, 60);
	// script column set automatically
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MacroTable::~MacroTable()
{
}

/******************************************************************************
 GetData

 ******************************************************************************/

void
MacroTable::GetData
	(
	MacroManager* mgr
	)
	const
{
	assert( !IsEditing() );

	mgr->RemoveAllMacros();

	const JStringTableData* data = GetStringData();
	const JSize count            = GetRowCount();
	for (JIndex i=1; i<=count; i++)
	{
		mgr->AddMacro(data->GetString(i, kMacroColumn).GetBytes(),
					  data->GetString(i, kScriptColumn).GetBytes());
	}
}

/******************************************************************************
 SetData

 ******************************************************************************/

void
MacroTable::SetData
	(
	const MacroManager& mgr
	)
{
	JStringTableData* data = GetStringData();
	data->RemoveAllRows();

	const MacroList& macroList = mgr.GetMacroList();
	const JSize count            = macroList.GetItemCount();
	data->AppendRows(count);
	for (JIndex i=1; i<=count; i++)
	{
		const MacroManager::MacroInfo info = macroList.GetItem(i);
		data->SetString(i,kMacroColumn,  *(info.macro));
		data->SetString(i,kScriptColumn, *(info.script));
	}

	Activate();
}

/******************************************************************************
 ClearData

 ******************************************************************************/

void
MacroTable::ClearData()
{
	GetStringData()->RemoveAllRows();
	Deactivate();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
MacroTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLoadButton && message.Is(JXButton::kPushed))
	{
		LoadMacros();
	}
	else if (sender == itsSaveButton && message.Is(JXButton::kPushed))
	{
		SaveMacros();
	}

	else
	{
		KeyScriptTableBase::Receive(sender, message);
	}
}

/******************************************************************************
 LoadMacros (private)

 ******************************************************************************/

void
MacroTable::LoadMacros()
{
	if (GetDialog()->ContentsValid())
	{
		auto* dlog =
			ListChooseFileDialog::Create(JGetString("ReplaceMacroList::MacroTable"),
										 JGetString("AppendToMacroList::MacroTable"),
										 JXChooseFileDialog::kSelectSingleFile);

		if (dlog->DoDialog())
		{
			ReadData(dlog->GetFullName(), dlog->ReplaceExisting());
		}
	}
}

/******************************************************************************
 ReadData (private)

	We read the format:  (\n*macro\n+script\n)*

 ******************************************************************************/

void
MacroTable::ReadData
	(
	const JString&	fileName,
	const bool		replace
	)
{
	JStringTableData* data = GetStringData();
	if (replace)
	{
		data->RemoveAllRows();
	}

	JIndex firstNewRow = 0;

	std::ifstream input(fileName.GetBytes());
	JString macro, script;
	JIndex state = 1;
	while (!input.eof() && !input.fail())
	{
		if (state == 1)
		{
			macro = JReadLine(input);
			if (!macro.IsEmpty())
			{
				state = 2;
			}
		}
		else if (state == 2)
		{
			script = JReadLine(input);
			if (!script.IsEmpty())
			{
				data->AppendRows(1);
				const JSize rowCount = data->GetRowCount();
				data->SetString(rowCount,kMacroColumn,  macro);
				data->SetString(rowCount,kScriptColumn, script);
				if (firstNewRow == 0)
				{
					firstNewRow = rowCount;
				}
				state = 1;
			}
		}
	}

	if (firstNewRow != 0)
	{
		ScrollTo((GetBounds()).bottomLeft());
		BeginEditing(JPoint(kMacroColumn, firstNewRow));
	}
}

/******************************************************************************
 SaveMacros (private)

 ******************************************************************************/

void
MacroTable::SaveMacros()
	const
{
	JString origName;
	if (GetDialog()->ContentsValid() &&
		GetDialog()->GetCurrentMacroSetName(&origName))
	{
		auto* dlog = JXSaveFileDialog::Create(JGetString("SaveMacroListPrompt::MacroTable"), origName);
		if (dlog->DoDialog())
		{
			WriteData(dlog->GetFullName());
		}
	}
}

/******************************************************************************
 WriteData (private)

	We write the format:  (macro\nscript\n\n)*

 ******************************************************************************/

void
MacroTable::WriteData
	(
	const JString& fileName
	)
	const
{
	std::ofstream output(fileName.GetBytes());

	const JStringTableData* data = GetStringData();
	const JSize count            = GetRowCount();
	for (JIndex i=1; i<=count; i++)
	{
		data->GetString(i, kMacroColumn).Print(output);
		output << '\n';
		data->GetString(i, kScriptColumn).Print(output);
		output << "\n\n";
	}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
MacroTable::Activate()
{
	KeyScriptTableBase::Activate();
	if (WouldBeActive())
	{
		itsLoadButton->Activate();
		itsSaveButton->Activate();
	}
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

void
MacroTable::Deactivate()
{
	KeyScriptTableBase::Deactivate();
	if (!WouldBeActive())
	{
		itsLoadButton->Deactivate();
		itsSaveButton->Deactivate();
	}
}

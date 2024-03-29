/******************************************************************************
 CharActionTable.cpp

	BASE CLASS = KeyScriptTableBase

	Copyright © 1998-2008 by John Lindal.

 ******************************************************************************/

#include "CharActionTable.h"
#include "CharActionManager.h"
#include "EditMacroDialog.h"
#include "ListChooseFileDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXCharInput.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXSaveFileDialog.h>
#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CharActionTable::CharActionTable
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

	SetColWidth(kMacroColumn, 40);
	// script column set automatically
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CharActionTable::~CharActionTable()
{
}

/******************************************************************************
 GetData

 ******************************************************************************/

void
CharActionTable::GetData
	(
	CharActionManager* mgr
	)
	const
{
	assert( !IsEditing() );

	mgr->ClearAllActions();

	const JStringTableData* data = GetStringData();
	const JSize count            = GetRowCount();
	for (JIndex i=1; i<=count; i++)
	{
		mgr->SetAction(data->GetString(i, kMacroColumn).GetFirstCharacter(),
					   data->GetString(i, kScriptColumn));
	}
}

/******************************************************************************
 SetData

 ******************************************************************************/

void
CharActionTable::SetData
	(
	const CharActionManager& mgr
	)
{
	JStringTableData* data = GetStringData();
	data->RemoveAllRows();

	const JStringPtrMap<JString>& actionMap = mgr.GetActionMap();
	JStringMapCursor cursor(&actionMap);
	while (cursor.Next())
	{
		data->AppendRows(1);
		data->SetString(GetRowCount(), kMacroColumn,  cursor.GetKey());
		data->SetString(GetRowCount(), kScriptColumn, *cursor.GetValue());
	}

	Activate();
}

/******************************************************************************
 ClearData

 ******************************************************************************/

void
CharActionTable::ClearData()
{
	GetStringData()->RemoveAllRows();
	Deactivate();
}

/******************************************************************************
 CreateStringTableInput (virtual protected)

 ******************************************************************************/

JXInputField*
CharActionTable::CreateStringTableInput
	(
	const JPoint&		cell,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	JXInputField* input =
		KeyScriptTableBase::CreateStringTableInput(
									cell, enclosure, hSizing, vSizing, x,y, w,h);

	if (cell.x == kMacroColumn)
	{
		jdelete input;
		input = jnew JXCharInput(enclosure, hSizing, vSizing, x,y, w,h);
	}

	return input;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CharActionTable::Receive
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
CharActionTable::LoadMacros()
{
	if (GetDialog()->ContentsValid())
	{
		auto* dlog =
			ListChooseFileDialog::Create(JGetString("ReplaceCharActionList::CharActionTable"),
										 JGetString("AppendToCharActionList::CharActionTable"),
										 JXChooseFileDialog::kSelectSingleFile);

		if (dlog->DoDialog())
		{
			ReadData(dlog->GetFullName(), dlog->ReplaceExisting());
		}
	}
}

/******************************************************************************
 ReadData (private)

	We read the format:  (\n*action\n+script\n)*

 ******************************************************************************/

void
CharActionTable::ReadData
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
	JString action, script;
	JIndex state = 1;
	while (!input.eof() && !input.fail())
	{
		if (state == 1)
		{
			action = JReadLine(input);
			if (!action.IsEmpty())
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
				data->SetString(rowCount,kMacroColumn,  JString(action.GetFirstCharacter()));
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
CharActionTable::SaveMacros()
	const
{
	JString origName;
	if (GetDialog()->ContentsValid() &&
		GetDialog()->GetCurrentMacroSetName(&origName))
	{
		auto* dlog = JXSaveFileDialog::Create(JGetString("SaveCharActionListPrompt::CharActionTable"), origName);
		if (dlog->DoDialog())
		{
			WriteData(dlog->GetFullName());
		}
	}
}

/******************************************************************************
 WriteData (private)

	We write the format:  (action\nscript\n\n)*

 ******************************************************************************/

void
CharActionTable::WriteData
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
CharActionTable::Activate()
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
CharActionTable::Deactivate()
{
	KeyScriptTableBase::Deactivate();
	if (!WouldBeActive())
	{
		itsLoadButton->Deactivate();
		itsSaveButton->Deactivate();
	}
}

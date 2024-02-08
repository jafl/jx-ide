/******************************************************************************
 EditMacroDialog.cpp

	BASE CLASS = JXModalDialogDirector, JPrefObject

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "EditMacroDialog.h"
#include "MacroSetTable.h"
#include "CharActionTable.h"
#include "MacroTable.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXVertPartition.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

EditMacroDialog::EditMacroDialog
	(
	JArray<PrefsManager::MacroSetInfo>*	macroList,
	const JIndex						initialSelection,
	const JIndex						firstUnusedID
	)
	:
	JXModalDialogDirector(true),
	JPrefObject(GetPrefsManager(), kEditMacroDialogID)
{
	BuildWindow(macroList, initialSelection, firstUnusedID);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditMacroDialog::~EditMacroDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 GetMacroList

 ******************************************************************************/

JArray<PrefsManager::MacroSetInfo>*
EditMacroDialog::GetMacroList
	(
	JIndex* firstNewID,
	JIndex* lastNewID
	)
	const
{
	return itsMacroSetTable->GetMacroList(firstNewID, lastNewID);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
EditMacroDialog::BuildWindow
	(
	JArray<PrefsManager::MacroSetInfo>*	macroList,
	const JIndex						initialSelection,
	const JIndex						firstUnusedID
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 350,530, JGetString("WindowTitle::EditMacroDialog::JXLayout"));

	JArray<JCoordinate> itsPartition_sizes, itsPartition_minSizes;
	itsPartition_sizes.AppendItem(166);
	itsPartition_minSizes.AppendItem(60);
	itsPartition_sizes.AppendItem(144);
	itsPartition_minSizes.AppendItem(60);
	itsPartition_sizes.AppendItem(140);
	itsPartition_minSizes.AppendItem(60);

	itsPartition =
		jnew JXVertPartition(itsPartition_sizes, 3, itsPartition_minSizes, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 310,460);

	auto* macroSetScrollbarSet =
		jnew JXScrollbarSet(itsPartition->GetCompartment(1),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 240,166);
	assert( macroSetScrollbarSet != nullptr );

	auto* newMacroSetButton =
		jnew JXTextButton(JGetString("newMacroSetButton::EditMacroDialog::JXLayout"), itsPartition->GetCompartment(1),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,10, 60,20);
	assert( newMacroSetButton != nullptr );

	auto* removeMacroSetButton =
		jnew JXTextButton(JGetString("removeMacroSetButton::EditMacroDialog::JXLayout"), itsPartition->GetCompartment(1),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,40, 60,20);
	assert( removeMacroSetButton != nullptr );

	auto* actionScrollbarSet =
		jnew JXScrollbarSet(itsPartition->GetCompartment(2),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 240,144);
	assert( actionScrollbarSet != nullptr );

	auto* newActionButton =
		jnew JXTextButton(JGetString("newActionButton::EditMacroDialog::JXLayout"), itsPartition->GetCompartment(2),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,10, 60,20);
	assert( newActionButton != nullptr );

	auto* removeActionButton =
		jnew JXTextButton(JGetString("removeActionButton::EditMacroDialog::JXLayout"), itsPartition->GetCompartment(2),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,40, 60,20);
	assert( removeActionButton != nullptr );

	auto* loadActionFileButton =
		jnew JXTextButton(JGetString("loadActionFileButton::EditMacroDialog::JXLayout"), itsPartition->GetCompartment(2),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,80, 60,20);
	assert( loadActionFileButton != nullptr );

	auto* saveActionFileButton =
		jnew JXTextButton(JGetString("saveActionFileButton::EditMacroDialog::JXLayout"), itsPartition->GetCompartment(2),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,110, 60,20);
	assert( saveActionFileButton != nullptr );

	auto* macroScrollbarSet =
		jnew JXScrollbarSet(itsPartition->GetCompartment(3),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 240,140);
	assert( macroScrollbarSet != nullptr );

	auto* newMacroButton =
		jnew JXTextButton(JGetString("newMacroButton::EditMacroDialog::JXLayout"), itsPartition->GetCompartment(3),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,10, 60,20);
	assert( newMacroButton != nullptr );

	auto* removeMacroButton =
		jnew JXTextButton(JGetString("removeMacroButton::EditMacroDialog::JXLayout"), itsPartition->GetCompartment(3),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,40, 60,20);
	assert( removeMacroButton != nullptr );

	auto* loadMacroFileButton =
		jnew JXTextButton(JGetString("loadMacroFileButton::EditMacroDialog::JXLayout"), itsPartition->GetCompartment(3),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,80, 60,20);
	assert( loadMacroFileButton != nullptr );

	auto* saveMacroFileButton =
		jnew JXTextButton(JGetString("saveMacroFileButton::EditMacroDialog::JXLayout"), itsPartition->GetCompartment(3),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,110, 60,20);
	assert( saveMacroFileButton != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,500, 70,20);
	assert( cancelButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 145,500, 70,20);
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::shortcuts::EditMacroDialog::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 250,500, 70,20);
	assert( okButton != nullptr );

	itsActionTable =
		jnew CharActionTable(this, newActionButton, removeActionButton, loadActionFileButton, saveActionFileButton, actionScrollbarSet, actionScrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 240,124);

	itsMacroTable =
		jnew MacroTable(this, newMacroButton, removeMacroButton, loadMacroFileButton, saveMacroFileButton, macroScrollbarSet, macroScrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 240,120);

	itsMacroSetTable =
		jnew MacroSetTable(macroList, initialSelection, firstUnusedID, itsActionTable, itsMacroTable, newMacroSetButton, removeMacroSetButton, macroSetScrollbarSet, macroSetScrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 240,146);

	auto* actionColHeader =
		jnew JXColHeaderWidget(itsActionTable, actionScrollbarSet, actionScrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 240,20);
	assert( actionColHeader != nullptr );

	auto* macroColHeader =
		jnew JXColHeaderWidget(itsMacroTable, macroScrollbarSet, macroScrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 240,20);
	assert( macroColHeader != nullptr );

	auto* macroSetColHeader =
		jnew JXColHeaderWidget(itsMacroSetTable, macroSetScrollbarSet, macroSetScrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 240,20);
	assert( macroSetColHeader != nullptr );

// end JXLayout

	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	macroSetColHeader->SetColTitle(1, JGetString("Column1Set::EditMacroDialog"));

	actionColHeader->SetColTitle(1, JGetString("Column1Char::EditMacroDialog"));
	actionColHeader->SetColTitle(2, JGetString("Column2::EditMacroDialog"));
	actionColHeader->TurnOnColResizing(20);

	macroColHeader->SetColTitle(1, JGetString("Column1Macro::EditMacroDialog"));
	macroColHeader->SetColTitle(2, JGetString("Column2::EditMacroDialog"));
	macroColHeader->TurnOnColResizing(20);
}

/******************************************************************************
 ContentsValid

	Check that the table contents are valid.

 ******************************************************************************/

bool
EditMacroDialog::ContentsValid()
	const
{
	return itsMacroSetTable->ContentsValid();
}

/******************************************************************************
 GetCurrentMacroSetName

 ******************************************************************************/

bool
EditMacroDialog::GetCurrentMacroSetName
	(
	JString* name
	)
	const
{
	return itsMacroSetTable->GetCurrentMacroSetName(name);
}

/******************************************************************************
 OKToDeactivate (virtual protected)

	Check that the table contents are valid.

 ******************************************************************************/

bool
EditMacroDialog::OKToDeactivate()
{
	return Cancelled() ||
		(JXModalDialogDirector::OKToDeactivate() && ContentsValid());
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
EditMacroDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("MacroHelp");
	}
	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
EditMacroDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
	{
		return;
	}

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);
	window->Deiconify();

	itsPartition->ReadGeometry(input);

	JCoordinate w;
	input >> w;
	itsActionTable->SetColWidth(CharActionTable::kMacroColumn, w);
	input >> w;
	itsMacroTable->SetColWidth(MacroTable::kMacroColumn, w);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
EditMacroDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsPartition->WriteGeometry(output);

	output << ' ' << itsActionTable->GetColWidth(CharActionTable::kMacroColumn);
	output << ' ' << itsMacroTable->GetColWidth(MacroTable::kMacroColumn);
}

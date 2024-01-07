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
	const JIndex							initialSelection,
	const JIndex							firstUnusedID
	)
{
	JArray<JCoordinate> heights(3);
	heights.AppendItem(150);
	heights.AppendItem(150);
	heights.AppendItem(150);

	const JIndex elasticIndex = 3;

	JArray<JCoordinate> minHeights(3);
	minHeights.AppendItem(60);
	minHeights.AppendItem(60);
	minHeights.AppendItem(60);

// begin JXLayout

	auto* window = jnew JXWindow(this, 350,530, JString::empty);

	itsPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 310,460);
	assert( itsPartition != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,500, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 250,500, 70,20);
	assert( okButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 145,500, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::EditMacroDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::EditMacroDialog"));
	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	// create action table

	JXContainer* compartment = itsPartition->GetCompartment(2);

// begin actionLayout

	const JRect actionLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(310 - actionLayout_Aperture.width(), 150 - actionLayout_Aperture.height());

	auto* newActionButton =
		jnew JXTextButton(JGetString("newActionButton::EditMacroDialog::actionLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,30, 60,20);
	assert( newActionButton != nullptr );

	auto* removeActionButton =
		jnew JXTextButton(JGetString("removeActionButton::EditMacroDialog::actionLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,60, 60,20);
	assert( removeActionButton != nullptr );

	auto* actionScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 240,130);
	assert( actionScrollbarSet != nullptr );

	auto* actionColHeaderEncl =
		jnew JXWidgetSet(compartment,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 240,20);
	assert( actionColHeaderEncl != nullptr );

	auto* loadActionFileButton =
		jnew JXTextButton(JGetString("loadActionFileButton::EditMacroDialog::actionLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,100, 60,20);
	assert( loadActionFileButton != nullptr );

	auto* saveActionFileButton =
		jnew JXTextButton(JGetString("saveActionFileButton::EditMacroDialog::actionLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,130, 60,20);
	assert( saveActionFileButton != nullptr );

	compartment->SetSize(actionLayout_Aperture.width(), actionLayout_Aperture.height());

// end actionLayout

	itsActionTable =
		jnew CharActionTable(this, newActionButton, removeActionButton,
							  loadActionFileButton, saveActionFileButton,
							  actionScrollbarSet, actionScrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsActionTable != nullptr );

	actionColHeaderEncl->ClearNeedsInternalFTC();

	auto* colHeader =
		jnew JXColHeaderWidget(itsActionTable, actionScrollbarSet, actionColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, JGetString("Column1Char::EditMacroDialog"));
	colHeader->SetColTitle(2, JGetString("Column2::EditMacroDialog"));
	colHeader->TurnOnColResizing(20);

	// create macro table

	compartment = itsPartition->GetCompartment(3);

// begin macroLayout

	const JRect macroLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(310 - macroLayout_Aperture.width(), 150 - macroLayout_Aperture.height());

	auto* newMacroButton =
		jnew JXTextButton(JGetString("newMacroButton::EditMacroDialog::macroLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,30, 60,20);
	assert( newMacroButton != nullptr );

	auto* removeMacroButton =
		jnew JXTextButton(JGetString("removeMacroButton::EditMacroDialog::macroLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,60, 60,20);
	assert( removeMacroButton != nullptr );

	auto* macroScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 240,130);
	assert( macroScrollbarSet != nullptr );

	auto* macroColHeaderEncl =
		jnew JXWidgetSet(compartment,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 240,20);
	assert( macroColHeaderEncl != nullptr );

	auto* loadMacroFileButton =
		jnew JXTextButton(JGetString("loadMacroFileButton::EditMacroDialog::macroLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,100, 60,20);
	assert( loadMacroFileButton != nullptr );

	auto* saveMacroFileButton =
		jnew JXTextButton(JGetString("saveMacroFileButton::EditMacroDialog::macroLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,130, 60,20);
	assert( saveMacroFileButton != nullptr );

	compartment->SetSize(macroLayout_Aperture.width(), macroLayout_Aperture.height());

// end macroLayout

	itsMacroTable =
		jnew MacroTable(this, newMacroButton, removeMacroButton,
						 loadMacroFileButton, saveMacroFileButton,
						 macroScrollbarSet, macroScrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsMacroTable != nullptr );

	macroColHeaderEncl->ClearNeedsInternalFTC();

	colHeader =
		jnew JXColHeaderWidget(itsMacroTable, macroScrollbarSet, macroColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, JGetString("Column1Macro::EditMacroDialog"));
	colHeader->SetColTitle(2, JGetString("Column2::EditMacroDialog"));
	colHeader->TurnOnColResizing(20);

	// create macro set table

	compartment = itsPartition->GetCompartment(1);

// begin macroSetLayout

	const JRect macroSetLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(310 - macroSetLayout_Aperture.width(), 150 - macroSetLayout_Aperture.height());

	auto* newMacroSetButton =
		jnew JXTextButton(JGetString("newMacroSetButton::EditMacroDialog::macroSetLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,30, 60,20);
	assert( newMacroSetButton != nullptr );

	auto* removeMacroSetButton =
		jnew JXTextButton(JGetString("removeMacroSetButton::EditMacroDialog::macroSetLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,60, 60,20);
	assert( removeMacroSetButton != nullptr );

	auto* macroSetScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 240,130);
	assert( macroSetScrollbarSet != nullptr );

	auto* macroSetColHeaderEncl =
		jnew JXWidgetSet(compartment,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 240,20);
	assert( macroSetColHeaderEncl != nullptr );

	compartment->SetSize(macroSetLayout_Aperture.width(), macroSetLayout_Aperture.height());

// end macroSetLayout

	itsMacroSetTable =
		jnew MacroSetTable(macroList, initialSelection, firstUnusedID,
							itsActionTable, itsMacroTable,
							newMacroSetButton, removeMacroSetButton,
							macroSetScrollbarSet, macroSetScrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsMacroSetTable != nullptr );
	itsMacroSetTable->FitToEnclosure();

	macroSetColHeaderEncl->ClearNeedsInternalFTC();

	colHeader =
		jnew JXColHeaderWidget(itsMacroSetTable, macroSetScrollbarSet, macroSetColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, JGetString("Column1Set::EditMacroDialog"));
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

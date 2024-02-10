/******************************************************************************
 EditCRMDialog.cpp

	BASE CLASS = JXModalDialogDirector, JPrefObject

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "EditCRMDialog.h"
#include "CRMRuleListTable.h"
#include "CRMRuleTable.h"
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

EditCRMDialog::EditCRMDialog
	(
	JArray<PrefsManager::CRMRuleListInfo>*	crmList,
	const JIndex							initialSelection,
	const JIndex							firstUnusedID
	)
	:
	JXModalDialogDirector(true),
	JPrefObject(GetPrefsManager(), kEditCRMDialogID)
{
	BuildWindow(crmList, initialSelection, firstUnusedID);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditCRMDialog::~EditCRMDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 GetCRMRuleLists

 ******************************************************************************/

JArray<PrefsManager::CRMRuleListInfo>*
EditCRMDialog::GetCRMRuleLists
	(
	JIndex* firstNewID,
	JIndex* lastNewID
	)
	const
{
	return itsCRMTable->GetCRMRuleLists(firstNewID, lastNewID);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
EditCRMDialog::BuildWindow
	(
	JArray<PrefsManager::CRMRuleListInfo>*	crmList,
	const JIndex							initialSelection,
	const JIndex							firstUnusedID
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 540,380, JGetString("WindowTitle::EditCRMDialog::JXLayout"));

	JArray<JCoordinate> itsPartition_sizes, itsPartition_minSizes;
	itsPartition_sizes.AppendItem(152);
	itsPartition_minSizes.AppendItem(60);
	itsPartition_sizes.AppendItem(158);
	itsPartition_minSizes.AppendItem(60);

	itsPartition =
		jnew JXVertPartition(itsPartition_sizes, 2, itsPartition_minSizes, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 500,315);

	auto* crmScrollbarSet =
		jnew JXScrollbarSet(itsPartition->GetCompartment(1),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 430,152);
	assert( crmScrollbarSet != nullptr );

	auto* newCRMButton =
		jnew JXTextButton(JGetString("newCRMButton::EditCRMDialog::JXLayout"), itsPartition->GetCompartment(1),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,30, 60,20);
	assert( newCRMButton != nullptr );

	auto* removeCRMButton =
		jnew JXTextButton(JGetString("removeCRMButton::EditCRMDialog::JXLayout"), itsPartition->GetCompartment(1),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,60, 60,20);
	assert( removeCRMButton != nullptr );

	auto* ruleScrollbarSet =
		jnew JXScrollbarSet(itsPartition->GetCompartment(2),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 430,158);
	assert( ruleScrollbarSet != nullptr );

	auto* newRuleButton =
		jnew JXTextButton(JGetString("newRuleButton::EditCRMDialog::JXLayout"), itsPartition->GetCompartment(2),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,30, 60,20);
	assert( newRuleButton != nullptr );

	auto* removeRuleButton =
		jnew JXTextButton(JGetString("removeRuleButton::EditCRMDialog::JXLayout"), itsPartition->GetCompartment(2),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,60, 60,20);
	assert( removeRuleButton != nullptr );

	auto* loadRuleFileButton =
		jnew JXTextButton(JGetString("loadRuleFileButton::EditCRMDialog::JXLayout"), itsPartition->GetCompartment(2),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,100, 60,20);
	assert( loadRuleFileButton != nullptr );

	auto* saveRuleFileButton =
		jnew JXTextButton(JGetString("saveRuleFileButton::EditCRMDialog::JXLayout"), itsPartition->GetCompartment(2),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,130, 60,20);
	assert( saveRuleFileButton != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditCRMDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,350, 70,20);
	assert( cancelButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditCRMDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 235,350, 70,20);
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::shortcuts::EditCRMDialog::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditCRMDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 400,350, 70,20);
	assert( okButton != nullptr );

	itsRuleTable =
		jnew CRMRuleTable(this, newRuleButton, removeRuleButton, loadRuleFileButton, saveRuleFileButton, ruleScrollbarSet, ruleScrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 430,138);

	itsCRMTable =
		jnew CRMRuleListTable(crmList, initialSelection, firstUnusedID, itsRuleTable, newCRMButton, removeCRMButton, crmScrollbarSet, crmScrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 430,132);

	auto* ruleColHeader =
		jnew JXColHeaderWidget(itsRuleTable, ruleScrollbarSet, ruleScrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 430,20);
	assert( ruleColHeader != nullptr );

	auto* crmColHeader =
		jnew JXColHeaderWidget(itsCRMTable, crmScrollbarSet, crmScrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 430,20);
	assert( crmColHeader != nullptr );

// end JXLayout

	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	crmColHeader->SetColTitle(1, JGetString("Column1::EditCRMDialog"));

	ruleColHeader->TurnOnColResizing(20);
	itsRuleTable->SetColTitles(ruleColHeader);
}

/******************************************************************************
 GetCurrentCRMRuleSetName

 ******************************************************************************/

bool
EditCRMDialog::GetCurrentCRMRuleSetName
	(
	JString* name
	)
	const
{
	return itsCRMTable->GetCurrentCRMRuleSetName(name);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
EditCRMDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("CRMHelp");
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
EditCRMDialog::ReadPrefs
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
	itsRuleTable->ReadGeometry(input);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
EditCRMDialog::WritePrefs
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

	output << ' ';
	itsRuleTable->WriteGeometry(output);
}

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
	JArray<JCoordinate> heights(2);
	heights.AppendItem(150);
	heights.AppendItem(150);

	const JIndex elasticIndex = 2;

	JArray<JCoordinate> minHeights(2);
	minHeights.AppendItem(60);
	minHeights.AppendItem(60);

// begin JXLayout

	auto* window = jnew JXWindow(this, 540,370, JString::empty);

	itsPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 500,305);
	assert( itsPartition != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditCRMDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,340, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditCRMDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 400,340, 70,20);
	assert( okButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditCRMDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 235,340, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::EditCRMDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::EditCRMDialog"));
	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	// create rule table

	JXContainer* compartment = itsPartition->GetCompartment(2);

// begin ruleLayout

	const JRect ruleLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(500 - ruleLayout_Aperture.width(), 150 - ruleLayout_Aperture.height());

	auto* newRuleButton =
		jnew JXTextButton(JGetString("newRuleButton::EditCRMDialog::ruleLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,30, 60,20);
	assert( newRuleButton != nullptr );

	auto* removeRuleButton =
		jnew JXTextButton(JGetString("removeRuleButton::EditCRMDialog::ruleLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,60, 60,20);
	assert( removeRuleButton != nullptr );

	auto* ruleScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 430,150);
	assert( ruleScrollbarSet != nullptr );

	auto* loadRuleFileButton =
		jnew JXTextButton(JGetString("loadRuleFileButton::EditCRMDialog::ruleLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,100, 60,20);
	assert( loadRuleFileButton != nullptr );

	auto* saveRuleFileButton =
		jnew JXTextButton(JGetString("saveRuleFileButton::EditCRMDialog::ruleLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,130, 60,20);
	assert( saveRuleFileButton != nullptr );

	compartment->SetSize(ruleLayout_Aperture.width(), ruleLayout_Aperture.height());

// end ruleLayout

	JXWidgetSet* encl                  = ruleScrollbarSet->GetScrollEnclosure();
	const JCoordinate kColHeaderHeight = 20;
	const JCoordinate w                = encl->GetApertureWidth();

	itsRuleTable =
		jnew CRMRuleTable(this, newRuleButton, removeRuleButton,
						   loadRuleFileButton, saveRuleFileButton,
						   ruleScrollbarSet, encl,
						   JXWidget::kHElastic, JXWidget::kVElastic,
						   0, kColHeaderHeight,
						   w, encl->GetApertureHeight() - kColHeaderHeight);
	assert( itsRuleTable != nullptr );

	auto* colHeader =
		jnew JXColHeaderWidget(itsRuleTable, ruleScrollbarSet, encl,
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  0,0, w, kColHeaderHeight);
	colHeader->TurnOnColResizing(20);
	itsRuleTable->SetColTitles(colHeader);

	// create CRM table

	compartment = itsPartition->GetCompartment(1);

// begin crmLayout

	const JRect crmLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(500 - crmLayout_Aperture.width(), 150 - crmLayout_Aperture.height());

	auto* newCRMButton =
		jnew JXTextButton(JGetString("newCRMButton::EditCRMDialog::crmLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,30, 60,20);
	assert( newCRMButton != nullptr );

	auto* removeCRMButton =
		jnew JXTextButton(JGetString("removeCRMButton::EditCRMDialog::crmLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,60, 60,20);
	assert( removeCRMButton != nullptr );

	auto* crmScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 430,130);
	assert( crmScrollbarSet != nullptr );

	auto* crmColHeaderEncl =
		jnew JXWidgetSet(compartment,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 430,20);
	assert( crmColHeaderEncl != nullptr );

	compartment->SetSize(crmLayout_Aperture.width(), crmLayout_Aperture.height());

// end crmLayout

	crmColHeaderEncl->ClearNeedsInternalFTC();

	itsCRMTable =
		jnew CRMRuleListTable(crmList, initialSelection, firstUnusedID, itsRuleTable,
							   newCRMButton, removeCRMButton,
							   crmScrollbarSet, crmScrollbarSet->GetScrollEnclosure(),
							   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsCRMTable != nullptr );
	itsCRMTable->FitToEnclosure();

	colHeader =
		jnew JXColHeaderWidget(itsCRMTable, crmScrollbarSet, crmColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, JGetString("Column1::EditCRMDialog"));
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

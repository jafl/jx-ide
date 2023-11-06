/******************************************************************************
 EditFileTypesDialog.cpp

	BASE CLASS = JXModalDialogDirector, JPrefObject

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "EditFileTypesDialog.h"
#include "FileTypeTable.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
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

EditFileTypesDialog::EditFileTypesDialog
	(
	const JArray<PrefsManager::FileTypeInfo>&		fileTypeList,
	const JArray<PrefsManager::MacroSetInfo>&		macroList,
	const JArray<PrefsManager::CRMRuleListInfo>&	crmList,
	const bool										execOutputWordWrap,
	const bool										unknownTypeWordWrap
	)
	:
	JXModalDialogDirector(true),
	JPrefObject(GetPrefsManager(), kEditFileTypesDialogID)
{
	BuildWindow(fileTypeList, macroList, crmList,
				execOutputWordWrap, unknownTypeWordWrap);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditFileTypesDialog::~EditFileTypesDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
EditFileTypesDialog::BuildWindow
	(
	const JArray<PrefsManager::FileTypeInfo>&		fileTypeList,
	const JArray<PrefsManager::MacroSetInfo>&		macroList,
	const JArray<PrefsManager::CRMRuleListInfo>&	crmList,
	const bool										execOutputWordWrap,
	const bool										unknownTypeWordWrap
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 700,400, JString::empty);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 130,370, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 510,370, 70,20);
	assert( okButton != nullptr );

	itsExecOutputWrapCB =
		jnew JXTextCheckbox(JGetString("itsExecOutputWrapCB::EditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 80,300, 310,20);
	assert( itsExecOutputWrapCB != nullptr );

	itsUnknownTypeWrapCB =
		jnew JXTextCheckbox(JGetString("itsUnknownTypeWrapCB::EditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 80,330, 310,20);
	assert( itsUnknownTypeWrapCB != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 315,370, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::EditFileTypesDialog::shortcuts::JXLayout"));

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 570,260);
	assert( scrollbarSet != nullptr );

	auto* addTypeButton =
		jnew JXTextButton(JGetString("addTypeButton::EditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 610,50, 70,20);
	assert( addTypeButton != nullptr );
	addTypeButton->SetShortcuts(JGetString("addTypeButton::EditFileTypesDialog::shortcuts::JXLayout"));

	auto* duplicateTypeButton =
		jnew JXTextButton(JGetString("duplicateTypeButton::EditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 610,80, 70,20);
	assert( duplicateTypeButton != nullptr );
	duplicateTypeButton->SetShortcuts(JGetString("duplicateTypeButton::EditFileTypesDialog::shortcuts::JXLayout"));

	auto* removeTypeButton =
		jnew JXTextButton(JGetString("removeTypeButton::EditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 610,110, 70,20);
	assert( removeTypeButton != nullptr );
	removeTypeButton->SetShortcuts(JGetString("removeTypeButton::EditFileTypesDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::EditFileTypesDialog"));
	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	JXWidgetSet* encl = scrollbarSet->GetScrollEnclosure();
	const JCoordinate kColHeaderHeight = 20;
	const JCoordinate w = encl->GetApertureWidth();

	itsTable =
		jnew FileTypeTable(fileTypeList, macroList, crmList,
							addTypeButton, removeTypeButton,
							duplicateTypeButton, scrollbarSet, encl,
							JXWidget::kHElastic, JXWidget::kVElastic,
							0, kColHeaderHeight,
							w, encl->GetApertureHeight() - kColHeaderHeight);
	assert( itsTable != nullptr );

	auto* colHeader =
		jnew JXColHeaderWidget(itsTable, scrollbarSet, encl,
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  0,0, w, kColHeaderHeight);
	colHeader->TurnOnColResizing(20);
	itsTable->SetColTitles(colHeader);

	itsExecOutputWrapCB->SetState(execOutputWordWrap);
	itsUnknownTypeWrapCB->SetState(unknownTypeWordWrap);
}

/******************************************************************************
 GetFileTypeInfo

 ******************************************************************************/

void
EditFileTypesDialog::GetFileTypeInfo
	(
	JArray<PrefsManager::FileTypeInfo>*	fileTypeList,
	bool*								execOutputWordWrap,
	bool*								unknownTypeWordWrap
	)
	const
{
	itsTable->GetFileTypeList(fileTypeList);

	*execOutputWordWrap  = itsExecOutputWrapCB->IsChecked();
	*unknownTypeWordWrap = itsUnknownTypeWrapCB->IsChecked();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
EditFileTypesDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("FileTypeHelp");
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
EditFileTypesDialog::ReadPrefs
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

	itsTable->ReadGeometry(input);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
EditFileTypesDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsTable->WriteGeometry(output);
}

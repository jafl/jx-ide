/******************************************************************************
 EditSymbolPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "EditSymbolPrefsDialog.h"
#include "ProjectDocument.h"
#include "SymbolDirector.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

EditSymbolPrefsDialog::EditSymbolPrefsDialog
	(
	const bool raiseTreeOnRightClick
	)
	:
	JXDialogDirector(GetApplication(), true)
{
	BuildWindow(raiseTreeOnRightClick);
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditSymbolPrefsDialog::~EditSymbolPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
EditSymbolPrefsDialog::BuildWindow
	(
	const bool raiseTreeOnRightClick
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 470,90, JString::empty);
	assert( window != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditSymbolPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,60, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditSymbolPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 340,60, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::EditSymbolPrefsDialog::shortcuts::JXLayout"));

	itsRaiseTreeOnRightClickCB =
		jnew JXTextCheckbox(JGetString("itsRaiseTreeOnRightClickCB::EditSymbolPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 430,20);
	assert( itsRaiseTreeOnRightClickCB != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditSymbolPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 195,60, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::EditSymbolPrefsDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::EditSymbolPrefsDialog"));
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	itsRaiseTreeOnRightClickCB->SetState(raiseTreeOnRightClick);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
EditSymbolPrefsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			UpdateSettings();
		}
	}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		(JXGetHelpManager())->ShowSection("SymbolHelp-Prefs");
	}

	else
	{
		JXDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateSettings (private)

 ******************************************************************************/

void
EditSymbolPrefsDialog::UpdateSettings()
{
	GetApplication()->DisplayBusyCursor();

	JPtrArray<ProjectDocument>* docList =
		GetDocumentManager()->GetProjectDocList();
	const JSize docCount = docList->GetElementCount();

//	JProgressDisplay* pg = JNewPG();
//	pg->FixedLengthProcessBeginning(docCount, "Updating preferences...",
//									false, false);

	for (JIndex i=1; i<=docCount; i++)
	{
		((docList->GetElement(i))->GetSymbolDirector())->
			SetPrefs(itsRaiseTreeOnRightClickCB->IsChecked(),
					 i==1);

//		pg->IncrementProgress();
	}

//	pg->ProcessFinished();
//	jdelete pg;
}

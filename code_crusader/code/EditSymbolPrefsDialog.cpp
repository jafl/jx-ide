/******************************************************************************
 EditSymbolPrefsDialog.cpp

	BASE CLASS = JXModalDialogDirector

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
	JXModalDialogDirector()
{
	BuildWindow(raiseTreeOnRightClick);
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
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("SymbolHelp-Prefs");
	}

	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateSettings

 ******************************************************************************/

void
EditSymbolPrefsDialog::UpdateSettings()
{
	GetApplication()->DisplayBusyCursor();

	JIndex i = 1;
	for (auto* doc : *GetDocumentManager()->GetProjectDocList())
	{
		doc->GetSymbolDirector()->
			SetPrefs(itsRaiseTreeOnRightClickCB->IsChecked(), i==1);
		i++;
	}
}

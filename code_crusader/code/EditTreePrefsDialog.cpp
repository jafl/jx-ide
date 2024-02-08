/******************************************************************************
 EditTreePrefsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "EditTreePrefsDialog.h"
#include "ProjectDocument.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXFontSizeMenu.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

EditTreePrefsDialog::EditTreePrefsDialog
	(
	const JSize	fontSize,
	const bool	autoMinMILinks,
	const bool	drawMILinksOnTop,
	const bool	raiseWhenSingleMatch
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(fontSize, autoMinMILinks, drawMILinksOnTop, raiseWhenSingleMatch);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditTreePrefsDialog::~EditTreePrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
EditTreePrefsDialog::BuildWindow
	(
	const JSize	fontSize,
	const bool	autoMinMILinks,
	const bool	drawMILinksOnTop,
	const bool	raiseWhenSingleMatch
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 320,270, JGetString("WindowTitle::EditTreePrefsDialog::JXLayout"));

	itsFontSizeMenu =
		jnew JXFontSizeMenu(JFontManager::GetDefaultFontName(), JGetString("FontSize::EditTreePrefsDialog"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 280,30);

	itsRaiseSingleMatchCB =
		jnew JXTextCheckbox(JGetString("itsRaiseSingleMatchCB::EditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 280,20);

	itsAutoMinMILinkCB =
		jnew JXTextCheckbox(JGetString("itsAutoMinMILinkCB::EditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,110, 280,20);

	itsMILinkStyleRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 140,140, 134,74);

	auto* aboveLabel =
		jnew JXTextRadioButton(1, JGetString("aboveLabel::EditTreePrefsDialog::JXLayout"), itsMILinkStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 110,20);
	assert( aboveLabel != nullptr );

	auto* miLinksLabel =
		jnew JXStaticText(JGetString("miLinksLabel::EditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 45,170, 90,20);
	miLinksLabel->SetToLabel(false);

	auto* belowLabel =
		jnew JXTextRadioButton(2, JGetString("belowLabel::EditTreePrefsDialog::JXLayout"), itsMILinkStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 110,20);
	assert( belowLabel != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,240, 70,20);
	assert( cancelButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 125,240, 70,20);
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::shortcuts::EditTreePrefsDialog::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 219,239, 72,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::EditTreePrefsDialog::JXLayout"));

// end JXLayout

	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	itsFontSizeMenu->SetFontSize(fontSize);
	itsFontSizeMenu->SetToPopupChoice();

	itsAutoMinMILinkCB->SetState(autoMinMILinks);
	itsRaiseSingleMatchCB->SetState(raiseWhenSingleMatch);

	itsMILinkStyleRG->SelectItem(drawMILinksOnTop ? kDrawMILinksAbove : kDrawMILinksBelow);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
EditTreePrefsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("TreePrefsHelp");
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
EditTreePrefsDialog::UpdateSettings()
{
	GetApplication()->DisplayBusyCursor();

	JPtrArray<ProjectDocument>* docList =
		GetDocumentManager()->GetProjectDocList();

	JProgressDisplay* pg = JNewPG();
	pg->FixedLengthProcessBeginning(docList->GetItemCount(),
		JGetString("UpdatingPrefs::EditTreePrefsDialog"), false, true);

	JIndex i = 1;
	for (auto* doc : *docList)
	{
		doc->SetTreePrefs(itsFontSizeMenu->GetFontSize(),
						  itsAutoMinMILinkCB->IsChecked(),
						  itsMILinkStyleRG->GetSelectedItem() == kDrawMILinksAbove,
						  itsRaiseSingleMatchCB->IsChecked(),
						  i==1);

		pg->IncrementProgress();
		i++;
	}

	pg->ProcessFinished();
	jdelete pg;
}

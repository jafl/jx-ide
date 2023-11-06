/******************************************************************************
 ViewManPageDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#include "ViewManPageDialog.h"
#include "ManPageDocument.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXCharInput.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXStringHistoryMenu.h>
#include <jx-af/jx/JXDocumentMenu.h>
#include <jx-af/jx/JXCSFDialogBase.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jAssert.h>

const JSize kHistoryLength = 20;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version 1 stores itsStayOpenCB

/******************************************************************************
 Constructor

 ******************************************************************************/

ViewManPageDialog::ViewManPageDialog
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GetPrefsManager(), kViewManPagePrefID)
{
	BuildWindow();
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ViewManPageDialog::~ViewManPageDialog()
{
	// prefs written by DeleteGlobals()
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
ViewManPageDialog::Activate()
{
	JXWindowDirector::Activate();

	if (IsActive())
	{
		itsFnName->Focus();
		itsFnName->SelectAll();
	}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
ViewManPageDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 360,150, JString::empty);

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::ViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 120,120, 60,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::ViewManPageDialog::shortcuts::JXLayout"));

	itsViewButton =
		jnew JXTextButton(JGetString("itsViewButton::ViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 280,120, 60,20);
	assert( itsViewButton != nullptr );
	itsViewButton->SetShortcuts(JGetString("itsViewButton::ViewManPageDialog::shortcuts::JXLayout"));

	itsFnName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 290,20);
	assert( itsFnName != nullptr );

	auto* nameLabel =
		jnew JXStaticText(JGetString("nameLabel::ViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 100,20);
	assert( nameLabel != nullptr );
	nameLabel->SetToLabel();

	auto* sectionLabel =
		jnew JXStaticText(JGetString("sectionLabel::ViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 90,20);
	assert( sectionLabel != nullptr );
	sectionLabel->SetToLabel();

	itsAproposCheckbox =
		jnew JXTextCheckbox(JGetString("itsAproposCheckbox::ViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 210,80, 80,20);
	assert( itsAproposCheckbox != nullptr );
	itsAproposCheckbox->SetShortcuts(JGetString("itsAproposCheckbox::ViewManPageDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::ViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 200,120, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::ViewManPageDialog::shortcuts::JXLayout"));

	itsFnHistoryMenu =
		jnew JXStringHistoryMenu(kHistoryLength, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 310,40, 30,20);
	assert( itsFnHistoryMenu != nullptr );

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::ViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,120, 90,20);
	assert( itsStayOpenCB != nullptr );

	itsManIndex =
		jnew JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,80, 50,20);
	assert( itsManIndex != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::ViewManPageDialog"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(true);

	ListenTo(itsViewButton);
	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);
	ListenTo(itsFnHistoryMenu);

	itsFnName->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);
	ListenTo(itsFnName);

	itsManIndex->SetIsRequired(false);
	itsStayOpenCB->SetState(true);

	// create hidden JXDocument so Meta-# shortcuts work

	jnew JXDocumentMenu(JString::empty, window,
					   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
ViewManPageDialog::UpdateDisplay()
{
	if (itsFnName->GetText()->IsEmpty())
	{
		itsViewButton->Deactivate();
	}
	else
	{
		itsViewButton->Activate();
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
ViewManPageDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsViewButton && message.Is(JXButton::kPushed))
	{
		ViewManPage();
		if (!itsStayOpenCB->IsChecked())
		{
			Deactivate();
		}
	}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
	{
		Deactivate();
	}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("ManPageHelp");
	}

	else if (sender == itsFnHistoryMenu && message.Is(JXMenu::kItemSelected))
	{
		SetFunction(itsFnHistoryMenu->GetItemText(message));
		itsFnName->Focus();
	}

	else if (sender == itsFnName &&
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
	{
		UpdateDisplay();
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 ViewManPage (private)

 ******************************************************************************/

void
ViewManPageDialog::ViewManPage()
{
	itsFnName->GetText()->DeactivateCurrentUndo();

	if (itsFnName->InputValid() && itsManIndex->InputValid())
	{
		JString manIndex;
		if (!itsManIndex->GetText()->IsEmpty())
		{
			manIndex = itsManIndex->GetText()->GetText();
		}

		ManPageDocument::Create(nullptr, itsFnName->GetText()->GetText(), manIndex,
								  itsAproposCheckbox->IsChecked());
	}
}

/******************************************************************************
 AddToHistory

	By having ManPageDocument call this function, others can create
	ManPageDocuments directly without telling us.

 ******************************************************************************/

void
ViewManPageDialog::AddToHistory
	(
	const JString&	pageName,
	const JString&	pageIndex,
	const bool	apropos
	)
{
	JString historyStr = pageName;
	if (apropos)
	{
		historyStr += " (*)";
	}
	else if (!pageIndex.IsEmpty())
	{
		historyStr += " (";
		historyStr += pageIndex;
		historyStr += ")";
	}

	itsFnHistoryMenu->AddString(historyStr);
}

/******************************************************************************
 SetFunction (private)

 ******************************************************************************/

void
ViewManPageDialog::SetFunction
	(
	const JString& historyStr
	)
{
	JString fnName = historyStr;

	JUtf8Character manIndex;
	if (fnName.GetLastCharacter() == ')')
	{
		JStringIterator iter(&fnName, JStringIterator::kStartAtEnd);
		iter.SkipPrev();
		iter.Prev(&manIndex);
		iter.SkipPrev(2);
		iter.RemoveAllNext();
	}

	if (manIndex == '*')
	{
		manIndex = ' ';
		itsAproposCheckbox->SetState(true);
	}
	else
	{
		itsAproposCheckbox->SetState(false);
	}

	itsFnName->GetText()->SetText(fnName);
	itsManIndex->GetText()->SetText(JString(manIndex));
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
ViewManPageDialog::ReadPrefs
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

	itsFnHistoryMenu->ReadSetup(input);

	if (vers >= 1)
	{
		bool stayOpen;
		input >> JBoolFromString(stayOpen);
		itsStayOpenCB->SetState(stayOpen);
	}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
ViewManPageDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsFnHistoryMenu->WriteSetup(output);

	output << ' ' << JBoolToString(itsStayOpenCB->IsChecked());
	output << ' ';
}

/******************************************************************************
 ChooseProcessDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#include "ChooseProcessDialog.h"
#include "ProcessText.h"
#include "gdb/GDBLink.h"
#include "globals.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXFontManager.h>

#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

#ifdef _J_CYGWIN
static const JString kCmdStr("ps s", JString::kNoCopy);
#else
static const JString kCmdStr("ps xco pid,stat,command", JString::kNoCopy);
#endif

/******************************************************************************
 Constructor

 ******************************************************************************/

ChooseProcessDialog::ChooseProcessDialog
	(
	JXDirector*	supervisor,
	const bool	attachToSelection,
	const bool	stopProgram
	)
	:
	JXDialogDirector(supervisor, true),
	itsAttachToSelectionFlag(attachToSelection),
	itsStopProgramFlag(stopProgram)
{
	BuildWindow();

	int inFD;
	const JError err = JExecute(kCmdStr, nullptr,
								kJIgnoreConnection, nullptr,
								kJCreatePipe, &inFD);
	if (err.OK())
	{
		JString text;
		JReadAll(inFD, &text);
		text.TrimWhitespace();
		itsText->GetText()->SetText(text);
	}

	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ChooseProcessDialog::~ChooseProcessDialog()
{
}

/******************************************************************************
 SetProcessID

 ******************************************************************************/

void
ChooseProcessDialog::SetProcessID
	(
	const JInteger value
	)
{
	itsProcessIDInput->SetValue(value);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
ChooseProcessDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 390,500, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,50, 350,400);
	assert( scrollbarSet != nullptr );

	itsProcessIDInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 90,470, 80,20);
	assert( itsProcessIDInput != nullptr );

	auto* pidLabel =
		jnew JXStaticText(JGetString("pidLabel::ChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,470, 70,20);
	assert( pidLabel != nullptr );
	pidLabel->SetToLabel();

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 215,470, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::ChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 310,470, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::ChooseProcessDialog::shortcuts::JXLayout"));

	auto* hint =
		jnew JXStaticText(JGetString("hint::ChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 360,20);
	assert( hint != nullptr );
	hint->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::ChooseProcessDialog"));
	SetButtons(okButton, cancelButton);

	itsText =
		jnew ProcessText(this, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsText != nullptr );
	itsText->FitToEnclosure();
	itsText->GetText()->SetDefaultFont(JFontManager::GetDefaultMonospaceFont());
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
ChooseProcessDialog::Receive
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
			JInteger pid;
			const bool ok = itsProcessIDInput->GetValue(&pid);
			assert(ok);
			if (itsAttachToSelectionFlag)
			{
				GetLink()->AttachToProcess(pid);
			}
			else
			{
				dynamic_cast<GDBLink*>(GetLink())->ProgramStarted(pid);
			}

			if (itsStopProgramFlag)
			{
				GetLink()->StopProgram();
			}
		}
	}

	JXDialogDirector::Receive(sender, message);
}

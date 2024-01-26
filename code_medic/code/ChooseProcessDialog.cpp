/******************************************************************************
 ChooseProcessDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#include "ChooseProcessDialog.h"
#include "ProcessText.h"
#include "globals.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringMatch.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

#ifdef _J_CYGWIN
static const JString kCmdStr("ps s");
#else
static const JString kCmdStr("ps xco pid,stat,command");
#endif

/******************************************************************************
 Constructor

 ******************************************************************************/

ChooseProcessDialog::ChooseProcessDialog()
	:
	JXModalDialogDirector()
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

		JString fullName;
		if (GetLink()->GetProgram(&fullName))
		{
			JString path, name;
			JSplitPathAndName(fullName, &path, &name);

			JRegex p(JRegex::BackslashForLiteral(name));

			JStyledText::TextIndex i(1,1);
			bool wrapped;
			while (true)
			{
				const JStringMatch m = itsText->GetText()->SearchForward(i, p, true, false, &wrapped);
				if (m.IsEmpty())
				{
					break;
				}

				const JStyledText::TextRange r(m.GetCharacterRange(), m.GetUtf8ByteRange());

				itsText->GetText()->SetFontStyle(r,
					JFontStyle(true, false, 0, false), true);

				i = r.GetAfter();
			}
		}
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ChooseProcessDialog::~ChooseProcessDialog()
{
}

/******************************************************************************
 GetProcessID

 ******************************************************************************/

bool
ChooseProcessDialog::GetProcessID
	(
	JInteger* pid
	)
	const
{
	return itsProcessIDInput->GetValue(pid);
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

	auto* window = jnew JXWindow(this, 390,500, JGetString("WindowTitle::ChooseProcessDialog::JXLayout"));

	auto* hint =
		jnew JXStaticText(JGetString("hint::ChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 360,20);
	hint->SetToLabel(false);

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,50, 350,400);
	assert( scrollbarSet != nullptr );

	itsText =
		jnew ProcessText(this, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 350,400);

	auto* pidLabel =
		jnew JXStaticText(JGetString("pidLabel::ChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,470, 70,20);
	pidLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 215,470, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::ChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 309,469, 62,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::ChooseProcessDialog::JXLayout"));

	itsProcessIDInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 90,470, 80,20);
	itsProcessIDInput->SetIsRequired(false);

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsText->GetText()->SetDefaultFont(JFontManager::GetDefaultMonospaceFont());
}

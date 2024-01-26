/******************************************************************************
 EditCommandsDialog.cpp

	BASE CLASS = public JXModalDialogDirector, JPrefObject

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#include <EditCommandsDialog.h>
#include <EditCommandsTable.h>
#include <PrefsManager.h>
#include <globals.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

EditCommandsDialog::EditCommandsDialog()
	:
	JXModalDialogDirector(true),
	JPrefObject(GetPrefsManager(), kEditCommandsDialogID)
{
	itsCommands	= jnew JPtrArray<JString>(JPtrArrayT::kForgetAll);
	assert(itsCommands != nullptr);

	GetPrefsManager()->GetCmdList(itsCommands);

	BuildWindow();

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

EditCommandsDialog::~EditCommandsDialog()
{
	JPrefObject::WritePrefs();

	itsCommands->DeleteAll();
	jdelete itsCommands;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
EditCommandsDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 350,370, JGetString("WindowTitle::EditCommandsDialog::JXLayout"));

	auto* gdbCmdTitle =
		jnew JXStaticText(JGetString("gdbCmdTitle::EditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 150,20);
	gdbCmdTitle->SetToLabel(false);

	auto* hint =
		jnew JXStaticText(JGetString("hint::EditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 270,20);
	hint->SetToLabel(false);

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,50, 230,280);
	assert( scrollbarSet != nullptr );

	itsNewButton =
		jnew JXTextButton(JGetString("itsNewButton::EditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,70, 70,20);
	itsNewButton->SetShortcuts(JGetString("itsNewButton::shortcuts::EditCommandsDialog::JXLayout"));

	itsRemoveButton =
		jnew JXTextButton(JGetString("itsRemoveButton::EditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,110, 70,20);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 80,340, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 209,339, 62,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::EditCommandsDialog::JXLayout"));

	auto* itsWidget =
		jnew EditCommandsTable(this, itsRemoveButton, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 230,280);
	assert( itsWidget != nullptr );

// end JXLayout

	SetButtons(okButton, cancelButton);

	ListenTo(itsNewButton);
	ListenTo(itsRemoveButton);

	itsWidget->SyncWithData();

	hint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
EditCommandsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsNewButton && message.Is(JXButton::kPushed))
	{
		itsCommands->Append(JString::empty);
		itsWidget->NewStringAppended();
	}
	if (sender == itsRemoveButton && message.Is(JXButton::kPushed))
	{
		itsWidget->RemoveCurrent();
	}
	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 AppendString

 ******************************************************************************/

void
EditCommandsDialog::AppendString
	(
	const JString& str
	)
{
	itsCommands->Append(str);
}

/******************************************************************************
 SetString

 ******************************************************************************/

void
EditCommandsDialog::SetString
	(
	const JIndex	index,
	const JString&	str
	)
{
	*itsCommands->GetItem(index) = str;
}

/******************************************************************************
 DeleteString

 ******************************************************************************/

void
EditCommandsDialog::DeleteString
	(
	const JIndex index
	)
{
	itsCommands->DeleteItem(index);
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
EditCommandsDialog::OKToDeactivate()
{
	if (Cancelled())
	{
		return true;
	}

	if (!itsWidget->EndEditing())
	{
		return false;
	}

	GetPrefsManager()->SetCmdList(*itsCommands);
	return true;
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
EditCommandsDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
	{
		JXWindow* window = GetWindow();
		window->ReadGeometry(input);
		window->Deiconify();
	}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
EditCommandsDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;
	output << ' ';
	GetWindow()->WriteGeometry(output);
}

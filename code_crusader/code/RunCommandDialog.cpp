/******************************************************************************
 RunCommandDialog.cpp

	BASE CLASS = JXModalDialogDirector, JPrefObject

	Copyright © 2002 by John Lindal.

 ******************************************************************************/

#include "RunCommandDialog.h"
#include "ProjectDocument.h"
#include "CommandManager.h"
#include "CommandPathInput.h"
#include "globals.h"
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXDocumentMenu.h>
#include <jx-af/jx/JXPathHistoryMenu.h>
#include <jx-af/jx/JXCSFDialogBase.h>
#include <jx-af/jfs/JXFSRunFileDialog.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

const JSize kHistoryLength = 20;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version  1 saves isVCS flag

/******************************************************************************
 Constructor

 ******************************************************************************/

RunCommandDialog::RunCommandDialog
	(
	ProjectDocument*	projDoc,
	TextDocument*		textDoc
	)
	:
	JXModalDialogDirector(true),
	JPrefObject(GetPrefsManager(), kRunCommandDialogID),
	itsProjDoc(projDoc),
	itsTextDoc(textDoc),
	itsFullNameList(nullptr),
	itsLineIndexList(nullptr)
{
	assert( itsTextDoc != nullptr );

	BuildWindow();
	JPrefObject::ReadPrefs();
}

RunCommandDialog::RunCommandDialog
	(
	ProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList
	)
	:
	JXModalDialogDirector(true),
	JPrefObject(GetPrefsManager(), kRunCommandDialogID),
	itsProjDoc(projDoc),
	itsTextDoc(nullptr)
{
	itsFullNameList = jnew JDCCPtrArray<JString>(fullNameList, JPtrArrayT::kDeleteAll);
	assert( itsFullNameList != nullptr );

	itsLineIndexList = jnew JArray<JIndex>(lineIndexList);
	assert( itsLineIndexList != nullptr );

	BuildWindow();
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RunCommandDialog::~RunCommandDialog()
{
	jdelete itsFullNameList;
	jdelete itsLineIndexList;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
RunCommandDialog::Activate()
{
	JXModalDialogDirector::Activate();

	if (IsActive())
	{
		itsCmdInput->Focus();
		itsCmdInput->SelectAll();
	}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

#include "RunCommandDialog-SaveCmd.h"

void
RunCommandDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 470,260, JGetString("WindowTitle::RunCommandDialog::JXLayout"));

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 200,20);
	pathLabel->SetToLabel(false);

	itsPathInput =
		jnew CommandPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 320,20);

	itsPathHistoryMenu =
		jnew JXPathHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,40, 30,20);

	itsChoosePathButton =
		jnew JXTextButton(JGetString("itsChoosePathButton::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 390,40, 60,20);

	auto* cmdLabel =
		jnew JXStaticText(JGetString("cmdLabel::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 130,20);
	cmdLabel->SetToLabel(false);

	itsChooseCmdButton =
		jnew JXTextButton(JGetString("itsChooseCmdButton::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 390,90, 60,20);

	itsIsMakeCB =
		jnew JXTextCheckbox(JGetString("itsIsMakeCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 220,20);
	itsIsMakeCB->SetShortcuts(JGetString("itsIsMakeCB::shortcuts::RunCommandDialog::JXLayout"));

	itsUseWindowCB =
		jnew JXTextCheckbox(JGetString("itsUseWindowCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,130, 220,20);
	itsUseWindowCB->SetShortcuts(JGetString("itsUseWindowCB::shortcuts::RunCommandDialog::JXLayout"));

	itsIsCVSCB =
		jnew JXTextCheckbox(JGetString("itsIsCVSCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,150, 220,20);
	itsIsCVSCB->SetShortcuts(JGetString("itsIsCVSCB::shortcuts::RunCommandDialog::JXLayout"));

	itsRaiseCB =
		jnew JXTextCheckbox(JGetString("itsRaiseCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,150, 220,20);
	itsRaiseCB->SetShortcuts(JGetString("itsRaiseCB::shortcuts::RunCommandDialog::JXLayout"));

	itsBeepCB =
		jnew JXTextCheckbox(JGetString("itsBeepCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,170, 220,20);
	itsBeepCB->SetShortcuts(JGetString("itsBeepCB::shortcuts::RunCommandDialog::JXLayout"));

	itsSaveAllCB =
		jnew JXTextCheckbox(JGetString("itsSaveAllCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 220,20);
	itsSaveAllCB->SetShortcuts(JGetString("itsSaveAllCB::shortcuts::RunCommandDialog::JXLayout"));

	itsOneAtATimeCB =
		jnew JXTextCheckbox(JGetString("itsOneAtATimeCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,200, 220,20);
	itsOneAtATimeCB->SetShortcuts(JGetString("itsOneAtATimeCB::shortcuts::RunCommandDialog::JXLayout"));

	itsRunButton =
		jnew JXTextButton(JGetString("itsRunButton::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 388,233, 64,24);
	itsRunButton->SetShortcuts(JGetString("itsRunButton::shortcuts::RunCommandDialog::JXLayout"));

	itsSaveCmdMenu =
		jnew JXTextMenu(JGetString("itsSaveCmdMenu::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,235, 120,20);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 210,235, 60,20);
	assert( cancelButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,235, 60,20);
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::shortcuts::RunCommandDialog::JXLayout"));

	itsCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,90, 370,20);
	itsCmdInput->SetIsRequired();
	itsCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());

// end JXLayout

	window->LockCurrentMinSize();
	SetButtons(itsRunButton, cancelButton);

	ListenTo(itsHelpButton);
	ListenTo(itsChoosePathButton);
	ListenTo(itsChooseCmdButton);
	ListenTo(itsPathHistoryMenu);
	ListenTo(itsIsMakeCB);
	ListenTo(itsIsCVSCB);
	ListenTo(itsUseWindowCB);

	itsPathInput->GetText()->SetText(JString("." ACE_DIRECTORY_SEPARATOR_STR, JString::kNoCopy));
	itsPathInput->ShouldAllowInvalidPath();
	ListenTo(itsPathInput);

	if (itsProjDoc != nullptr)
	{
		itsPathInput->SetBasePath(itsProjDoc->GetFilePath());
	}

	itsCmdInput->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);
	ListenTo(itsCmdInput);

	itsSaveCmdMenu->SetMenuItems(kSaveCmdMenuStr);
	itsSaveCmdMenu->AttachHandlers(this,
		&RunCommandDialog::UpdateSaveCmdMenu,
		&RunCommandDialog::HandleSaveCmdMenu);
	ConfigureSaveCmdMenu(itsSaveCmdMenu);

	// create hidden JXDocument so Meta-# shortcuts work

	jnew JXDocumentMenu(JString::empty, window,
					   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
RunCommandDialog::UpdateDisplay()
{
	JString p;
	if (!itsCmdInput->GetText()->IsEmpty() && itsPathInput->GetPath(&p))
	{
		itsRunButton->Activate();
	}
	else
	{
		itsRunButton->Deactivate();
	}

	if (itsIsMakeCB->IsChecked())
	{
		itsSaveAllCB->Deactivate();
		itsSaveAllCB->SetState(true);

		itsUseWindowCB->Deactivate();
		itsUseWindowCB->SetState(true);
	}
	else if (itsIsCVSCB->IsChecked())
	{
		itsSaveAllCB->Deactivate();
		itsSaveAllCB->SetState(true);

		itsUseWindowCB->Activate();
	}
	else
	{
		itsSaveAllCB->Activate();
		itsUseWindowCB->Activate();
	}

	if (itsUseWindowCB->IsChecked())
	{
		itsRaiseCB->Activate();
	}
	else
	{
		itsRaiseCB->Deactivate();
		itsRaiseCB->SetState(false);
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
RunCommandDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("TasksHelp");
	}

	else if (sender == itsPathHistoryMenu && message.Is(JXMenu::kItemSelected))
	{
		itsPathHistoryMenu->UpdateInputField(message, itsPathInput);
	}
	else if (sender == itsChoosePathButton && message.Is(JXButton::kPushed))
	{
		itsPathInput->ChoosePath(JString::empty);
	}

	else if (sender == itsChooseCmdButton && message.Is(JXButton::kPushed))
	{
		JXFSRunFileDialog::HandleChooseCmdButton(itsCmdInput);
	}

	else if ((sender == itsCmdInput || sender == itsPathInput) &&
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
	{
		UpdateDisplay();
	}

	else if ((sender == itsIsMakeCB ||
			  sender == itsIsCVSCB  ||
			  sender == itsUseWindowCB) &&
			 message.Is(JXCheckbox::kPushed))
	{
		UpdateDisplay();
	}

	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 Exec (private)

 ******************************************************************************/

void
RunCommandDialog::Exec()
{
	auto* path = jnew JString;

	if (!itsPathInput->GetPath(path))
	{
		jdelete path;
		return;
	}
	itsPathHistoryMenu->AddString(itsPathInput->GetText()->GetText());

	auto* cmd = jnew JString(itsCmdInput->GetText()->GetText());
	assert( cmd != nullptr );

	auto* ss = jnew JString;

	auto* mt = jnew JString;

	auto* ms = jnew JString;

	auto* mi = jnew JString;

	CommandManager::CmdInfo info(path, cmd, ss,
								   itsIsMakeCB->IsChecked(), itsIsCVSCB->IsChecked(),
								   itsSaveAllCB->IsChecked(), itsOneAtATimeCB->IsChecked(),
								   itsUseWindowCB->IsChecked(), itsRaiseCB->IsChecked(),
								   itsBeepCB->IsChecked(), mt, ms, mi, false);

	if (itsTextDoc != nullptr)
	{
		CommandManager::Exec(info, itsProjDoc, itsTextDoc);
	}
	else if (itsFullNameList != nullptr)
	{
		CommandManager::Exec(info, itsProjDoc, *itsFullNameList, *itsLineIndexList);
	}

	info.Free();
}

/******************************************************************************
 UpdateSaveCmdMenu (private)

 ******************************************************************************/

void
RunCommandDialog::UpdateSaveCmdMenu()
{
	itsSaveCmdMenu->EnableItem(kSaveForAllCmd);

	if (itsProjDoc != nullptr)
	{
		itsSaveCmdMenu->EnableItem(kSaveForActiveCmd);

		const JUtf8Byte* map[] =
		{
			"project", (itsProjDoc->GetName()).GetBytes()
		};
		const JString s = JGetString("SaveForActive::RunCommandDialog", map, sizeof(map));
		itsSaveCmdMenu->SetItemText(kSaveForActiveCmd, s);
	}
}

/******************************************************************************
 HandleSaveCmdMenu (private)

 ******************************************************************************/

void
RunCommandDialog::HandleSaveCmdMenu
	(
	const JIndex index
	)
{
	if (index == kSaveForAllCmd)
	{
		AddCommandToMenu(GetCommandManager());
		(GetCommandManager())->UpdateMenuIDs();
	}
	else if (index == kSaveForActiveCmd)
	{
		if (itsProjDoc != nullptr)
		{
			AddCommandToMenu(itsProjDoc->GetCommandManager());
		}
	}
}

/******************************************************************************
 AddCommandToMenu (private)

 ******************************************************************************/

void
RunCommandDialog::AddCommandToMenu
	(
	CommandManager* mgr
	)
{
	mgr->AppendCommand(itsPathInput->GetText()->GetText(),
					   itsCmdInput->GetText()->GetText(), JString::empty,
					   itsIsMakeCB->IsChecked(), itsIsCVSCB->IsChecked(),
					   itsSaveAllCB->IsChecked(), itsOneAtATimeCB->IsChecked(),
					   itsUseWindowCB->IsChecked(), itsRaiseCB->IsChecked(),
					   itsBeepCB->IsChecked(), JString::empty, JString::empty, false);
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
RunCommandDialog::ReadPrefs
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

	JString s;
	bool checked;

	input >> s;
	itsPathInput->GetText()->SetText(s);

	itsPathHistoryMenu->ReadSetup(input);

	input >> s;
	itsCmdInput->GetText()->SetText(s);

	input >> JBoolFromString(checked);
	itsIsMakeCB->SetState(checked);

	if (vers >= 1)
	{
		input >> JBoolFromString(checked);
		itsIsCVSCB->SetState(checked);
	}

	input >> JBoolFromString(checked);
	itsSaveAllCB->SetState(checked);

	input >> JBoolFromString(checked);
	itsOneAtATimeCB->SetState(checked);

	input >> JBoolFromString(checked);
	itsUseWindowCB->SetState(checked);

	input >> JBoolFromString(checked);
	itsRaiseCB->SetState(checked);

	input >> JBoolFromString(checked);
	itsBeepCB->SetState(checked);

	itsOneAtATimeCB->SetState(true);
	itsOneAtATimeCB->Deactivate();
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
RunCommandDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ' << itsPathInput->GetText()->GetText();

	output << ' ';
	itsPathHistoryMenu->WriteSetup(output);

	output << ' ' << itsCmdInput->GetText()->GetText();

	output << ' ' << JBoolToString(itsIsMakeCB->IsChecked())
				  << JBoolToString(itsIsCVSCB->IsChecked())
				  << JBoolToString(itsSaveAllCB->IsChecked())
				  << JBoolToString(itsOneAtATimeCB->IsChecked())
				  << JBoolToString(itsUseWindowCB->IsChecked())
				  << JBoolToString(itsRaiseCB->IsChecked())
				  << JBoolToString(itsBeepCB->IsChecked());
}

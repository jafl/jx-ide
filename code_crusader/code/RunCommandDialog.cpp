/******************************************************************************
 RunCommandDialog.cpp

	BASE CLASS = JXDialogDirector, JPrefObject

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
#include <jx-af/jx/JXChooseSaveFile.h>
#include <jx-af/jfs/JXFSRunFileDialog.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

const JSize kHistoryLength = 20;

// Save Command menu

static const JUtf8Byte* kSaveCmdMenuStr =
	"  For all projects"
	"| For active project";

enum
{
	kSaveForAllCmd = 1,
	kSaveForActiveCmd
};

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
	JXDialogDirector(JXGetApplication(), true),
	JPrefObject(GetPrefsManager(), kRunCommandDialogID),
	itsProjDoc(projDoc),
	itsTextDoc(textDoc),
	itsFullNameList(nullptr),
	itsLineIndexList(nullptr)
{
	assert( itsTextDoc != nullptr );

	BuildWindow();
	JPrefObject::ReadPrefs();
	ListenTo(this);
}

RunCommandDialog::RunCommandDialog
	(
	ProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList
	)
	:
	JXDialogDirector(JXGetApplication(), true),
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
	ListenTo(this);
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
	JXDialogDirector::Activate();

	if (IsActive())
	{
		itsCmdInput->Focus();
		itsCmdInput->SelectAll();
	}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
RunCommandDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 470,260, JString::empty);
	assert( window != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 210,235, 60,20);
	assert( cancelButton != nullptr );

	itsRunButton =
		jnew JXTextButton(JGetString("itsRunButton::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 389,234, 62,22);
	assert( itsRunButton != nullptr );
	itsRunButton->SetShortcuts(JGetString("itsRunButton::RunCommandDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,235, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::RunCommandDialog::shortcuts::JXLayout"));

	itsPathInput =
		jnew CommandPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 320,20);
	assert( itsPathInput != nullptr );

	itsSaveAllCB =
		jnew JXTextCheckbox(JGetString("itsSaveAllCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 220,20);
	assert( itsSaveAllCB != nullptr );
	itsSaveAllCB->SetShortcuts(JGetString("itsSaveAllCB::RunCommandDialog::shortcuts::JXLayout"));

	itsOneAtATimeCB =
		jnew JXTextCheckbox(JGetString("itsOneAtATimeCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,200, 220,20);
	assert( itsOneAtATimeCB != nullptr );
	itsOneAtATimeCB->SetShortcuts(JGetString("itsOneAtATimeCB::RunCommandDialog::shortcuts::JXLayout"));

	itsUseWindowCB =
		jnew JXTextCheckbox(JGetString("itsUseWindowCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,130, 220,20);
	assert( itsUseWindowCB != nullptr );
	itsUseWindowCB->SetShortcuts(JGetString("itsUseWindowCB::RunCommandDialog::shortcuts::JXLayout"));

	itsIsMakeCB =
		jnew JXTextCheckbox(JGetString("itsIsMakeCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 220,20);
	assert( itsIsMakeCB != nullptr );
	itsIsMakeCB->SetShortcuts(JGetString("itsIsMakeCB::RunCommandDialog::shortcuts::JXLayout"));

	itsCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,90, 370,20);
	assert( itsCmdInput != nullptr );

	auto* cmdLabel =
		jnew JXStaticText(JGetString("cmdLabel::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 130,20);
	assert( cmdLabel != nullptr );
	cmdLabel->SetToLabel();

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 200,20);
	assert( pathLabel != nullptr );
	pathLabel->SetToLabel();

	itsPathHistoryMenu =
		jnew JXPathHistoryMenu(kHistoryLength, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,40, 30,20);
	assert( itsPathHistoryMenu != nullptr );

	itsChoosePathButton =
		jnew JXTextButton(JGetString("itsChoosePathButton::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 390,40, 60,20);
	assert( itsChoosePathButton != nullptr );

	itsChooseCmdButton =
		jnew JXTextButton(JGetString("itsChooseCmdButton::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 390,90, 60,20);
	assert( itsChooseCmdButton != nullptr );

	itsRaiseCB =
		jnew JXTextCheckbox(JGetString("itsRaiseCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,150, 220,20);
	assert( itsRaiseCB != nullptr );
	itsRaiseCB->SetShortcuts(JGetString("itsRaiseCB::RunCommandDialog::shortcuts::JXLayout"));

	itsBeepCB =
		jnew JXTextCheckbox(JGetString("itsBeepCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,170, 220,20);
	assert( itsBeepCB != nullptr );
	itsBeepCB->SetShortcuts(JGetString("itsBeepCB::RunCommandDialog::shortcuts::JXLayout"));

	itsSaveCmdMenu =
		jnew JXTextMenu(JGetString("itsSaveCmdMenu::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,235, 90,20);
	assert( itsSaveCmdMenu != nullptr );

	itsIsCVSCB =
		jnew JXTextCheckbox(JGetString("itsIsCVSCB::RunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,150, 220,20);
	assert( itsIsCVSCB != nullptr );
	itsIsCVSCB->SetShortcuts(JGetString("itsIsCVSCB::RunCommandDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::RunCommandDialog"));
	SetButtons(itsRunButton, cancelButton);
	UseModalPlacement(false);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

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

	itsCmdInput->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsCmdInput);

	itsCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());

	itsSaveCmdMenu->SetMenuItems(kSaveCmdMenuStr, "RunCommandDialog");
	ListenTo(itsSaveCmdMenu);

	// create hidden JXDocument so Meta-# shortcuts work

	auto* fileListMenu =
		jnew JXDocumentMenu(JString::empty, window,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);
	assert( fileListMenu != nullptr );

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
	if (sender == this && message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			Exec();
			JPrefObject::WritePrefs();
		}
	}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("TasksHelp");
	}

	else if (sender == itsSaveCmdMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateSaveCmdMenu();
	}
	else if (sender == itsSaveCmdMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleSaveCmdMenu(selection->GetIndex());
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
		JXDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 Exec (private)

 ******************************************************************************/

void
RunCommandDialog::Exec()
{
	auto* path = jnew JString;
	assert( path != nullptr );

	if (!itsPathInput->GetPath(path))
	{
		jdelete path;
		return;
	}
	itsPathHistoryMenu->AddString(itsPathInput->GetText()->GetText());

	auto* cmd = jnew JString(itsCmdInput->GetText()->GetText());
	assert( cmd != nullptr );

	auto* ss = jnew JString;
	assert( ss != nullptr );

	auto* mt = jnew JString;
	assert( mt != nullptr );

	auto* ms = jnew JString;
	assert( ms != nullptr );

	auto* mi = jnew JString;
	assert( mi != nullptr );

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

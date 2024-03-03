/******************************************************************************
 EditPrefsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2001-10 by John Lindal.

 ******************************************************************************/

#include "EditPrefsDialog.h"
#include "DebuggerProgramInput.h"
#include "sharedUtil.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXChooseFileDialog.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

EditPrefsDialog::EditPrefsDialog
	(
	const JString&				gdbCmd,
	const JString&				jdbCmd,
	const JString&				editFileCmd,
	const JString&				editFileLineCmd,
	const JPtrArray<JString>&	cSourceSuffixes,
	const JPtrArray<JString>&	cHeaderSuffixes,
	const JPtrArray<JString>&	javaSuffixes,
	const JPtrArray<JString>&	phpSuffixes,
	const JPtrArray<JString>&	fortranSuffixes,
	const JPtrArray<JString>&	dSuffixes,
	const JPtrArray<JString>&	goSuffixes
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(gdbCmd, jdbCmd, editFileCmd, editFileLineCmd,
				cSourceSuffixes, cHeaderSuffixes,
				javaSuffixes, phpSuffixes, fortranSuffixes,
				dSuffixes, goSuffixes);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditPrefsDialog::~EditPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
EditPrefsDialog::BuildWindow
	(
	const JString&				gdbCmd,
	const JString&				jdbCmd,
	const JString&				editFileCmd,
	const JString&				editFileLineCmd,
	const JPtrArray<JString>&	cSourceSuffixes,
	const JPtrArray<JString>&	cHeaderSuffixes,
	const JPtrArray<JString>&	javaSuffixes,
	const JPtrArray<JString>&	phpSuffixes,
	const JPtrArray<JString>&	fortranSuffixes,
	const JPtrArray<JString>&	dSuffixes,
	const JPtrArray<JString>&	goSuffixes
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 450,460, JGetString("WindowTitle::EditPrefsDialog::JXLayout"));

	auto* gdbCmdLabel =
		jnew JXStaticText(JGetString("gdbCmdLabel::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 340,20);
	gdbCmdLabel->SetToLabel(false);

	itsGDBCmd =
		jnew DebuggerProgramInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 350,20);

	itsChooseGDBButton =
		jnew JXTextButton(JGetString("itsChooseGDBButton::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 370,40, 60,20);

	auto* gdbScriptHint =
		jnew JXStaticText(JGetString("gdbScriptHint::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 410,20);
	gdbScriptHint->SetToLabel(false);

	auto* jvmCmdLabel =
		jnew JXStaticText(JGetString("jvmCmdLabel::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 340,20);
	jvmCmdLabel->SetToLabel(false);

	itsJDBCmd =
		jnew DebuggerProgramInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,110, 350,20);

	itsChooseJDBButton =
		jnew JXTextButton(JGetString("itsChooseJDBButton::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 370,110, 60,20);

	auto* jvmScriptHint =
		jnew JXStaticText(JGetString("jvmScriptHint::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 410,20);
	jvmScriptHint->SetToLabel(false);

	auto* editFileLabel =
		jnew JXStaticText(JGetString("editFileLabel::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 170,20);
	editFileLabel->SetToLabel(false);

	auto* editLineLabel =
		jnew JXStaticText(JGetString("editLineLabel::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 170,20);
	editLineLabel->SetToLabel(false);

	auto* cSourceFileLabel =
		jnew JXStaticText(JGetString("cSourceFileLabel::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,220, 170,20);
	cSourceFileLabel->SetToLabel(false);

	auto* cHeaderFileLabel =
		jnew JXStaticText(JGetString("cHeaderFileLabel::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,240, 170,20);
	cHeaderFileLabel->SetToLabel(false);

	auto* javaSourceLabel =
		jnew JXStaticText(JGetString("javaSourceLabel::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 31,270, 159,20);
	javaSourceLabel->SetToLabel(false);

	auto* phpSourceLabel =
		jnew JXStaticText(JGetString("phpSourceLabel::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 31,300, 159,20);
	phpSourceLabel->SetToLabel(false);

	auto* fortranSourceLabel =
		jnew JXStaticText(JGetString("fortranSourceLabel::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,330, 170,20);
	fortranSourceLabel->SetToLabel(false);

	auto* dSourceLabel =
		jnew JXStaticText(JGetString("dSourceLabel::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,360, 170,20);
	dSourceLabel->SetToLabel(false);

	auto* goSourceLabel =
		jnew JXStaticText(JGetString("goSourceLabel::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,390, 170,20);
	goSourceLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,430, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 280,430, 60,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::EditPrefsDialog::JXLayout"));

	itsEditFileCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,160, 240,20);
	itsEditFileCmdInput->SetIsRequired();
	itsEditFileCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsEditFileCmdInput->SetHint(JGetString("itsEditFileCmdInput::EditPrefsDialog::JXLayout"));

	itsEditFileLineCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,180, 240,20);
	itsEditFileLineCmdInput->SetIsRequired();
	itsEditFileLineCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsEditFileLineCmdInput->SetHint(JGetString("itsEditFileLineCmdInput::EditPrefsDialog::JXLayout"));

	itsCSourceSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,220, 240,20);
	itsCSourceSuffixInput->SetIsRequired();
	itsCSourceSuffixInput->SetFont(JFontManager::GetDefaultMonospaceFont());

	itsCHeaderSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,240, 240,20);
	itsCHeaderSuffixInput->SetIsRequired();
	itsCHeaderSuffixInput->SetFont(JFontManager::GetDefaultMonospaceFont());

	itsJavaSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,270, 240,20);
	itsJavaSuffixInput->SetIsRequired();
	itsJavaSuffixInput->SetFont(JFontManager::GetDefaultMonospaceFont());

	itsPHPSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,300, 240,20);
	itsPHPSuffixInput->SetIsRequired();
	itsPHPSuffixInput->SetFont(JFontManager::GetDefaultMonospaceFont());

	itsFortranSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,330, 240,20);
	itsFortranSuffixInput->SetIsRequired();
	itsFortranSuffixInput->SetFont(JFontManager::GetDefaultMonospaceFont());

	itsDSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,360, 240,20);
	itsDSuffixInput->SetIsRequired();
	itsDSuffixInput->SetFont(JFontManager::GetDefaultMonospaceFont());

	itsGoSuffixInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,390, 240,20);
	itsGoSuffixInput->SetIsRequired();
	itsGoSuffixInput->SetFont(JFontManager::GetDefaultMonospaceFont());

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsGDBCmd->GetText()->SetText(gdbCmd);
	itsGDBCmd->SetIsRequired();
	itsGDBCmd->ShouldRequireExecutable();

	itsJDBCmd->GetText()->SetText(jdbCmd);
	itsJDBCmd->SetIsRequired();
	itsJDBCmd->ShouldRequireExecutable();

	itsEditFileCmdInput->GetText()->SetText(editFileCmd);
	itsEditFileLineCmdInput->GetText()->SetText(editFileLineCmd);
	SetSuffixList(itsCSourceSuffixInput, cSourceSuffixes);
	SetSuffixList(itsCHeaderSuffixInput, cHeaderSuffixes);
	SetSuffixList(itsJavaSuffixInput, javaSuffixes);
	SetSuffixList(itsPHPSuffixInput, phpSuffixes);
	SetSuffixList(itsFortranSuffixInput, fortranSuffixes);
	SetSuffixList(itsDSuffixInput, dSuffixes);
	SetSuffixList(itsGoSuffixInput, goSuffixes);

	gdbScriptHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	jvmScriptHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);

	ListenTo(itsChooseGDBButton);
	ListenTo(itsChooseJDBButton);
}

/******************************************************************************
 GetPrefs

 ******************************************************************************/

void
EditPrefsDialog::GetPrefs
	(
	JString*			gdbCmd,
	JString*			jdbCmd,
	JString*			editFileCmd,
	JString*			editFileLineCmd,
	JPtrArray<JString>*	cSourceSuffixes,
	JPtrArray<JString>*	cHeaderSuffixes,
	JPtrArray<JString>*	javaSuffixes,
	JPtrArray<JString>*	phpSuffixes,
	JPtrArray<JString>*	fortranSuffixes,
	JPtrArray<JString>*	dSuffixes,
	JPtrArray<JString>*	goSuffixes
	)
	const
{
	*gdbCmd          = itsGDBCmd->GetText()->GetText();
	*jdbCmd          = itsJDBCmd->GetText()->GetText();
	*editFileCmd     = itsEditFileCmdInput->GetText()->GetText();
	*editFileLineCmd = itsEditFileLineCmdInput->GetText()->GetText();

	GetSuffixList(itsCSourceSuffixInput, cSourceSuffixes);
	GetSuffixList(itsCHeaderSuffixInput, cHeaderSuffixes);
	GetSuffixList(itsJavaSuffixInput, javaSuffixes);
	GetSuffixList(itsPHPSuffixInput, phpSuffixes);
	GetSuffixList(itsFortranSuffixInput, fortranSuffixes);
	GetSuffixList(itsDSuffixInput, dSuffixes);
	GetSuffixList(itsGoSuffixInput, goSuffixes);
}

/******************************************************************************
 GetSuffixList

	Checks that each string starts with a period.

 ******************************************************************************/

static const JRegex wsPattern("[ \t]+");

void
EditPrefsDialog::GetSuffixList
	(
	JXInputField*		inputField,
	JPtrArray<JString>*	list
	)
	const
{
	list->CleanOut();

	JString text = inputField->GetText()->GetText();
	text.TrimWhitespace();
	if (!text.IsEmpty())
	{
		text.Split(wsPattern, list);
	}

	const JSize count = list->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		JString* suffix = list->GetItem(i);
		if (suffix->GetFirstCharacter() != '.')
		{
			suffix->Prepend(".");
		}
	}
}

/******************************************************************************
 SetSuffixList

 ******************************************************************************/

void
EditPrefsDialog::SetSuffixList
	(
	JXInputField*				inputField,
	const JPtrArray<JString>&	list
	)
{
	JString text;

	const JSize count = list.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (i > 1)
		{
			text += " ";
		}
		text += *(list.GetItem(i));
	}

	inputField->GetText()->SetText(text);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
EditPrefsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsChooseGDBButton && message.Is(JXButton::kPushed))
	{
		ChooseDebugger("gdb", itsGDBCmd);
	}
	else if (sender == itsChooseJDBButton && message.Is(JXButton::kPushed))
	{
		ChooseDebugger("jdb", itsJDBCmd);
	}
	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 ChooseDebugger (private)

 ******************************************************************************/

void
EditPrefsDialog::ChooseDebugger
	(
	const JString&	name,
	JXInputField*	input
	)
{
	auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectSingleFile, JString::empty, name);
	if (dlog->DoDialog())
	{
		JString text = input->GetText()->GetText();

		JStringIterator iter(&text);
		iter.BeginMatch();
		if (iter.Next(" "))
		{
			iter.FinishMatch();
			iter.ReplaceLastMatch(dlog->GetFullName());
		}
		else
		{
			text = dlog->GetFullName();
		}
		iter.Invalidate();

		input->GetText()->SetText(text);
	}
}

/******************************************************************************
 EditCommandsDialog.cpp

	BASE CLASS = JXModalDialogDirector, JPrefObject

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "EditCommandsDialog.h"
#include "CommandTable.h"
#include "ProjectDocument.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXVertPartition.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

	thisCmdList can be nullptr

 ******************************************************************************/

EditCommandsDialog::EditCommandsDialog
	(
	ProjectDocument* projDoc
	)
	:
	JXModalDialogDirector(true),
	JPrefObject(GetPrefsManager(), kEditCommandsDialogID),
	itsCmdMgr(projDoc == nullptr ? nullptr : projDoc->GetCommandManager())
{
	BuildWindow(projDoc);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditCommandsDialog::~EditCommandsDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 UpdateCommands

 ******************************************************************************/

void
EditCommandsDialog::UpdateCommands()
{
	itsAllTable->GetCommandList(GetCommandManager()->GetCommandList());
	GetCommandManager()->UpdateMenuIDs();

	if (itsCmdMgr != nullptr)
	{
		assert( itsMakeDependCmd != nullptr );
		itsCmdMgr->SetMakeDependCommand(itsMakeDependCmd->GetText()->GetText());

		assert( itsThisTable != nullptr );
		itsThisTable->GetCommandList(itsCmdMgr->GetCommandList());
	}

	GetCommandManager()->UpdateAllCommandMenus();
}

/******************************************************************************
 BuildWindow (private)

	thisCmdList can be nullptr

 ******************************************************************************/

void
EditCommandsDialog::BuildWindow
	(
	ProjectDocument* projDoc
	)
{
	JArray<JCoordinate> heights(2);
	heights.AppendElement(210);
	heights.AppendElement(240);

	const JIndex elasticIndex = 2;

	JArray<JCoordinate> minHeights(2);
	minHeights.AppendElement(100);
	minHeights.AppendElement(120);

	const JCoordinate kHeaderHeight = 20;

// begin JXLayout

	auto* window = jnew JXWindow(this, 620,530, JString::empty);
	assert( window != nullptr );

	itsPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 580,455);
	assert( itsPartition != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 100,500, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 440,500, 70,20);
	assert( okButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 270,500, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::EditCommandsDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::EditCommandsDialog"));
	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	// commands available to all projects

	JXContainer* compartment;
	if (itsCmdMgr != nullptr)
	{
		compartment = itsPartition->GetCompartment(1);
	}
	else
	{
		const JRect frame = itsPartition->GetFrame();
		compartment =
			jnew JXWidgetSet(window, JXWidget::kHElastic, JXWidget::kVElastic,
							frame.left, frame.top, frame.width(), frame.height());
		assert( compartment != nullptr );
		itsPartition->Hide();
	}

// begin allProjectsLayout

	const JRect allProjectsLayout_Frame    = compartment->GetFrame();
	const JRect allProjectsLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(580 - allProjectsLayout_Aperture.width(), 210 - allProjectsLayout_Aperture.height());

	auto* allNewButton =
		jnew JXTextButton(JGetString("allNewButton::EditCommandsDialog::allProjectsLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,60, 60,20);
	assert( allNewButton != nullptr );

	auto* allRemoveButton =
		jnew JXTextButton(JGetString("allRemoveButton::EditCommandsDialog::allProjectsLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,120, 60,20);
	assert( allRemoveButton != nullptr );

	auto* allScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 510,180);
	assert( allScrollbarSet != nullptr );

	auto* allDuplicateButton =
		jnew JXTextButton(JGetString("allDuplicateButton::EditCommandsDialog::allProjectsLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,90, 60,20);
	assert( allDuplicateButton != nullptr );

	auto* anyProjectTitle =
		jnew JXStaticText(JGetString("anyProjectTitle::EditCommandsDialog::allProjectsLayout"), compartment,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 580,20);
	assert( anyProjectTitle != nullptr );
	anyProjectTitle->SetToLabel();

	auto* allExportButton =
		jnew JXTextButton(JGetString("allExportButton::EditCommandsDialog::allProjectsLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,160, 60,20);
	assert( allExportButton != nullptr );

	auto* allImportButton =
		jnew JXTextButton(JGetString("allImportButton::EditCommandsDialog::allProjectsLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,190, 60,20);
	assert( allImportButton != nullptr );

	compartment->SetSize(allProjectsLayout_Frame.width(), allProjectsLayout_Frame.height());

// end allProjectsLayout

	if (itsCmdMgr == nullptr)
	{
		allScrollbarSet->AdjustSize(0,
			(compartment->GetAperture()).bottom -
			(allScrollbarSet->GetFrame()).bottom);
	}

	const CommandManager::CmdList* allCmdList = (GetCommandManager())->GetCommandList();

	itsAllTable =
		jnew CommandTable(*allCmdList, allNewButton, allRemoveButton, allDuplicateButton,
						   allExportButton, allImportButton,
						   allScrollbarSet, allScrollbarSet->GetScrollEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsAllTable != nullptr );
	itsAllTable->FitToEnclosure();
	itsAllTable->Move(0, kHeaderHeight);
	itsAllTable->AdjustSize(0, -kHeaderHeight);

	auto* colHeader =
		jnew JXColHeaderWidget(itsAllTable, allScrollbarSet,
							  allScrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  0,0, 10,kHeaderHeight);
	assert( colHeader != nullptr );
	colHeader->FitToEnclosure(true, false);
	itsAllTable->SetColTitles(colHeader);
	colHeader->TurnOnColResizing(20);

	// commands available to currently selected project

	itsMakeDependCmd = nullptr;
	itsThisTable     = nullptr;

	if (itsCmdMgr != nullptr)
	{
		compartment = itsPartition->GetCompartment(2);

// begin thisProjectLayout

	const JRect thisProjectLayout_Frame    = compartment->GetFrame();
	const JRect thisProjectLayout_Aperture = compartment->GetAperture();
	compartment->AdjustSize(580 - thisProjectLayout_Aperture.width(), 240 - thisProjectLayout_Aperture.height());

	auto* thisNewButton =
		jnew JXTextButton(JGetString("thisNewButton::EditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,90, 60,20);
	assert( thisNewButton != nullptr );

	auto* thisRemoveButton =
		jnew JXTextButton(JGetString("thisRemoveButton::EditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,150, 60,20);
	assert( thisRemoveButton != nullptr );

	auto* thisScrollbarSet =
		jnew JXScrollbarSet(compartment,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,70, 510,170);
	assert( thisScrollbarSet != nullptr );

	itsMakeDependCmd =
		jnew JXInputField(compartment,
					JXWidget::kHElastic, JXWidget::kFixedTop, 260,40, 250,20);
	assert( itsMakeDependCmd != nullptr );

	auto* updateMakefileLabel =
		jnew JXStaticText(JGetString("updateMakefileLabel::EditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,40, 260,20);
	assert( updateMakefileLabel != nullptr );
	updateMakefileLabel->SetToLabel();

	auto* thisDuplicateButton =
		jnew JXTextButton(JGetString("thisDuplicateButton::EditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,120, 60,20);
	assert( thisDuplicateButton != nullptr );

	auto* thisCaption =
		jnew JXStaticText(JGetString("thisCaption::EditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,10, 580,20);
	assert( thisCaption != nullptr );
	thisCaption->SetToLabel();

	auto* thisExportButton =
		jnew JXTextButton(JGetString("thisExportButton::EditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,190, 60,20);
	assert( thisExportButton != nullptr );

	auto* thisImportButton =
		jnew JXTextButton(JGetString("thisImportButton::EditCommandsDialog::thisProjectLayout"), compartment,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 520,220, 60,20);
	assert( thisImportButton != nullptr );

	compartment->SetSize(thisProjectLayout_Frame.width(), thisProjectLayout_Frame.height());

// end thisProjectLayout

		const JUtf8Byte* map[] =
		{
			"project", (projDoc->GetName()).GetBytes()
		};
		thisCaption->GetText()->SetText(
			JGetString("ProjectTableCaption::EditCommandsDialog", map, sizeof(map)));

		itsMakeDependCmd->GetText()->SetText(itsCmdMgr->GetMakeDependCommand());
		itsMakeDependCmd->SetIsRequired();

		const CommandManager::CmdList* thisCmdList = itsCmdMgr->GetCommandList();

		itsThisTable =
			jnew CommandTable(*thisCmdList, thisNewButton, thisRemoveButton, thisDuplicateButton,
							   thisExportButton, thisImportButton,
							   thisScrollbarSet, thisScrollbarSet->GetScrollEnclosure(),
							   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
		assert( itsThisTable != nullptr );
		itsThisTable->FitToEnclosure();
		itsThisTable->Move(0, kHeaderHeight);
		itsThisTable->AdjustSize(0, -kHeaderHeight);

		colHeader =
			jnew JXColHeaderWidget(itsThisTable, thisScrollbarSet,
								  thisScrollbarSet->GetScrollEnclosure(),
								  JXWidget::kHElastic, JXWidget::kFixedTop,
								  0,0, 10,kHeaderHeight);
		assert( colHeader != nullptr );
		colHeader->FitToEnclosure(true, false);
		itsThisTable->SetColTitles(colHeader);
		colHeader->TurnOnColResizing(20);
	}
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
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("TasksHelp");
	}

	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
EditCommandsDialog::ReadPrefs
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

	itsPartition->ReadGeometry(input);
	itsAllTable->ReadGeometry(input);

	if (itsThisTable != nullptr)
	{
		itsThisTable->ReadGeometry(input);
	}
	else
	{
		JReadAll(input, &itsThisTableSetup);
	}
}

/******************************************************************************
 WritePrefs (virtual protected)

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

	output << ' ';
	itsPartition->WriteGeometry(output);

	output << ' ';
	itsAllTable->WriteGeometry(output);

	if (itsThisTable != nullptr)
	{
		output << ' ';
		itsThisTable->WriteGeometry(output);
	}
	else if (!itsThisTableSetup.IsEmpty())
	{
		if (itsThisTableSetup.GetFirstCharacter() != ' ')
		{
			output << ' ';
		}
		itsThisTableSetup.Print(output);
	}
	else	// use setup from itsAllTable
	{
		output << ' ';
		itsAllTable->WriteGeometry(output);
	}
}

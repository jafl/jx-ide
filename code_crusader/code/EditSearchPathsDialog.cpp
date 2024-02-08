/******************************************************************************
 EditSearchPathsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "EditSearchPathsDialog.h"
#include "PathTable.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

EditSearchPathsDialog::EditSearchPathsDialog
	(
	const DirList& dirList
	)
	:
	JXModalDialogDirector(true),
	JPrefObject(GetPrefsManager(), kEditSearchPathsDialogID)
{
	BuildWindow(dirList);
	JPrefObject::ReadPrefs();
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditSearchPathsDialog::~EditSearchPathsDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 AddDirectories

 ******************************************************************************/

void
EditSearchPathsDialog::AddDirectories
	(
	const JPtrArray<JString>& list
	)
{
	itsTable->AddDirectories(list);
}

/******************************************************************************
 GetPathList

 ******************************************************************************/

void
EditSearchPathsDialog::GetPathList
	(
	DirList* pathList
	)
	const
{
	itsTable->GetPathList(pathList);
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
EditSearchPathsDialog::BuildWindow
	(
	const DirList& dirList
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,460, JGetString("WindowTitle::EditSearchPathsDialog::JXLayout"));

	auto* instrText =
		jnew JXStaticText(JGetString("instrText::EditSearchPathsDialog::JXLayout"), true, false, false, nullptr, window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 330,150);
	assert( instrText != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,190, 240,220);
	assert( scrollbarSet != nullptr );

	auto* addPathButton =
		jnew JXTextButton(JGetString("addPathButton::EditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 280,200, 70,20);
	addPathButton->SetShortcuts(JGetString("addPathButton::shortcuts::EditSearchPathsDialog::JXLayout"));

	auto* removePathButton =
		jnew JXTextButton(JGetString("removePathButton::EditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 280,230, 70,20);
	removePathButton->SetShortcuts(JGetString("removePathButton::shortcuts::EditSearchPathsDialog::JXLayout"));

	auto* choosePathButton =
		jnew JXTextButton(JGetString("choosePathButton::EditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 280,280, 70,20);
	choosePathButton->SetShortcuts(JGetString("choosePathButton::shortcuts::EditSearchPathsDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,430, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 230,430, 70,20);
	assert( okButton != nullptr );

	itsTable =
		jnew PathTable(dirList, addPathButton, removePathButton, choosePathButton, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 225,205);

// end JXLayout

	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
EditSearchPathsDialog::ReadPrefs
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
EditSearchPathsDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);
}

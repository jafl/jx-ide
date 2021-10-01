/******************************************************************************
 EditSearchPathsDialog.cpp

	BASE CLASS = JXDialogDirector

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
	JXDirector*			supervisor,
	const DirList&	dirList,
	RelPathCSF*		csf
	)
	:
	JXDialogDirector(supervisor, true),
	JPrefObject(GetPrefsManager(), kEditSearchPathsDialogID)
{
	BuildWindow(dirList, csf);
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
	const DirList&	dirList,
	RelPathCSF*		csf
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,370, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,100, 240,220);
	assert( scrollbarSet != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,340, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 230,340, 70,20);
	assert( okButton != nullptr );

	auto* instrText =
		jnew JXStaticText(JGetString("instrText::EditSearchPathsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 330,70);
	assert( instrText != nullptr );

	auto* addPathButton =
		jnew JXTextButton(JGetString("addPathButton::EditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,110, 70,20);
	assert( addPathButton != nullptr );
	addPathButton->SetShortcuts(JGetString("addPathButton::EditSearchPathsDialog::shortcuts::JXLayout"));

	auto* removePathButton =
		jnew JXTextButton(JGetString("removePathButton::EditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,140, 70,20);
	assert( removePathButton != nullptr );
	removePathButton->SetShortcuts(JGetString("removePathButton::EditSearchPathsDialog::shortcuts::JXLayout"));

	auto* choosePathButton =
		jnew JXTextButton(JGetString("choosePathButton::EditSearchPathsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,190, 70,20);
	assert( choosePathButton != nullptr );
	choosePathButton->SetShortcuts(JGetString("choosePathButton::EditSearchPathsDialog::shortcuts::JXLayout"));

// end JXLayout

	instrText->GetText()->SetText(JGetString("Instructions::EditSearchPathsDialog"));
	window->AdjustSize(0, instrText->GetBoundsHeight() - instrText->GetFrameHeight());
	instrText->SetSizing(JXWidget::kHElastic, JXWidget::kFixedTop);
	scrollbarSet->SetSizing(JXWidget::kHElastic, JXWidget::kVElastic);
	addPathButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
	removePathButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
	choosePathButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);

	window->SetTitle(JGetString("WindowTitle::EditSearchPathsDialog"));
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	UseModalPlacement(false);
	SetButtons(okButton, cancelButton);

	itsTable =
		jnew PathTable(dirList, addPathButton, removePathButton,
						choosePathButton, csf,
						scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsTable != nullptr );
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

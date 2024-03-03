/******************************************************************************
 ChooseRelativeFileDialog.cpp

	BASE CLASS = JXChooseFileDialog

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "ChooseRelativeFileDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXPathInput.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXPathHistoryMenu.h>
#include <jx-af/jx/JXCurrentPathMenu.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

ChooseRelativeFileDialog*
ChooseRelativeFileDialog::Create
	(
	const ProjectTable::PathType	pathType,
	const SelectType				selectType,
	const JString&					selectName,
	const JString&					fileFilter,
	const JString&					message
	)
{
	auto* dlog = jnew ChooseRelativeFileDialog(fileFilter);
	dlog->BuildWindow(pathType, selectType, selectName, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

ChooseRelativeFileDialog::ChooseRelativeFileDialog
	(
	const JString& fileFilter
	)
	:
	JXChooseFileDialog(fileFilter)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ChooseRelativeFileDialog::~ChooseRelativeFileDialog()
{
}

/******************************************************************************
 GetPathType

 ******************************************************************************/

ProjectTable::PathType
ChooseRelativeFileDialog::GetPathType()
	const
{
	return (ProjectTable::PathType) itsPathTypeRG->GetSelectedItem();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
ChooseRelativeFileDialog::BuildWindow
	(
	const ProjectTable::PathType	pathType,
	const SelectType				selectType,
	const JString&					selectName,
	const JString&					message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,430, JString::empty);

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::ChooseRelativeFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	pathLabel->SetToLabel(false);

	auto* pathHistory =
		jnew JXPathHistoryMenu(1, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::ChooseRelativeFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	filterLabel->SetToLabel(false);

	auto* filterHistory =
		jnew JXStringHistoryMenu(1, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	auto* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::ChooseRelativeFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != nullptr );

	auto* currPathMenu =
		jnew JXCurrentPathMenu("/", window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,190);
	assert( scrollbarSet != nullptr );

	auto* upButton =
		jnew JXTextButton(JGetString("upButton::ChooseRelativeFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	auto* homeButton =
		jnew JXTextButton(JGetString("homeButton::ChooseRelativeFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	auto* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::ChooseRelativeFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != nullptr );

	auto* selectAllButton =
		jnew JXTextButton(JGetString("selectAllButton::ChooseRelativeFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( selectAllButton != nullptr );

	auto* openButton =
		jnew JXTextButton(JGetString("openButton::ChooseRelativeFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	openButton->SetShortcuts(JGetString("openButton::shortcuts::ChooseRelativeFileDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ChooseRelativeFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	itsPathTypeRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,340, 204,84);

	auto* absolutePathRB =
		jnew JXTextRadioButton(ProjectTable::kAbsolutePath, JGetString("absolutePathRB::ChooseRelativeFileDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 180,20);
	assert( absolutePathRB != nullptr );

	auto* projectRelativeRB =
		jnew JXTextRadioButton(ProjectTable::kProjectRelative, JGetString("projectRelativeRB::ChooseRelativeFileDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 180,20);
	assert( projectRelativeRB != nullptr );

	auto* homeDirRB =
		jnew JXTextRadioButton(ProjectTable::kHomeRelative, JGetString("homeDirRB::ChooseRelativeFileDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 180,20);
	assert( homeDirRB != nullptr );

	auto* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	pathInput->SetIsRequired(true);
	pathInput->ShouldAllowInvalidPath(false);
	pathInput->ShouldRequireWritable(false);

	auto* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != nullptr );

// end JXLayout

	SetObjects(scrollbarSet, pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   openButton, cancelButton, upButton, homeButton,
			   desktopButton, selectAllButton,
			   showHiddenCB, currPathMenu,
			   selectType, selectName, message);

	itsPathTypeRG->SelectItem(pathType);

	JString homeDir;
	if (!JGetHomeDirectory(&homeDir))
	{
		homeDirRB->Deactivate();
	}
}

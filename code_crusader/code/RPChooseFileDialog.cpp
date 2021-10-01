/******************************************************************************
 RPChooseFileDialog.cpp

	BASE CLASS = JXChooseFileDialog

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "RPChooseFileDialog.h"
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

RPChooseFileDialog*
RPChooseFileDialog::Create
	(
	JXDirector*						supervisor,
	JDirInfo*						dirInfo,
	const JString&					fileFilter,
	const bool					allowSelectMultiple,
	const RelPathCSF::PathType	pathType,
	const JString&					origName,
	const JString&					message
	)
{
	auto* dlog =
		jnew RPChooseFileDialog(supervisor, dirInfo, fileFilter, allowSelectMultiple);
	assert( dlog != nullptr );
	dlog->BuildWindow(pathType, origName, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

RPChooseFileDialog::RPChooseFileDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const bool	allowSelectMultiple
	)
	:
	JXChooseFileDialog(supervisor, dirInfo, fileFilter, allowSelectMultiple)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RPChooseFileDialog::~RPChooseFileDialog()
{
}

/******************************************************************************
 GetPathType

 ******************************************************************************/

RelPathCSF::PathType
RPChooseFileDialog::GetPathType()
	const
{
	return (RelPathCSF::PathType) itsPathTypeRG->GetSelectedItem();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
RPChooseFileDialog::BuildWindow
	(
	const RelPathCSF::PathType	pathType,
	const JString&					origName,
	const JString&					message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,430, JString::empty);
	assert( window != nullptr );

	auto* openButton =
		jnew JXTextButton(JGetString("openButton::RPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( openButton != nullptr );
	openButton->SetShortcuts(JGetString("openButton::RPChooseFileDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::RPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	auto* homeButton =
		jnew JXTextButton(JGetString("homeButton::RPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::RPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != nullptr );
	pathLabel->SetToLabel();

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,190);
	assert( scrollbarSet != nullptr );

	auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::RPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != nullptr );
	filterLabel->SetToLabel();

	auto* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::RPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != nullptr );

	auto* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != nullptr );

	auto* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != nullptr );

	auto* pathHistory =
		jnew JXPathHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	auto* filterHistory =
		jnew JXStringHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	auto* upButton =
		jnew JXTextButton(JGetString("upButton::RPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	auto* selectAllButton =
		jnew JXTextButton(JGetString("selectAllButton::RPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( selectAllButton != nullptr );

	itsPathTypeRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,340, 204,84);
	assert( itsPathTypeRG != nullptr );

	auto* absolutePathRB =
		jnew JXTextRadioButton(RelPathCSF::kAbsolutePath, JGetString("absolutePathRB::RPChooseFileDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 180,20);
	assert( absolutePathRB != nullptr );

	auto* projectRelativeRB =
		jnew JXTextRadioButton(RelPathCSF::kProjectRelative, JGetString("projectRelativeRB::RPChooseFileDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 180,20);
	assert( projectRelativeRB != nullptr );

	auto* homeDirRB =
		jnew JXTextRadioButton(RelPathCSF::kHomeRelative, JGetString("homeDirRB::RPChooseFileDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 180,20);
	assert( homeDirRB != nullptr );

	auto* currPathMenu =
		jnew JXCurrentPathMenu(JString("/", JString::kNoCopy), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != nullptr );

	auto* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::RPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != nullptr );

// end JXLayout

	SetObjects(scrollbarSet, pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   openButton, cancelButton, upButton, homeButton,
			   desktopButton, selectAllButton,
			   showHiddenCB, currPathMenu, origName, message);

	itsPathTypeRG->SelectItem(pathType);

	JString homeDir;
	if (!JGetHomeDirectory(&homeDir))
	{
		homeDirRB->Deactivate();
	}
}

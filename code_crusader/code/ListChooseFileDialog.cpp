/******************************************************************************
 ListChooseFileDialog.cpp

	BASE CLASS = JXChooseFileDialog

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "ListChooseFileDialog.h"
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
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

const JIndex kReplaceCmd = 1;
const JIndex kAppendCmd  = 2;

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

ListChooseFileDialog*
ListChooseFileDialog::Create
	(
	const JString&		replaceListStr,
	const JString&		appendToListStr,
	const SelectType	selectType,
	const JString&		selectName,
	const JString&		fileFilter,
	const JString&		message
	)
{
	auto* dlog = jnew ListChooseFileDialog(fileFilter);
	dlog->BuildWindow(replaceListStr, appendToListStr, selectType, selectName, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

ListChooseFileDialog::ListChooseFileDialog
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

ListChooseFileDialog::~ListChooseFileDialog()
{
}

/******************************************************************************
 ReplaceExisting

 ******************************************************************************/

bool
ListChooseFileDialog::ReplaceExisting()
	const
{
	return itsAppendReplaceRG->GetSelectedItem() == kReplaceCmd;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
ListChooseFileDialog::BuildWindow
	(
	const JString&		replaceListStr,
	const JString&		appendToListStr,
	const SelectType	selectType,
	const JString&		selectName,
	const JString&		message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,420, JString::empty);

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::ListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	pathLabel->SetToLabel(false);

	auto* pathHistory =
		jnew JXPathHistoryMenu(1, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::ListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	filterLabel->SetToLabel(false);

	auto* filterHistory =
		jnew JXStringHistoryMenu(1, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	auto* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::ListChooseFileDialog::JXLayout"), window,
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
		jnew JXTextButton(JGetString("upButton::ListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	auto* homeButton =
		jnew JXTextButton(JGetString("homeButton::ListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	auto* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::ListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != nullptr );

	auto* selectAllButton =
		jnew JXTextButton(JGetString("selectAllButton::ListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( selectAllButton != nullptr );

	auto* openButton =
		jnew JXTextButton(JGetString("openButton::ListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 219,279, 72,22);
	openButton->SetShortcuts(JGetString("openButton::shortcuts::ListChooseFileDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	itsAppendReplaceRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,340, 99,74);

	auto* replaceRB =
		jnew JXTextRadioButton(kReplaceCmd, JGetString("replaceRB::ListChooseFileDialog::JXLayout"), itsAppendReplaceRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 80,20);
	assert( replaceRB != nullptr );

	auto* appendRB =
		jnew JXTextRadioButton(kAppendCmd, JGetString("appendRB::ListChooseFileDialog::JXLayout"), itsAppendReplaceRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,40, 80,20);
	assert( appendRB != nullptr );

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
			   showHiddenCB, currPathMenu, selectType, selectName, message);

	itsAppendReplaceRG->SelectItem(kAppendCmd);
	replaceRB->SetLabel(replaceListStr);
	appendRB->SetLabel(appendToListStr);
}

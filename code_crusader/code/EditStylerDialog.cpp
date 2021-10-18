/******************************************************************************
 EditStylerDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "EditStylerDialog.h"
#include "StylerTable.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kMaxTypeRowCount = 10;

/******************************************************************************
 Constructor

 ******************************************************************************/

EditStylerDialog::EditStylerDialog
	(
	const JString&							windowTitle,
	const bool								active,
	const JUtf8Byte**						typeNames,
	const JArray<JFontStyle>&				typeStyles,
	const JArray<StylerBase::WordStyle>&	wordList,
	const TextFileType						fileType
	)
	:
	JXDialogDirector(JXGetApplication(), true)
{
	BuildWindow(windowTitle, active, typeNames, typeStyles, wordList, fileType);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditStylerDialog::~EditStylerDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
EditStylerDialog::BuildWindow
	(
	const JString&							windowTitle,
	const bool								active,
	const JUtf8Byte**						typeNames,
	const JArray<JFontStyle>&				typeStyles,
	const JArray<StylerBase::WordStyle>&	wordList,
	const TextFileType						fileType
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,390, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet1 =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,110, 240,110);
	assert( scrollbarSet1 != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,360, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 250,360, 70,20);
	assert( okButton != nullptr );

	auto* instrText =
		jnew JXStaticText(JGetString("instrText::EditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 330,50);
	assert( instrText != nullptr );

	auto* newWordButton =
		jnew JXTextButton(JGetString("newWordButton::EditStylerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,240, 70,20);
	assert( newWordButton != nullptr );
	newWordButton->SetShortcuts(JGetString("newWordButton::EditStylerDialog::shortcuts::JXLayout"));

	auto* removeButton =
		jnew JXTextButton(JGetString("removeButton::EditStylerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,270, 70,20);
	assert( removeButton != nullptr );
	removeButton->SetShortcuts(JGetString("removeButton::EditStylerDialog::shortcuts::JXLayout"));

	auto* scrollbarSet2 =
		jnew JXScrollbarSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,230, 240,110);
	assert( scrollbarSet2 != nullptr );

	itsActiveCB =
		jnew JXTextCheckbox(JGetString("itsActiveCB::EditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,15, 220,20);
	assert( itsActiveCB != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 145,360, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::EditStylerDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(windowTitle);
	SetButtons(okButton, cancelButton);

	itsActiveCB->SetState(active);
	instrText->GetText()->SetText(JGetString("Instructions::EditStylerDialog"));
	ListenTo(itsHelpButton);

	// create tables

	itsTypeTable =
		jnew StylerTable(typeNames, typeStyles, scrollbarSet1,
						  scrollbarSet1->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsTypeTable != nullptr );
	itsTypeTable->FitToEnclosure();

	itsWordTable =
		jnew StylerTable(fileType, wordList, newWordButton, removeButton,
						  scrollbarSet2, scrollbarSet2->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsWordTable != nullptr );
	itsWordTable->FitToEnclosure();

	// adjust window size

	JCoordinate rowBorderWidth;
	JColorID rowBorderColor;
	itsTypeTable->GetRowBorderInfo(&rowBorderWidth, &rowBorderColor);

	const JCoordinate bdh =
		JMin(itsTypeTable->GetBoundsHeight(),
			 kMaxTypeRowCount * itsTypeTable->GetDefaultRowHeight() +
			 (kMaxTypeRowCount-1) * rowBorderWidth);
	const JCoordinate aph = itsTypeTable->GetApertureHeight();
	window->AdjustSize(0, bdh - aph);

	scrollbarSet1->SetSizing(JXWidget::kHElastic, JXWidget::kFixedTop);
	scrollbarSet2->SetSizing(JXWidget::kHElastic, JXWidget::kVElastic);

	okButton->SetSizing(JXWidget::kFixedLeft, JXWidget::kFixedBottom);
	cancelButton->SetSizing(JXWidget::kFixedLeft, JXWidget::kFixedBottom);
	itsHelpButton->SetSizing(JXWidget::kFixedLeft, JXWidget::kFixedBottom);

	newWordButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
	removeButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);

	UseModalPlacement(false);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
}

/******************************************************************************
 GetData

	colorList can be nullptr.

 ******************************************************************************/

void
EditStylerDialog::GetData
	(
	bool*					active,
	JArray<JFontStyle>*		typeStyles,
	JStringMap<JFontStyle>*	wordStyles
	)
	const
{
	*active = itsActiveCB->IsChecked();

	itsTypeTable->GetData(typeStyles);
	itsWordTable->GetData(wordStyles);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
EditStylerDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		#if defined CODE_CRUSADER
		JXGetHelpManager()->ShowSection("StylerHelp");
		#endif
	}
	else
	{
		JXDialogDirector::Receive(sender, message);
	}
}

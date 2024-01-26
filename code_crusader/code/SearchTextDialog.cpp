/******************************************************************************
 SearchTextDialog.cpp

	BASE CLASS = JXSearchTextDialog, JPrefObject

	Copyright © 1996-99 by John Lindal.

 ******************************************************************************/

#include "SearchTextDialog.h"
#include "SearchDocument.h"
#include "TextEditor.h"
#include "SearchPathHistoryMenu.h"
#include "SearchFilterHistoryMenu.h"
#include "ListChooseFileDialog.h"
#include "ProjectDocument.h"
#include "sharedUtil.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXPathInput.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXSearchTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXFileListTable.h>
#include <jx-af/jx/JXStringHistoryMenu.h>
#include <jx-af/jx/JXDocumentMenu.h>
#include <jx-af/jx/JXDownRect.h>
#include <jx-af/jx/JXChooseFileDialog.h>
#include <jx-af/jx/JXSaveFileDialog.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/JLatentPG.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <sstream>
#include <stdio.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 73;	// first digit must be >= 7

	// version 73 stores path filter info
	// version 72 stores itsInvertFileFilterCB
	// version 71 stores directory information
	// version 7  split off JXSearchTextDialog
	// version 6  no longer stores search direction
	// version 5  stores itsListFilesWithoutMatchFlag, itsFileSetName
	// version 4  stores search file list, itsOnlyListFilesFlag
	// version 3  stores singleLine, matchCase
	// version 2  stores replaceIsRegex
	// version 1  uses JXWindow::Read/WriteGeometry()

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

SearchTextDialog*
SearchTextDialog::Create()
{
	auto* dlog = jnew SearchTextDialog;
	dlog->BuildWindow();
	dlog->JXSearchTextDialogX();
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

SearchTextDialog::SearchTextDialog()
	:
	JPrefObject(GetPrefsManager(), kSearchTextID),
	itsFileSetName("Untitled")
{
	itsOnlyListFilesFlag         = false;
	itsListFilesWithoutMatchFlag = false;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SearchTextDialog::~SearchTextDialog()
{
	// prefs written by DeleteGlobals()
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
SearchTextDialog::Activate()
{
	JXSearchTextDialog::Activate();

	if (IsActive())
	{
		itsMultifileCB->SetState(false);
		itsSearchDirCB->SetState(false);
	}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "SearchTextDialog-FileList.h"

void
SearchTextDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 450,460, JString::empty);

	auto* replaceLabel =
		jnew JXStaticText(JGetString("replaceLabel::SearchTextDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,85, 220,20);
	assert( replaceLabel != nullptr );
	replaceLabel->SetToLabel();

	auto* searchLabel =
		jnew JXStaticText(JGetString("searchLabel::SearchTextDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,9, 220,20);
	assert( searchLabel != nullptr );
	searchLabel->SetToLabel();

	auto* searchInput =
		jnew JXInputField(true, false, window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,30, 220,45);
	assert( searchInput != nullptr );

	auto* replaceInput =
		jnew JXInputField(true, false, window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,105, 220,45);
	assert( replaceInput != nullptr );

	auto* closeButton =
		jnew JXTextButton(JGetString("closeButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,430, 80,20);
	assert( closeButton != nullptr );

	auto* ignoreCaseCB =
		jnew JXTextCheckbox(JGetString("ignoreCaseCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,170, 130,20);
	assert( ignoreCaseCB != nullptr );

	auto* wrapSearchCB =
		jnew JXTextCheckbox(JGetString("wrapSearchCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,230, 130,20);
	assert( wrapSearchCB != nullptr );

	auto* replaceButton =
		jnew JXTextButton(JGetString("replaceButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,50, 140,20);
	assert( replaceButton != nullptr );

	auto* entireWordCB =
		jnew JXTextCheckbox(JGetString("entireWordCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,200, 130,20);
	assert( entireWordCB != nullptr );

	auto* prevReplaceMenu =
		jnew JXStringHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 245,118, 30,20);
	assert( prevReplaceMenu != nullptr );

	auto* prevSearchMenu =
		jnew JXStringHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 245,43, 30,20);
	assert( prevSearchMenu != nullptr );

	auto* helpButton =
		jnew JXTextButton(JGetString("helpButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,430, 80,20);
	assert( helpButton != nullptr );
	helpButton->SetShortcuts(JGetString("helpButton::SearchTextDialog::shortcuts::JXLayout"));

	auto* searchIsRegexCB =
		jnew JXTextCheckbox(JGetString("searchIsRegexCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,170, 150,20);
	assert( searchIsRegexCB != nullptr );

	auto* replaceIsRegexCB =
		jnew JXTextCheckbox(JGetString("replaceIsRegexCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,170, 110,20);
	assert( replaceIsRegexCB != nullptr );

	auto* singleLineCB =
		jnew JXTextCheckbox(JGetString("singleLineCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,200, 150,20);
	assert( singleLineCB != nullptr );

	auto* preserveCaseCB =
		jnew JXTextCheckbox(JGetString("preserveCaseCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,200, 110,20);
	assert( preserveCaseCB != nullptr );

	auto* qRefButton =
		jnew JXTextButton(JGetString("qRefButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 190,430, 80,20);
	assert( qRefButton != nullptr );

	itsMultifileCB =
		jnew JXTextCheckbox(JGetString("itsMultifileCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,350, 100,20);
	assert( itsMultifileCB != nullptr );
	itsMultifileCB->SetShortcuts(JGetString("itsMultifileCB::SearchTextDialog::shortcuts::JXLayout"));

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 140,350, 300,60);
	assert( scrollbarSet != nullptr );

	auto* findLabel =
		jnew JXStaticText(JGetString("findLabel::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,20, 100,20);
	assert( findLabel != nullptr );
	findLabel->SetToLabel(true);

	auto* findBackButton =
		jnew JXSearchTextButton(false, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,20, 20,20);
	assert( findBackButton != nullptr );

	auto* findFwdButton =
		jnew JXSearchTextButton(true, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,20, 20,20);
	assert( findFwdButton != nullptr );

	auto* replaceFindLabel =
		jnew JXStaticText(JGetString("replaceFindLabel::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,80, 100,20);
	assert( replaceFindLabel != nullptr );
	replaceFindLabel->SetToLabel(true);

	auto* replaceFindBackButton =
		jnew JXSearchTextButton(false, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,80, 20,20);
	assert( replaceFindBackButton != nullptr );

	auto* replaceFindFwdButton =
		jnew JXSearchTextButton(true, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,80, 20,20);
	assert( replaceFindFwdButton != nullptr );

	auto* stayOpenCB =
		jnew JXTextCheckbox(JGetString("stayOpenCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,230, 150,20);
	assert( stayOpenCB != nullptr );

	auto* retainFocusCB =
		jnew JXTextCheckbox(JGetString("retainFocusCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,230, 110,20);
	assert( retainFocusCB != nullptr );

	itsFileListMenu =
		jnew JXTextMenu(JGetString("itsFileListMenu::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,380, 90,30);
	assert( itsFileListMenu != nullptr );

	itsSearchDirCB =
		jnew JXTextCheckbox(JGetString("itsSearchDirCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,260, 130,20);
	assert( itsSearchDirCB != nullptr );
	itsSearchDirCB->SetShortcuts(JGetString("itsSearchDirCB::SearchTextDialog::shortcuts::JXLayout"));

	itsDirInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,260, 190,20);
	assert( itsDirInput != nullptr );

	auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,290, 100,20);
	assert( filterLabel != nullptr );
	filterLabel->SetToLabel();

	itsDirHistory =
		jnew SearchPathHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,260, 30,20);
	assert( itsDirHistory != nullptr );

	itsFileFilterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,290, 190,20);
	assert( itsFileFilterInput != nullptr );

	itsFileFilterHistory =
		jnew SearchFilterHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,290, 30,20);
	assert( itsFileFilterHistory != nullptr );

	itsRecurseDirCB =
		jnew JXTextCheckbox(JGetString("itsRecurseDirCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,320, 215,20);
	assert( itsRecurseDirCB != nullptr );

	itsChooseDirButton =
		jnew JXTextButton(JGetString("itsChooseDirButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,260, 60,20);
	assert( itsChooseDirButton != nullptr );

	itsInvertFileFilterCB =
		jnew JXTextCheckbox(JGetString("itsInvertFileFilterCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,290, 60,20);
	assert( itsInvertFileFilterCB != nullptr );

	itsPathFilterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 255,320, 155,20);
	assert( itsPathFilterInput != nullptr );

	itsPathFilterHistory =
		jnew SearchFilterHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,320, 30,20);
	assert( itsPathFilterHistory != nullptr );

	auto* replaceAllButton =
		jnew JXTextButton(JGetString("replaceAllButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,110, 70,20);
	assert( replaceAllButton != nullptr );

	auto* replaceAllInSelButton =
		jnew JXTextButton(JGetString("replaceAllInSelButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 370,110, 70,20);
	assert( replaceAllInSelButton != nullptr );

// end JXLayout

	SetObjects(searchInput, prevSearchMenu,
			   ignoreCaseCB, entireWordCB, wrapSearchCB,
			   searchIsRegexCB, singleLineCB,
			   replaceInput, prevReplaceMenu,
			   replaceIsRegexCB, preserveCaseCB,
			   stayOpenCB, retainFocusCB,
			   findFwdButton, findBackButton,
			   replaceButton,
			   replaceFindFwdButton, replaceFindBackButton,
			   replaceAllButton, replaceAllInSelButton,
			   closeButton, helpButton, qRefButton);

	ListenTo(itsSearchDirCB);
	ListenTo(itsDirInput);
	ListenTo(itsMultifileCB);

	itsFileList =
		jnew JXFileListTable(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 10,10);
	assert( itsFileList != nullptr );
	itsFileList->FitToEnclosure();
	itsFileList->ShouldAcceptFileDrop();
	itsFileList->BackspaceShouldRemoveSelectedFiles();
	ListenTo(itsFileList);

	itsFileListMenu->SetMenuItems(kOptionsMenuStr);
	itsFileListMenu->AttachHandlers(this,
		&SearchTextDialog::UpdateFileListMenu,
		&SearchTextDialog::HandleFileListMenu);
	ConfigureOptionsMenu(itsFileListMenu);

	ListenTo(itsChooseDirButton);
	ListenTo(itsDirHistory);
	ListenTo(itsFileFilterHistory);
	ListenTo(itsPathFilterHistory);

	const JFont& font = JFontManager::GetDefaultMonospaceFont();

	itsDirInput->ShouldAllowInvalidPath();
	itsDirHistory->SetDefaultFont(font, true);
	itsFileFilterInput->GetText()->SetDefaultFont(font);
	itsFileFilterHistory->SetDefaultFont(font, true);
	itsPathFilterInput->GetText()->SetDefaultFont(font);
	itsPathFilterHistory->SetDefaultFont(font, true);
	itsRecurseDirCB->SetState(true);

	UpdateBasePath();
	ListenTo(GetDocumentManager());

	PrefsManager* prefsMgr = GetPrefsManager();
	JPrefObject::ReadPrefs();

	JString fontName;
	JSize fontSize;
	prefsMgr->GetDefaultFont(&fontName, &fontSize);
	SetFont(JFontManager::GetFont(fontName, fontSize));

	searchInput->GetText()->SetCharacterInWordFunction(IsCharacterInWord);
	replaceInput->GetText()->SetCharacterInWordFunction(IsCharacterInWord);

	// create hidden JXDocument so Meta-# shortcuts work

	jnew JXDocumentMenu(JString::empty, window,
					   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);

	// decor

	const JRect wFrame  = window->GetFrame();
	const JRect cbFrame = itsSearchDirCB->GetFrame();

	auto* line =
		jnew JXDownRect(window, JXWidget::kFixedLeft, JXWidget::kFixedTop,
					   cbFrame.left, cbFrame.top-6,
					   wFrame.right-10-cbFrame.left, 2);
	line->SetBorderWidth(1);
}

/******************************************************************************
 UpdateDisplay (virtual protected)

 ******************************************************************************/

void
SearchTextDialog::UpdateDisplay()
{
	JXSearchTextDialog::UpdateDisplay();

	if (itsFileList->GetRowCount() == 0)
	{
		itsMultifileCB->SetState(false);
	}

	if (itsMultifileCB->IsChecked() || itsSearchDirCB->IsChecked())
	{
		GetFindBackButton()->Deactivate();
		GetReplaceButton()->Deactivate();
		GetReplaceFindFwdButton()->Deactivate();
		GetReplaceFindBackButton()->Deactivate();
		GetReplaceAllInSelButton()->Deactivate();

		// These two buttons will already be deactivated if !HasSearchText().

		if (HasSearchText())
		{
			GetFindFwdButton()->Activate();

			if (GetDocumentManager()->WillEditTextFilesLocally())
			{
				GetReplaceAllButton()->Activate();
			}
		}
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SearchTextDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetFindFwdButton() && message.Is(JXButton::kPushed) &&
		(itsMultifileCB->IsChecked() || itsSearchDirCB->IsChecked()))
	{
		SearchFiles();
	}
	else if (sender == GetReplaceAllButton() && message.Is(JXButton::kPushed) &&
			 (itsMultifileCB->IsChecked() || itsSearchDirCB->IsChecked()))
	{
		SearchFilesAndReplace();
	}

	else if (sender == itsFileList && message.Is(JXFileListTable::kProcessSelection))
	{
		OpenSelectedFiles();
	}
	else if (sender == itsFileList &&
			 (message.Is(JTable::kRowsInserted) ||
			  message.Is(JTable::kRowsRemoved)))
	{
		UpdateDisplay();
	}

	else if (sender == itsSearchDirCB && message.Is(JXCheckbox::kPushed))
	{
		UpdateDisplay();
	}
	else if (sender == itsDirInput &&
			 (message.Is(JStyledText::kTextChanged) ||
			  message.Is(JStyledText::kTextSet)))
	{
		itsSearchDirCB->SetState(true);
	}
	else if (sender == itsMultifileCB && message.Is(JXCheckbox::kPushed))
	{
		const bool on = itsMultifileCB->IsChecked();
		if (on && itsFileList->GetRowCount() == 0)
		{
			AddSearchFiles();
		}
		UpdateDisplay();
	}

	else if (sender == GetDocumentManager() &&
			 message.Is(DocumentManager::kProjectDocumentActivated))
	{
		UpdateBasePath();
	}
	else if (sender == itsChooseDirButton && message.Is(JXButton::kPushed))
	{
		itsDirInput->ChoosePath(JString::empty);
		itsSearchDirCB->SetState(true);
	}

	else if (sender == itsDirHistory && message.Is(JXMenu::kItemSelected))
	{
		bool recurse;
		itsDirInput->GetText()->SetText(itsDirHistory->GetPath(message, &recurse));
		itsRecurseDirCB->SetState(recurse);
		itsSearchDirCB->SetState(true);
	}
	else if (sender == itsFileFilterHistory && message.Is(JXMenu::kItemSelected))
	{
		bool invert;
		itsFileFilterInput->GetText()->SetText(itsFileFilterHistory->GetFilter(message, &invert));
		itsInvertFileFilterCB->SetState(invert);
		itsSearchDirCB->SetState(true);
	}
	else if (sender == itsPathFilterHistory && message.Is(JXMenu::kItemSelected))
	{
		bool invert;
		itsPathFilterInput->GetText()->SetText(itsPathFilterHistory->GetFilter(message, &invert));
		itsSearchDirCB->SetState(true);
	}

	else
	{
		JXSearchTextDialog::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateFileListMenu (private)

 ******************************************************************************/

void
SearchTextDialog::UpdateFileListMenu()
{
	itsFileListMenu->EnableItem(kShowMatchTextCmd);
	if (!itsOnlyListFilesFlag)
	{
		itsFileListMenu->CheckItem(kShowMatchTextCmd);
	}

	itsFileListMenu->EnableItem(kListFilesWithoutMatchCmd);
	if (itsListFilesWithoutMatchFlag)
	{
		itsFileListMenu->CheckItem(kListFilesWithoutMatchCmd);
	}

	itsFileListMenu->EnableItem(kAddFilesCmd);
	itsFileListMenu->EnableItem(kLoadFileSetCmd);

	if (itsFileList->GetRowCount() > 0)
	{
		itsFileListMenu->EnableItem(kRemoveAllCmd);
		itsFileListMenu->EnableItem(kSaveFileSetCmd);

		if (itsFileList->HasSelection())
		{
			itsFileListMenu->EnableItem(kRemoveSelCmd);
		}
	}
}

/******************************************************************************
 HandleFileListMenu (private)

 ******************************************************************************/

void
SearchTextDialog::HandleFileListMenu
	(
	const JIndex index
	)
{
	if (index == kShowMatchTextCmd)
	{
		itsOnlyListFilesFlag = !itsOnlyListFilesFlag;
	}
	else if (index == kListFilesWithoutMatchCmd)
	{
		itsListFilesWithoutMatchFlag = !itsListFilesWithoutMatchFlag;
	}

	else if (index == kAddFilesCmd)
	{
		AddSearchFiles();
	}

	else if (index == kRemoveSelCmd)
	{
		itsFileList->RemoveSelectedFiles();
	}
	else if (index == kRemoveAllCmd)
	{
		itsFileList->RemoveAllFiles();
	}

	else if (index == kLoadFileSetCmd)
	{
		LoadFileSet();
	}
	else if (index == kSaveFileSetCmd)
	{
		SaveFileSet();
	}
}

/******************************************************************************
 SearchFiles (private)

	We use GetSearchParameters() to check that everything is ok.

 ******************************************************************************/

void
SearchTextDialog::SearchFiles()
	const
{
	JRegex* searchRegex;
	JString replaceStr;
	JInterpolate* interpolator;
	bool entireWord, wrapSearch, preserveCase;

	auto* fileList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( fileList != nullptr );

	auto* nameList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( nameList != nullptr );

	if (GetSearchParameters(&searchRegex, &entireWord, &wrapSearch,
							&replaceStr, &interpolator, &preserveCase) &&
		BuildSearchFileList(fileList, nameList))
	{
		// takes ownership of fileList & nameList
		SearchDocument::Create(fileList, nameList, itsOnlyListFilesFlag,
							   itsListFilesWithoutMatchFlag);
	}
}

/******************************************************************************
 SearchFilesAndReplace (private)

 ******************************************************************************/

void
SearchTextDialog::SearchFilesAndReplace()
{
	JRegex* searchRegex;
	JString replaceStr;
	JInterpolate* interpolator;
	bool entireWord, wrapSearch, preserveCase;

	auto* fileList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( fileList != nullptr );

	auto* nameList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( nameList != nullptr );

	if (GetSearchParameters(&searchRegex, &entireWord, &wrapSearch,
							&replaceStr, &interpolator, &preserveCase) &&
		BuildSearchFileList(fileList, nameList))
	{
		// takes ownership of fileList & nameList
		SearchDocument::Create(fileList, nameList, replaceStr);
	}
}

/******************************************************************************
 BuildSearchFileList (private)

	The client need not call DeleteAll() because the clean up action is set
	for each array.

 ******************************************************************************/

bool
SearchTextDialog::BuildSearchFileList
	(
	JPtrArray<JString>* fileList,
	JPtrArray<JString>* nameList
	)
	const
{
	JXGetApplication()->DisplayBusyCursor();

	fileList->SetCleanUpAction(JPtrArrayT::kDeleteAll);
	fileList->SetCompareFunction(JCompareStringsCaseSensitive);

	nameList->SetCleanUpAction(JPtrArrayT::kDeleteAll);
	nameList->SetCompareFunction(JCompareStringsCaseSensitive);

	if (itsMultifileCB->IsChecked())
	{
		const JPtrArray<JString>& fullNameList = itsFileList->GetFullNameList();

		const JSize count = fullNameList.GetItemCount();
		for (JIndex i=1; i<=count; i++)
		{
			SaveFileForSearch(*(fullNameList.GetItem(i)), fileList, nameList);
		}
	}

	bool ok = true;

	JString path, fileFilter, pathFilter;
	if (GetSearchDirectory(&path, &fileFilter, &pathFilter))
	{
		JRegex* fileRegex = nullptr;
		JString regexStr;
		if (JDirInfo::BuildRegexFromWildcardFilter(fileFilter, &regexStr))
		{
			fileRegex = jnew JRegex(regexStr, "i");
		}

		JRegex* pathRegex = nullptr;
		if (JDirInfo::BuildRegexFromWildcardFilter(pathFilter, &regexStr))
		{
			pathRegex = jnew JRegex(regexStr, "i");
		}

		JLatentPG pg(100);
		pg.VariableLengthProcessBeginning(JGetString("CollectingFiles::SearchTextDialog"), true, true);
		ok = SearchDirectory(path, fileRegex, pathRegex, fileList, nameList, pg);
		pg.ProcessFinished();

		jdelete fileRegex;
		jdelete pathRegex;
	}

	return ok && !fileList->IsEmpty();
}

/******************************************************************************
 SearchDirectory (private)

 ******************************************************************************/

bool
SearchTextDialog::SearchDirectory
	(
	const JString&		path,
	const JRegex*		fileRegex,
	const JRegex*		pathRegex,
	JPtrArray<JString>*	fileList,
	JPtrArray<JString>*	nameList,
	JProgressDisplay&	pg
	)
	const
{
	JDirInfo* info;
	if (!JDirInfo::Create(path, &info))
	{
		return true;	// user didn't cancel
	}
	info->SetWildcardFilter(const_cast<JRegex*>(fileRegex), false,
							itsInvertFileFilterCB->IsChecked());

	bool keepGoing = true;

	const JSize count = info->GetEntryCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JDirEntry& entry = info->GetEntry(i);
		if (entry.IsFile())
		{
			if (!pg.IncrementProgress())
			{
				keepGoing = false;
				break;
			}
			SaveFileForSearch(entry.GetFullName(), fileList, nameList);
		}
		else if (itsRecurseDirCB->IsChecked() &&
				 entry.IsDirectory() && !entry.IsLink() &&
				 !JIsVCSDirectory(entry.GetName()))
		{
			bool match = true;
			if (pathRegex != nullptr)
			{
				match = ! pathRegex->Match(entry.GetName());
			}

			if (match &&
				!SearchDirectory(entry.GetFullName(), fileRegex, pathRegex,
								 fileList, nameList, pg))
			{
				keepGoing = false;
				break;
			}
		}
	}

	jdelete info;
	return keepGoing;
}

/******************************************************************************
 SaveFileForSearch (private)

	Save the file in /tmp if necessary so the current text will be searched.

 ******************************************************************************/

void
SearchTextDialog::SaveFileForSearch
	(
	const JString&		fullName,
	JPtrArray<JString>*	fileList,
	JPtrArray<JString>*	nameList
	)
	const
{
	bool exists;
	const JIndex index =
		nameList->GetInsertionSortIndex(const_cast<JString*>(&fullName), &exists);
	if (!exists)
	{
		auto* file = jnew JString(fullName);

		JXFileDocument* doc;
		if (GetDocumentManager()->FileDocumentIsOpen(*file, &doc) &&
			doc->NeedsSave())
		{
			auto* textDoc = dynamic_cast<TextDocument*>(doc);
			if (textDoc != nullptr &&
				JCreateTempFile(file).OK())
			{
				std::ofstream output(file->GetBytes());
				textDoc->GetTextEditor()->GetText()->GetText().Print(output);
			}
		}

		if (!file->IsEmpty())
		{
			auto* name = jnew JString(fullName);
			nameList->InsertAtIndex(index, name);

			fileList->InsertAtIndex(index, file);
		}
		else
		{
			jdelete file;
		}
	}
}

/******************************************************************************
 UpdateBasePath (private)

 ******************************************************************************/

void
SearchTextDialog::UpdateBasePath()
{
	ProjectDocument* doc;
	if (GetDocumentManager()->GetActiveProjectDocument(&doc))
	{
		itsDirInput->SetBasePath(doc->GetFilePath());
	}
	else
	{
		itsDirInput->ClearBasePath();
	}
}

/******************************************************************************
 GetSearchDirectory (private)

 ******************************************************************************/

bool
SearchTextDialog::GetSearchDirectory
	(
	JString* path,
	JString* fileFilter,
	JString* pathFilter
	)
	const
{
	if (!itsSearchDirCB->IsChecked())
	{
		path->Clear();
		fileFilter->Clear();
		pathFilter->Clear();
		return true;
	}

	*fileFilter = itsFileFilterInput->GetText()->GetText();
	*pathFilter = itsPathFilterInput->GetText()->GetText();

	itsDirInput->ShouldAllowInvalidPath(false);
	const bool ok = itsDirInput->GetPath(path);
	if (ok)
	{
		itsDirHistory->AddPath(itsDirInput->GetText()->GetText(), itsRecurseDirCB->IsChecked());
		itsFileFilterHistory->AddFilter(itsFileFilterInput->GetText()->GetText(), itsInvertFileFilterCB->IsChecked());
		itsPathFilterHistory->AddFilter(itsPathFilterInput->GetText()->GetText(), false);
	}
	else
	{
		itsDirInput->Focus();
		itsDirInput->InputValid();	// report error
	}
	itsDirInput->ShouldAllowInvalidPath(true);

	return ok;
}

/******************************************************************************
 LoadFileSet (private)

 ******************************************************************************/

void
SearchTextDialog::LoadFileSet()
{
	auto* dlog =
		ListChooseFileDialog::Create(JGetString("ReplaceFileList::SearchTextDialog"),
									 JGetString("AppendToFileList::SearchTextDialog"),
									 JXChooseFileDialog::kSelectSingleFile);

	if (dlog->DoDialog())
	{
		if (dlog->ReplaceExisting())
		{
			itsFileList->RemoveAllFiles();
		}

		const JString fullName = dlog->GetFullName();
		std::ifstream input(fullName.GetBytes());
		JString file;
		while (!input.eof() && !input.fail())
		{
			file = JReadLine(input);
			if (JFileReadable(file))
			{
				itsFileList->AddFile(file);
			}
		}
	}
}

/******************************************************************************
 SaveFileSet (private)

 ******************************************************************************/

void
SearchTextDialog::SaveFileSet()
	const
{
	auto* dlog = JXSaveFileDialog::Create(JGetString("SaveFileSet::SearchTextDialog"), itsFileSetName);
	if (dlog->DoDialog())
	{
		itsFileSetName = dlog->GetFullName();

		std::ofstream output(itsFileSetName.GetBytes());
		const JPtrArray<JString>& fullNameList = itsFileList->GetFullNameList();
		const JSize fileCount = fullNameList.GetItemCount();
		for (JIndex i=1; i<=fileCount; i++)
		{
			fullNameList.GetItem(i)->Print(output);
			output << '\n';
		}
	}
}

/******************************************************************************
 AddSearchFiles (private)

 ******************************************************************************/

void
SearchTextDialog::AddSearchFiles()
{
	auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectMultipleFiles);
	if (dlog->DoDialog())
	{
		JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
		dlog->GetFullNames(&fileList);

		for (const auto* f : fileList)
		{
			itsFileList->AddFile(*f);
		}
	}
}

/******************************************************************************
 AddFileToSearch

 ******************************************************************************/

void
SearchTextDialog::AddFileToSearch
	(
	const JString& fileName
	)
	const
{
	itsFileList->AddFile(fileName);
}

/******************************************************************************
 ShouldSearchFiles

 ******************************************************************************/

void
SearchTextDialog::ShouldSearchFiles
	(
	const bool search
	)
{
	itsMultifileCB->SetState(search);
}

/******************************************************************************
 ClearFileList

 ******************************************************************************/

void
SearchTextDialog::ClearFileList()
{
	itsFileList->RemoveAllFiles();
}

/******************************************************************************
 OpenSelectedFiles (private)

 ******************************************************************************/

void
SearchTextDialog::OpenSelectedFiles()
	const
{
	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	if (itsFileList->GetSelection(&fileList))
	{
		GetDocumentManager()->OpenSomething(fileList);
	}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
SearchTextDialog::ReadPrefs
	(
	std::istream& input
	)
{
	input >> std::ws;
	const JUtf8Byte versChar = input.peek();

	JFileVersion vers;
	if (versChar <= '6')
	{
		// leave version number in stream
		vers = versChar - '0';
	}
	else
	{
		input >> vers;
	}

	if (vers > kCurrentSetupVersion)
	{
		return;
	}

	JXSearchTextDialog::ReadSetup(input);

	// file list

	if (vers >= 4)
	{
		input >> JBoolFromString(itsOnlyListFilesFlag);
		if (vers >= 5)
		{
			input >> JBoolFromString(itsListFilesWithoutMatchFlag) >> itsFileSetName;
		}

		JSize fileCount;
		input >> fileCount;

		JString fullName;
		for (JIndex i=1; i<=fileCount; i++)
		{
			input >> fullName;
			if (JFileReadable(fullName))
			{
				itsFileList->AddFile(fullName);
			}
		}
	}

	// directory

	if (vers >= 71)
	{
		bool b;
		input >> JBoolFromString(b);
		itsRecurseDirCB->SetState(b);

		if (vers >= 72)
		{
			input >> JBoolFromString(b);
			itsInvertFileFilterCB->SetState(b);
		}

		JString s;
		input >> s;
		itsDirInput->GetText()->SetText(s);
		input >> s;
		itsFileFilterInput->GetText()->SetText(s);

		itsDirHistory->ReadSetup(input);
		itsFileFilterHistory->ReadSetup(input);

		if (vers >= 73)
		{
			input >> s;
			itsPathFilterInput->GetText()->SetText(s);

			input >> JBoolFromString(b);

			itsPathFilterHistory->ReadSetup(input);
		}
	}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
SearchTextDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	JXSearchTextDialog::WriteSetup(output);

	// file list

	output << ' ' << JBoolToString(itsOnlyListFilesFlag);
	output << ' ' << JBoolToString(itsListFilesWithoutMatchFlag);
	output << ' ' << itsFileSetName;

	const JPtrArray<JString>& fullNameList = itsFileList->GetFullNameList();

	const JSize fileCount = fullNameList.GetItemCount();
	output << ' ' << fileCount;

	for (JIndex i=1; i<=fileCount; i++)
	{
		output << ' ' << *(fullNameList.GetItem(i));
	}

	// directory

	output << ' ' << JBoolToString(itsRecurseDirCB->IsChecked());
	output << ' ' << JBoolToString(itsInvertFileFilterCB->IsChecked());
	output << ' ' << itsDirInput->GetText()->GetText();
	output << ' ' << itsFileFilterInput->GetText()->GetText();

	output << ' ';
	itsDirHistory->WriteSetup(output);

	output << ' ';
	itsFileFilterHistory->WriteSetup(output);

	output << ' ' << itsPathFilterInput->GetText()->GetText();
	output << ' ' << JBoolToString(false);	// placeholder for invert, if we ever find a use
	output << ' ';
	itsPathFilterHistory->WriteSetup(output);

	output << ' ';
}

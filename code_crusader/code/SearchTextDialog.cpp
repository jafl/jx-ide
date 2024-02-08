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

const JFileVersion kCurrentSetupVersion = 74;	// first digit must be >= 7

	// version 74 no longer stores itsFileSetName and file list
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
	JPrefObject(GetPrefsManager(), kSearchTextID)
{
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
		itsSearchDirCB->SetState(false);
	}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SearchTextDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 460,410, JString::empty);

	auto* searchLabel =
		jnew JXStaticText(JGetString("searchLabel::SearchTextDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,9, 220,20);
	searchLabel->SetToLabel(false);

	auto* findBackButton =
		jnew JXSearchTextButton(false, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,20, 20,20);
	assert( findBackButton != nullptr );

	auto* findLabel =
		jnew JXStaticText(JGetString("findLabel::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,20, 100,20);
	findLabel->SetToLabel(true);

	auto* findFwdButton =
		jnew JXSearchTextButton(true, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,20, 20,20);
	assert( findFwdButton != nullptr );

	auto* prevSearchMenu =
		jnew JXStringHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 245,43, 30,20);
	assert( prevSearchMenu != nullptr );

	auto* replaceButton =
		jnew JXTextButton(JGetString("replaceButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,50, 140,20);
	assert( replaceButton != nullptr );

	auto* replaceFindBackButton =
		jnew JXSearchTextButton(false, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,80, 20,20);
	assert( replaceFindBackButton != nullptr );

	auto* replaceFindLabel =
		jnew JXStaticText(JGetString("replaceFindLabel::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,80, 100,20);
	replaceFindLabel->SetToLabel(true);

	auto* replaceFindFwdButton =
		jnew JXSearchTextButton(true, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,80, 20,20);
	assert( replaceFindFwdButton != nullptr );

	auto* replaceLabel =
		jnew JXStaticText(JGetString("replaceLabel::SearchTextDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,85, 220,20);
	replaceLabel->SetToLabel(false);

	auto* replaceAllButton =
		jnew JXTextButton(JGetString("replaceAllButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,110, 70,20);
	assert( replaceAllButton != nullptr );

	auto* replaceAllInSelButton =
		jnew JXTextButton(JGetString("replaceAllInSelButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 370,110, 70,20);
	assert( replaceAllInSelButton != nullptr );

	auto* prevReplaceMenu =
		jnew JXStringHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 245,118, 30,20);
	assert( prevReplaceMenu != nullptr );

	auto* ignoreCaseCB =
		jnew JXTextCheckbox(JGetString("ignoreCaseCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,170, 130,20);
	assert( ignoreCaseCB != nullptr );

	auto* searchIsRegexCB =
		jnew JXTextCheckbox(JGetString("searchIsRegexCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,170, 150,20);
	assert( searchIsRegexCB != nullptr );

	auto* replaceIsRegexCB =
		jnew JXTextCheckbox(JGetString("replaceIsRegexCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,170, 110,20);
	assert( replaceIsRegexCB != nullptr );

	auto* entireWordCB =
		jnew JXTextCheckbox(JGetString("entireWordCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,200, 130,20);
	assert( entireWordCB != nullptr );

	auto* singleLineCB =
		jnew JXTextCheckbox(JGetString("singleLineCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,200, 150,20);
	assert( singleLineCB != nullptr );

	auto* preserveCaseCB =
		jnew JXTextCheckbox(JGetString("preserveCaseCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,200, 110,20);
	assert( preserveCaseCB != nullptr );

	auto* wrapSearchCB =
		jnew JXTextCheckbox(JGetString("wrapSearchCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,230, 130,20);
	assert( wrapSearchCB != nullptr );

	auto* stayOpenCB =
		jnew JXTextCheckbox(JGetString("stayOpenCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,230, 150,20);
	assert( stayOpenCB != nullptr );

	auto* retainFocusCB =
		jnew JXTextCheckbox(JGetString("retainFocusCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,230, 110,20);
	assert( retainFocusCB != nullptr );

	auto* line =
		jnew JXDownRect(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,255, 420,2);
	line->SetBorderWidth(1);

	itsSearchDirCB =
		jnew JXTextCheckbox(JGetString("itsSearchDirCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,260, 130,20);
	itsSearchDirCB->SetShortcuts(JGetString("itsSearchDirCB::shortcuts::SearchTextDialog::JXLayout"));

	itsDirHistory =
		jnew SearchPathHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,260, 30,20);

	itsChooseDirButton =
		jnew JXTextButton(JGetString("itsChooseDirButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,260, 60,20);

	auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,290, 100,20);
	filterLabel->SetToLabel(false);

	itsFileFilterHistory =
		jnew SearchFilterHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,290, 30,20);

	itsInvertFileFilterCB =
		jnew JXTextCheckbox(JGetString("itsInvertFileFilterCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,290, 60,20);

	itsRecurseDirCB =
		jnew JXTextCheckbox(JGetString("itsRecurseDirCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,320, 215,20);

	itsPathFilterHistory =
		jnew SearchFilterHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,320, 30,20);

	auto* line2 =
		jnew JXDownRect(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,345, 420,2);
	line2->SetBorderWidth(1);

	itsOnlyListFilesCB =
		jnew JXTextCheckbox(JGetString("itsOnlyListFilesCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,350, 140,20);
	itsOnlyListFilesCB->SetShortcuts(JGetString("itsOnlyListFilesCB::shortcuts::SearchTextDialog::JXLayout"));

	itsInvertMatchingCB =
		jnew JXTextCheckbox(JGetString("itsInvertMatchingCB::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,350, 190,20);
	itsInvertMatchingCB->SetShortcuts(JGetString("itsInvertMatchingCB::shortcuts::SearchTextDialog::JXLayout"));

	auto* helpButton =
		jnew JXTextButton(JGetString("helpButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 61,380, 80,20);
	helpButton->SetShortcuts(JGetString("helpButton::shortcuts::SearchTextDialog::JXLayout"));

	auto* qRefButton =
		jnew JXTextButton(JGetString("qRefButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 191,380, 80,20);
	assert( qRefButton != nullptr );

	auto* closeButton =
		jnew JXTextButton(JGetString("closeButton::SearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 321,380, 80,20);
	assert( closeButton != nullptr );

	auto* searchInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,30, 220,45);
	assert( searchInput != nullptr );

	auto* replaceInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,105, 220,45);
	assert( replaceInput != nullptr );

	itsDirInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,260, 190,20);
	itsDirInput->SetIsRequired(false);
	itsDirInput->ShouldAllowInvalidPath(true);
	itsDirInput->ShouldRequireWritable(false);

	itsFileFilterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,290, 190,20);
	itsFileFilterInput->SetFont(JFontManager::GetDefaultMonospaceFont());

	itsPathFilterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 255,320, 155,20);
	itsPathFilterInput->SetFont(JFontManager::GetDefaultMonospaceFont());

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

	ListenTo(itsChooseDirButton);
	ListenTo(itsDirHistory);
	ListenTo(itsFileFilterHistory);
	ListenTo(itsPathFilterHistory);

	const JFont& font = JFontManager::GetDefaultMonospaceFont();

	itsDirHistory->SetDefaultFont(font, true);
	itsFileFilterHistory->SetDefaultFont(font, true);
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
}

/******************************************************************************
 UpdateDisplay (virtual protected)

 ******************************************************************************/

void
SearchTextDialog::UpdateDisplay()
{
	JXSearchTextDialog::UpdateDisplay();

	if (itsSearchDirCB->IsChecked())
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
		itsSearchDirCB->IsChecked())
	{
		SearchFiles();
	}
	else if (sender == GetReplaceAllButton() && message.Is(JXButton::kPushed) &&
			 itsSearchDirCB->IsChecked())
	{
		SearchFilesAndReplace();
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
		SearchDocument::Create(fileList, nameList, itsOnlyListFilesCB->IsChecked(),
							   itsInvertMatchingCB->IsChecked());
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
		bool onlyListFiles;
		input >> JBoolFromString(onlyListFiles);
		itsOnlyListFilesCB->SetState(onlyListFiles);

		if (vers >= 5)
		{
			bool invert;
			input >> JBoolFromString(invert);
			itsInvertMatchingCB->SetState(invert);

			if (vers < 74)
			{
				JString fileSetName;
				input >> fileSetName;
			}
		}

		if (vers < 74)
		{
			JSize fileCount;
			input >> fileCount;

			JString fullName;
			for (JIndex i=1; i<=fileCount; i++)
			{
				input >> fullName;
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

	output << ' ' << JBoolToString(itsOnlyListFilesCB->IsChecked());
	output << ' ' << JBoolToString(itsInvertMatchingCB->IsChecked());

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

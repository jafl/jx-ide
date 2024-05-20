/******************************************************************************
 SearchDocument.cpp

	BASE CLASS = CommandOutputDocument

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "SearchDocument.h"
#include "TextEditor.h"
#include "SearchTextDialog.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXProgressIndicator.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/jFileUtil.h>
#include <thread>
#include <boost/fiber/operations.hpp>
#include <jx-af/jcore/jAssert.h>

const JSize kMenuButtonWidth       = 60;
const JCoordinate kIndicatorHeight = 10;

static const JString kDoubleNewline("\n\n");

/******************************************************************************
 Constructor function (static)

	Search results.

 ******************************************************************************/

void
SearchDocument::Create
	(
	JPtrArray<JString>*	fileList,
	JPtrArray<JString>*	nameList,
	const bool			onlyListFiles,
	const bool			listFilesWithoutMatch
	)
{
	assert( !fileList->IsEmpty() );
	assert( fileList->GetItemCount() == nameList->GetItemCount() );

	const JUtf8Byte* map[] =
	{
		"s", GetSearchTextDialog()->GetSearchText().GetBytes()
	};
	const JString windowTitle = JGetString("SearchTitle::SearchDocument", map, sizeof(map));

	auto* doc =
		jnew SearchDocument(false, onlyListFiles || listFilesWithoutMatch,
							fileList->GetItemCount(), windowTitle);
	doc->Activate();

	std::thread t([fileList, nameList, onlyListFiles, listFilesWithoutMatch, doc]()
	{
		SearchST st;
		st.SearchFiles(*fileList, *nameList, onlyListFiles, listFilesWithoutMatch, doc);
		jdelete fileList;
		jdelete nameList;
	});
	t.detach();

	doc->RecvFromChannel();
}

/******************************************************************************
 Constructor function (static)

	Replace All results.

 ******************************************************************************/

void
SearchDocument::Create
	(
	JPtrArray<JString>*	fileList,
	JPtrArray<JString>*	nameList,
	const JString&		replaceStr
	)
{
	assert( !fileList->IsEmpty() );
	assert( fileList->GetItemCount() == nameList->GetItemCount() );

	const JUtf8Byte* map[] =
	{
		"s", GetSearchTextDialog()->GetSearchText().GetBytes(),
		"r", replaceStr.GetBytes()
	};
	const JString windowTitle = JGetString("ReplaceTitle::SearchDocument", map, sizeof(map));

	auto* doc = jnew SearchDocument(true, true, fileList->GetItemCount(), windowTitle);

	JXGetApplication()->Suspend();	// do this first so result window is active
	doc->Activate();

	std::thread t([fileList, nameList, doc]()
	{
		SearchST st;
		st.SearchFiles(*fileList, *nameList, true, false, doc);
		jdelete fileList;
		jdelete nameList;
	});
	t.detach();

	doc->RecvFromChannel();
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

#include "SearchDocument-Match.h"

SearchDocument::SearchDocument
	(
	const bool		isReplace,
	const bool		onlyListFiles,
	const JSize		fileCount,
	const JString&	windowTitle
	)
	:
	CommandOutputDocument(kSearchOutputFT, "SearchTextHelp-Multifile",
						  "Code_Crusader_Editor_Search_Output",
						  JGetString("NoCloseWhileSearching::SearchDocument")),
	itsIsReplaceFlag(isReplace),
	itsOnlyListFilesFlag(onlyListFiles),
	itsFoundFlag(false),
	itsReplaceST(nullptr),
	itsSearchST(nullptr)
{
	JXWindow* window = GetWindow();

	itsChannel = jnew boost::fibers::buffered_channel<JBroadcaster::Message*>(kJBufferedChannelCapacity);

	// button in upper right

	JXMenuBar* menuBar = GetMenuBar();
	const JRect rect   = window->GetBounds();
	const JSize h      = menuBar->GetFrameHeight();

	itsStopButton =
		jnew JXTextButton(JGetString("StopLabel::ExecOutputDocument"), window,
						  JXWidget::kFixedRight, JXWidget::kFixedTop,
						  rect.right - kMenuButtonWidth,0, kMenuButtonWidth,h);
	ListenTo(itsStopButton);
	itsStopButton->SetShortcuts("^C#.");
	itsStopButton->SetHint(JGetString("StopButtonHint::ExecOutputDocument"));

	ListenTo(itsStopButton, std::function([this](const JXButton::Pushed&)
	{
		if (itsSearchST != nullptr)
		{
			itsSearchST->Cancel();
		}
	}));

	menuBar->AdjustSize(-kMenuButtonWidth, 0);

	// progress indicator

	JXWidget::HSizingOption hSizing;
	JXWidget::VSizingOption vSizing;
	GetFileDisplayInfo(&hSizing, &vSizing);

	itsIndicator =
		jnew JXProgressIndicator(GetWindow(), hSizing, vSizing,
								 -1000, -1000, 500, kIndicatorHeight);
	itsIndicator->SetMaxValue(fileCount);

	itsMatchMenu = InsertTextMenu(JGetString("MenuTitle::SearchDocument_Match"));
	itsMatchMenu->SetMenuItems(kMatchMenuStr);
	itsMatchMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsMatchMenu->Deactivate();
	itsMatchMenu->AttachHandlers(this,
		&SearchDocument::UpdateMatchMenu,
		&SearchDocument::HandleMatchMenu);
	ConfigureMatchMenu(itsMatchMenu);

	// allow Meta-_ to parallel Shift key required for Meta-plus

	JXKeyModifiers modifiers(GetDisplay());
	modifiers.SetState(kJXMetaKeyIndex, true);
	GetWindow()->InstallMenuShortcut(itsMatchMenu, kPrevMatchCmd, '_', modifiers);

	GetDocumentManager()->SetActiveListDocument(this);

	if (itsIsReplaceFlag)
	{
		itsReplaceST = jnew SearchST;
	}

	FileChanged(windowTitle, false);
	UpdateButtons();
	GetTextEditor()->SetWritable(false);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SearchDocument::~SearchDocument()
{
	jdelete itsReplaceST;
	jdelete itsChannel;
}

/******************************************************************************
 PlaceCustomWidgets (virtual protected)

 ******************************************************************************/

void
SearchDocument::PlaceCustomWidgets()
{
	JXWidget::HSizingOption hSizing;
	JXWidget::VSizingOption vSizing;
	const JRect frame = GetFileDisplayInfo(&hSizing, &vSizing);

	itsIndicator->Place(frame.left, frame.ycenter() - kIndicatorHeight/2);
	itsIndicator->SetSize(frame.width(), kIndicatorHeight);

	UpdateButtons();
}

/******************************************************************************
 RecvFromChannel (private)

 ******************************************************************************/

void
SearchDocument::RecvFromChannel()
{
	JBroadcaster::Message* m;
	JSize count = 0;
	while (itsChannel->pop(m) == boost::fibers::channel_op_status::success)
	{
		Receive(nullptr, *m);
		jdelete m;

		count++;
		if (count >= 5)		// avoid getting stuck if channel is flooded
		{
			boost::this_fiber::sleep_for(
				std::chrono::nanoseconds(1));
		}
	}

	// finished

	if (itsIsReplaceFlag)
	{
		jdelete itsReplaceST;
		itsReplaceST = nullptr;

		JXGetApplication()->Resume();
		if (itsFoundFlag)
		{
			GetDocumentManager()->SymbolDatabasesNeedUpdate();
		}
	}

	jdelete itsChannel;
	itsChannel = nullptr;

	jdelete itsIndicator;
	itsIndicator = nullptr;

	UpdateButtons();

	if (!itsFoundFlag)
	{
		AppendText(JGetString("NoMatches::SearchDocument"));
		GetTextEditor()->GetText()->ClearUndo();
	}
	else if (!GetTextEditor()->HasSelection())
	{
		ShowFirstMatch();
	}

	DataReverted();
}

/******************************************************************************
 QueueMessage (protected)

 ******************************************************************************/

void
SearchDocument::QueueMessage
	(
	JBroadcaster::Message* m
	)
{
	itsChannel->push(m);
}

/******************************************************************************
 AppendSearchResult (private)

 ******************************************************************************/

void
SearchDocument::AppendSearchResult
	(
	const SearchST::SearchResult& msg
	)
{
	itsFoundFlag = true;

	auto* te = GetTextEditor();
	auto* st = te->GetText();

	JStyledText::TextIndex start = st->GetBeyondEnd();
	te->SetCaretLocation(start.charIndex);

	// display file name in bold

	te->Paste(msg.GetFileName());
	st->SetFontStyle(JStyledText::TextRange(start, st->GetBeyondEnd()),
					 GetFileNameStyle(), true);

	// line number

	start = st->GetBeyondEnd();
	te->SetCurrentFont(st->GetDefaultFont());

	te->Paste(":");
	te->Paste(JString(msg.GetLineIndex()));
	te->Paste(kDoubleNewline);

	start = st->GetBeyondEnd();

	JStyledText::TextRange r = msg.GetRange();
	r.charRange += start.charIndex - 1;
	r.byteRange += start.byteIndex - 1;

	te->Paste(msg.GetQuotedText());
	te->Paste(kDoubleNewline);

	// underline match

	st->SetFontStyle(r, GetMatchStyle(), true);

	// save text range in case of multiple matches

	itsPrevQuoteIndex = start;

	itsMatchMenu->Activate();
}

/******************************************************************************
 MarkAdditionalMatch (private)

 ******************************************************************************/

void
SearchDocument::MarkAdditionalMatch
	(
	const JStyledText::TextRange& range
	)
{
	itsFoundFlag = true;

	auto* te = GetTextEditor();
	auto* st = te->GetText();

	assert( itsPrevQuoteIndex.charIndex > 1 );

	// underline match

	JStyledText::TextRange r(range);
	r.charRange += itsPrevQuoteIndex.charIndex - 1;
	r.byteRange += itsPrevQuoteIndex.byteIndex - 1;
	st->SetFontStyle(r, GetMatchStyle(), true);

	itsMatchMenu->Activate();
}

/******************************************************************************
 AppendFileName (private)

 ******************************************************************************/

void
SearchDocument::AppendFileName
	(
	const JString& fileName
	)
{
	itsFoundFlag = true;

	AppendText(fileName);

	if (itsIsReplaceFlag)
	{
		ReplaceAll(fileName);
	}
}

/******************************************************************************
 AppendError (private)

 ******************************************************************************/

void
SearchDocument::AppendError
	(
	const JString& text
	)
{
	auto* te = GetTextEditor();
	auto* st = te->GetText();

	const auto start = st->GetBeyondEnd();
	te->SetCaretLocation(start.charIndex);
	te->Paste(text);
	st->SetFontStyle(JStyledText::TextRange(start, st->GetBeyondEnd()),
					 GetErrorStyle(), true);

	te->Paste(itsOnlyListFilesFlag ? JString::newline : kDoubleNewline);
}

/******************************************************************************
 AppendText (private)

	Append the text to our text editor.  Derived classes can override
	to filter or otherwise process the text.

 ******************************************************************************/

void
SearchDocument::AppendText
	(
	const JString& text
	)
{
	TextEditor* te = GetTextEditor();
	te->SetCaretLocation(te->GetText()->GetBeyondEnd().charIndex);
	te->Paste(text);
	te->Paste(JString::newline);
}

/******************************************************************************
 SearchFinished (protected)

 ******************************************************************************/

void
SearchDocument::SearchFinished()
{
	itsSearchST = nullptr;
	itsChannel->close();
}

/******************************************************************************
 ReplaceAll (private)

 ******************************************************************************/

void
SearchDocument::ReplaceAll
	(
	const JString& fileName
	)
{
	JStyledText::PlainTextFormat format;

	JXFileDocument* doc;
	if (GetDocumentManager()->FileDocumentIsOpen(fileName, &doc))
	{
		auto* textDoc = dynamic_cast<TextDocument*>(doc);
		if (textDoc != nullptr)
		{
			textDoc->GetWindow()->Update();

			TextEditor* te = textDoc->GetTextEditor();
			te->SetCaretLocation(1);
			te->ReplaceAll(false);
		}
	}
	else if (JFileReadable(fileName) &&
			 itsReplaceST->ReadPlainText(fileName, &format, false))
	{
		if (itsReplaceST->ReplaceAllForward())
		{
			itsReplaceST->WritePlainText(fileName, format);
		}
	}
}

/******************************************************************************
 OpenPrevListItem (virtual)

	Derived classes which claim to manage a list of files must override this.

 ******************************************************************************/

void
SearchDocument::OpenPrevListItem()
{
	if (ShowPrevMatch())
	{
		GetTextEditor()->OpenSelection();
	}
}

/******************************************************************************
 OpenNextListItem (virtual)

	Derived classes which claim to manage a list of files must override this.

 ******************************************************************************/

void
SearchDocument::OpenNextListItem()
{
	if (ShowNextMatch())
	{
		GetTextEditor()->OpenSelection();
	}
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
SearchDocument::UpdateButtons()
{
	if (CommandRunning())
	{
		itsStopButton->Activate();
		SetFileDisplayVisible(false);
	}
	else
	{
		itsStopButton->Deactivate();
		SetFileDisplayVisible(true);
	}

	GetTextEditor()->Focus();
}

/******************************************************************************
 ConvertSelectionToFullPath (virtual)

	Find the preceding "Entering directory" or "Leaving directory" statement
	printed by make.

 ******************************************************************************/

void
SearchDocument::ConvertSelectionToFullPath
	(
	JString* fileName
	)
	const
{
	GetDocumentManager()->SetActiveListDocument(const_cast<SearchDocument*>(this));

	CommandOutputDocument::ConvertSelectionToFullPath(fileName);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SearchDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(SearchST::kIncrementProgress))
	{
		itsIndicator->IncrementValue();
	}
	else if (message.Is(SearchST::kSearchResult))
	{
		auto& result = dynamic_cast<const SearchST::SearchResult&>(message);
		AppendSearchResult(result);
	}
	else if (message.Is(SearchST::kAdditionalMatch))
	{
		auto& result = dynamic_cast<const SearchST::AdditionalMatch&>(message);
		MarkAdditionalMatch(result.GetRange());
	}
	else if (message.Is(SearchST::kFileName))
	{
		auto& result = dynamic_cast<const SearchST::FileName&>(message);
		AppendFileName(result.GetFileName());
	}
	else if (message.Is(SearchST::kError))
	{
		auto& result = dynamic_cast<const SearchST::Error&>(message);
		AppendError(result.GetMessage());
	}

	else
	{
		CommandOutputDocument::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateMatchMenu (private)

 ******************************************************************************/

void
SearchDocument::UpdateMatchMenu()
{
	itsMatchMenu->SetItemEnabled(kOpenFileCmd, GetTextEditor()->HasSelection());
}

/******************************************************************************
 HandleMatchMenu (private)

 ******************************************************************************/

void
SearchDocument::HandleMatchMenu
	(
	const JIndex index
	)
{
	GetDocumentManager()->SetActiveListDocument(this);

	if (index == kFirstMatchCmd)
	{
		ShowFirstMatch();
	}

	else if (index == kPrevMatchCmd)
	{
		ShowPrevMatch();
	}
	else if (index == kNextMatchCmd)
	{
		ShowNextMatch();
	}

	else if (index == kOpenFileCmd)
	{
		GetTextEditor()->OpenSelection();
	}
}

/******************************************************************************
 ShowFirstMatch (private)

 ******************************************************************************/

void
SearchDocument::ShowFirstMatch()
{
	GetTextEditor()->SetCaretLocation(1);
	ShowNextMatch();
}

/******************************************************************************
 ShowPrevMatch (private)

 ******************************************************************************/

bool
jMatchFileName
	(
	const JFont& font
	)
{
	return font.GetStyle().bold;
}


bool
SearchDocument::ShowPrevMatch()
{
	TextEditor* te = GetTextEditor();
	te->Focus();

	bool wrapped;
	if (te->SearchBackward(jMatchFileName, false, &wrapped))
	{
		te->TEScrollToSelection(true);
		return true;
	}
	else
	{
		GetDisplay()->Beep();
		return false;
	}
}

/******************************************************************************
 ShowNextMatch (private)

 ******************************************************************************/

bool
SearchDocument::ShowNextMatch()
{
	TextEditor* te = GetTextEditor();
	te->Focus();

	bool wrapped;
	if (te->SearchForward(jMatchFileName, false, &wrapped))
	{
		te->TEScrollToSelection(true);
		return true;
	}
	else
	{
		GetDisplay()->Beep();
		return false;
	}
}

/******************************************************************************
 GetFileNameStyle (private)

 ******************************************************************************/

JFontStyle
SearchDocument::GetFileNameStyle()
	const
{
	JFont font = GetTextEditor()->GetText()->GetDefaultFont();
	font.SetBold(true);
	return font.GetStyle();
}

/******************************************************************************
 GetMatchStyle (private)

 ******************************************************************************/

JFontStyle
SearchDocument::GetMatchStyle()
	const
{
	JFont font = GetTextEditor()->GetText()->GetDefaultFont();
	font.SetUnderlineCount(1);
	return font.GetStyle();
}

/******************************************************************************
 GetErrorStyle (private)

	Including bold allows next/prev will highlight it, but this can swamp
	out the useful matches.

 ******************************************************************************/

JFontStyle
SearchDocument::GetErrorStyle()
	const
{
	JFont font = GetTextEditor()->GetText()->GetDefaultFont();
//	font.SetBold(!itsOnlyListFilesFlag);
	font.SetColor(JColorManager::GetDarkRedColor());
	return font.GetStyle();
}

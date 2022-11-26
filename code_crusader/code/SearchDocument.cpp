/******************************************************************************
 SearchDocument.cpp

	BASE CLASS = ExecOutputDocument

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "SearchDocument.h"
#include "TextEditor.h"
#include "SearchTE.h"
#include "SearchTextDialog.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXProgressIndicator.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JThisProcess.h>
#include <jx-af/jcore/JOutPipeStream.h>
#include <jx-af/jcore/JMemoryManager.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jSysUtil.h>
#include <sstream>
#include <stdlib.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kIndicatorHeight = 10;

// Match menu

static const JUtf8Byte* kMatchMenuStr =
	"    First match             %k Ctrl-1"
	"%l| Previous match          %k Meta-minus"		// and Meta-_
	"  | Next match              %k Meta-plus"
	"%l| Open selection to match %k Meta-Shift-O";

enum
{
	kFirstMatchCmd = 1,
	kPrevMatchCmd, kNextMatchCmd,
	kOpenFileCmd
};

/******************************************************************************
 Constructor function (static)

	Search results.

 ******************************************************************************/

JError
SearchDocument::Create
	(
	const JPtrArray<JString>&	fileList,
	const JPtrArray<JString>&	nameList,
	const JRegex&				searchRegex,
	const bool				onlyListFiles,
	const bool				listFilesWithoutMatch
	)
{
	assert( !fileList.IsEmpty() );
	assert( fileList.GetElementCount() == nameList.GetElementCount() );

	int fd[2];
	JError err = JCreatePipe(fd);
	if (!err.OK())
	{
		return err;
	}

	pid_t pid;
	err = JThisProcess::Fork(&pid);
	if (!err.OK())
	{
		return err;
	}

	// child

	else if (pid == 0)
	{
		close(fd[0]);
		JMemoryManager::Instance()->SetPrintExitStats(false);

		// get rid of JXCreatePG, since we must not use X connection
		// (binary files may trigger it)
		JInitCore();

		SearchTE te;
		JOutPipeStream output(fd[1], true);
		te.SearchFiles(fileList, nameList,
					   onlyListFiles, listFilesWithoutMatch,
					   output);
		output.close();
		exit(0);
	}

	// parent

	else
	{
		close(fd[1]);

		auto* process = jnew JProcess(pid);
		assert( process != nullptr );

		const JUtf8Byte* map[] =
		{
			"s", GetSearchTextDialog()->GetSearchText().GetBytes()
		};
		const JString windowTitle = JGetString("SearchTitle::SearchDocument", map, sizeof(map));

		auto* doc =
			jnew SearchDocument(false, onlyListFiles || listFilesWithoutMatch,
								 fileList.GetElementCount(),
								 process, fd[0], windowTitle);
		assert( doc != nullptr );
		doc->Activate();

		RecordLink* link;
		const bool ok = doc->GetRecordLink(&link);
		assert( ok );
		SearchTE::SetProtocol(link);
	}

	return JNoError();
}

/******************************************************************************
 Constructor function (static)

	Replace All results.

 ******************************************************************************/

JError
SearchDocument::Create
	(
	const JPtrArray<JString>&	fileList,
	const JPtrArray<JString>&	nameList,
	const JRegex&				searchRegex,
	const JString&				replaceStr
	)
{
	assert( !fileList.IsEmpty() );
	assert( fileList.GetElementCount() == nameList.GetElementCount() );

	int fd[2];
	JError err = JCreatePipe(fd);
	if (!err.OK())
	{
		return err;
	}

	pid_t pid;
	err = JThisProcess::Fork(&pid);
	if (!err.OK())
	{
		return err;
	}

	// child

	else if (pid == 0)
	{
		close(fd[0]);

		// get rid of JXCreatePG, since we must not use X connection
		// (binary files may trigger it)
		JInitCore();

		SearchTE te;
		JOutPipeStream output(fd[1], true);
		te.SearchFiles(fileList, nameList, true, false, output);
		output.close();
		exit(0);
	}

	// parent

	else
	{
		close(fd[1]);

		auto* process = jnew JProcess(pid);
		assert( process != nullptr );

		const JUtf8Byte* map[] =
		{
			"s", GetSearchTextDialog()->GetSearchText().GetBytes(),
			"r", replaceStr.GetBytes()
		};
		const JString windowTitle = JGetString("ReplaceTitle::SearchDocument", map, sizeof(map));

		auto* doc =
			jnew SearchDocument(true, true, fileList.GetElementCount(),
								 process, fd[0], windowTitle);
		assert( doc != nullptr );

		JXGetApplication()->Suspend();	// do this first so result window is active
		doc->Activate();

		RecordLink* link;
		const bool ok = doc->GetRecordLink(&link);
		assert( ok );
		SearchTE::SetProtocol(link);
	}

	return JNoError();
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

SearchDocument::SearchDocument
	(
	const bool		isReplace,
	const bool		onlyListFiles,
	const JSize			fileCount,
	JProcess*			p,
	const int			fd,
	const JString&		windowTitle
	)
	:
	ExecOutputDocument(kSearchOutputFT, "SearchTextHelp-Multifile", false, false),
	itsIsReplaceFlag(isReplace),
	itsOnlyListFilesFlag(onlyListFiles),
	itsReplaceTE(nullptr)
{
	itsFoundFlag = false;

	JXWidget::HSizingOption hSizing;
	JXWidget::VSizingOption vSizing;
	GetFileDisplayInfo(&hSizing, &vSizing);

	itsIndicator =
		jnew JXProgressIndicator(GetWindow(), hSizing, vSizing,
								 -1000, -1000, 500, kIndicatorHeight);
	assert( itsIndicator != nullptr );
	itsIndicator->SetMaxValue(fileCount);

	itsMatchMenu = InsertTextMenu(JGetString("MatchMenuTitle::SearchDocument"));
	itsMatchMenu->SetMenuItems(kMatchMenuStr, "SearchDocument");
	itsMatchMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsMatchMenu->Deactivate();
	ListenTo(itsMatchMenu);

	// allow Meta-_ to parallel Shift key required for Meta-plus

	JXKeyModifiers modifiers(GetDisplay());
	modifiers.SetState(kJXMetaKeyIndex, true);
	GetWindow()->InstallMenuShortcut(itsMatchMenu, kPrevMatchCmd, '_', modifiers);

	GetWindow()->SetWMClass(GetWMClassInstance(), GetSearchOutputWindowClass());

	SetConnection(p, fd, ACE_INVALID_HANDLE,
				  windowTitle, JGetString("NoCloseWhileSearching::SearchDocument"),
				  JString("/", JString::kNoCopy), windowTitle, false);

	GetDocumentManager()->SetActiveListDocument(this);

	if (itsIsReplaceFlag)
	{
		itsReplaceTE = jnew SearchTE;
		assert( itsReplaceTE != nullptr );
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SearchDocument::~SearchDocument()
{
	jdelete itsReplaceTE;
}

/******************************************************************************
 PlaceCmdLineWidgets (virtual protected)

 ******************************************************************************/

void
SearchDocument::PlaceCmdLineWidgets()
{
	ExecOutputDocument::PlaceCmdLineWidgets();

	JXWidget::HSizingOption hSizing;
	JXWidget::VSizingOption vSizing;
	const JRect frame = GetFileDisplayInfo(&hSizing, &vSizing);

	itsIndicator->Place(frame.left, frame.ycenter() - kIndicatorHeight/2);
	itsIndicator->SetSize(frame.width(), kIndicatorHeight);
}

/******************************************************************************
 NeedsFormattedData (virtual protected)

 ******************************************************************************/

bool
SearchDocument::NeedsFormattedData()
	const
{
	return true;
}

/******************************************************************************
 ProcessFinished (virtual protected)

 ******************************************************************************/

bool
SearchDocument::ProcessFinished
	(
	const JProcess::Finished& info
	)
{
	if (!ExecOutputDocument::ProcessFinished(info))
	{
		return false;
	}

	if (itsIsReplaceFlag)
	{
		jdelete itsReplaceTE;
		itsReplaceTE = nullptr;

		JXGetApplication()->Resume();
	}

	jdelete itsIndicator;
	itsIndicator = nullptr;

	SetFileDisplayVisible(true);

	if (!itsFoundFlag)
	{
		ExecOutputDocument::AppendText(JGetString("NoMatches::SearchDocument"));
		DataReverted();
		GetTextEditor()->GetText()->ClearUndo();
	}
	else if (!GetTextEditor()->HasSelection())
	{
		ShowFirstMatch();
	}

	return true;
}

/******************************************************************************
 AppendText (virtual protected)

 ******************************************************************************/

static const JString kDoubleNewline("\n\n", JString::kNoCopy);

void
SearchDocument::AppendText
	(
	const JString& text
	)
{
	if (text.IsEmpty())
	{
		return;
	}
	else if (text.GetFirstCharacter() == SearchTE::kIncrementProgress)
	{
		itsIndicator->IncrementValue();
		return;
	}

	TextEditor* te = GetTextEditor();

	itsFoundFlag = true;
	const std::string s(text.GetBytes(), text.GetByteCount());
	std::istringstream input(s);

	JStyledText* st = te->GetText();

	if (text.GetFirstCharacter() == SearchTE::kError)
	{
		input.ignore();
		JString msg;
		JReadAll(input, &msg);

		const JStyledText::TextIndex start = st->GetBeyondEnd();
		te->SetCaretLocation(start.charIndex);
		te->Paste(msg);
		st->SetFontStyle(JStyledText::TextRange(start, st->GetBeyondEnd()),
						 GetErrorStyle(), true);

		te->Paste(itsOnlyListFilesFlag ? JString::newline : kDoubleNewline);
	}
	else if (itsOnlyListFilesFlag)
	{
		JString fileName;
		input >> fileName;
		ExecOutputDocument::AppendText(fileName);

		if (itsIsReplaceFlag)
		{
			ReplaceAll(fileName);
		}
	}
	else
	{
		JUtf8Byte mode;
		input.get(mode);

		if (mode == SearchTE::kNewMatchLine)
		{
			JString fileName;
			JUInt64 lineIndex;
			JString text1;
			JCharacterRange matchCharRange;
			JUtf8ByteRange matchByteRange;
			input >> fileName >> lineIndex >> text1 >> matchCharRange >> matchByteRange;

			JStyledText::TextIndex start = st->GetBeyondEnd();
			te->SetCaretLocation(start.charIndex);

			// display file name in bold

			te->Paste(fileName);
			st->SetFontStyle(JStyledText::TextRange(start, st->GetBeyondEnd()),
							 GetFileNameStyle(), true);

			// line number

			start = st->GetBeyondEnd();
			te->SetCurrentFont(st->GetDefaultFont());

			te->Paste(JString(":", JString::kNoCopy));
			te->Paste(JString(lineIndex));
			te->Paste(kDoubleNewline);

			start           = st->GetBeyondEnd();
			matchCharRange += start.charIndex - 1;
			matchByteRange += start.byteIndex - 1;

			te->Paste(text1);
			te->Paste(kDoubleNewline);

			// underline match

			st->SetFontStyle(JStyledText::TextRange(matchCharRange, matchByteRange),
							 GetMatchStyle(), true);

			// save text range in case of multiple matches

			itsPrevQuoteIndex = start;
		}
		else
		{
			assert( mode == SearchTE::kRepeatMatchLine &&
					itsPrevQuoteIndex.charIndex > 1 );

			JCharacterRange matchCharRange;
			JUtf8ByteRange matchByteRange;
			input >> matchCharRange >> matchByteRange;

			// underline match

			matchCharRange += itsPrevQuoteIndex.charIndex - 1;
			matchByteRange += itsPrevQuoteIndex.byteIndex - 1;
			st->SetFontStyle(JStyledText::TextRange(matchCharRange, matchByteRange),
							 GetMatchStyle(), true);
		}

		itsMatchMenu->Activate();
	}
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
			(textDoc->GetWindow())->Update();

			TextEditor* te = textDoc->GetTextEditor();
			te->SetCaretLocation(1);
			te->ReplaceAll(false);
		}
	}
	else if (JFileReadable(fileName) &&
			 itsReplaceTE->GetText()->ReadPlainText(fileName, &format, false))
	{
		itsReplaceTE->SetCaretLocation(1);
		if (itsReplaceTE->ReplaceAllForward())
		{
			itsReplaceTE->GetText()->WritePlainText(fileName, format);
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

	ExecOutputDocument::ConvertSelectionToFullPath(fileName);
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
	if (sender == itsMatchMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateMatchMenu();
	}
	else if (sender == itsMatchMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleMatchMenu(selection->GetIndex());
	}

	else
	{
		ExecOutputDocument::Receive(sender, message);
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
	if (te->JTextEditor::SearchBackward(jMatchFileName, false, &wrapped))
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
	if (te->JTextEditor::SearchForward(jMatchFileName, false, &wrapped))
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

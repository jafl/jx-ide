/******************************************************************************
 CompileDocument.cpp

	BASE CLASS = ExecOutputDocument

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#include "CompileDocument.h"
#include "TextEditor.h"
#include "CommandMenu.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JStack.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JRegex dirMarkerPattern = "((Entering)|(Leaving)) directory `";
static const JRegex endMarkerPattern = "['`]";

// static data

bool CompileDocument::theDoubleSpaceFlag = true;

// Error menu

static const JUtf8Byte* kErrorMenuStr =
	"    First error             %k Ctrl-1"
	"%l| Previous error          %k Meta-minus"		// and Meta-_
	"  | Next error              %k Meta-plus"
	"%l| Open selection to error %k Meta-Shift-O";

enum
{
	kFirstErrorCmd = 1,
	kPrevErrorCmd, kNextErrorCmd,
	kOpenFileCmd
};

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CompileDocument::CompileDocument
	(
	ProjectDocument* projDoc
	)
	:
	ExecOutputDocument(kExecOutputFT, "CompileHelp", false),
	itsHasErrorsFlag(false)
{
	GetWindow()->SetWMClass(GetWMClassInstance(), GetCompileOutputWindowClass());

	itsErrorMenu = InsertTextMenu(JGetString("ErrorMenuTitle::CompileDocument"));
	itsErrorMenu->SetMenuItems(kErrorMenuStr, "CompileDocument");
	itsErrorMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsErrorMenu->AttachHandlers(this,
		&CompileDocument::UpdateErrorMenu,
		&CompileDocument::HandleErrorMenu);

	GetCommandMenu()->SetProjectDocument(projDoc);

	// allow Meta-_ to parallel Shift key required for Meta-plus

	JXKeyModifiers modifiers(GetDisplay());
	modifiers.SetState(kJXMetaKeyIndex, true);
	GetWindow()->InstallMenuShortcut(itsErrorMenu, kPrevErrorCmd, '_', modifiers);

	Activate();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CompileDocument::~CompileDocument()
{
}

/******************************************************************************
 NeedsFormattedData (virtual protected)

 ******************************************************************************/

bool
CompileDocument::NeedsFormattedData()
	const
{
	return true;
}

/******************************************************************************
 SetConnection (virtual)

 ******************************************************************************/

void
CompileDocument::SetConnection
	(
	JProcess*		p,
	const int		inFD,
	const int		outFD,
	const JString&	windowTitle,
	const JString&	dontCloseMsg,
	const JString&	execDir,
	const JString&	execCmd,
	const bool	showPID
	)
{
	GetDocumentManager()->SetActiveListDocument(this);

	itsPrevLine.Clear();

	if (ShouldClearWhenStart())
	{
		itsHasErrorsFlag = false;
	}
	if (!itsHasErrorsFlag)
	{
		itsErrorMenu->Deactivate();
	}

	ExecOutputDocument::SetConnection(p, inFD, outFD, windowTitle,
										dontCloseMsg, execDir, execCmd, false);

	if (!execCmd.IsEmpty())
	{
		TextEditor* te = GetTextEditor();

		const JUtf8Byte* map[] =
		{
			"d", execDir.GetBytes()
		};
		te->Paste(JGetString("ChangeDirectory::CompileDocument", map, sizeof(map)));
		te->Paste(JString("\n\n", JString::kNoCopy));
		te->GetText()->ClearUndo();
	}
}

/******************************************************************************
 ProcessFinished (virtual protected)

	Jump to the first error.

 ******************************************************************************/

bool
CompileDocument::ProcessFinished
	(
	const JProcess::Finished& info
	)
{
	if (!ExecOutputDocument::ProcessFinished(info))
	{
		return false;
	}

	if (!GetTextEditor()->HasSelection())
	{
		ShowFirstError();
	}

	return true;
}

/******************************************************************************
 AppendText (virtual protected)

	Double space all the output and display each error message on a single line.

	The dots at the front of the regexes guarantee that the file name is
	not empty.

	gcc, g++, g77:  $:#:
	bison3:         $:#.#-#:
	cc, CC, flex:   "$", line #:
	bison:          ("$", line #) error:
	make:           make[#]: *** ... (ignored)
	Absoft F77:      error on line # of $:
	javac:           [javac] $:#:
	maven2:         $:[#,...]
	maven3:         $:#:#::

 ******************************************************************************/

static const JRegex gccErrorRegex    = "(?<=.):[0-9]+(\\.[0-9]+-[0-9]+)?(:[0-9]+)?: (?:(?:fatal )?error|warning):";
static const JRegex flexErrorRegex   = "(?<=..)\", line [0-9]+: ";
static const JRegex bisonErrorRegex  = "(?<=...)\", line [0-9]+\\) error: ";
static const JRegex makeErrorRegex   = ".(\\[[0-9]+\\])?: \\*\\*\\*";
static const JRegex absoftErrorRegex = " error on line [0-9]+ of ([^:]+): ";
static const JRegex javacOutputRegex = "^\\s+\\[.+?\\]\\s+";
static const JRegex javacErrorRegex  = "^\\s+\\[.+?\\]\\s+(.+?):[0-9]+: ";
static const JRegex maven2ErrorRegex = "^(?:\\[.+?\\]\\s+)?(.+?):\\[[0-9]+,[0-9]+\\] ";
static const JRegex maven3ErrorRegex = "^(?:\\[.+?\\]\\s+)?(.+?):[0-9]+:[0-9]+::";

static const JUtf8Byte* makeIgnoreErrorStr = "(ignored)";
static const JUtf8Byte* gccMultilinePrefix = "   ";
const JSize kGCCMultilinePrefixLength      = strlen(gccMultilinePrefix);

void
setErrorRangeFromMatchStart
	(
	const JStringMatch&		m,
	JStyledText::TextRange*	r
	)
{
	r->SetCount(
		JStyledText::TextCount(
			m.GetCharacterRange().first-1,
			m.GetUtf8ByteRange().first-1));
}

JStyledText::TextRange
computeErrorRangeFromFirstSubmatch
	(
	const JStyledText::TextIndex	i,
	const JStringMatch&				m
	)
{
	JCharacterRange cr = m.GetCharacterRange(1);
	JUtf8ByteRange br  = m.GetUtf8ByteRange(1);

	cr += i.charIndex-1;
	br += i.byteIndex-1;

	return JStyledText::TextRange(cr, br);
}

void
CompileDocument::AppendText
	(
	const JString& origText
	)
{
	JString text = origText;

	JStringIterator iter(&text, JStringIterator::kStartAtEnd);
	JUtf8Character c;
	if (iter.Prev(&c) && c == '\r')		// convert DOS \r\n
	{
		iter.RemoveNext();
	}

	if (iter.Prev("\r"))				// in console, this means "overwrite"
	{
		iter.SkipNext();
		iter.RemoveAllPrev();
	}
	iter.Invalidate();

	const bool isJavacError = javacOutputRegex.Match(text);

	const JStringMatch gccMatch         = gccErrorRegex.Match(text, JRegex::kIgnoreSubmatches),
					   gccPrevLineMatch = gccErrorRegex.Match(itsPrevLine, JRegex::kIgnoreSubmatches);

	const bool isGCCError = !isJavacError && !gccMatch.IsEmpty();

	const JStringMatch flexMatch = flexErrorRegex.Match(text, JRegex::kIgnoreSubmatches);
	const bool isFlexError       = !flexMatch.IsEmpty();

	const JStringMatch bisonMatch = bisonErrorRegex.Match(text, JRegex::kIgnoreSubmatches);
	const bool isBisonError       = !bisonMatch.IsEmpty();

	const JStringMatch makeMatch = makeErrorRegex.Match(text, JRegex::kIgnoreSubmatches);
	const bool isMakeError       = !makeMatch.IsEmpty() && !text.EndsWith(makeIgnoreErrorStr);

	const JStringMatch absoftMatch = absoftErrorRegex.Match(text, JRegex::kIncludeSubmatches);
	const bool isAbsoftError       = !absoftMatch.IsEmpty();

	const JStringMatch maven2Match = maven2ErrorRegex.Match(text, JRegex::kIncludeSubmatches);
	const bool isMaven2Error       = !maven2Match.IsEmpty();

	const JStringMatch maven3Match = maven3ErrorRegex.Match(text, JRegex::kIncludeSubmatches);
	const bool isMaven3Error       = !maven3Match.IsEmpty();

	TextEditor* te = GetTextEditor();

	if (!isJavacError && !isGCCError && !gccPrevLineMatch.IsEmpty() &&
		text.StartsWith(gccMultilinePrefix) &&
		text.GetByteCount() > kGCCMultilinePrefixLength)
	{
		JStringIterator iter(&text, JStringIterator::kStartAfterByte, kGCCMultilinePrefixLength);
		if (iter.Next(&c, JStringIterator::kStay) && !c.IsSpace())
		{
			iter.RemoveAllPrev();
			iter.Invalidate();

			te->SetCaretLocation(te->GetText()->GetText().GetCharacterCount() - (theDoubleSpaceFlag ? 1 : 0));
			te->Paste(text);
			return;
		}
	}

	const JStyledText::TextIndex startIndex = te->GetText()->GetBeyondEnd();

	ExecOutputDocument::AppendText(text);
	if (theDoubleSpaceFlag)
	{
		te->Paste(JString::newline);
	}

	itsPrevLine = text;

	// display file name in bold and activate Errors menu

	JStyledText::TextRange boldRange(startIndex, JStyledText::TextCount());
	if (isJavacError)
	{
		const JStringMatch javacMatch = javacErrorRegex.Match(text, JRegex::kIncludeSubmatches);
		if (!javacMatch.IsEmpty())
		{
			boldRange = computeErrorRangeFromFirstSubmatch(startIndex, javacMatch);
		}
	}
	else if (isGCCError)
	{
		setErrorRangeFromMatchStart(gccMatch, &boldRange);
	}
	else if (isFlexError)
	{
		setErrorRangeFromMatchStart(flexMatch, &boldRange);
	}
	else if (isBisonError)
	{
		setErrorRangeFromMatchStart(bisonMatch, &boldRange);
	}
	else if (isMakeError)
	{
		boldRange.SetCount(
			JStyledText::TextCount(
				text.GetCharacterCount(),
				text.GetByteCount()));
	}
	else if (isAbsoftError)
	{
		boldRange = computeErrorRangeFromFirstSubmatch(startIndex, absoftMatch);
	}
	else if (isMaven2Error)
	{
		boldRange = computeErrorRangeFromFirstSubmatch(startIndex, maven2Match);
	}
	else if (isMaven3Error)
	{
		boldRange = computeErrorRangeFromFirstSubmatch(startIndex, maven3Match);
	}

	if (!boldRange.IsEmpty())
	{
		te->GetText()->SetFontBold(boldRange, true, true);

		if (!itsHasErrorsFlag)
		{
			itsHasErrorsFlag = true;
			itsErrorMenu->Activate();

			JXWindow* window    = GetWindow();
			JString windowTitle = window->GetTitle();

			JStringIterator iter(&windowTitle);
			c = '!';
			iter.SetNext(c);
			iter.SetNext(c);
			iter.SetNext(c);
			iter.Invalidate();

			window->SetTitle(windowTitle);
		}
	}
}

/******************************************************************************
 OpenPrevListItem (virtual)

	Derived classes which claim to manage a list of files must override this.

 ******************************************************************************/

void
CompileDocument::OpenPrevListItem()
{
	TextEditor* te = GetTextEditor();
	JString s;
	while (ShowPrevError())
	{
		const bool ok = te->GetSelection(&s);
		assert( ok );
		if (makeErrorRegex.Match(s))
		{
			continue;
		}

		te->OpenSelection();
		break;
	}
}

/******************************************************************************
 OpenNextListItem (virtual)

	Derived classes which claim to manage a list of files must override this.

 ******************************************************************************/

void
CompileDocument::OpenNextListItem()
{
	TextEditor* te = GetTextEditor();
	JString s;
	while (ShowNextError())
	{
		const bool ok = te->GetSelection(&s);
		assert( ok );
		if (makeErrorRegex.Match(s))
		{
			continue;
		}

		te->OpenSelection();
		break;
	}
}

/******************************************************************************
 ConvertSelectionToFullPath (virtual)

	Find the preceding "Entering directory" or "Leaving directory" statement
	printed by make.

 ******************************************************************************/

void
CompileDocument::ConvertSelectionToFullPath
	(
	JString* fileName
	)
	const
{
	GetDocumentManager()->SetActiveListDocument(const_cast<CompileDocument*>(this));

	if (JIsAbsolutePath(*fileName))
	{
		return;
	}

	TextEditor* te       = GetTextEditor();
	const JIndex caretChar = te->GetInsertionCharIndex();

	JStack<JIndex, JArray<JIndex> > dirStack;

	JStringIterator iter(te->GetText()->GetText());
	JIndex i;
	while (iter.Next(dirMarkerPattern) &&
		   iter.GetNextCharacterIndex(&i) && i < caretChar)
	{
		const JStringMatch& m = iter.GetLastMatch();
		if (!m.GetCharacterRange(2).IsEmpty())					// Entering
		{
			dirStack.Push(i);
		}
		else													// Leaving
		{
			assert( !m.GetCharacterRange(3).IsEmpty() );
			dirStack.Pop(&i);
		}
	}

	if (dirStack.Peek(&i))
	{
		iter.MoveTo(JStringIterator::kStartBeforeChar, i);
		iter.BeginMatch();
		JUtf8Character c;
		if (iter.Next(&c) && c != '`' && c != '\'' && iter.Next(endMarkerPattern))
		{
			const JStringMatch& m  = iter.FinishMatch();
			const JString testName = JCombinePathAndName(m.GetString(), *fileName);
			if (JFileExists(testName))
			{
				*fileName = testName;
				return;
			}
		}
	}

	iter.Invalidate();

	ExecOutputDocument::ConvertSelectionToFullPath(fileName);
}

/******************************************************************************
 UpdateErrorMenu (private)

 ******************************************************************************/

void
CompileDocument::UpdateErrorMenu()
{
	itsErrorMenu->SetItemEnabled(kOpenFileCmd, GetTextEditor()->HasSelection());
}

/******************************************************************************
 HandleErrorMenu (private)

 ******************************************************************************/

void
CompileDocument::HandleErrorMenu
	(
	const JIndex index
	)
{
	GetDocumentManager()->SetActiveListDocument(this);

	if (index == kFirstErrorCmd)
	{
		ShowFirstError();
	}

	else if (index == kPrevErrorCmd)
	{
		ShowPrevError();
	}
	else if (index == kNextErrorCmd)
	{
		ShowNextError();
	}

	else if (index == kOpenFileCmd)
	{
		GetTextEditor()->OpenSelection();
	}
}

/******************************************************************************
 ShowFirstError (private)

 ******************************************************************************/

void
CompileDocument::ShowFirstError()
{
	if (itsHasErrorsFlag)
	{
		GetTextEditor()->SetCaretLocation(1);
		ShowNextError();
	}
}

/******************************************************************************
 ShowPrevError (private)

 ******************************************************************************/

bool
jMatchErrorStyle
	(
	const JFont& font
	)
{
	return font.GetStyle().bold;
}

bool
CompileDocument::ShowPrevError()
{
	TextEditor* te = GetTextEditor();
	te->Focus();

	bool wrapped;
	if (te->JTextEditor::SearchBackward(jMatchErrorStyle, false, &wrapped))
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
 ShowNextError (private)

 ******************************************************************************/

bool
CompileDocument::ShowNextError()
{
	TextEditor* te = GetTextEditor();
	te->Focus();

	bool wrapped;
	if (te->JTextEditor::SearchForward(jMatchErrorStyle, false, &wrapped))
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

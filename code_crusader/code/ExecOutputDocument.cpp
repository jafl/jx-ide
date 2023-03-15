/******************************************************************************
 ExecOutputDocument.cpp

	BASE CLASS = CommandOutputDocument

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#include "ExecOutputDocument.h"
#include "TextEditor.h"
#include "CmdLineInput.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/JOutPipeStream.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JThisProcess.h>
#include <jx-af/jcore/jTextUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jSignal.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

const JSize kMenuButtonWidth = 60;

/******************************************************************************
 Constructor

 ******************************************************************************/

ExecOutputDocument::ExecOutputDocument
	(
	const TextFileType	fileType,
	const JUtf8Byte*	helpSectionName,
	const bool			focusToCmd
	)
	:
	CommandOutputDocument(fileType, helpSectionName, JGetString("WaitCloseMsg::ExecOutputDocument")),
	itsProcess(nullptr),
	itsReceivedDataFlag(false),
	itsProcessPausedFlag(false),
	itsClearWhenStartFlag(true),
	itsRecordLink(nullptr),
	itsDataLink(nullptr),
	itsCmdStream(nullptr),
	itsFocusToCmdFlag(focusToCmd)
{
	JXWindow* window = GetWindow();
	window->SetWMClass(GetWMClassInstance(), GetExecOutputWindowClass());

	// buttons in upper right

	JXMenuBar* menuBar = GetMenuBar();
	const JRect rect   = window->GetBounds();
	const JSize h      = menuBar->GetFrameHeight();

	itsPauseButton =
		jnew JXTextButton(JGetString("PauseLabel::ExecOutputDocument"), window,
						  JXWidget::kFixedRight, JXWidget::kFixedTop,
						  rect.right - 3*kMenuButtonWidth,0, kMenuButtonWidth,h);
	assert( itsPauseButton != nullptr );
	ListenTo(itsPauseButton);
	itsPauseButton->SetShortcuts(JString("^Z", JString::kNoCopy));
	itsPauseButton->SetHint(JGetString("PauseButtonHint::ExecOutputDocument"));

	itsStopButton =
		jnew JXTextButton(JGetString("StopLabel::ExecOutputDocument"), window,
						  JXWidget::kFixedRight, JXWidget::kFixedTop,
						  rect.right - 2*kMenuButtonWidth,0, kMenuButtonWidth,h);
	assert( itsStopButton != nullptr );
	ListenTo(itsStopButton);
	itsStopButton->SetShortcuts(JString("^C#.", JString::kNoCopy));
	itsStopButton->SetHint(JGetString("StopButtonHint::ExecOutputDocument"));

	itsKillButton =
		jnew JXTextButton(JGetString("KillLabel::ExecOutputDocument"), window,
						  JXWidget::kFixedRight, JXWidget::kFixedTop,
						  rect.right - kMenuButtonWidth,0, kMenuButtonWidth,h);
	assert( itsKillButton != nullptr );
	ListenTo(itsKillButton);

	menuBar->AdjustSize(-3*kMenuButtonWidth, 0);

	// command line input

	JXWidget::HSizingOption hSizing;
	JXWidget::VSizingOption vSizing;
	GetFileDisplayInfo(&hSizing, &vSizing);

	itsCmdPrompt =
		jnew JXStaticText(JGetString("CmdPrompt::ExecOutputDocument"), window,
						  JXWidget::kFixedLeft, vSizing,
						  -1000, -1000, 0, 500);
	assert( itsCmdPrompt != nullptr );
	itsCmdPrompt->SetToLabel();
	itsCmdPrompt->Hide();

	itsCmdInput =
		jnew CmdLineInput(this, window, hSizing, vSizing,
							-1000, -1000, 500, 500);
	assert( itsCmdInput != nullptr );
	itsCmdInput->ShareEditMenu(GetTextEditor());
	itsCmdInput->Hide();

	itsEOFButton =
		jnew JXTextButton(JGetString("EOFButtonTitle::ExecOutputDocument"), window,
						  JXWidget::kFixedRight, vSizing,
						  -1000, -1000, 500, 500);
	assert( itsEOFButton != nullptr );
	itsEOFButton->SetShortcuts(JString("^D", JString::kNoCopy));
	itsEOFButton->Hide();
	ListenTo(itsEOFButton);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ExecOutputDocument::~ExecOutputDocument()
{
	delete itsRecordLink;
	itsRecordLink = nullptr;

	delete itsDataLink;
	itsDataLink = nullptr;

	CloseOutFD();

	jdelete itsProcess;
}

/******************************************************************************
 PlaceCmdLineWidgets (virtual protected)

 ******************************************************************************/

const JCoordinate kMinCmdInputWidth = 20;

void
ExecOutputDocument::PlaceCustomWidgets()
{
	JXWindow* window = GetWindow();

	JXWidget::HSizingOption hSizing;
	JXWidget::VSizingOption vSizing;
	const JRect fileRect = GetFileDisplayInfo(&hSizing, &vSizing);

	itsCmdPrompt->Place(fileRect.left, fileRect.top);
	itsCmdPrompt->AdjustSize(0, fileRect.height() - itsCmdPrompt->GetFrameHeight());

	const JCoordinate promptWidth = itsCmdPrompt->GetFrameWidth();

	const JCoordinate eofWidth =
		itsEOFButton->GetFont().GetStringWidth(GetDisplay()->GetFontManager(), itsEOFButton->GetLabel()) +
		2 * itsEOFButton->GetPadding().x +
		2 * itsEOFButton->GetBorderWidth();

	itsCmdInput->Place(fileRect.left + promptWidth, fileRect.top);

	JCoordinate cmdInputWidth = fileRect.width() - promptWidth - eofWidth;
	if (cmdInputWidth < kMinCmdInputWidth)
	{
		window->AdjustSize(kMinCmdInputWidth - cmdInputWidth, 0);
		cmdInputWidth = kMinCmdInputWidth;
	}
	itsCmdInput->SetSize(cmdInputWidth, fileRect.height());

	const JPoint p = window->GetMinSize();
	window->SetMinSize(window->GetFrameWidth() - (cmdInputWidth - kMinCmdInputWidth), p.y);

	itsEOFButton->Place(fileRect.right - eofWidth, fileRect.top);
	itsEOFButton->SetSize(eofWidth, fileRect.height());

	UpdateButtons();
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
ExecOutputDocument::Activate()
{
	CommandOutputDocument::Activate();

	if (IsActive() && itsFocusToCmdFlag)
	{
		itsCmdInput->Focus();
	}
}

/******************************************************************************
 UseCountUpdated

 ******************************************************************************/

void
ExecOutputDocument::UseCountUpdated
	(
	const JSize count
	)
{
	if (count == 0)
	{
		itsClearWhenStartFlag = !ProcessRunning();
	}
}

/******************************************************************************
 SetConnection (virtual)

	execCmd can be nullptr.  If it is, the header (timestamp + path + cmd)
	is not printed.

 ******************************************************************************/

void
ExecOutputDocument::SetConnection
	(
	JProcess*		p,
	const int		inFD,
	const int		outFD,
	const JString&	windowTitle,
	const JString&	dontCloseMsg,
	const JString&	execDir,
	const JString&	execCmd,
	const bool		showPID
	)
{
	assert( !ProcessRunning() && itsRecordLink == nullptr && itsDataLink == nullptr );

	itsProcess = p;
	ListenTo(itsProcess);

	if (NeedsFormattedData())
	{
		itsRecordLink = new RecordLink(inFD);
		assert( itsRecordLink != nullptr );
		itsRecordLink->SetWantsBlankMessages();
		ListenTo(itsRecordLink);
	}
	else
	{
		itsDataLink = new DataLink(inFD);
		assert( itsDataLink != nullptr );
		ListenTo(itsDataLink);
	}

	if (outFD != ACE_INVALID_HANDLE)
	{
		itsCmdStream = jnew JOutPipeStream(outFD, true);
		assert( itsCmdStream != nullptr );
	}

	TextEditor* te = GetTextEditor();
	if (itsClearWhenStartFlag)
	{
		te->GetText()->SetText(JString::empty);
	}
	else if (!te->GetText()->IsEmpty())
	{
		const JString& text = te->GetText()->GetText();
		JStringIterator iter(text, kJIteratorStartAtEnd);
		JUtf8Character c;
		while (iter.Prev(&c, kJIteratorStay) && c == '\n')
		{
			iter.SkipPrev();
		}

		if (!iter.AtEnd())
		{
			te->SetSelection(JCharacterRange(
				iter.GetNextCharacterIndex(), text.GetCharacterCount()));
		}
		iter.Invalidate();

		te->Paste(JString("\n\n----------\n\n", JString::kNoCopy));
		te->GetText()->ClearUndo();
	}

	if (!execCmd.IsEmpty())
	{
		const JString timeStamp = JGetTimeStamp();

		te->Paste(timeStamp);
		te->Paste(JString::newline);
		te->Paste(execDir);
		te->Paste(JString::newline);
		te->Paste(execCmd);

		if (showPID)
		{
			te->Paste(JString::newline);
			te->Paste(JGetString("ProcessID::ExecOutputDocument"));
			te->Paste(JString((JUInt64) p->GetPID()));
		}

		te->Paste(JString::newline);
		te->Paste(JString::newline);
		te->GetText()->ClearUndo();
	}

	itsPath              = execDir;
	itsReceivedDataFlag  = false;
	itsProcessPausedFlag = false;
	SetDontCloseMessage(dontCloseMsg);
	FileChanged(windowTitle, false);

	UpdateButtons();
	te->SetWritable(false);
	itsCmdInput->GetText()->SetText(JString::empty);
}

/******************************************************************************
 SendText

 ******************************************************************************/

void
ExecOutputDocument::SendText
	(
	const JString& text
	)
{
	if (ProcessRunning() && itsCmdStream != nullptr)
	{
		text.Print(*itsCmdStream);
		itsCmdStream->flush();

		TextEditor* te = GetTextEditor();
		te->SetCaretLocation(te->GetText()->GetText().GetCharacterCount() + 1);

		if (text.GetFirstCharacter() != '\0' && text.GetFirstCharacter() != '\n' &&
			itsRecordLink != nullptr &&
			itsRecordLink->PeekPartialMessage(&itsLastPrompt))
		{
			te->Paste(itsLastPrompt);
		}

		te->Paste(text);
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
ExecOutputDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsRecordLink && message.Is(JMessageProtocolT::kMessageReady))
	{
		ReceiveRecord();
	}
	else if (sender == itsDataLink && message.Is(JAsynchDataReceiverT::kDataReady))
	{
		ReceiveData(message);
	}

	else if (sender == itsProcess && message.Is(JProcess::kFinished))
	{
		const auto* info =
			dynamic_cast<const JProcess::Finished*>(&message);
		assert( info != nullptr );
		const bool stayOpen = ProcessFinished(*info);

		// let somebody else start a new process

		itsClearWhenStartFlag = false;	// in case they call SetConnection() in ReceiveWithFeedback()
		Finished msg(info->Successful(), info->GetReason() != kJChildFinished);
		BroadcastWithFeedback(&msg);
		itsClearWhenStartFlag = GetUseCount() == 0 && !msg.SomebodyIsWaiting();

		if (GetUseCount() == 0 && !stayOpen && !ProcessRunning())
		{
			Close();
		}
	}

	else if (sender == itsPauseButton && message.Is(JXButton::kPushed))
	{
		ToggleProcessRunning();
	}
	else if (sender == itsStopButton && message.Is(JXButton::kPushed))
	{
		StopProcess();
	}
	else if (sender == itsKillButton && message.Is(JXButton::kPushed))
	{
		KillProcess();
	}

	else if (sender == itsEOFButton && message.Is(JXButton::kPushed))
	{
		if (!itsCmdInput->GetText()->IsEmpty())
		{
			itsCmdInput->HandleKeyPress(
				JUtf8Character(kJReturnKey), 0, JXKeyModifiers(GetDisplay()));
		}
		CloseOutFD();
		UpdateButtons();
	}

	else
	{
		CommandOutputDocument::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveRecord (private)

 ******************************************************************************/

void
ExecOutputDocument::ReceiveRecord()
{
	assert( itsRecordLink != nullptr );

	JString text;
	const bool ok = itsRecordLink->GetNextMessage(&text);
	assert( ok );

	// remove text that has already been printed

	if (!itsLastPrompt.IsEmpty() && text.StartsWith(itsLastPrompt))
	{
		JStringIterator iter(&text, kJIteratorStartAfter, itsLastPrompt.GetCharacterCount());
		iter.RemoveAllPrev();
	}
	itsLastPrompt.Clear();

	const JXTEBase::DisplayState state = GetTextEditor()->SaveDisplayState();

	AppendText(text);
	GetTextEditor()->GetText()->ClearUndo();

	if (!itsReceivedDataFlag)
	{
		itsReceivedDataFlag = true;
		if (!IsActive())
		{
			Activate();
		}
	}

	GetTextEditor()->RestoreDisplayState(state);
}

/******************************************************************************
 AppendText (virtual protected)

	Append the text to our text editor.  Derived classes can override
	to filter or otherwise process the text.

 ******************************************************************************/

void
ExecOutputDocument::AppendText
	(
	const JString& text
	)
{
	TextEditor* te = GetTextEditor();
	JPasteUNIXTerminalOutput(text, te->GetText()->GetBeyondEnd(), te->GetText());
	te->SetCaretLocation(te->GetText()->GetBeyondEnd().charIndex);
	te->Paste(JString::newline);
}

/******************************************************************************
 ReceiveData (private)

 ******************************************************************************/

void
ExecOutputDocument::ReceiveData
	(
	const Message& message
	)
{
	const auto* info =
		dynamic_cast<const JAsynchDataReceiverT::DataReady*>(&message);
	assert( info != nullptr );

	TextEditor* te                   = GetTextEditor();
	const JXTEBase::DisplayState state = te->SaveDisplayState();

	JPasteUNIXTerminalOutput(info->GetData(), te->GetText()->GetBeyondEnd(), te->GetText());
	te->GetText()->ClearUndo();

	if (!itsReceivedDataFlag)
	{
		itsReceivedDataFlag = true;
		if (!IsActive())
		{
			Activate();
		}
	}

	te->RestoreDisplayState(state);
}

/******************************************************************************
 ToggleProcessRunning (protected)

 ******************************************************************************/

void
ExecOutputDocument::ToggleProcessRunning()
{
	if (!ProcessRunning())
	{
		return;
	}

	if (!itsProcessPausedFlag)
	{
		if (itsProcess->SendSignalToGroup(SIGSTOP) == kJNoError)
		{
			itsProcessPausedFlag = true;
			UpdateButtons();
		}
	}
	else
	{
		if (itsProcess->SendSignalToGroup(SIGCONT) == kJNoError)
		{
			itsProcessPausedFlag = false;
			UpdateButtons();
		}
	}
}

/******************************************************************************
 StopProcess (protected)

 ******************************************************************************/

void
ExecOutputDocument::StopProcess()
{
	if (ProcessRunning())
	{
		itsProcess->Quit();
	}
}

/******************************************************************************
 KillProcess (protected)

 ******************************************************************************/

void
ExecOutputDocument::KillProcess()
{
	if (ProcessRunning())
	{
		itsProcess->Kill();
	}
}

/******************************************************************************
 CloseOutFD (private)

	Caller must call UpdateButtons() afterwards.

 ******************************************************************************/

void
ExecOutputDocument::CloseOutFD()
{
	if (itsCmdStream != nullptr)
	{
		jdelete itsCmdStream;
		itsCmdStream = nullptr;
	}
}

/******************************************************************************
 ProcessFinished (virtual protected)

	Returns true if document should stay open.

 ******************************************************************************/

bool
ExecOutputDocument::ProcessFinished
	(
	const JProcess::Finished& info
	)
{
	// Since the process has terminated, we know that it has written
	// everything to the pipe.  When handle_input() no longer broadcasts,
	// we know we have read everything.

	do
	{
		itsReceivedDataFlag = false;
		JThisProcess::CheckACEReactor();
	}
		while (itsReceivedDataFlag);

	const pid_t pid = itsProcess->GetPID();
	JProcess* p = itsProcess;
	itsProcess = nullptr;
	jdelete p;

	delete itsRecordLink;
	itsRecordLink = nullptr;

	delete itsDataLink;
	itsDataLink = nullptr;

	CloseOutFD();
	UpdateButtons();

	TextEditor* te                   = GetTextEditor();
	const JXTEBase::DisplayState state = te->SaveDisplayState();

	te->SetCaretLocation(te->GetText()->GetText().GetCharacterCount()+1);

	bool stayOpen = true;

	int result;
	const JChildExitReason reason = info.GetReason(&result);

	if (info.Successful() ||
		(reason == kJChildSignalled && result == SIGKILL))
	{
		DataReverted();
		te->GetText()->ClearUndo();
		if (!IsActive())
		{
			stayOpen = false;
		}
		else
		{
			const JString reasonStr = JPrintChildExitReason(reason, result);

			te->Paste(JString::newline);
			te->Paste(reasonStr);
			te->Paste(JString::newline);

			DataReverted();
			te->GetText()->ClearUndo();
		}
	}
	else
	{
		const JString reasonStr = JPrintChildExitReason(reason, result);

		te->Paste(JString::newline);
		te->Paste(reasonStr);
		te->Paste(JString::newline);

		if (reason != kJChildFinished)
		{
			#ifdef _J_MACOS
			const JString path("/cores/", JString::kNoCopy);
			#else
			const JString path = itsPath;
			#endif

			const JString coreName = "core." + JString((JUInt64) pid);
			const JString coreFull = JCombinePathAndName(path, coreName);
			if (JFileExists(coreFull))
			{
				te->Paste(JGetString("CoreLocation::ExecOutputDocument"));
				te->SetCurrentFontBold(true);
				te->Paste(coreFull);
				te->SetCurrentFontBold(false);
			}
			else
			{
				const JUtf8Byte* map[] =
				{
					"name", coreName.GetBytes()
				};
				const JString msg = JGetString("CoreName::ExecOutputDocument", map, sizeof(map));
				te->Paste(msg);
			}
			te->Paste(JString::newline);
		}

		DataReverted();
		te->GetText()->ClearUndo();
		if (!IsActive() || GetWindow()->IsIconified())	// don't raise if active
		{
			Activate();
		}
		GetDisplay()->Beep();
	}

	if (state.hadSelection)
	{
		te->RestoreDisplayState(state);
	}

	return stayOpen;
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
ExecOutputDocument::UpdateButtons()
{
	if (ProcessRunning())
	{
		itsPauseButton->Activate();
		itsKillButton->Activate();
		if (itsStopButton != nullptr)
		{
			itsStopButton->Activate();
		}
		itsCmdPrompt->Show();
		itsCmdInput->Show();
		itsEOFButton->Show();
		SetFileDisplayVisible(false);

		if (itsProcessPausedFlag)
		{
			itsPauseButton->SetLabel(JGetString("ResumeLabel::ExecOutputDocument"));
		}
		else
		{
			itsPauseButton->SetLabel(JGetString("PauseLabel::ExecOutputDocument"));
		}

		if (!itsProcessPausedFlag && itsCmdStream != nullptr)
		{
			itsCmdInput->Activate();
			itsEOFButton->Activate();
			if (itsFocusToCmdFlag)
			{
				itsCmdInput->Focus();
			}
		}
		else if (itsCmdStream == nullptr)
		{
			itsCmdPrompt->Hide();
			itsCmdInput->Hide();
			itsEOFButton->Hide();
			GetTextEditor()->Focus();
		}
		else
		{
			itsCmdInput->Deactivate();
			itsEOFButton->Deactivate();
			GetTextEditor()->Focus();
		}
	}
	else
	{
		itsPauseButton->Deactivate();
		itsKillButton->Deactivate();
		if (itsStopButton != nullptr)
		{
			itsStopButton->Deactivate();
		}
		itsCmdPrompt->Hide();
		itsCmdInput->Hide();
		itsEOFButton->Hide();
		SetFileDisplayVisible(true);
		GetTextEditor()->Focus();

		itsPauseButton->SetLabel(JGetString("PauseLabel::ExecOutputDocument"));
	}
}

/******************************************************************************
 OpenPrevListItem (virtual)

	Derived classes which claim to manage a list of files must override this.

 ******************************************************************************/

void
ExecOutputDocument::OpenPrevListItem()
{
	assert(0);
}

/******************************************************************************
 OpenNextListItem (virtual)

	Derived classes which claim to manage a list of files must override this.

 ******************************************************************************/

void
ExecOutputDocument::OpenNextListItem()
{
	assert(0);
}

/******************************************************************************
 ConvertSelectionToFullPath (virtual)

	Check if the file is relative to the exec path.

 ******************************************************************************/

void
ExecOutputDocument::ConvertSelectionToFullPath
	(
	JString* fileName
	)
	const
{
	if (JIsAbsolutePath(*fileName))
	{
		return;
	}

	const JString testName = JCombinePathAndName(itsPath, *fileName);
	if (JFileExists(testName))
	{
		*fileName = testName;
	}
	else
	{
		CommandOutputDocument::ConvertSelectionToFullPath(fileName);
	}
}

/******************************************************************************
 NeedsFormattedData (virtual protected)

	Derived classes should override to return true if they need to use
	RecordLink.

 ******************************************************************************/

bool
ExecOutputDocument::NeedsFormattedData()
	const
{
	return false;
}

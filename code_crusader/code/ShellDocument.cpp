/******************************************************************************
 ShellDocument.cpp

	BASE CLASS = TextDocument

	Copyright © 2009 by John Lindal.

 ******************************************************************************/

#include "ShellDocument.h"
#include "ShellEditor.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/JOutPipeStream.h>
#include <jx-af/jcore/jSysUtil.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kMenuButtonWidth = 60;

/******************************************************************************
 Create (static)

 ******************************************************************************/

bool
ShellDocument::Create
	(
	ShellDocument** doc
	)
{
	const JString shell    = JGetUserShell();
	const JUtf8Byte* cmd[] = { shell.GetBytes(), "-i", nullptr };

	JProcess* p;
	int inFD, outFD;
	const JError err = JProcess::Create(&p, cmd, sizeof(cmd), kJCreatePipe, &outFD,
										kJCreatePipe, &inFD, kJAttachToFromFD);
	if (err.OK())
	{
		*doc = jnew ShellDocument(p, inFD, outFD);
		assert( *doc != nullptr );

		(**doc).Activate();
		return true;
	}
	else
	{
		err.ReportIfError();
		*doc = nullptr;
		return false;
	}
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

ShellDocument::ShellDocument
	(
	JProcess*	p,
	const int	inFD,
	const int	outFD
	)
	:
	TextDocument(kShellOutputFT, "ShellHelp", false, ConstructShellEditor)
{
	itsShellEditor = (ShellEditor*) GetTextEditor();
	itsProcess     = nullptr;
	itsDataLink    = nullptr;
	itsCmdStream   = nullptr;

	// buttons in upper right

	JXWindow* window = GetWindow();
	const JRect rect = window->GetBounds();

	JXMenuBar* menuBar = GetMenuBar();
	const JSize h      = menuBar->GetFrameHeight();

	itsKillButton =
		jnew JXTextButton(JGetString("KillLabel::ExecOutputDocument"), window,
						 JXWidget::kFixedRight, JXWidget::kFixedTop,
						 rect.right - kMenuButtonWidth,0, kMenuButtonWidth,h);
	assert( itsKillButton != nullptr );
	ListenTo(itsKillButton);

	itsKillButton->SetShortcuts(JGetString("KillButtonShortcuts::ShellDocument"));
	itsKillButton->SetHint(JGetString("StopButtonHint::ExecOutputDocument"));

	menuBar->AdjustSize(-kMenuButtonWidth, 0);

	SetConnection(p, inFD, outFD);

	window->SetWMClass(GetWMClassInstance(), GetShellWindowClass());
}

// static private

TextEditor*
ShellDocument::ConstructShellEditor
	(
	TextDocument*		document,
	const JString&		fileName,
	JXMenuBar*			menuBar,
	TELineIndexInput*	lineInput,
	TEColIndexInput*	colInput,
	JXScrollbarSet*		scrollbarSet
	)
{
	auto* te =
		jnew ShellEditor(document, fileName, menuBar, lineInput, colInput,
						   scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( te != nullptr );

	return te;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ShellDocument::~ShellDocument()
{
	DeleteLinks();
	CloseOutFD();

	jdelete itsProcess;
}

/******************************************************************************
 SetConnection (private)

 ******************************************************************************/

void
ShellDocument::SetConnection
	(
	JProcess*	p,
	const int	inFD,
	const int	outFD
	)
{
	assert( !ProcessRunning() && itsDataLink == nullptr );

	itsProcess = p;
	ListenTo(itsProcess);

	itsDataLink = new DataLink(inFD);
	assert( itsDataLink != nullptr );
	ListenTo(itsDataLink);

	itsCmdStream = jnew JOutPipeStream(outFD, true);
	assert( itsCmdStream != nullptr );

	UpdateButtons();
}

/******************************************************************************
 DeleteLinks (private)

 ******************************************************************************/

void
ShellDocument::DeleteLinks()
{
	delete itsDataLink;
	itsDataLink = nullptr;
}

/******************************************************************************
 SendCommand

 ******************************************************************************/

void
ShellDocument::SendCommand
	(
	const JString& cmd
	)
{
	if (itsCmdStream != nullptr)
	{
		*itsCmdStream << cmd;
		itsCmdStream->flush();
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
ShellDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDataLink && message.Is(JAsynchDataReceiverT::kDataReady))
	{
		ReceiveData(message);
	}

	else if (sender == itsProcess && message.Is(JProcess::kFinished))
	{
		DeleteLinks();
		CloseOutFD();
		UpdateButtons();
	}

	else if (sender == itsKillButton && message.Is(JXButton::kPushed))
	{
		KillProcess();
	}

	else
	{
		TextDocument::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveData (private)

 ******************************************************************************/

void
ShellDocument::ReceiveData
	(
	const Message& message
	)
{
	const auto* info =
		dynamic_cast<const JAsynchDataReceiverT::DataReady*>(&message);
	assert( info != nullptr );

	itsShellEditor->InsertText(info->GetData());
}

/******************************************************************************
 KillProcess (protected)

 ******************************************************************************/

void
ShellDocument::KillProcess()
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
ShellDocument::CloseOutFD()
{
	jdelete itsCmdStream;
	itsCmdStream = nullptr;
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
ShellDocument::UpdateButtons()
{
	if (ProcessRunning())
	{
		itsKillButton->Activate();
	}
	else
	{
		itsKillButton->Deactivate();
	}
}

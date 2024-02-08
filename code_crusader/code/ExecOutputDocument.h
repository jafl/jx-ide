/******************************************************************************
 ExecOutputDocument.h

	Interface for the ExecOutputDocument class

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_ExecOutputDocument
#define _H_ExecOutputDocument

#include "CommandOutputDocument.h"
#include <jx-af/jcore/JProcess.h>			// for JProcess::Finished
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <jx-af/jcore/JMessageProtocol.h>	// template; requires ace includes
#include <jx-af/jcore/JAsynchDataReceiver.h>

class JOutPipeStream;
class JXStaticText;
class CmdLineInput;

class ExecOutputDocument : public CommandOutputDocument
{
	friend class ExecOutputPostFTCTask;

public:

	using RecordLink = JMessageProtocol<ACE_LSOCK_STREAM>;
	using DataLink   = JAsynchDataReceiver<ACE_LSOCK_STREAM>;

public:

	ExecOutputDocument(const TextFileType fileType = kExecOutputFT,
						 const JUtf8Byte* helpSectionName = "RunProgramHelp",
						 const JUtf8Byte* wmClass = nullptr,
						 const bool focusToCmd = true);

	~ExecOutputDocument() override;

	void	Activate() override;

	virtual void	SetConnection(JProcess* p, const int inFD, const int outFD,
								  const JString& windowTitle,
								  const JString& dontCloseMsg,
								  const JString& execDir,
								  const JString& execCmd,
								  const bool showPID);

	bool	CommandRunning() const override;
	bool	ProcessRunning() const;
	void	SendText(const JString& text);

	void	OpenPrevListItem() override;
	void	OpenNextListItem() override;

	void	ConvertSelectionToFullPath(JString* fileName) const override;

protected:

	void	UseCountUpdated(const JSize count) override;
	bool	ShouldClearWhenStart() const;

	void	ToggleProcessRunning();
	void	StopProcess();
	void	KillProcess();

	void			PlaceCustomWidgets() override;
	virtual void	AppendText(const JString& text);
	virtual bool	ProcessFinished(const JProcess::Finished& info);
	virtual bool	NeedsFormattedData() const;

	bool	GetRecordLink(RecordLink** link) const;
	bool	GetDataLink(DataLink** link) const;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JProcess*	itsProcess;				// deleted when we get a new one
	JString		itsPath;
	bool		itsReceivedDataFlag;
	bool		itsProcessPausedFlag;
	bool		itsClearWhenStartFlag;

	RecordLink*		itsRecordLink;		// can be nullptr
	DataLink*		itsDataLink;		// can be nullptr
	JOutPipeStream*	itsCmdStream;		// can be nullptr
	JString			itsLastPrompt;		// latest cmd line prompt (partial message)

	JXTextButton*	itsPauseButton;
	JXTextButton*	itsStopButton;
	JXTextButton*	itsKillButton;
	JXStaticText*	itsCmdPrompt;
	CmdLineInput*	itsCmdInput;
	JXTextButton*	itsEOFButton;
	const bool		itsFocusToCmdFlag;

private:

	void	ReceiveRecord();
	void	ReceiveData(const Message& message);
	void	UpdateButtons();
	void	CloseOutFD();
};


/******************************************************************************
 CommandRunning (virtual)

 ******************************************************************************/

inline bool
ExecOutputDocument::CommandRunning()
	const
{
	return ProcessRunning() || itsRecordLink != nullptr || itsDataLink != nullptr;
}

/******************************************************************************
 ProcessRunning

 ******************************************************************************/

inline bool
ExecOutputDocument::ProcessRunning()
	const
{
	return itsProcess != nullptr && !itsProcess->IsFinished();
}

/******************************************************************************
 GetRecordLink (protected)

 ******************************************************************************/

inline bool
ExecOutputDocument::GetRecordLink
	(
	RecordLink** link
	)
	const
{
	*link = itsRecordLink;
	return itsRecordLink != nullptr;
}

/******************************************************************************
 GetDataLink (protected)

 ******************************************************************************/

inline bool
ExecOutputDocument::GetDataLink
	(
	DataLink** link
	)
	const
{
	*link = itsDataLink;
	return itsDataLink != nullptr;
}

/******************************************************************************
 ShouldClearWhenStart (protected)

 ******************************************************************************/

inline bool
ExecOutputDocument::ShouldClearWhenStart()
	const
{
	return itsClearWhenStartFlag;
}

#endif

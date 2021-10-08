/******************************************************************************
 ExecOutputDocument.h

	Interface for the ExecOutputDocument class

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_ExecOutputDocument
#define _H_ExecOutputDocument

#include "TextDocument.h"
#include <jx-af/jcore/JProcess.h>			// need definition of JProcess::Finished
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <jx-af/jcore/JMessageProtocol.h>	// template; requires ace includes
#include <jx-af/jcore/JAsynchDataReceiver.h>

class JOutPipeStream;
class JXStaticText;
class CmdLineInput;

class ExecOutputDocument : public TextDocument
{
	friend class ExecOutputPostFTCTask;

public:

	using RecordLink = JMessageProtocol<ACE_LSOCK_STREAM>;
	using DataLink   = JAsynchDataReceiver<ACE_LSOCK_STREAM>;

public:

	ExecOutputDocument(const TextFileType fileType = kExecOutputFT,
						 const JUtf8Byte* helpSectionName = "RunProgramHelp",
						 const bool focusToCmd = true,
						 const bool allowStop = true);

	~ExecOutputDocument();

	void	Activate() override;

	void	IncrementUseCount();
	void	DecrementUseCount();

	virtual void	SetConnection(JProcess* p, const int inFD, const int outFD,
								  const JString& windowTitle,
								  const JString& dontCloseMsg,
								  const JString& execDir,
								  const JString& execCmd,
								  const bool showPID);

	bool	ProcessRunning() const;
	void	SendText(const JString& text);

	virtual void	OpenPrevListItem();
	virtual void	OpenNextListItem();

	void	ConvertSelectionToFullPath(JString* fileName) const override;

protected:

	bool	ShouldClearWhenStart() const;

	void	ToggleProcessRunning();
	void	StopProcess();
	void	KillProcess();

	virtual void	PlaceCmdLineWidgets();
	virtual void	AppendText(const JString& text);
	virtual bool	ProcessFinished(const JProcess::Finished& info);
	virtual bool	NeedsFormattedData() const;

	bool	GetRecordLink(RecordLink** link) const;
	bool	GetDataLink(DataLink** link) const;

	bool	OKToClose() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JProcess*	itsProcess;				// deleted when we get a new one
	JString		itsPath;
	bool		itsReceivedDataFlag;
	bool		itsProcessPausedFlag;
	JString		itsDontCloseMsg;
	bool		itsClearWhenStartFlag;
	JSize		itsUseCount;

	RecordLink*		itsRecordLink;			// can be nullptr
	DataLink*		itsDataLink;			// can be nullptr
	JOutPipeStream*	itsCmdStream;			// can be nullptr
	JString			itsLastPrompt;			// latest cmd line prompt (partial message)

	JXTextButton*	itsPauseButton;
	JXTextButton*	itsStopButton;			// can be nullptr
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

	static TextEditor*	ConstructTextEditor(TextDocument* document,
											const JString& fileName,
											JXMenuBar* menuBar,
											TELineIndexInput* lineInput,
											TEColIndexInput* colInput,
											JXScrollbarSet* scrollbarSet);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFinished;

	class Finished : public JBroadcaster::Message
		{
		public:

			Finished(const bool success, const bool cancelled)
				:
				JBroadcaster::Message(kFinished),
				itsSuccessFlag(success && !cancelled),
				itsCancelledFlag(cancelled),
				itsSomebodyIsWaitingFlag(false)
				{ };

			bool
			Successful() const
			{
				return itsSuccessFlag;
			};

			bool
			Cancelled() const
			{
				return itsCancelledFlag;
			};

			bool
			SomebodyIsWaiting()
			{
				return itsSomebodyIsWaitingFlag;
			};

			void
			SetSomebodyIsWaiting()
			{
				itsSomebodyIsWaitingFlag = true;
			};

		private:

			bool	itsSuccessFlag;
			bool	itsCancelledFlag;
			bool	itsSomebodyIsWaitingFlag;
		};
};


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

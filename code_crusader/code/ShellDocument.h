/******************************************************************************
 ShellDocument.h

	Copyright © 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_ShellDocument
#define _H_ShellDocument

#include "TextDocument.h"
#include <jx-af/jcore/JProcess.h>
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <jx-af/jcore/JAsynchDataReceiver.h>

class JOutPipeStream;
class ShellEditor;

class ShellDocument : public TextDocument
{
public:

	typedef JAsynchDataReceiver<ACE_LSOCK_STREAM>	DataLink;

public:

	static bool	Create(ShellDocument** doc);

	~ShellDocument();

	bool	ProcessRunning() const;
	void	SendCommand(const JString& cmd);

protected:

	ShellDocument(JProcess* p, const int inFD, const int outFD);

	void	KillProcess();

	bool	GetDataLink(DataLink** link) const;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	ShellEditor*	itsShellEditor;
	JProcess*		itsProcess;
	DataLink*		itsDataLink;
	JOutPipeStream*	itsCmdStream;

	JXTextButton*	itsKillButton;

private:

	void	SetConnection(JProcess* p, const int inFD, const int outFD);
	void	ReceiveData(const Message& message);
	void	UpdateButtons();
	void	DeleteLinks();
	void	CloseOutFD();

	static TextEditor*	ConstructShellEditor(TextDocument* document,
												 const JString& fileName,
												 JXMenuBar* menuBar,
												 TELineIndexInput* lineInput,
												 TEColIndexInput* colInput,
												 JXScrollbarSet* scrollbarSet);
};


/******************************************************************************
 ProcessRunning

 ******************************************************************************/

inline bool
ShellDocument::ProcessRunning()
	const
{
	return itsProcess != nullptr && !itsProcess->IsFinished();
}

/******************************************************************************
 GetDataLink (protected)

 ******************************************************************************/

inline bool
ShellDocument::GetDataLink
	(
	DataLink** link
	)
	const
{
	*link = itsDataLink;
	return itsDataLink != nullptr;
}

#endif

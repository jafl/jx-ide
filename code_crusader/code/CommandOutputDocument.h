/******************************************************************************
 CommandOutputDocument.h

	Interface for the CommandOutputDocument class

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_CommandOutputDocument
#define _H_CommandOutputDocument

#include "TextDocument.h"

class CommandOutputDocument : public TextDocument
{
	friend class CommandOutputPostFTCTask;

public:

	CommandOutputDocument(const TextFileType fileType,
						  const JUtf8Byte* helpSectionName,
						  const JUtf8Byte* wmClass,
						  const JString& dontCloseMessage);

	~CommandOutputDocument() override;

	JSize	GetUseCount() const;
	void	IncrementUseCount();
	void	DecrementUseCount();

	void	SetDontCloseMessage(const JString& msg);

	virtual bool	CommandRunning() const = 0;

	virtual void	OpenPrevListItem() = 0;
	virtual void	OpenNextListItem() = 0;

protected:

	virtual void	PlaceCustomWidgets() = 0;
	virtual void	UseCountUpdated(const JSize count);

	bool	OKToClose() override;

private:

	JSize	itsUseCount;
	JString	itsDontCloseMsg;

private:

	void	ReceiveRecord();

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
 Use count

 ******************************************************************************/

inline JSize
CommandOutputDocument::GetUseCount()
	const
{
	return itsUseCount;
}

inline void
CommandOutputDocument::IncrementUseCount()
{
	itsUseCount++;
	UseCountUpdated(itsUseCount);
}

/******************************************************************************
 SetDontCloseMessage

 ******************************************************************************/

inline void
CommandOutputDocument::SetDontCloseMessage
	(
	const JString& msg
	)
{
	itsDontCloseMsg = msg;
}

#endif

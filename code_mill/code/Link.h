/******************************************************************************
 Link.h

	Interface for the Link class

	Copyright (C) 2002 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_Link
#define _H_Link

// Superclass Header
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <jx-af/jcore/JMessageProtocol.h>

class JOutPipeStream;
class JProcess;

class Class;
class MemberFunction;

class Link : public JBroadcaster
{
public:

	Link();
	virtual ~Link();

	bool	StartCTags();
	void		ParseClass(Class* list, const JString& filename, const JString& classname);
	
private:

	JProcess*		itsCTagsProcess;			// can be nullptr

	JOutPipeStream*	itsOutputLink;				// nullptr if process not started
	int				itsInputFD;					// -1 if process not started

	Class*		itsClassList;
	JString			itsCurrentClass;
	JString			itsCurrentFile;

private:

	void	StopCTags();
	void	ParseLine(const JString& data);
	void	ParseInterface(MemberFunction* fn, const JIndex line);

	// not allowed

	Link(const Link&) = delete;
	Link& operator=(const Link&) = delete;

public:

	static const JUtf8Byte* kFileParsed;

	class FileParsed : public JBroadcaster::Message
		{
		public:

			FileParsed()
				:
				JBroadcaster::Message(kFileParsed)
				{ };
		};

};

#endif

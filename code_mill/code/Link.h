/******************************************************************************
 Link.h

	Interface for the Link class

	Copyright (C) 2002 by Glenn W. Bach.
	Copyright (C) 2023 by John Lindal.
	
 *****************************************************************************/

#ifndef _H_Link
#define _H_Link

#include "CtagsUser.h"

class Class;
class MemberFunction;

class Link : public CtagsUser
{
public:

	Link();

	~Link() override;

	void	ParseClass(Class* list, const JString& fileName, const JString& className);

private:

	void	ReadFile(const JString& fullName, JPtrArray<JString>* lines) const;
	JString	GetReturnType(const JString& name, const JIndex lineIndex,
						  const JPtrArray<JString>& lines) const;

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

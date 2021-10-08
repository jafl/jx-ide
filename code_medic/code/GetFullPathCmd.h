/******************************************************************************
 GetFullPathCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetFullPath
#define _H_GetFullPath

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

class GetFullPathCmd : public Command, virtual public JBroadcaster
{
public:

	GetFullPathCmd(const JString& cmd,
				  const JString& fileName, const JIndex lineIndex);

	~GetFullPathCmd() override;

	const JString&	GetFileName() const;
	JIndex			GetLineIndex() const;

private:

	const JString	itsFileName;
	const JIndex	itsLineIndex;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFileFound;
	static const JUtf8Byte* kFileNotFound;
	static const JUtf8Byte* kNewCommand;

	class FileFound : public JBroadcaster::Message
		{
		public:

			FileFound(const JString& fullName, const JIndex lineIndex)
				:
				JBroadcaster::Message(kFileFound),
				itsFullName(fullName), itsLineIndex(lineIndex)
				{ };

			const JString&
			GetFullName() const
			{
				return itsFullName;
			}

			JIndex
			GetLineIndex() const
			{
				return itsLineIndex;
			}

		private:

			const JString&	itsFullName;
			const JIndex	itsLineIndex;
		};

	class FileNotFound : public JBroadcaster::Message
		{
		public:

			FileNotFound(const JString& fileName)
				:
				JBroadcaster::Message(kFileNotFound),
				itsFileName(fileName)
				{ };

			const JString&
			GetFileName() const
			{
				return itsFileName;
			}

		private:

			const JString& itsFileName;
		};

	class NewCommand : public JBroadcaster::Message
		{
		public:

			NewCommand(GetFullPathCmd* cmd)
				:
				JBroadcaster::Message(kNewCommand),
				itsCmd(cmd)
				{ };

			GetFullPathCmd*
			GetNewCommand() const
			{
				return itsCmd;
			}

		private:

			GetFullPathCmd*	itsCmd;
		};
};


/******************************************************************************
 GetFileName

 ******************************************************************************/

inline const JString&
GetFullPathCmd::GetFileName()
	const
{
	return itsFileName;
}

/******************************************************************************
 GetFileName

 ******************************************************************************/

inline JIndex
GetFullPathCmd::GetLineIndex()
	const
{
	return itsLineIndex;
}

#endif

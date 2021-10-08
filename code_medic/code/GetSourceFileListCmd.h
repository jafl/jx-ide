/******************************************************************************
 GetSourceFileListCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetSourceFileListCmd
#define _H_GetSourceFileListCmd

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

class FileListDir;

class GetSourceFileListCmd : public Command, virtual public JBroadcaster
{
public:

	GetSourceFileListCmd(const JString& cmd, FileListDir* fileList);

	~GetSourceFileListCmd() override;

	FileListDir*	GetFileList();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	FileListDir*	itsFileList;			// not owned
	bool			itsNeedRedoOnFirstStop;
};


/******************************************************************************
 GetFileList

 ******************************************************************************/

inline FileListDir*
GetSourceFileListCmd::GetFileList()
{
	return itsFileList;
}

#endif

/******************************************************************************
 GetSourceFileList.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetSourceFileList
#define _H_GetSourceFileList

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

class FileListDir;

class GetSourceFileList : public Command, virtual public JBroadcaster
{
public:

	GetSourceFileList(const JString& cmd, FileListDir* fileList);

	virtual	~GetSourceFileList();

	FileListDir*	GetFileList();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	FileListDir*	itsFileList;			// not owned
	bool			itsNeedRedoOnFirstStop;
};


/******************************************************************************
 GetFileList

 ******************************************************************************/

inline FileListDir*
GetSourceFileList::GetFileList()
{
	return itsFileList;
}

#endif

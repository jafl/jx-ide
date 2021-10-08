/******************************************************************************
 GDBGetSourceFileListCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetSourceFileListCmd
#define _H_GDBGetSourceFileListCmd

#include "GetSourceFileListCmd.h"

class FileListDir;

namespace gdb {

class GetSourceFileListCmd : public ::GetSourceFileListCmd
{
public:

	GetSourceFileListCmd(FileListDir* fileList);

	~GetSourceFileListCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif

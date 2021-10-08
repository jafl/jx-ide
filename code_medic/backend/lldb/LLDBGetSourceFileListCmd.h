/******************************************************************************
 LLDBGetSourceFileListCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetSourceFileListCmd
#define _H_LLDBGetSourceFileListCmd

#include "GetSourceFileListCmd.h"

class FileListDir;

namespace lldb {

class GetSourceFileListCmd : public ::GetSourceFileListCmd
{
public:

	GetSourceFileListCmd(FileListDir* fileList);

	~GetSourceFileListCmd();

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif

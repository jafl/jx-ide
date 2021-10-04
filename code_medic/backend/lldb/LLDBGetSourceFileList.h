/******************************************************************************
 LLDBGetSourceFileList.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetSourceFileList
#define _H_LLDBGetSourceFileList

#include "GetSourceFileListCmd.h"

class FileListDir;

class LLDBGetSourceFileList : public GetSourceFileListCmd
{
public:

	LLDBGetSourceFileList(FileListDir* fileList);

	virtual	~LLDBGetSourceFileList();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif

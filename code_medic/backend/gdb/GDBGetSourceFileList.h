/******************************************************************************
 GDBGetSourceFileList.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetSourceFileList
#define _H_GDBGetSourceFileList

#include "GetSourceFileList.h"

class FileListDir;

class GDBGetSourceFileList : public GetSourceFileList
{
public:

	GDBGetSourceFileList(FileListDir* fileList);

	virtual	~GDBGetSourceFileList();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif

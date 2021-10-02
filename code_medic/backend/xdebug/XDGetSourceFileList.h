/******************************************************************************
 XDGetSourceFileList.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetSourceFileList
#define _H_XDGetSourceFileList

#include "GetSourceFileList.h"

class FileListDir;

class XDGetSourceFileList : public GetSourceFileList
{
public:

	XDGetSourceFileList(FileListDir* fileList);

	virtual	~XDGetSourceFileList();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	void	ScanDirectory(const JString& path);
};

#endif

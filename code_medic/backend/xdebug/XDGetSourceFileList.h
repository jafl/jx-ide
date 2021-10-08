/******************************************************************************
 XDGetSourceFileList.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetSourceFileList
#define _H_XDGetSourceFileList

#include "GetSourceFileListCmd.h"

class FileListDir;

class XDGetSourceFileList : public GetSourceFileListCmd
{
public:

	XDGetSourceFileList(FileListDir* fileList);

	~XDGetSourceFileList();

	void	Starting() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	void	ScanDirectory(const JString& path);
};

#endif

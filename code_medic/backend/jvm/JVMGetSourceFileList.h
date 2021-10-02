/******************************************************************************
 JVMGetSourceFileList.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetSourceFileList
#define _H_JVMGetSourceFileList

#include "GetSourceFileList.h"

class FileListDir;

class JVMGetSourceFileList : public GetSourceFileList
{
public:

	JVMGetSourceFileList(FileListDir* fileList);

	virtual	~JVMGetSourceFileList();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	void	ScanDirectory(const JString& path);
};

#endif

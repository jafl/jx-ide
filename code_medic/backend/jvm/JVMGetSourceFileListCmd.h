/******************************************************************************
 JVMGetSourceFileListCmd.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetSourceFileListCmd
#define _H_JVMGetSourceFileListCmd

#include "GetSourceFileListCmd.h"

class FileListDir;

class JVMGetSourceFileListCmd : public GetSourceFileListCmd
{
public:

	JVMGetSourceFileListCmd(FileListDir* fileList);

	virtual	~JVMGetSourceFileListCmd();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	void	ScanDirectory(const JString& path);
};

#endif

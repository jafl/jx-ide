/******************************************************************************
 GetSourceFileListCmd.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetSourceFileListCmd
#define _H_XDGetSourceFileListCmd

#include <GetSourceFileListCmd.h>

class FileListDir;

namespace xdebug {

class GetSourceFileListCmd : public ::GetSourceFileListCmd
{
public:

	GetSourceFileListCmd(FileListDir* fileList);

	~GetSourceFileListCmd() override;

	void	Starting() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	void	ScanDirectory(const JString& path);
};

};

#endif

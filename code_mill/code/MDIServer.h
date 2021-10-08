/******************************************************************************
 MDIServer.h

	Copyright (C) 2002 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_MDIServer
#define _H_MDIServer

#include <jx-af/jx/JXMDIServer.h>

class MDIServer : public JXMDIServer
{
public:

	MDIServer();

	~MDIServer() override;

	static void	PrintCommandLineHelp();

protected:

	void	HandleMDIRequest(const JString& dir,
							 const JPtrArray<JString>& argList) override;
};

#endif

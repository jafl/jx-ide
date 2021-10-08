/******************************************************************************
 MDIServer.h

	Interface for the MDIServer class

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_MDIServer
#define _H_MDIServer

#include <jx-af/jx/JXMDIServer.h>

class MDIServer : public JXMDIServer, virtual public JBroadcaster
{
public:

	MDIServer();

	~MDIServer() override;

	static void	UpdateDebuggerType(const JString& program);
	static bool	IsBinary(const JString& fileName);
	static bool	GetLanguage(const JString& fileName, JString* language);

protected:

	void	HandleMDIRequest(const JString& dir,
							 const JPtrArray<JString>& argList) override;

private:

	bool	itsFirstTimeFlag;
};

#endif

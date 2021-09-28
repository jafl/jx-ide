/******************************************************************************
 GFGApp.h

	Copyright (C) 2002 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GFGApp
#define _H_GFGApp

#include <jx-af/jx/JXApplication.h>

class GFGApp : public JXApplication
{
public:

	GFGApp(int* argc, char* argv[], bool* displayAbout, JString* prevVersStr);

	virtual	~GFGApp();

	void	DisplayAbout(const JString& prevVersStr = JString::empty);

	static const JUtf8Byte*	GetAppSignature();
	static void				InitStrings();

	bool	IsDeletingTemplate() const;
	void	ShouldBeDeletingTemplate(const bool delTemplate);

protected:

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	bool	itsDeletingTemplate;
};

#endif

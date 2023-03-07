/******************************************************************************
 App.h

	Interface for the App class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_App
#define _H_App

#include <jx-af/jx/JXApplication.h>

class App : public JXApplication
{
public:

	App(int* argc, char* argv[], bool* displayAbout, JString* prevVersStr);

	~App() override;

	void	EditFile(const JString& fileName, const JIndex lineIndex = 0) const;

	void	DisplayAbout(const bool showLicense = false,
						 const JString& prevVersStr = JString::empty);

	static void				InitStrings();
	static const JUtf8Byte*	GetAppSignature();

protected:

	void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason) override;
};

#endif

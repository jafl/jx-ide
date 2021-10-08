/******************************************************************************
 App.h

	Interface for the App class

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_App
#define _H_App

#include <jx-af/jx/JXApplication.h>
#include <jx-af/jcore/JPrefObject.h>

class JXMenuBar;
class JXTextMenu;
class JXToolBar;
class DirInfoList;

class App : public JXApplication, public JPrefObject
{
public:

	App(int* argc, char* argv[], const bool useMDI,
		  bool* displayAbout, JString* prevVersStr);

	~App();

	virtual void	Quit();

	static void				InitStrings();
	static const JUtf8Byte*	GetAppSignature();

	void	DisplayAbout(const JString& prevVersStr = JString::empty,
						 const bool init = false);
	void	EditMiscPrefs();

	JXTextMenu*	CreateHelpMenu(JXMenuBar* menuBar, const JUtf8Byte* idNamespace);
	void		AppendHelpMenuToToolBar(JXToolBar* toolBar, JXTextMenu* menu);
	void		UpdateHelpMenu(JXTextMenu* menu);
	void		HandleHelpMenu(JXTextMenu* menu, const JUtf8Byte* windowSectionName,
							   const JIndex index);

	bool	FindFile(const JString& fileName, const JString::Case caseSensitive,
					 JString* fullName) const;
	bool	FindAndViewFile(const JString& fileName,
							const JIndexRange lineRange = JIndexRange(),
							const JString::Case caseSensitive = JString::kCompareCase) const;

	// called by EditGenPrefsDialog

	void	GetWarnings(bool* warnBeforeQuit) const;
	void	SetWarnings(const bool warnBeforeQuit);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual bool	Close();
	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	bool				itsWarnBeforeQuitFlag;
	JPtrArray<JString>*	itsSystemIncludeDirs;

private:

	void	GetSystemIncludeDirectories();
	void	CollectSearchPaths(DirInfoList* searchPaths) const;
};


/******************************************************************************
 Warnings

 ******************************************************************************/

inline void
App::GetWarnings
	(
	bool* warnBeforeQuit
	)
	const
{
	*warnBeforeQuit = itsWarnBeforeQuitFlag;
}

inline void
App::SetWarnings
	(
	const bool warnBeforeQuit
	)
{
	itsWarnBeforeQuitFlag = warnBeforeQuit;
}

#endif

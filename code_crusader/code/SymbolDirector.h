/******************************************************************************
 SymbolDirector.h

	Interface for the SymbolDirector class

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_SymbolDirector
#define _H_SymbolDirector

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPrefObject.h>
#include "PrefsManager.h"		// for FileTypesChanged
#include <jx-af/jx/JXButtonStates.h>

class JProgressDisplay;
class JXTextMenu;
class JXToolBar;
class SymbolList;
class SymbolTable;
class ProjectDocument;
class SymbolSRDirector;
class CommandMenu;
class Class;

class SymbolDirector : public JXWindowDirector, public JPrefObject
{
public:

	SymbolDirector(ProjectDocument* supervisor);
	SymbolDirector(std::istream& projInput, const JFileVersion projVers,
					std::istream* setInput, const JFileVersion setVers,
					std::istream* symInput, const JFileVersion symVers,
					ProjectDocument* supervisor, const bool subProject);

	~SymbolDirector() override;

	void	EditPrefs();
	void	SetPrefs(const bool raiseTreeOnRightClick, const bool writePrefs);

	ProjectDocument*	GetProjectDocument() const;
	SymbolList*			GetSymbolList() const;
	bool				FindSymbol(const JString& name, const JString& fileName,
								   const JXMouseButton button);
	bool				ViewSymbol(const JIndex symbolIndex) const;
	bool				FindAllSymbols(const Class* theClass, const bool findDeclaration,
									   const bool findDefinition);

	bool	HasSymbolBrowsers() const;
	void	CloseSymbolBrowsers();

	void	StreamOut(std::ostream& projOutput,
					  std::ostream* setOutput, std::ostream* symOutput) const;

	// for loading updated symbols

	void	ReadSetup(std::istream& symInput, const JFileVersion symVers);

	// called by ProjectDocument

	void	FileTypesChanged(const PrefsManager::FileTypesChanged& info);

	// called by FileListTable

	void	PrepareForListUpdate(const bool reparseAll, JProgressDisplay& pg);
	bool	ListUpdateFinished(const JArray<JFAID_t>& deadFileList, JProgressDisplay& pg);

	// called by SymbolSRDirector

	void	SRDirectorClosed(SymbolSRDirector* dir);

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	ProjectDocument*	itsProjDoc;				// not owned
	SymbolList*			itsSymbolList;
	bool				itsRaiseTreeOnRightClickFlag;

	JPtrArray<SymbolSRDirector>*	itsSRList;	// contents not owned

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsSymbolMenu;
	JXTextMenu*		itsProjectMenu;
	CommandMenu*	itsCmdMenu;
	JXTextMenu*		itsPrefsMenu;

// begin JXLayout

	JXToolBar*   itsToolBar;
	SymbolTable* itsSymbolTable;

// end JXLayout

private:

	void	SymbolDirectorX(ProjectDocument* projDoc);
	void	BuildWindow(SymbolList* symbolList);
	void	AdjustWindowTitle();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateSymbolMenu();
	void	HandleSymbolMenu(const JIndex index);

	void	UpdateProjectMenu();
	void	HandleProjectMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	static void	UpgradeToolBarID(JString* s);
};


/******************************************************************************
 GetProjectDocument

 ******************************************************************************/

inline ProjectDocument*
SymbolDirector::GetProjectDocument()
	const
{
	return itsProjDoc;
}

/******************************************************************************
 GetSymbolList

 ******************************************************************************/

inline SymbolList*
SymbolDirector::GetSymbolList()
	const
{
	return itsSymbolList;
}

/******************************************************************************
 HasSymbolBrowsers

 ******************************************************************************/

inline bool
SymbolDirector::HasSymbolBrowsers()
	const
{
	return !itsSRList->IsEmpty();
}

#endif

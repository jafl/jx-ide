/******************************************************************************
 ProjectDocument.h

	Interface for the ProjectDocument class

	Copyright © 1996-2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_ProjectDocument
#define _H_ProjectDocument

#include <jx-af/jx/JXFileDocument.h>
#include "DirList.h"
#include "TextFileType.h"
#include "ProjectTable.h"		// for PathType, DropFileAction
#include "BuildManager.h"		// for MakefileMethod
#include "ExecOutputDocument.h"	// for RecordLink
#include "CommandManager.h"		// for CmdList

class JProcess;
class JPTPrinter;
class JXTextMenu;
class JXTextButton;
class JXToolBar;
class JXWidgetSet;
class JXPTPrinter;
class JXFunctionTask;
class JXProgressDisplay;
class JXProgressIndicator;
class ProjectTree;
class SymbolDirector;
class TreeDirector;
class CTreeDirector;
class DTreeDirector;
class GoTreeDirector;
class JavaTreeDirector;
class PHPTreeDirector;
class CommandMenu;
class FileListDirector;
class FileListTable;
class WaitForSymbolUpdateTask;
class DelaySymbolUpdateTask;

class ProjectDocument : public JXFileDocument
{
public:

	enum UpdateMessageType
	{
		kFixedLengthStart,
		kVariableLengthStart,
		kProgressIncrement,
		kLockSymbolTable,
		kSymbolTableLocked,
		kSymbolTableWritten,
		kDoItYourself
	};

public:

	static void			Create();
	static FileStatus	Create(const JString& fullName, const bool silent,
							   ProjectDocument** doc);
	static FileStatus	CanReadFile(const JString& fullName);
	static FileStatus	CanReadFile(std::istream& input, JFileVersion* actualFileVersion);

	~ProjectDocument() override;

	void			Activate() override;
	const JString&	GetName() const override;
	bool			GetMenuIcon(const JXImage** icon) const override;
	bool			Close() override;

	ProjectTree*	GetFileTree() const;
	ProjectTable*	GetFileTable() const;
	void			AddFile(const JString& fullName,
							const ProjectTable::PathType pathType);
	void			EditMakeConfig();

	CommandManager*		GetCommandManager() const;
	BuildManager*		GetBuildManager() const;
	const JString&		GetBuildTargetName() const;
	FileListDirector*	GetFileListDirector() const;
	FileListTable*		GetAllFileList() const;
	SymbolDirector*		GetSymbolDirector() const;
	CTreeDirector*		GetCTreeDirector() const;
	DTreeDirector*		GetDTreeDirector() const;
	GoTreeDirector*		GetGoTreeDirector() const;
	JavaTreeDirector*	GetJavaTreeDirector() const;
	PHPTreeDirector*	GetPHPTreeDirector() const;

	const JPtrArray<TreeDirector>&	GetTreeDirectorList() const;

	bool			HasDirectories() const;
	const DirList&	GetDirectories() const;
	void			EditSearchPaths(const JPtrArray<JString>* dirList = nullptr);

	void	DelayUpdateSymbolDatabase();
	void	UpdateSymbolDatabase();
	void	CancelUpdateSymbolDatabase();
	void	RefreshVCSStatus();

	void	SetProjectPrefs(const bool reopenTextFiles,
							const bool doubleSpaceCompile,
							const bool rebuildMakefileDaily,
							const ProjectTable::DropFileAction dropFileAction);

	static void	ReadStaticGlobalPrefs(std::istream& input, const JFileVersion vers);
	static void	WriteStaticGlobalPrefs(std::ostream& output);

	static bool	WillReopenTextFiles();
	static void	ShouldReopenTextFiles(const bool reopen);

	static bool	WillAskOKToOpenOldVersion();
	static void	ShouldAskOKToOpenOldVersion(const bool ask);

	static const JString&	GetAddFilesFilter();
	static void				SetAddFilesFilter(const JString& filter);

	static const JUtf8Byte*	GetProjectFileSuffix();

	void	ConvertCompileRunDialogs(std::istream& projInput, const JFileVersion vers);

	// called by NewProjectSaveFileDialog

	static const JUtf8Byte*	GetTemplateDirectoryName();
	static bool				GetProjectTemplateType(const JString& fullName,
												   JString* type);

	// called by EditTreePrefsDialog

	void	SetTreePrefs(const JSize fontSize,
						 const bool autoMinMILinks, const bool drawMILinksOnTop,
						 const bool raiseWhenSingleMatch, const bool writePrefs);

	// called by CommandTable

	static bool	ReadTasksFromProjectFile(std::istream& input, CommandManager::CmdList* cmdList);

protected:

	ProjectDocument(const JString& fullName,
					  const BuildManager::MakefileMethod makefileMethod,
					  const bool fromTemplate, const JString& tmplFile);
	ProjectDocument(std::istream& input, const JString& projName,
					  const JString& setName, const JString& symName,
					  const bool silent);

	void	DiscardChanges() override;
	JError	WriteFile(const JString& fullName, const bool safetySave) const override;
	void	WriteTextFile(std::ostream& output, const bool safetySave) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	ProjectTree*	itsFileTree;
	ProjectTable*	itsFileTable;
	bool			itsProcessNodeMessageFlag;		// true => process messages from itsFileTree

	CommandManager*		itsCmdMgr;
	BuildManager*		itsBuildMgr;
	JXFunctionTask*		itsSaveTask;
	JString				itsPrintName;
	mutable JString		itsDocName;					// so GetName() can return JString&

	DirList*					itsDirList;
	FileListDirector*			itsAllFileDirector;
	SymbolDirector*				itsSymbolDirector;
	CTreeDirector*				itsCTreeDirector;
	DTreeDirector*				itsDTreeDirector;
	GoTreeDirector*				itsGoTreeDirector;
	JavaTreeDirector*			itsJavaTreeDirector;
	PHPTreeDirector*			itsPHPTreeDirector;
	JPtrArray<TreeDirector>*	itsTreeDirectorList;

	JProcess*						itsUpdateProcess;
	ExecOutputDocument::RecordLink*	itsUpdateLink;
	std::ostream*					itsUpdateStream;
	JXProgressDisplay*				itsUpdatePG;
	WaitForSymbolUpdateTask*		itsWaitForUpdateTask;
	DelaySymbolUpdateTask*			itsDelaySymbolUpdateTask;
	JFloat							itsLastSymbolLoadTime;

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsTreeMenu;
	JXTextMenu*		itsProjectMenu;
	JXTextMenu*		itsSourceMenu;
	CommandMenu*	itsCmdMenu;
	JXTextMenu*		itsPrefsMenu;

	static bool	theReopenTextFilesFlag;
	static bool	theWarnOpenOldVersionFlag;
	static JString	theAddFilesFilter;

// begin JXLayout

	JXToolBar*           itsToolBar;
	JXTextButton*        itsConfigButton;
	JXWidgetSet*         itsUpdateContainer;
	JXStaticText*        itsUpdateLabel;
	JXProgressIndicator* itsUpdateCleanUpIndicator;
	JXStaticText*        itsUpdateCounter;

// end JXLayout

private:

	void	ProjectDocumentX(ProjectTree* fileList);
	void	BuildWindow(ProjectTree* fileList);

	void	WriteFiles(std::ostream& projOutput,
					   const JString& setName, std::ostream* setOutput,
					   const JString& symName, std::ostream* symOutput) const;

	void	SaveAsTemplate() const;
	void	ReadTemplate(std::istream& input, const JFileVersion tmplVers,
						 const JFileVersion projVers);
	void	WriteTemplate(const JString& fileName) const;

	static JString	GetSettingFileName(const JString& fullName);
	static JString	GetSymbolFileName(const JString& fullName);

	void	ProcessNodeMessage(const Message& message);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	Print(JPTPrinter& p) const;

	void	UpdateTreeMenu();
	void	HandleTreeMenu(const JIndex index);

	void	UpdateProjectMenu();
	void	HandleProjectMenu(const JIndex index);

	void	UpdateSourceMenu();
	void	HandleSourceMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);
	void	EditProjectPrefs();

	void	SymbolUpdateProgress();
	void	SymbolUpdateFinished();
	void	ShowUpdatePG(const bool visible);
	void	DeleteUpdateLink();

	int		StartSymbolLoadTimer();
	void	StopSymbolLoadTimer(const int timerStatus);

	static void	UpgradeToolBarID(JString* s);
};


/******************************************************************************
 GetFileTree

 ******************************************************************************/

inline ProjectTree*
ProjectDocument::GetFileTree()
	const
{
	return itsFileTree;
}

/******************************************************************************
 GetFileTable

 ******************************************************************************/

inline ProjectTable*
ProjectDocument::GetFileTable()
	const
{
	return itsFileTable;
}

/******************************************************************************
 GetCommandManager

 ******************************************************************************/

inline CommandManager*
ProjectDocument::GetCommandManager()
	const
{
	return itsCmdMgr;
}

/******************************************************************************
 GetBuildManager

 ******************************************************************************/

inline BuildManager*
ProjectDocument::GetBuildManager()
	const
{
	return itsBuildMgr;
}

/******************************************************************************
 GetBuildTargetName

 ******************************************************************************/

inline const JString&
ProjectDocument::GetBuildTargetName()
	const
{
	return itsBuildMgr->GetBuildTargetName();
}

/******************************************************************************
 HasDirectories

 ******************************************************************************/

inline bool
ProjectDocument::HasDirectories()
	const
{
	return !itsDirList->IsEmpty();
}

/******************************************************************************
 GetDirectories

 ******************************************************************************/

inline const DirList&
ProjectDocument::GetDirectories()
	const
{
	return *itsDirList;
}

/******************************************************************************
 GetFileListDirector

 ******************************************************************************/

inline FileListDirector*
ProjectDocument::GetFileListDirector()
	const
{
	return itsAllFileDirector;
}

/******************************************************************************
 GetSymbolDirector

 ******************************************************************************/

inline SymbolDirector*
ProjectDocument::GetSymbolDirector()
	const
{
	return itsSymbolDirector;
}

/******************************************************************************
 GetCTreeDirector

 ******************************************************************************/

inline CTreeDirector*
ProjectDocument::GetCTreeDirector()
	const
{
	return itsCTreeDirector;
}

/******************************************************************************
 GetDTreeDirector

 ******************************************************************************/

inline DTreeDirector*
ProjectDocument::GetDTreeDirector()
	const
{
	return itsDTreeDirector;
}

/******************************************************************************
 GetGoTreeDirector

 ******************************************************************************/

inline GoTreeDirector*
ProjectDocument::GetGoTreeDirector()
	const
{
	return itsGoTreeDirector;
}

/******************************************************************************
 GetJavaTreeDirector

 ******************************************************************************/

inline JavaTreeDirector*
ProjectDocument::GetJavaTreeDirector()
	const
{
	return itsJavaTreeDirector;
}

/******************************************************************************
 GetPHPTreeDirector

 ******************************************************************************/

inline PHPTreeDirector*
ProjectDocument::GetPHPTreeDirector()
	const
{
	return itsPHPTreeDirector;
}

/******************************************************************************
 GetTreeDirectorList

 ******************************************************************************/

inline const JPtrArray<TreeDirector>&
ProjectDocument::GetTreeDirectorList()
	const
{
	return *itsTreeDirectorList;
}

/******************************************************************************
 Reopen text files when open project (static)

 ******************************************************************************/

inline bool
ProjectDocument::WillReopenTextFiles()
{
	return theReopenTextFilesFlag;
}

inline void
ProjectDocument::ShouldReopenTextFiles
	(
	const bool reopen
	)
{
	theReopenTextFilesFlag = reopen;
}

/******************************************************************************
 Reopen text files when open project (static)

 ******************************************************************************/

inline bool
ProjectDocument::WillAskOKToOpenOldVersion()
{
	return theWarnOpenOldVersionFlag;
}

inline void
ProjectDocument::ShouldAskOKToOpenOldVersion
	(
	const bool ask
	)
{
	theWarnOpenOldVersionFlag = ask;
}

/******************************************************************************
 Filter for adding new files (static)

 ******************************************************************************/

inline const JString&
ProjectDocument::GetAddFilesFilter()
{
	return theAddFilesFilter;
}

inline void
ProjectDocument::SetAddFilesFilter
	(
	const JString& filter
	)
{
	theAddFilesFilter = filter;
}

#endif

/******************************************************************************
 DocumentManager.h

	Interface for the DocumentManager class

	Copyright © 1997-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_DocumentManager
#define _H_DocumentManager

#include <jx-af/jx/JXDocumentManager.h>
#include "TextFileType.h"
#include <jx-af/jcore/JPrefObject.h>
#include <jx-af/jcore/JPtrArray.h>

class JRegex;
class JSTStyler;
class JXWindow;
class ProjectDocument;
class TextDocument;
class ShellDocument;
class ExecOutputDocument;
class ExtEditorDialog;
class FileHistoryMenu;

class DocumentManager : public JXDocumentManager, public JPrefObject
{
public:

	enum FileHistoryType
	{
		kProjectFileHistory,
		kTextFileHistory
	};

	enum
	{
		kMinWarnFileSize = 5000000
	};

public:

	DocumentManager();

	virtual ~DocumentManager();

	bool	GetTemplateDirectory(const JUtf8Byte* dirName,
									 const bool create, JString* fullName);
	bool	GetTextTemplateDirectory(const bool create, JString* tmplDir);

	bool	NewProjectDocument(ProjectDocument** doc = nullptr);
	void		NewTextDocument();
	void		NewTextDocumentFromTemplate();
	void		NewShellDocument();
	bool	OpenTextDocument(const JString& fileName,
								 const JIndex lineIndex = 0,
								 TextDocument** doc = nullptr,
								 const bool iconify = false,
								 const bool forceReload = false);
	bool	OpenTextDocument(const JString& fileName,
								 const JIndexRange& lineRange,
								 TextDocument** doc = nullptr,
								 const bool iconify = false,
								 const bool forceReload = false);
	static bool	WarnFileSize(const JString& fileName);

	void	OpenBinaryDocument(const JString& fileName);

	void	OpenSomething(const JString& fileName = JString::empty,
						  const JIndexRange lineRange = JIndexRange(),
						  const bool iconify = false,
						  const bool forceReload = false);
	void	OpenSomething(const JPtrArray<JString>& fileNameList);

	void	ReadFromProject(std::istream& input, const JFileVersion vers);
	void	WriteForProject(std::ostream& output) const;

	bool	HasProjectDocuments() const;
	bool	GetActiveProjectDocument(ProjectDocument** doc) const;
	bool	ProjectDocumentIsOpen(const JString& fileName,
									  ProjectDocument** doc) const;
	bool	CloseProjectDocuments();

	bool	HasTextDocuments() const;
	JSize		GetTextDocumentCount() const;
	bool	GetActiveTextDocument(TextDocument** doc) const;
	bool	TextDocumentsNeedSave();
	bool	SaveTextDocuments(const bool saveUntitled);
	void		ReloadTextDocuments(const bool force);
	bool	CloseTextDocuments();
	void		FileRenamed(const JString& origFullName, const JString& newFullName);
	void		StylerChanged(JSTStyler* styler);

	void	ProjDocCreated(ProjectDocument* doc);
	void	ProjDocDeleted(ProjectDocument* doc);
	void	SetActiveProjectDocument(ProjectDocument* doc);

	void	TextDocumentCreated(TextDocument* doc);
	void	TextDocumentDeleted(TextDocument* doc);
	void	SetActiveTextDocument(TextDocument* doc);
	void	TextDocumentNeedsSave();

	bool	GetActiveListDocument(ExecOutputDocument** doc) const;
	void		SetActiveListDocument(ExecOutputDocument* doc);

	bool	OpenComplementFile(const JString& fullName, const TextFileType type,
								   ProjectDocument* projDoc = nullptr,
								   const bool searchDirs = true);
	bool	GetComplementFile(const JString& inputName, const TextFileType inputType,
								  JString* outputName, ProjectDocument* projDoc = nullptr,
								  const bool searchDirs = true) const;
	bool	GetOpenComplementFile(const JString& inputName,
									  const TextFileType inputType,
									  JXFileDocument** doc) const;

	JPtrArray<ProjectDocument>*	GetProjectDocList() const;
	JPtrArray<TextDocument>*		GetTextDocList() const;

	void	AddToFileHistoryMenu(const FileHistoryType type,
								 const JString& fullName);

	void	UpdateSymbolDatabases();
	void	CancelUpdateSymbolDatabases();
	void	RefreshVCSStatus();

	void		ChooseEditors();
	bool	WillEditTextFilesLocally() const;
	void		ShouldEditTextFilesLocally(const bool local);
	bool	WillEditBinaryFilesLocally() const;
	void		ShouldEditBinaryFilesLocally(const bool local);

	// called by PrefsManager

	bool	RestoreState(std::istream& input);
	bool	SaveState(std::ostream& output) const;

	// called by EditGenPrefsDialog

	void	GetWarnings(bool* warnBeforeSaveAll, bool* warnBeforeCloseAll) const;
	void	SetWarnings(const bool warnBeforeSaveAll, const bool warnBeforeCloseAll);

	// called by CreateGlobals()

	void	CreateFileHistoryMenus(JXWindow* window);

	// called by FileHistoryMenu

	FileHistoryMenu*	GetFileHistoryMenu(const FileHistoryType type) const;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JPtrArray<ProjectDocument>*	itsProjectDocuments;

	JPtrArray<TextDocument>*		itsTextDocuments;
	ExecOutputDocument*			itsListDocument;
	bool						itsTextNeedsSaveFlag;

	bool	itsWarnBeforeSaveAllFlag;
	bool	itsWarnBeforeCloseAllFlag;

	// recent files

	FileHistoryMenu*	itsRecentProjectMenu;
	FileHistoryMenu*	itsRecentTextMenu;

	// external editors

	bool	itsEditTextLocalFlag;
	JString		itsEditTextFileCmd;
	JString		itsEditTextFileLineCmd;

	bool	itsEditBinaryLocalFlag;
	JString		itsEditBinaryFileCmd;

	ExtEditorDialog*	itsExtEditorDialog;

private:

	void		PrivateOpenSomething(const JString& fileName,
									 const JIndexRange& lineRange,
									 const bool iconify,
									 const bool forceReload);
	bool	PrivateOpenTextDocument(const JString& fullName,
										const JIndexRange& lineRange,
										const bool iconify,
										const bool forceReload,
										TextDocument** doc) const;
	void		PrivateOpenBinaryDocument(const JString& fullName,
										  const bool iconify,
										  const bool forceReload) const;

	bool	FindComplementFile(const JString& inputName,
								   const TextFileType outputType,
								   JString* outputName,
								   ProjectDocument* projDoc,
								   const bool searchDirs) const;
	bool	SearchForComplementFile(ProjectDocument* projDoc,
										const JString& origFullName,
										const JString& baseFullName,
										const JPtrArray<JString>& suffixList,
										JString* outputName) const;

	bool	FindOpenComplementFile(const JString& inputName,
									   const TextFileType outputType,
									   JXFileDocument** doc) const;

private:

	// JBroadcaster message base class

	class ProjectDocumentMessage : public JBroadcaster::Message
		{
		public:

			ProjectDocumentMessage(const JUtf8Byte* type, ProjectDocument* doc)
				:
				JBroadcaster::Message(type),
				itsDoc(doc)
				{ };

			ProjectDocument*
			GetProjectDocument() const
			{
				return itsDoc;
			}

		private:

			ProjectDocument*	itsDoc;
		};

public:

	// JBroadcaster messages

	static const JUtf8Byte* kProjectDocumentCreated;
	static const JUtf8Byte* kProjectDocumentDeleted;
	static const JUtf8Byte* kProjectDocumentActivated;
	static const JUtf8Byte* kAddFileToHistory;

	class ProjectDocumentCreated : public ProjectDocumentMessage
		{
		public:

			ProjectDocumentCreated(ProjectDocument* doc)
				:
				ProjectDocumentMessage(kProjectDocumentCreated, doc)
				{ };
		};

	class ProjectDocumentDeleted : public ProjectDocumentMessage
		{
		public:

			ProjectDocumentDeleted(ProjectDocument* doc)
				:
				ProjectDocumentMessage(kProjectDocumentDeleted, doc)
				{ };
		};

	class ProjectDocumentActivated : public ProjectDocumentMessage
		{
		public:

			ProjectDocumentActivated(ProjectDocument* doc)
				:
				ProjectDocumentMessage(kProjectDocumentActivated, doc)
				{ };

			ProjectDocumentActivated(JPtrArray<ProjectDocument>* list)
				:
				ProjectDocumentMessage(kProjectDocumentActivated,
									   list->IsEmpty() ?
											(ProjectDocument*) nullptr :
											list->GetFirstElement())
				{ };
		};

	class AddFileToHistory : public JBroadcaster::Message
		{
		public:

			AddFileToHistory(const FileHistoryType type,
							 const JString& fullName)
				:
				JBroadcaster::Message(kAddFileToHistory),
				itsType(type), itsFullName(fullName)
				{ };

			FileHistoryType
			GetFileHistoryType() const
			{
				return itsType;
			}

			const JString&
			GetFullName() const
			{
				return itsFullName;
			}

		private:

			const FileHistoryType	itsType;
			const JString&			itsFullName;
		};
};


/******************************************************************************
 HasTextDocuments

 ******************************************************************************/

inline bool
DocumentManager::HasTextDocuments()
	const
{
	return !itsTextDocuments->IsEmpty();
}

/******************************************************************************
 GetTextDocumentCount

 ******************************************************************************/

inline JSize
DocumentManager::GetTextDocumentCount()
	const
{
	return itsTextDocuments->GetElementCount();
}

/******************************************************************************
 GetTextDocList

 ******************************************************************************/

inline JPtrArray<TextDocument>*
DocumentManager::GetTextDocList()
	const
{
	return itsTextDocuments;
}

/******************************************************************************
 TextDocumentsNeedSave

 ******************************************************************************/

inline bool
DocumentManager::TextDocumentsNeedSave()
{
	return !itsTextDocuments->IsEmpty() && itsTextNeedsSaveFlag;
}

/******************************************************************************
 TextDocumentNeedsSave

	Called by TextDocument.

 ******************************************************************************/

inline void
DocumentManager::TextDocumentNeedsSave()
{
	itsTextNeedsSaveFlag = true;
}

/******************************************************************************
 HasProjectDocuments

 ******************************************************************************/

inline bool
DocumentManager::HasProjectDocuments()
	const
{
	return !itsProjectDocuments->IsEmpty();
}

/******************************************************************************
 GetProjectDocList

 ******************************************************************************/

inline JPtrArray<ProjectDocument>*
DocumentManager::GetProjectDocList()
	const
{
	return itsProjectDocuments;
}

/******************************************************************************
 SetActiveProjectDocument

	Called by ProjectDocument.

 ******************************************************************************/

inline void
DocumentManager::SetActiveProjectDocument
	(
	ProjectDocument* doc
	)
{
	JIndex i;
	if (itsProjectDocuments->Find(doc, &i) && i != 1)
		{
		itsProjectDocuments->MoveElementToIndex(i, 1);
		}
}

/******************************************************************************
 Warnings

 ******************************************************************************/

inline void
DocumentManager::GetWarnings
	(
	bool* warnBeforeSaveAll,
	bool* warnBeforeCloseAll
	)
	const
{
	*warnBeforeSaveAll  = itsWarnBeforeSaveAllFlag;
	*warnBeforeCloseAll = itsWarnBeforeCloseAllFlag;
}

inline void
DocumentManager::SetWarnings
	(
	const bool warnBeforeSaveAll,
	const bool warnBeforeCloseAll
	)
{
	itsWarnBeforeSaveAllFlag  = warnBeforeSaveAll;
	itsWarnBeforeCloseAllFlag = warnBeforeCloseAll;
}

/******************************************************************************
 Edit files locally

 ******************************************************************************/

inline bool
DocumentManager::WillEditTextFilesLocally()
	const
{
	return itsEditTextLocalFlag;
}

inline void
DocumentManager::ShouldEditTextFilesLocally
	(
	const bool local
	)
{
	itsEditTextLocalFlag = local;
}

inline bool
DocumentManager::WillEditBinaryFilesLocally()
	const
{
	return itsEditBinaryLocalFlag;
}

inline void
DocumentManager::ShouldEditBinaryFilesLocally
	(
	const bool local
	)
{
	itsEditBinaryLocalFlag = local;
}

#endif

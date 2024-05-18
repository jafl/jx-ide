/******************************************************************************
 DocumentManager.cpp

	BASE CLASS = JXDocumentManager, JPrefObject

	Copyright © 1997-99 by John Lindal.

 ******************************************************************************/

#include "DocumentManager.h"
#include "ProjectDocument.h"
#include "ProjectTree.h"
#include "ProjectNode.h"
#include "ExecOutputDocument.h"
#include "TextEditor.h"
#include "ExtEditorDialog.h"
#include "FileHistoryMenu.h"
#include "fileVersions.h"
#include "globals.h"
#include "sharedUtil.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStandAlonePG.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXChooseFileDialog.h>
#include <jx-af/jx/JXSaveFileDialog.h>
#include <jx-af/jx/JXSearchTextDialog.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JSubstitute.h>
#include <jx-af/jcore/JLatentPG.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/JSimpleProcess.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JListUtil.h>
#include <ranges>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kTextTemplateDir = "text_templates";

// External editors

static const JString kDefEditTextFileCmd("emacsclient $f");
static const JString kDefEditTextFileLineCmd("emacsclient +$l $f");

#ifdef _J_MACOS
static const JString kDefEditBinaryFileCmd("open -a \"Hex Fiend\" $f");
#else
static const JString kDefEditBinaryFileCmd("khexedit $f"); // "xterm -title \"$f\" -e vi $f";
#endif

// setup information

const JFileVersion kCurrentSetupVersion = 6;

	// version 6 removes itsUpdateSymbolDBAfterBuildFlag
	// version 5 stores itsEditBinaryLocalFlag and itsEditBinaryFileCmd
	// version 4 stores itsUpdateSymbolDBAfterBuildFlag
	// version 3 converts variable marker from % to $
	// version 2 stores recent projects and recent text documents
	// version 1 stores itsWarnBeforeSaveAllFlag, itsWarnBeforeCloseAllFlag

// state information

const JFileVersion kCurrentStateVersion = 2;

	// version  2 stores open TextDocuments even if projects are open
	// version  1 stores project count instead of boolean

// JBroadcaster message types

const JUtf8Byte* DocumentManager::kProjectDocumentCreated =
	"Created::Project::DocumentManager";

const JUtf8Byte* DocumentManager::kProjectDocumentDeleted =
	"Deleted::Project::DocumentManager";

const JUtf8Byte* DocumentManager::kProjectDocumentsDeleted =
	"MultipleDeleted::Project::DocumentManager";

const JUtf8Byte* DocumentManager::kProjectDocumentActivated =
	"Activated::Project::DocumentManager";

const JUtf8Byte* DocumentManager::kAddFileToHistory =
	"AddFileToHistory::DocumentManager";

/******************************************************************************
 Constructor

 ******************************************************************************/

DocumentManager::DocumentManager()
	:
	JXDocumentManager(),
	JPrefObject(GetPrefsManager(), kDocMgrID),
	itsClosingAllProjectsFlag(false),
	itsListDocument(nullptr),
	itsTextNeedsSaveFlag(false),
	itsWarnBeforeSaveAllFlag(false),
	itsWarnBeforeCloseAllFlag(false),
	itsRecentProjectMenu(nullptr),
	itsRecentTextMenu(nullptr),
	itsEditTextLocalFlag(true),
	itsEditTextFileCmd(kDefEditTextFileCmd),
	itsEditTextFileLineCmd(kDefEditTextFileLineCmd),
	itsEditBinaryLocalFlag(false),
	itsEditBinaryFileCmd(kDefEditBinaryFileCmd)
{
	itsProjectDocuments = jnew JPtrArray<ProjectDocument>(JPtrArrayT::kForgetAll);
	ListenTo(itsProjectDocuments);

	itsTextDocuments = jnew JPtrArray<TextDocument>(JPtrArrayT::kForgetAll);

	PrefsManager* prefsMgr = GetPrefsManager();
	ListenTo(prefsMgr);
	// ReadPrefs() called in CreateHistoryMenus()
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DocumentManager::~DocumentManager()
{
	// DeleteGlobals() calls WritePrefs()

	jdelete itsProjectDocuments;		// objects deleted by JXDirector
	jdelete itsTextDocuments;		// objects deleted by JXDirector
}

/******************************************************************************
 CreateFileHistoryMenus

	Called by CreateGlobals() after permanent windows have been created.

	*** This must only be called once.

 ******************************************************************************/

void
DocumentManager::CreateFileHistoryMenus
	(
	JXWindow* window
	)
{
	assert( itsRecentProjectMenu == nullptr &&
			itsRecentTextMenu    == nullptr );

	itsRecentProjectMenu =
		jnew FileHistoryMenu(kProjectFileHistory, window,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  -20,0, 10,10);
	itsRecentProjectMenu->Hide();

	itsRecentTextMenu =
		jnew FileHistoryMenu(kTextFileHistory, window,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  -20,0, 10,10);
	itsRecentTextMenu->Hide();

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 GetFileHistoryMenu

	Called by FileHistoryMenu.

 ******************************************************************************/

FileHistoryMenu*
DocumentManager::GetFileHistoryMenu
	(
	const FileHistoryType type
	)
	const
{
	if (type == kProjectFileHistory)
	{
		return itsRecentProjectMenu;
	}
	else
	{
		assert( type == kTextFileHistory );
		return itsRecentTextMenu;
	}
}

/******************************************************************************
 AddToFileHistoryMenu

 ******************************************************************************/

void
DocumentManager::AddToFileHistoryMenu
	(
	const FileHistoryType	type,
	const JString&			fullName
	)
{
	Broadcast(AddFileToHistory(type, fullName));
}

/******************************************************************************
 RefreshVCSStatus

 ******************************************************************************/

void
DocumentManager::RefreshVCSStatus()
{
	for (auto* doc : *itsProjectDocuments)
	{
		doc->RefreshVCSStatus();
	}

	for (auto* doc : *itsTextDocuments)
	{
		doc->RefreshVCSStatus();
	}
}

/******************************************************************************
 NewProjectDocument

	If doc != nullptr, *doc is the new document, if one is successfully created.

 ******************************************************************************/

void
DocumentManager::NewProjectDocument()
{
	ProjectDocument::Create();
}

/******************************************************************************
 ProjDocCreated

	Called by ProjectDocument.

 ******************************************************************************/

void
DocumentManager::ProjDocCreated
	(
	ProjectDocument* doc
	)
{
	itsProjectDocuments->Prepend(doc);
	Broadcast(ProjectDocumentCreated(doc));
}

/******************************************************************************
 ProjDocDeleted

	Called by ProjectDocument.

 ******************************************************************************/

void
DocumentManager::ProjDocDeleted
	(
	ProjectDocument* doc
	)
{
	itsProjectDocuments->Remove(doc);
	if (!itsClosingAllProjectsFlag)
	{
		Broadcast(ProjectDocumentDeleted(doc));
	}

	bool onDisk;
	const JString fullName = doc->GetFullName(&onDisk);
	if (onDisk)
	{
		Broadcast(AddFileToHistory(kProjectFileHistory, fullName));
	}
}

/******************************************************************************
 GetActiveProjectDocument

 ******************************************************************************/

bool
DocumentManager::GetActiveProjectDocument
	(
	ProjectDocument** doc
	)
	const
{
	if (!itsProjectDocuments->IsEmpty())
	{
		*doc = itsProjectDocuments->GetFirstItem();
		return true;
	}
	else
	{
		*doc = nullptr;
		return false;
	}
}

/******************************************************************************
 ProjectDocumentIsOpen

	If there is a ProjectDocument that uses the specified file, we return it.

 ******************************************************************************/

bool
DocumentManager::ProjectDocumentIsOpen
	(
	const JString&		fileName,
	ProjectDocument**	doc
	)
	const
{
	*doc = nullptr;

	// check that the file exists

	if (!JFileExists(fileName))
	{
		return false;
	}

	// search for an open ProjectDocument that uses this file

	for (auto* d : *itsProjectDocuments)
	{
		bool onDisk;
		const JString docName = d->GetFullName(&onDisk);

		if (onDisk && JSameDirEntry(fileName, docName))
		{
			*doc = d;
			return true;
		}
	}

	return false;
}

/******************************************************************************
 CloseProjectDocuments

 ******************************************************************************/

bool
DocumentManager::CloseProjectDocuments()
{
	itsClosingAllProjectsFlag = true;

	for (auto* doc : std::views::reverse(*itsProjectDocuments))
	{
		if (!doc->Close())
		{
			break;
		}
	}

	itsClosingAllProjectsFlag = false;
	Broadcast(ProjectDocumentsDeleted());

	return itsProjectDocuments->IsEmpty();
}

/******************************************************************************
 SymbolDatabasesNeedUpdate

 ******************************************************************************/

void
DocumentManager::SymbolDatabasesNeedUpdate()
{
	for (auto* doc : *itsProjectDocuments)
	{
		doc->SymbolDatabaseNeedsUpdate();
	}
}

/******************************************************************************
 NewTextDocument

 ******************************************************************************/

void
DocumentManager::NewTextDocument()
{
	if (itsEditTextLocalFlag)
	{
		auto* doc = jnew TextDocument;
		doc->Activate();
		return;
	}

	auto* dlog = JXSaveFileDialog::Create(JGetString("NewFilePrompt::DocumentManager"));
	if (dlog->DoDialog())
	{
		const JString fullName = dlog->GetFullName();

		std::ofstream output(fullName.GetBytes());
		output.close();
		OpenTextDocument(fullName);
	}
}

/******************************************************************************
 NewTextDocumentFromTemplate

 ******************************************************************************/

void
DocumentManager::NewTextDocumentFromTemplate()
{
	JString tmplDir;
	const bool exists = GetTextTemplateDirectory(false, &tmplDir);
	if (!exists)
	{
		const JUtf8Byte* map[] =
		{
			"name", kTextTemplateDir
		};
		const JString msg = JGetString("NoTextTemplates::DocumentManager", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		return;
	}

	tmplDir += ACE_DIRECTORY_SEPARATOR_STR "*";		// make ChooseFile() happy

	auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectSingleFile, tmplDir);
	if (dlog->DoDialog())
	{
		if (itsEditTextLocalFlag)
		{
			auto* doc = jnew TextDocument(dlog->GetFullName(), kUnknownFT, true);
			doc->Activate();
			return;
		}

		auto* dlog = JXSaveFileDialog::Create(JGetString("NewFilePrompt::DocumentManager"));
		if (dlog->DoDialog())
		{
			const JString fullName = dlog->GetFullName();
			JString tmplText;
			JReadFile(fullName, &tmplText);
			std::ofstream output(fullName.GetBytes());
			tmplText.Print(output);
			output.close();
			OpenTextDocument(fullName);
		}
	}
}

/******************************************************************************
 GetTemplateDirectory

	If create==true, tries to create the directory if it doesn't exist.

 ******************************************************************************/

bool
DocumentManager::GetTextTemplateDirectory
	(
	const bool	create,
	JString*		tmplDir
	)
{
	return GetTemplateDirectory(kTextTemplateDir, create, tmplDir);
}

bool
DocumentManager::GetTemplateDirectory
	(
	const JUtf8Byte*	dirName,
	const bool		create,
	JString*			fullName
	)
{
	fullName->Clear();

	JString sysDir, userDir;
	const bool hasHomeDir   = JXGetProgramDataDirectories(dirName, &sysDir, &userDir);
	const bool hasUserDir   = hasHomeDir && JDirectoryExists(userDir);
	const bool hasUserFiles = hasUserDir && !JDirInfo::Empty(userDir);

	if (!hasHomeDir)
	{
		if (!JDirInfo::Empty(sysDir))
		{
			*fullName = sysDir;
			return true;
		}
		if (create)
		{
			JGetUserNotification()->ReportError(JGetString("NoHomeDirectory::DocumentManager"));
		}
		return false;
	}
	else if (hasUserDir)
	{
		if (!hasUserFiles && !JDirInfo::Empty(sysDir))
		{
			*fullName = sysDir;
			return true;
		}
		*fullName = userDir;
		return create || hasUserFiles;
	}
	else
	{
		if (!JDirInfo::Empty(sysDir))
		{
			*fullName = sysDir;
			return true;
		}
		if (!create)
		{
			return false;
		}

		JError err = JNoError();
		if (JCreateDirectory(userDir, &err))
		{
			*fullName = userDir;
			return true;
		}
		else
		{
			const JUtf8Byte* map[] =
			{
				"name", userDir.GetBytes(),
				"err",  err.GetMessage().GetBytes()
			};
			const JString msg = JGetString("CannotCreateTemplates::DocumentManager", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
			return false;
		}
	}
}

/******************************************************************************
 TextDocumentCreated

	Called by TextDocument.

 ******************************************************************************/

void
DocumentManager::TextDocumentCreated
	(
	TextDocument* doc
	)
{
	itsTextDocuments->Append(doc);
}

/******************************************************************************
 TextDocumentDeleted

	Called by TextDocument.

 ******************************************************************************/

void
DocumentManager::TextDocumentDeleted
	(
	TextDocument* doc
	)
{
	JIndex i;
	if (itsTextDocuments->Find(doc, &i))
	{
		itsTextDocuments->RemoveItem(i);

		if (i == 1 && !itsTextDocuments->IsEmpty())
		{
			JXGetSearchTextDialog()->SetActiveTE(
				itsTextDocuments->GetFirstItem()->GetTextEditor());
		}
	}

	if (doc == itsListDocument)
	{
		itsListDocument = nullptr;
	}

	bool onDisk;
	const JString fullName = doc->GetFullName(&onDisk);
	if (onDisk)
	{
		Broadcast(AddFileToHistory(kTextFileHistory, fullName));
	}
}

/******************************************************************************
 GetActiveTextDocument

 ******************************************************************************/

bool
DocumentManager::GetActiveTextDocument
	(
	TextDocument** doc
	)
	const
{
	if (!itsTextDocuments->IsEmpty())
	{
		*doc = itsTextDocuments->GetFirstItem();
		return true;
	}
	else
	{
		*doc = nullptr;
		return false;
	}
}

/******************************************************************************
 SetActiveTextDocument

	Called by TextDocument.

 ******************************************************************************/

void
DocumentManager::SetActiveTextDocument
	(
	TextDocument* doc
	)
{
	JIndex i;
	if (itsTextDocuments->Find(doc, &i) && i != 1)
	{
		itsTextDocuments->MoveItemToIndex(i, 1);
		doc->CheckIfModifiedByOthers();
		JXGetSearchTextDialog()->SetActiveTE(doc->GetTextEditor());
	}
}

/******************************************************************************
 GetActiveListDocument

 ******************************************************************************/

bool
DocumentManager::GetActiveListDocument
	(
	CommandOutputDocument** doc
	)
	const
{
	*doc = itsListDocument;
	return itsListDocument != nullptr;
}

/******************************************************************************
 SetActiveListDocument

	Called by SearchDocument and CompileDocument.

 ******************************************************************************/

void
DocumentManager::SetActiveListDocument
	(
	CommandOutputDocument* doc
	)
{
	itsListDocument = doc;
}

/******************************************************************************
 OpenSomething

	Determines the file type and creates the appropriate document.

	If fileName is nullptr or empty, we ask the user to choose a file.
	If lineIndex is not zero and we open a TextDocument, we go to that line.

 ******************************************************************************/

void
DocumentManager::OpenSomething
	(
	const JString&		fileName,
	const JIndexRange	lineRange,		// not reference because of default value
	const bool			iconify,
	const bool			forceReload
	)
{
	if (!fileName.IsEmpty())
	{
		if (JFileReadable(fileName))
		{
			PrivateOpenSomething(fileName, lineRange, iconify, forceReload);
		}
		else if (!JFileExists(fileName))
		{
			const JUtf8Byte* map[] =
			{
				"f", fileName.GetBytes()
			};
			const JString msg = JGetString("FileDoesNotExist::DocumentManager", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
		}
		else
		{
			const JUtf8Byte* map[] =
		{
				"f", fileName.GetBytes()
		};
			const JString msg = JGetString("CannotReadFile::DocumentManager", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
		}
	}
	else
	{
		auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectMultipleFiles);
		if (dlog->DoDialog())
		{
			JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
			dlog->GetFullNames(&list);
			OpenSomething(list);
		}
	}
}

void
DocumentManager::OpenSomething
	(
	const JPtrArray<JString>& fileNameList
	)
{
	const JSize count = fileNameList.GetItemCount();
	if (count > 0)
	{
		JXStandAlonePG pg;
		pg.RaiseWhenUpdate();
		pg.FixedLengthProcessBeginning(count, JGetString("OpeningFilesProgress::DocumentManager"), true, true);

		for (JIndex i=1; i<=count; i++)
		{
			const JString* fileName = fileNameList.GetItem(i);
			if (!fileName->IsEmpty())
			{
				OpenSomething(*fileName);
			}
			if (!pg.IncrementProgress())
			{
				break;
			}
		}

		pg.ProcessFinished();
	}
}

/******************************************************************************
 PrivateOpenSomething (private)

 ******************************************************************************/

void
DocumentManager::PrivateOpenSomething
	(
	const JString&		fileName,
	const JIndexRange&	lineRange,
	const bool			iconify,
	const bool			forceReload
	)
{
	ProjectDocument* doc;
	if (ProjectDocument::Create(fileName, false, &doc) == JXFileDocument::kNotMyFile)
	{
		JString cmd;
		if (GetPrefsManager()->EditWithOtherProgram(fileName, &cmd))
		{
			JSimpleProcess::Create(cmd, true);
		}
		else if (GetPrefsManager()->GetFileType(fileName) == kBinaryFT ||
				 TextDocument::OpenAsBinaryFile(fileName))
		{
			PrivateOpenBinaryDocument(fileName, iconify, forceReload);
		}
		else
		{
			PrivateOpenTextDocument(fileName, lineRange, iconify, forceReload, nullptr);
		}
	}
	else if (doc != nullptr && iconify)
	{
		doc->GetWindow()->Iconify();
	}
}

/******************************************************************************
 OpenTextDocument

	Open the given file, assuming that it is a text file.
	If lineIndex is not zero, we go to that line.

	Returns true if the file was successfully opened.  If doc != nullptr,
	*doc is set to the document.

 ******************************************************************************/

bool
DocumentManager::OpenTextDocument
	(
	const JString&	fileName,
	const JIndex	lineIndex,
	TextDocument**	doc,
	const bool		iconify,
	const bool		forceReload
	)
{
	return OpenTextDocument(fileName, JIndexRange(lineIndex, lineIndex),
							doc, iconify, forceReload);
}

bool
DocumentManager::OpenTextDocument
	(
	const JString&		fileName,
	const JIndexRange&	lineRange,
	TextDocument**	doc,
	const bool		iconify,
	const bool		forceReload
	)
{
	if (doc != nullptr)
	{
		*doc = nullptr;
	}

	if (fileName.IsEmpty())
	{
		return false;
	}

	const JUtf8Byte* map[] =
	{
		"f", fileName.GetBytes()
	};

	const bool isFile = JFileExists(fileName);
	if (!isFile && JNameUsed(fileName))
	{
		const JString msg = JGetString("NotAFile::DocumentManager", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		return false;
	}
	else if (!isFile)
	{
		const JString msg = JGetString("FileDoesNotExistCreate::DocumentManager", map, sizeof(map));
		if (!JGetUserNotification()->AskUserYes(msg))
		{
			return false;
		}
		else
		{
			std::ofstream temp(fileName.GetBytes());
			if (!temp.good())
			{
				const JString msg = JGetString("CannotCreateFile::DocumentManager", map, sizeof(map));
				JGetUserNotification()->ReportError(msg);
				return false;
			}
		}
	}

	return PrivateOpenTextDocument(fileName, lineRange, iconify, forceReload, doc);
}

/******************************************************************************
 PrivateOpenTextDocument (private)

	lineIndex can be zero.

	Returns true if the file was successfully opened locally.
	If doc != nullptr, *doc is set to the document.

 ******************************************************************************/

bool
DocumentManager::PrivateOpenTextDocument
	(
	const JString&		fullName,
	const JIndexRange&	lineRange,
	const bool			iconify,
	const bool			forceReload,
	TextDocument**		returnDoc
	)
	const
{
	assert( !fullName.IsEmpty() );

	if (returnDoc != nullptr)
	{
		*returnDoc = nullptr;
	}

	if (itsEditTextLocalFlag)
	{
		JXFileDocument* doc;
		if (!FileDocumentIsOpen(fullName, &doc))
		{
			if (!WarnFileSize(fullName))
			{
				return false;
			}
			doc = jnew TextDocument(fullName);
		}
		assert( doc != nullptr );
		if (iconify && !doc->IsActive())
		{
			doc->GetWindow()->Iconify();
		}
		if (!iconify || !doc->IsActive())
		{
			doc->Activate();
		}

		auto* textDoc = dynamic_cast<TextDocument*>(doc);
		if (textDoc != nullptr)
		{
			textDoc->RevertIfChangedByOthers(forceReload);
			if (!lineRange.IsEmpty())
			{
				textDoc->SelectLines(lineRange);
			}
		}

		if (returnDoc != nullptr)
		{
			*returnDoc = textDoc;
		}
		return textDoc != nullptr;
	}
	else
	{
		JString cmd;
		if (!lineRange.IsEmpty())
		{
			cmd = itsEditTextFileLineCmd;
		}
		else
		{
			cmd = itsEditTextFileCmd;
		}

		const JString name = JPrepArgForExec(fullName);
		const JString lineIndexStr(lineRange.first, 0);

		JSubstitute sub;
		sub.IgnoreUnrecognized();
		sub.DefineVariable("f", name);
		sub.DefineVariable("l", lineIndexStr);
		sub.Substitute(&cmd);

		JSimpleProcess::Create(cmd, true);
		return false;
	}
}

/******************************************************************************
 WarnFileSize (static)

 ******************************************************************************/

bool
DocumentManager::WarnFileSize
	(
	const JString& fullName
	)
{
	JSize size;
	if (JGetFileLength(fullName, &size) && size > kMinWarnFileSize)
	{
		JString path, fileName;
		JSplitPathAndName(fullName, &path, &fileName);

		const JUtf8Byte* map[] =
		{
			"f", fileName.GetBytes()
		};
		const JString msg = JGetString("WarnFileTooLarge::DocumentManager", map, sizeof(map));
		if (!JGetUserNotification()->AskUserNo(msg))
		{
			return false;
		}
	}

	return true;
}

/******************************************************************************
 SaveTextDocuments

 ******************************************************************************/

bool
DocumentManager::SaveTextDocuments
	(
	const bool saveUntitled
	)
{
	if (!itsWarnBeforeSaveAllFlag ||
		JGetUserNotification()->AskUserNo(JGetString("WarnSaveAll::DocumentManager")))
	{
		itsTextNeedsSaveFlag = false;

		for (auto* doc : *itsTextDocuments)
		{
			if (IsCommandOutput(doc->GetFileType()))
			{
				auto& cmdDoc = dynamic_cast<CommandOutputDocument&>(*doc);
				if (cmdDoc.CommandRunning())
				{
					continue;
				}
			}

			if ((saveUntitled || doc->ExistsOnDisk()) && !doc->Save())
			{
				itsTextNeedsSaveFlag = true;
			}
		}
	}

	return !itsTextNeedsSaveFlag;
}

/******************************************************************************
 ReloadTextDocuments

 ******************************************************************************/

void
DocumentManager::ReloadTextDocuments
	(
	const bool force
	)
{
	const JSize count = itsTextDocuments->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		TextDocument* doc = itsTextDocuments->GetItem(i);
		doc->RevertIfChangedByOthers(force);
	}
}

/******************************************************************************
 CloseTextDocuments

 ******************************************************************************/

bool
DocumentManager::CloseTextDocuments()
{
	if (!itsWarnBeforeCloseAllFlag ||
		JGetUserNotification()->AskUserNo(JGetString("WarnCloseAll::DocumentManager")))
	{
		// This is safe because there are no dependencies between documents.

		JIndex i=1;
		while (i <= itsTextDocuments->GetItemCount())
		{
			TextDocument* doc = itsTextDocuments->GetItem(i);
			if (IsCommandOutput(doc->GetFileType()))
			{
				auto& cmdDoc = dynamic_cast<CommandOutputDocument&>(*doc);
				if (cmdDoc.CommandRunning())
				{
					i++;
					continue;
				}
			}

			if (!doc->Close())
			{
				break;
			}
		}
	}

	return itsTextDocuments->IsEmpty();
}

/******************************************************************************
 FileRenamed

 ******************************************************************************/

void
DocumentManager::FileRenamed
	(
	const JString& origFullName,
	const JString& newFullName
	)
{
	const JSize count = itsProjectDocuments->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		itsProjectDocuments->GetItem(i)->
			GetFileTree()->GetProjectRoot()->
				FileRenamed(origFullName, newFullName);
	}
}

/******************************************************************************
 StylerChanged

 ******************************************************************************/

void
DocumentManager::StylerChanged
	(
	JSTStyler* styler
	)
{
	const JSize count = itsTextDocuments->GetItemCount();

	JLatentPG pg;
	pg.FixedLengthProcessBeginning(count, JGetString("AdjustStylesProgress::DocumentManager"), false, true);

	for (JIndex i=1; i<=count; i++)
	{
		(itsTextDocuments->GetItem(i))->StylerChanged(styler);
		pg.IncrementProgress();
	}

	pg.ProcessFinished();
}

/******************************************************************************
 OpenBinaryDocument

	Open the given file, assuming that it is a binary file.

 ******************************************************************************/

void
DocumentManager::OpenBinaryDocument
	(
	const JString& fileName
	)
{
	if (!fileName.IsEmpty() && JFileExists(fileName))
	{
		PrivateOpenBinaryDocument(fileName, false, false);
	}
}

/******************************************************************************
 PrivateOpenBinaryDocument (private)

 ******************************************************************************/

void
DocumentManager::PrivateOpenBinaryDocument
	(
	const JString&	fullName,
	const bool	iconify,
	const bool	forceReload
	)
	const
{
	assert( !fullName.IsEmpty() );

	JString cmd;
	if (GetPrefsManager()->EditWithOtherProgram(fullName, &cmd))
	{
		JSimpleProcess::Create(cmd, true);
	}
/*	else if (itsEditBinaryLocalFlag)
	{
		// some day...
	}
*/	else
	{
		cmd = itsEditBinaryFileCmd;

		JSubstitute sub;
		sub.IgnoreUnrecognized();
		sub.DefineVariable("f", fullName);
		sub.Substitute(&cmd);

		JSimpleProcess::Create(cmd, true);
	}
}

/******************************************************************************
 OpenComplementFile

 ******************************************************************************/

bool
DocumentManager::OpenComplementFile
	(
	const JString&		fullName,
	const TextFileType	type,
	ProjectDocument*	projDoc,
	const bool			searchDirs
	)
{
	JString complName;
	if (GetComplementFile(fullName, type, &complName, projDoc, searchDirs))
	{
		return OpenTextDocument(complName);
	}
	else if (HasComplementType(type))
	{
		JString path, fileName;
		JSplitPathAndName(fullName, &path, &fileName);

		const JUtf8Byte* map[] =
		{
			"name", fileName.GetBytes()
		};
		const JString msg = JGetString("ComplFileNotFound::DocumentManager", map, sizeof(map));

		auto* prefsMgr = GetPrefsManager();
		JIndex suffixIndex;
		const JString origName =
			prefsMgr->GetDefaultComplementSuffix(fullName, type, &suffixIndex);

		auto* dlog = JXSaveFileDialog::Create(JGetString("SaveNewFilePrompt::DocumentManager"), origName, JString::empty, msg);
		if (dlog->DoDialog())
		{
			prefsMgr->SetDefaultComplementSuffix(suffixIndex, fullName);

			const JString fullName = dlog->GetFullName();
			std::ofstream temp(fullName.GetBytes());
			temp.close();
			return OpenTextDocument(fullName);
		}
	}

	return false;
}

/******************************************************************************
 Get open complement file

	Return the document for the source file corresponding to the given header file,
	or visa versa.

 ******************************************************************************/

bool
DocumentManager::GetOpenComplementFile
	(
	const JString&		inputName,
	const TextFileType	inputType,
	JXFileDocument**	doc
	)
	const
{
	TextFileType outputType;
	return GetComplementType(inputType, &outputType) &&
				(FindOpenComplementFile(inputName, outputType, doc) ||
				 (IsHeaderType(inputType) &&
				  FindOpenComplementFile(inputName, kDocumentationFT, doc)));
}

// private

bool
DocumentManager::FindOpenComplementFile
	(
	const JString&			inputName,
	const TextFileType	outputType,
	JXFileDocument**		doc
	)
	const
{
JIndex i;

	JString baseName, suffix;
	JSplitRootAndSuffix(inputName, &baseName, &suffix);

	JPtrArray<JString> suffixList(JPtrArrayT::kDeleteAll);
	GetPrefsManager()->GetFileSuffixes(outputType, &suffixList);
	const JSize suffixCount = suffixList.GetItemCount();

	// check full name of each document

	JString fullName;
	for (i=1; i<=suffixCount; i++)
	{
		fullName = baseName + *(suffixList.GetItem(i));
		if (FileDocumentIsOpen(fullName, doc))
		{
			return true;
		}
	}

	// check file name of each document, in case complement is in different directory

	JString path, base, name;
	JSplitPathAndName(baseName, &path, &base);
	const JSize docCount = itsTextDocuments->GetItemCount();
	for (i=1; i<=suffixCount; i++)
	{
		name = base + *(suffixList.GetItem(i));
		for (JIndex j=1; j<=docCount; j++)
		{
			TextDocument* d = itsTextDocuments->GetItem(j);
			if (d->GetFileName() == name)
			{
				*doc = d;
				return true;
			}
		}
	}

	return false;
}

/******************************************************************************
 Get complement file

	Get the name of the source file corresponding to the given header file,
	or visa versa.

	If projDoc == nullptr, the active project document is used.

 ******************************************************************************/

bool
DocumentManager::GetComplementFile
	(
	const JString&			inputName,
	const TextFileType	inputType,
	JString*				outputName,
	ProjectDocument*		projDoc,
	const bool			searchDirs
	)
	const
{
	TextFileType outputType;
	return GetComplementType(inputType, &outputType) &&
				(FindComplementFile(inputName, outputType, outputName, projDoc, searchDirs) ||
				 (IsHeaderType(inputType) &&
				  FindComplementFile(inputName, kDocumentationFT, outputName, projDoc, searchDirs)));
}

// private -- projDoc can be nullptr

bool
DocumentManager::FindComplementFile
	(
	const JString&			inputName,
	const TextFileType	outputType,
	JString*				outputName,
	ProjectDocument*		projDoc,
	const bool			searchDirs
	)
	const
{
	JString baseName, suffix;
	JSplitRootAndSuffix(inputName, &baseName, &suffix);

	JPtrArray<JString> suffixList(JPtrArrayT::kDeleteAll);
	GetPrefsManager()->GetFileSuffixes(outputType, &suffixList);
	const JSize suffixCount = suffixList.GetItemCount();

	bool found = false;
	for (JIndex i=1; i<=suffixCount; i++)
	{
		*outputName = baseName + *(suffixList.GetItem(i));
		if (JFileExists(*outputName))
		{
			found = true;
			break;
		}
	}

	if (!found && searchDirs &&
		(projDoc != nullptr || GetActiveProjectDocument(&projDoc)))
	{
		found = SearchForComplementFile(projDoc, inputName, baseName,
										suffixList, outputName);
	}

	return found;
}

/******************************************************************************
 SearchForComplementFile (private)

	Sort the project directories by how close they are to the given file
	and then search each one.

	e.g. starting with /a/b/c, we could get { /a/b/d, /a/f/g, /e/h }

 ******************************************************************************/

struct DirMatchInfo
{
	JString*	path;
	bool		recurse;
	JSize		matchLength;
};

static std::weak_ordering
	CompareMatchLengths(const DirMatchInfo& i1, const DirMatchInfo& i2);

bool
DocumentManager::SearchForComplementFile
	(
	ProjectDocument*			projDoc,
	const JString&				origFullName,
	const JString&				baseFullName,
	const JPtrArray<JString>&	suffixList,
	JString*					outputName
	)
	const
{
JIndex i;

	JString origPath, baseName;
	JSplitPathAndName(baseFullName, &origPath, &baseName);

	// sort the directories

	const DirList& origDirList = projDoc->GetDirectories();
	JSize dirCount               = origDirList.GetItemCount();
	if (dirCount == 0)
	{
		return false;
	}

	JArray<DirMatchInfo> dirList(dirCount);
	dirList.SetSortOrder(JListT::kSortDescending);
	dirList.SetCompareFunction(CompareMatchLengths);

	DirMatchInfo info;
	for (i=1; i<=dirCount; i++)
	{
		info.path = jnew JString;

		if (origDirList.GetTruePath(i, info.path, &info.recurse))
		{
			info.matchLength = JString::CalcCharacterMatchLength(origPath, *(info.path));
			dirList.InsertSorted(info);
		}
		else
		{
			jdelete info.path;
		}
	}

	dirCount = dirList.GetItemCount();
	if (dirCount == 0)
	{
		return false;
	}

	// search for each suffix in all directories

	bool found          = false;
	bool cancelled      = false;
	const JSize suffixCount = suffixList.GetItemCount();

	JLatentPG pg;
	JString origFilePath, origFileName;
	JSplitPathAndName(origFullName, &origFilePath, &origFileName);

	const JUtf8Byte* map[] =
	{
		"name", origFileName.GetBytes()
	};
	const JString msg = JGetString("ComplFileProgress::DocumentManager", map, sizeof(map));
	pg.FixedLengthProcessBeginning(suffixCount * dirCount, msg, true, true);

	JString searchName, newPath, newName;
	for (i=1; i<=suffixCount; i++)
	{
		searchName = baseName + *(suffixList.GetItem(i));
		for (JIndex j=1; j<=dirCount; j++)
		{
			const DirMatchInfo info = dirList.GetItem(j);
			if (info.recurse &&
				JSearchSubdirs(*info.path, searchName, true, JString::kCompareCase,
							   &newPath, &newName))
			{
				*outputName = JCombinePathAndName(newPath, newName);
				found       = true;
				break;
			}
			else if (!info.recurse)
			{
				const JString fullName = JCombinePathAndName(*(info.path), searchName);
				if (JFileExists(fullName))
				{
					*outputName = fullName;
					found       = true;
					break;
				}
			}

			if (!pg.IncrementProgress())
			{
				cancelled = true;
				break;
			}
		}

		if (found || cancelled)
		{
			break;
		}
	}

	pg.ProcessFinished();

	// clean up

	for (i=1; i<=dirCount; i++)
	{
		jdelete (dirList.GetItem(i)).path;
	}

	return found;
}

// static

std::weak_ordering
CompareMatchLengths
	(
	const DirMatchInfo& i1,
	const DirMatchInfo& i2
	)
{
	return i1.matchLength <=> i2.matchLength;
}

/******************************************************************************
 ReadFromProject

	This must be at the end of the file so it can be cancelled.

 ******************************************************************************/

void
DocumentManager::ReadFromProject
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	JSize textCount;
	input >> textCount;

	if (textCount > 0)
	{
		JXStandAlonePG pg;
		pg.RaiseWhenUpdate();
		pg.FixedLengthProcessBeginning(textCount, JGetString("OpeningFiles::MDIServer"), true, true);

		for (JIndex i=1; i<=textCount; i++)
		{
			bool keep;
			auto* doc = jnew TextDocument(input, vers, &keep);
			if (keep)
			{
				doc->Activate();
			}
			else
			{
				doc->Close();
			}

			if (!pg.IncrementProgress())
			{
				break;
			}
		}

		pg.ProcessFinished();
	}
}

/******************************************************************************
 WriteForProject

	This must be at the end of the file so it can be cancelled.

 ******************************************************************************/

void
DocumentManager::WriteForProject
	(
	std::ostream& output
	)
	const
{
	const JSize textCount = itsTextDocuments->GetItemCount();
	output << textCount;

	for (JIndex i=1; i<=textCount; i++)
	{
		output << ' ';
		TextDocument* doc = itsTextDocuments->GetItem(i);
		doc->WriteForProject(output);
	}
}

/******************************************************************************
 RestoreState

	Reopens files that were open when we quit the last time.  Returns false
	if nothing is opened.

 ******************************************************************************/

bool
DocumentManager::RestoreState
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentStateVersion)
	{
		return false;
	}

	JSize projCount;
	if (vers == 0)
	{
		bool projectWasOpen;
		input >> JBoolFromString(projectWasOpen);
		projCount = projectWasOpen ? 1 : 0;
	}
	else
	{
		input >> projCount;
	}

	JSize trueProjCount = 0;
	if (projCount > 0)
	{
		bool saveReopen = ProjectDocument::WillReopenTextFiles();
		if (kCurrentStateVersion >= 2)
		{
			ProjectDocument::ShouldReopenTextFiles(false);
		}

		bool onDisk = true;
		JString fileName;
		for (JIndex i=1; i<=projCount; i++)
		{
			if (vers >= 1)
			{
				input >> JBoolFromString(onDisk);
			}
			input >> fileName;
			if (onDisk && JFileReadable(fileName))
			{
				trueProjCount++;
				JXApplication::StartFiber([this, fileName]()
				{
					OpenSomething(fileName);
				},
				"DocumentManager::RestoreState");
			}
		}

		ProjectDocument::ShouldReopenTextFiles(saveReopen);
	}

	if (kCurrentStateVersion >= 2 || projCount == 0)
	{
		JFileVersion projVers;
		input >> projVers;
		if (projVers <= kCurrentProjectFileVersion)
		{
			ReadFromProject(input, projVers);
		}
	}

	return trueProjCount > 0 || HasTextDocuments();
}

/******************************************************************************
 SaveState

	Saves files that are currently open so they can be reopened next time.
	Returns false if there is nothing to save.

	Always calls WriteForProject() because project documents might be
	write protected, e.g., CVS.

 ******************************************************************************/

bool
DocumentManager::SaveState
	(
	std::ostream& output
	)
	const
{
	if (itsProjectDocuments->IsEmpty() &&
		itsTextDocuments->IsEmpty())
	{
		return false;
	}

	output << kCurrentStateVersion;

	const JSize projCount = itsProjectDocuments->GetItemCount();
	output << ' ' << projCount;

	if (projCount > 0)
	{
		bool onDisk;
		JString fullName;
		for (JIndex i=1; i<=projCount; i++)
		{
			fullName = (itsProjectDocuments->GetItem(i))->GetFullName(&onDisk);
			output << ' ' << JBoolToString(onDisk) << ' ' << fullName;
		}
	}

	output << ' ' << kCurrentProjectFileVersion;
	output << ' ';
	WriteForProject(output);

	return true;
}

/******************************************************************************
 Receive (virtual protected)

	By calling TextDocument::UpdateFileType() here, we don't have to store
	another copy of the document list in JBroadcaster, and we display a
	progress display, since it can take a while.

 ******************************************************************************/

void
DocumentManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsProjectDocuments && message.Is(JListT::kItemsInserted))
	{
		auto& info = dynamic_cast<const JListT::ItemsInserted&>(message);
		if (info.Contains(1))
		{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
		}
	}
	else if (sender == itsProjectDocuments && message.Is(JListT::kItemsRemoved))
	{
		auto& info = dynamic_cast<const JListT::ItemsRemoved&>(message);
		if (info.Contains(1))
		{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
		}
	}
	else if (sender == itsProjectDocuments && message.Is(JListT::kItemMoved))
	{
		auto& info = dynamic_cast<const JListT::ItemMoved&>(message);
		if (info.GetOrigIndex() == 1 || info.GetNewIndex() == 1)
		{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
		}
	}
	else if (sender == itsProjectDocuments && message.Is(JListT::kItemsSwapped))
	{
		auto& info = dynamic_cast<const JListT::ItemsSwapped&>(message);
		if (info.GetIndex1() == 1 || info.GetIndex2() == 1)
		{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
		}
	}
	else if (sender == itsProjectDocuments && message.Is(JListT::kItemsChanged))
	{
		auto& info = dynamic_cast<const JListT::ItemsChanged&>(message);
		if (info.Contains(1))
		{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
		}
	}
	else if (sender == itsProjectDocuments && message.Is(JListT::kSorted))
	{
		assert_msg( 0, "not allowed" );
	}

	else if (sender == GetPrefsManager() &&
			 message.Is(PrefsManager::kFileTypesChanged))
	{
		const JSize count = itsTextDocuments->GetItemCount();
		if (count > 0)
		{
			JLatentPG pg;
			pg.FixedLengthProcessBeginning(count, JGetString("UpdateTypesProgress::DocumentManager"),
										   false, true);

			for (JIndex i=1; i<=count; i++)
			{
				(itsTextDocuments->GetItem(i))->UpdateFileType();
				pg.IncrementProgress();
			}

			pg.ProcessFinished();
		}
	}

	else
	{
		JXDocumentManager::Receive(sender, message);
	}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

static const JUtf8Byte* kVarNameList = "fl";

inline void
trimBkgdFlag
	(
	JString* str
	)
{
	str->TrimWhitespace();
	if (str->EndsWith("&"))
	{
		JStringIterator iter(str, JStringIterator::kStartAtEnd);
		iter.RemovePrev();

		str->TrimWhitespace();	// invalidates iter
	}
}

void
DocumentManager::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
	{
		return;
	}

	input >> JBoolFromString(itsEditTextLocalFlag);
	input >> itsEditTextFileCmd;
	input >> itsEditTextFileLineCmd;

	trimBkgdFlag(&itsEditTextFileCmd);
	trimBkgdFlag(&itsEditTextFileLineCmd);

	if (vers < 3)
	{
		JXWebBrowser::ConvertVarNames(&itsEditTextFileCmd,     kVarNameList);
		JXWebBrowser::ConvertVarNames(&itsEditTextFileLineCmd, kVarNameList);
	}

	if (vers >= 5)
	{
		input >> JBoolFromString(itsEditBinaryLocalFlag);
		input >> itsEditBinaryFileCmd;
	}

	if (vers >= 1)
	{
		input >> JBoolFromString(itsWarnBeforeSaveAllFlag)
			  >> JBoolFromString(itsWarnBeforeCloseAllFlag);
	}

	if (4 <= vers && vers < 6)
	{
		bool updateSymbolDBAfterBuild;
		input >> JBoolFromString(updateSymbolDBAfterBuild);
	}

	if (vers >= 2)
	{
		itsRecentProjectMenu->ReadSetup(input);
		itsRecentTextMenu->ReadSetup(input);
	}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
DocumentManager::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ' << JBoolToString(itsEditTextLocalFlag);
	output << ' ' << itsEditTextFileCmd;
	output << ' ' << itsEditTextFileLineCmd;

	output << ' ' << JBoolToString(itsEditBinaryLocalFlag);
	output << ' ' << itsEditBinaryFileCmd;

	output << ' ' << JBoolToString(itsWarnBeforeSaveAllFlag)
				  << JBoolToString(itsWarnBeforeCloseAllFlag);

	output << ' ';
	itsRecentProjectMenu->WriteSetup(output);

	output << ' ';
	itsRecentTextMenu->WriteSetup(output);
}

/******************************************************************************
 ChooseEditors

 ******************************************************************************/

void
DocumentManager::ChooseEditors()
{
	auto* dlog =
		jnew ExtEditorDialog(itsEditTextLocalFlag,
							 itsEditTextFileCmd, itsEditTextFileLineCmd,
							 itsEditBinaryLocalFlag, itsEditBinaryFileCmd);

	if (dlog->DoDialog())
	{
		dlog->GetPrefs(&itsEditTextLocalFlag, &itsEditTextFileCmd,
					   &itsEditTextFileLineCmd,
					   &itsEditBinaryLocalFlag, &itsEditBinaryFileCmd);
	}
}

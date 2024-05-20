/******************************************************************************
 ProjectDocument.cpp

	BASE CLASS = JXFileDocument

	Copyright © 1996-2001 by John Lindal.

 ******************************************************************************/

#include "ProjectDocument.h"
#include "ProjectTable.h"
#include "ProjectTree.h"
#include "ProjectNode.h"
#include "FileListDirector.h"
#include "FileListTable.h"
#include "SymbolDirector.h"
#include "SymbolList.h"
#include "CTreeDirector.h"
#include "CTree.h"
#include "CPreprocessor.h"
#include "DTreeDirector.h"
#include "GoTreeDirector.h"
#include "JavaTreeDirector.h"
#include "PHPTreeDirector.h"
#include "SymbolUpdatePG.h"
#include "FileHistoryMenu.h"
#include "CommandManager.h"
#include "CommandMenu.h"
#include "CompileDocument.h"
#include "EditSearchPathsDialog.h"
#include "EditProjPrefsDialog.h"
#include "ViewManPageDialog.h"
#include "FindFileDialog.h"
#include "DiffFileDialog.h"
#include "SearchTextDialog.h"
#include "SaveNewProjectDialog.h"
#include "PTPrinter.h"
#include "DirList.h"
#include "DocumentMenu.h"
#include "fileVersions.h"
#include "globals.h"

#include <jx-af/jx/JXApplication.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXSaveFileDialog.h>
#include <jx-af/jx/JXProgressDisplay.h>
#include <jx-af/jx/JXProgressIndicator.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXFunctionTask.h>
#include <jx-af/jx/JXProgressDisplay.h>
#include <jx-af/jx/JXImage.h>

#include <jx-af/jcore/JNamedTreeList.h>
#include <jx-af/jcore/JTreeNode.h>
#include <jx-af/jcore/JMemoryManager.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jSysUtil.h>
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

bool ProjectDocument::theReopenTextFilesFlag    = true;
bool ProjectDocument::theWarnOpenOldVersionFlag = false;
JString ProjectDocument::theAddFilesFilter;

static const JUtf8Byte* kProjectFileSignature = "jx_browser_data";
const JSize kProjectFileSignatureLength       = strlen(kProjectFileSignature);
static const JUtf8Byte* kProjectFileSuffix    = ".jcc";

static const JString kDataDirectory(".jcc");

static const JUtf8Byte* kSettingFileSignature = "jx_browser_local_settings";
const JSize kSettingFileSignatureLength       = strlen(kSettingFileSignature);
static const JString kSettingFileName("prefs");

static const JUtf8Byte* kSymbolFileSignature  = "jx_browser_symbol_table";
const JSize kSymbolFileSignatureLength        = strlen(kSymbolFileSignature);
static const JString kSymbolFileName("symbols");

static const JUtf8Byte* kProjTemplateDir      = "project_templates";
static const JUtf8Byte* kTmplFileSignature    = "jx_browser_project_template";
const JSize kTmplFileSignatureLength          = strlen(kTmplFileSignature);
static const JUtf8Byte* kWizardFileSignature  = "jx_browser_project_wizard";
const JSize kWizardFileSignatureLength        = strlen(kWizardFileSignature);

const JSize kSafetySavePeriod          = 600000;	// 10 minutes (ms)
const JSize kUpdateCheckInterval       = 100;		// ms
const JSize kSymbolUpdateScaleFactor   = 10;
const JFloat kSymbolUpdateSaveFraction = 0.05;		// fraction of changed files that triggers save

// catch crashes during child process parsing

class ChildAssertHandler : public JAssertBase
{
	int Assert(const JUtf8Byte* expr, const JUtf8Byte* file, const int line, const JUtf8Byte* message, const JUtf8Byte* function) override
	{
		return JAssertBase::DefaultAssert(expr, file, line, message, function);
	}
};

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

void
ProjectDocument::Create()
{
	auto* dlog =
		SaveNewProjectDialog::Create(JGetString("SaveFilePrompt::ProjectDocument"),
									 JGetString("NewFileName::ProjectDocument"));
	if (!dlog->DoDialog())
	{
		return;
	}

	JString tmplFile, tmplType;
	const bool fromTemplate = dlog->GetProjectTemplate(&tmplFile);
	if (fromTemplate)
	{
		const bool ok = GetProjectTemplateType(tmplFile, &tmplType);
		assert( ok );
	}

	const JString meta = JCombinePathAndName(dlog->GetPath(), kDataDirectory);
	if (!JKillDirectory(meta))
	{
		JGetUserNotification()->ReportError(JGetString("MetaDirectoryAlreadyExists::ProjectDocument"));
		return;
	}

	if (!fromTemplate || tmplType == kTmplFileSignature)
	{
		const JString fullName = dlog->GetFullName();
		std::ofstream temp(fullName.GetBytes());
		if (!temp.good())
		{
			JGetUserNotification()->ReportError(JGetString("FileCreateFailed::ProjectDocument"));
			return;
		}
		temp.close();

		auto* doc = jnew ProjectDocument(fullName, dlog->GetMakefileMethod(),
										 fromTemplate, tmplFile);
		doc->SaveInCurrentFile();
		doc->Activate();
	}
	else
	{
		assert( tmplType == kWizardFileSignature );

		JString name = dlog->GetFileName();
		if (dlog->GetFullName().EndsWith(kProjectFileSuffix))
		{
			JString root, suffix;
			JSplitRootAndSuffix(name, &root, &suffix);
			name = root;
		}

		std::ifstream input(tmplFile.GetBytes());
		input.ignore(kWizardFileSignatureLength);

		JFileVersion vers;
		input >> vers;
		assert( vers <= kCurrentProjectWizardVersion );

		JString cmd;
		JReadAll(input, &cmd);
		cmd.TrimWhitespace();

		const JUtf8Byte* map[] =
		{
			"path", dlog->GetPath().GetBytes(),
			"name", name.GetBytes()
		};
		JGetStringManager()->Replace(&cmd, map, sizeof(map));

		pid_t pid;
		const JError err = JExecute(cmd, &pid);
		if (!err.OK())
		{
			JGetStringManager()->ReportError("WizardExecError::ProjectDocument", err);
		}
	}
}

JXFileDocument::FileStatus
ProjectDocument::Create
	(
	const JString&		fullName,
	const bool			silent,
	ProjectDocument**	doc
	)
{
	assert( !fullName.IsEmpty() );

	// make sure we use the project file, not the symbol file

	JString projName = fullName;

	// legacy

	JString path, suffix;
	JSplitRootAndSuffix(fullName, &path, &suffix);

	if (suffix == ".jst" || suffix == ".jup" || suffix == BuildManager::GetSubProjectBuildSuffix())
	{
		projName = JCombineRootAndSuffix(path, kProjectFileSuffix);
	}

	JString name = JCombineRootAndSuffix(path, ".jst");
	if (JFileExists(name))
	{
		const JString symName = GetSymbolFileName(fullName);
		JRenameFile(name, symName);
	}

	name = JCombineRootAndSuffix(path, ".jup");
	if (JFileExists(name))
	{
		const JString setName = GetSettingFileName(fullName);
		JRenameFile(name, setName);
	}

	// open the file

	JXFileDocument* baseDoc;
	if (GetDocumentManager()->FileDocumentIsOpen(projName, &baseDoc))
	{
		*doc = dynamic_cast<ProjectDocument*>(baseDoc);
		if (*doc != nullptr && !silent)
		{
			(**doc).Activate();
		}
		return (*doc != nullptr ? kFileReadable : kNotMyFile);
	}

	*doc = nullptr;

	std::ifstream input(projName.GetBytes());
	JFileVersion vers;
	const FileStatus status = CanReadFile(input, &vers);
	if (status == kFileReadable &&
		(
		 !theWarnOpenOldVersionFlag ||
		 vers == kCurrentProjectFileVersion ||
		 JGetUserNotification()->AskUserYes(JGetString("WarnOldVersion::ProjectDocument"))))
	{
		JString testContent;
		JReadFile(projName, &testContent);
		if (testContent.Contains("<<<<<<<") &&
			testContent.Contains("=======") &&
			testContent.Contains(">>>>>>>"))
		{
			JGetUserNotification()->ReportError(JGetString("VCSConflict::ProjectDocument"));
			return kNotMyFile;
		}

		GetApplication()->DisplayBusyCursor();

		const JString symName = GetSymbolFileName(fullName);
		const JString setName = GetSettingFileName(fullName);

		*doc = jnew ProjectDocument(input, projName, setName, symName, silent);
	}
	else if (status == kNeedNewerVersion)
	{
		JGetUserNotification()->ReportError(JGetString("NeedNewerVersion::ProjectDocument"));
	}

	return status;
}

/******************************************************************************
 GetTemplateDirectoryName (static)

 ******************************************************************************/

const JUtf8Byte*
ProjectDocument::GetTemplateDirectoryName()
{
	return kProjTemplateDir;
}

/******************************************************************************
 GetProjectTemplateType (static)

 ******************************************************************************/

bool
ProjectDocument::GetProjectTemplateType
	(
	const JString&	fullName,
	JString*		type
	)
{
	JFileVersion actualFileVersion;
	std::ifstream input(fullName.GetBytes());
	FileStatus status =
		DefaultCanReadASCIIFile(input, kTmplFileSignature, kCurrentProjectTmplVersion,
								&actualFileVersion);
	if (status == kFileReadable)
	{
		*type = kTmplFileSignature;
		return true;
	}
	else if (status == kNeedNewerVersion)
	{
		type->Clear();
		return false;
	}
	else
	{
		status = DefaultCanReadASCIIFile(input, kWizardFileSignature,
										 kCurrentProjectWizardVersion,
										 &actualFileVersion);
		if (status == kFileReadable)
		{
			*type = kWizardFileSignature;
			return true;
		}
		else
		{
			type->Clear();
			return false;
		}
	}
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

ProjectDocument::ProjectDocument
	(
	const JString&						fullName,
	const BuildManager::MakefileMethod	makefileMethod,
	const bool							fromTemplate,
	const JString&						tmplFile
	)
	:
	JXFileDocument(GetApplication(),
				   fullName, true, false, kProjectFileSuffix)
{
	std::ifstream* input = nullptr;
	JFileVersion tmplVers, projVers;
	if (fromTemplate)
	{
		input = jnew std::ifstream(tmplFile.GetBytes());
		input->ignore(kTmplFileSignatureLength);

		*input >> tmplVers;
		assert( tmplVers <= kCurrentProjectTmplVersion );

		*input >> projVers;
		assert( projVers <= kCurrentProjectFileVersion );

		itsFileTree = jnew ProjectTree(*input, projVers, this);
	}
	else
	{
		itsFileTree = jnew ProjectTree(this);
	}

	ProjectDocumentX(itsFileTree);

	itsBuildMgr = jnew BuildManager(this);

	itsAllFileDirector = jnew FileListDirector(this);

	itsSymbolDirector = jnew SymbolDirector(this);

	itsCTreeDirector = jnew CTreeDirector(this);
	itsTreeDirectorList->Append(itsCTreeDirector);

	itsDTreeDirector = jnew DTreeDirector(this);
	itsTreeDirectorList->Append(itsDTreeDirector);

	itsGoTreeDirector = jnew GoTreeDirector(this);
	itsTreeDirectorList->Append(itsGoTreeDirector);

	itsJavaTreeDirector = jnew JavaTreeDirector(this);
	itsTreeDirectorList->Append(itsJavaTreeDirector);

	itsPHPTreeDirector = jnew PHPTreeDirector(this);
	itsTreeDirectorList->Append(itsPHPTreeDirector);

	if (fromTemplate)
	{
		ReadTemplate(*input, tmplVers, projVers);
	}
	else
	{
		JString path, name, targetName, suffix;
		JSplitPathAndName(fullName, &path, &name);
		JSplitRootAndSuffix(name, &targetName, &suffix);
		itsBuildMgr->SetBuildTargetName(targetName);

		itsBuildMgr->CreateMakeFiles(makefileMethod);
	}

	UpdateSymbolDatabase();
	ListenTo(itsFileTree);

	GetDocumentManager()->ProjDocCreated(this);

	jdelete input;
}

ProjectDocument::ProjectDocument
	(
	std::istream&	projInput,
	const JString&	projName,
	const JString&	setName,
	const JString&	symName,
	const bool		silent
	)
	:
	JXFileDocument(GetApplication(),
				   projName, true, false, kProjectFileSuffix)
{
	projInput.ignore(kProjectFileSignatureLength);

	JFileVersion projVers;
	projInput >> projVers;
	assert( projVers <= kCurrentProjectFileVersion );

	// open the setting file

	std::ifstream setStream(setName.GetBytes());

	const JString setSignature = JRead(setStream, kSettingFileSignatureLength);

	JFileVersion setVers;
	setStream >> setVers;

	std::istream* setInput = nullptr;
	if (setSignature == kSettingFileSignature &&
		setVers <= kCurrentProjectFileVersion &&
		setVers >= projVers)
	{
		setInput = &setStream;
	}

	const bool useProjSetData = setInput == nullptr || setVers < 71;

	// open the symbol file

	std::ifstream symStream(symName.GetBytes());

	const JString symSignature = JRead(symStream, kSymbolFileSignatureLength);

	JFileVersion symVers;
	symStream >> symVers;

	std::istream* symInput = nullptr;
	if (symSignature == kSymbolFileSignature &&
		symVers <= kCurrentProjectFileVersion &&
		symVers >= projVers)
	{
		symInput = &symStream;
	}

	// read in project file tree

	if (projVers >= 32)
	{
		if (projVers >= 71)
		{
			projInput >> std::ws;
			JIgnoreLine(projInput);
		}
		itsFileTree = jnew ProjectTree(projInput, projVers, this);
	}
	else
	{
		itsFileTree = jnew ProjectTree(this);
	}

	ProjectDocumentX(itsFileTree);

	if (!useProjSetData)
	{
		GetWindow()->ReadGeometry(*setInput);
	}
	if (33 <= projVers && projVers < 71 && useProjSetData)
	{
		GetWindow()->ReadGeometry(projInput);
	}
	else if (33 <= projVers && projVers < 71)
	{
		JXWindow::SkipGeometry(projInput);
	}
	GetWindow()->Deiconify();

	itsFileTable->ReadSetup(projInput, projVers, setInput, setVers);

	if (projVers >= 62)
	{
		if (projVers >= 71)
		{
			projInput >> std::ws;
			JIgnoreLine(projInput);
		}
		itsCmdMgr->ReadSetup(projInput);
	}

	itsBuildMgr = jnew BuildManager(projInput, projVers, setInput, setVers, this);

	if (36 <= projVers && projVers < 71)
	{
		projInput >> itsPrintName;
	}
	if (!useProjSetData)	// overwrite
	{
		*setInput >> itsPrintName;
	}

	// create file list

	itsAllFileDirector = jnew FileListDirector(projInput, projVers, setInput, setVers,
												 symInput, symVers, this, silent);

	// create symbol list

	itsSymbolDirector = jnew SymbolDirector(projInput, projVers, setInput, setVers,
											  symInput, symVers, this, silent);

	// read C++ class tree

	itsCTreeDirector = jnew CTreeDirector(projInput, projVers, setInput, setVers,
											symInput, symVers, this, silent, itsDirList);
	// activates itself
	itsTreeDirectorList->Append(itsCTreeDirector);

	// read D & Go class trees

	if (projVers >= 88)
	{
		itsDTreeDirector = jnew DTreeDirector(projInput, projVers, setInput, setVers,
												symInput, symVers, this, silent);
		// activates itself

		itsGoTreeDirector = jnew GoTreeDirector(projInput, projVers, setInput, setVers,
												  symInput, symVers, this, silent);
		// activates itself
	}
	else
	{
		itsDTreeDirector = jnew DTreeDirector(this);
		itsDTreeDirector->GetTree()->NextUpdateMustReparseAll();

		itsGoTreeDirector = jnew GoTreeDirector(this);
		itsGoTreeDirector->GetTree()->NextUpdateMustReparseAll();
	}
	itsTreeDirectorList->Append(itsDTreeDirector);
	itsTreeDirectorList->Append(itsGoTreeDirector);

	// read Java class tree

	if (projVers >= 48)
	{
		itsJavaTreeDirector = jnew JavaTreeDirector(projInput, projVers, setInput, setVers,
													  symInput, symVers, this, silent);
		// activates itself
	}
	else
	{
		itsJavaTreeDirector = jnew JavaTreeDirector(this);
		itsJavaTreeDirector->GetTree()->NextUpdateMustReparseAll();
	}
	itsTreeDirectorList->Append(itsJavaTreeDirector);

	// read PHP class tree

	if (projVers >= 85)
	{
		itsPHPTreeDirector = jnew PHPTreeDirector(projInput, projVers, setInput, setVers,
												   symInput, symVers, this, silent);
		// activates itself
	}
	else
	{
		itsPHPTreeDirector = jnew PHPTreeDirector(this);
		itsPHPTreeDirector->GetTree()->NextUpdateMustReparseAll();
	}
	itsTreeDirectorList->Append(itsPHPTreeDirector);

	// this must be last so it can be cancelled

	DocumentManager* docMgr = GetDocumentManager();
	if (!silent && theReopenTextFilesFlag)
	{
		if (projVers >= 2)
		{
			docMgr->DocumentMustStayOpen(this, true);		// stay open if editor is closed

			if (projVers < 71 && useProjSetData)
			{
				GetDocumentManager()->ReadFromProject(projInput, projVers);
			}
			else if (!useProjSetData)
			{
				GetDocumentManager()->ReadFromProject(*setInput, setVers);
			}
		}

		Activate();		// stay open
		docMgr->DocumentMustStayOpen(this, false);
	}
	else if (!silent)
	{
		Activate();
	}

	UpdateSymbolDatabase();
	ListenTo(itsFileTree);

	docMgr->ProjDocCreated(this);
}

// private

void
ProjectDocument::ProjectDocumentX
	(
	ProjectTree* fileList
	)
{
	itsProcessNodeMessageFlag = true;
	itsIsUpdatingFlag         = false;

	itsDirList = jnew DirList;
	itsDirList->SetBasePath(GetFilePath());

	itsCmdMgr = jnew CommandManager;

	ListenTo(GetPrefsManager());

	itsSaveTask = jnew JXFunctionTask(kSafetySavePeriod, [this]()
	{
		if (itsIsUpdatingFlag)
		{
			return;
		}

		bool onDisk;
		const JString fullName = GetFullName(&onDisk);
		if (onDisk && !JFileWritable(fullName))
		{
			JEditVCS(fullName);
		}
		WriteFile(fullName, false);		// always save .jst/.jup, even if .jcc not writable
		DataReverted(false);			// update file modification time
	},
	"ProjectDocument::PeriodicSave");
	itsSaveTask->Start();

	itsTreeDirectorList = jnew JPtrArray<TreeDirector>(JPtrArrayT::kForgetAll);

	itsUpdatePG              = nullptr;
	itsDelaySymbolUpdateTask = nullptr;

	SetSaveNewFilePrompt(JGetString("SaveFilePrompt::ProjectDocument"));

	BuildWindow(fileList);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ProjectDocument::~ProjectDocument()
{
	assert( !itsIsUpdatingFlag );
	assert( !CompleterUpdateRunning() );

	GetDocumentManager()->ProjDocDeleted(this);

	assert( !itsUpdatePG->ProcessRunning() );
	jdelete itsUpdatePG;

	jdelete itsCmdMgr;
	jdelete itsBuildMgr;
	jdelete itsFileTree;
	jdelete itsDirList;
	jdelete itsSaveTask;
	jdelete itsTreeDirectorList;
}

/******************************************************************************
 OKToClose (virtual protected)

	Update Make.files and .pro so user can build without running Code Crusader.

 ******************************************************************************/

bool
ProjectDocument::OKToClose()
{
	if (itsIsUpdatingFlag || CompleterUpdateRunning())
	{
		Activate();
		JGetUserNotification()->ReportError(
			JGetString("NoCloseWhileUpdating::ProjectDocument"));
		return false;
	}

	itsBuildMgr->UpdateMakeFiles(false);

	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (true)//NeedsSave() && onDisk && !JFileWritable(fullName))
	{
		JEditVCS(fullName);
		if (JFileWritable(fullName) && SaveInCurrentFile())
		{
			DataReverted(false);
		}
		else
		{
			DataModified();
		}
	}

	return JXFileDocument::OKToClose();
}

/******************************************************************************
 WriteFile (virtual protected)

	We override the default implementation because the project file might
	be read-only (e.g. CVS), but the symbol table still needs to be written.

 ******************************************************************************/

JError
ProjectDocument::WriteFile
	(
	const JString&	fullName,
	const bool		safetySave
	)
	const
{
	std::ofstream output(fullName.GetBytes());
	WriteTextFile(output, safetySave);
	if (output.good())
	{
		return JNoError();
	}
	else
	{
		return WriteFailed();
	}
}

/******************************************************************************
 WriteTextFile (virtual protected)

 ******************************************************************************/

inline void
writeSpace
	(
	std::ostream& projOutput,
	std::ostream* setOutput,
	std::ostream* symOutput
	)
{
	if (setOutput != nullptr)
	{
		*setOutput << ' ';
	}
	if (symOutput != nullptr)
	{
		*symOutput << ' ';
	}
}

void
ProjectDocument::WriteTextFile
	(
	std::ostream&	projOutput,
	const bool		safetySave
	)
	const
{
	JXGetApplication()->DisplayBusyCursor();

	// create the setting and symbol files

	JString setName, symName;
	std::ostream* setOutput = nullptr;
	std::ostream* symOutput = nullptr;
	if (!safetySave)
	{
		bool onDisk;
		setName   = GetSettingFileName(GetFullName(&onDisk));
		setOutput = jnew std::ofstream(setName.GetBytes());
		symName   = GetSymbolFileName(GetFullName(&onDisk));
		symOutput = jnew std::ofstream(symName.GetBytes());
	}

	WriteFiles(projOutput, setName, setOutput, symName, symOutput);
}

/******************************************************************************
 WriteFiles (private)

 ******************************************************************************/

void
ProjectDocument::WriteFiles
	(
	std::ostream&	projOutput,
	const JString&	setName,
	std::ostream*	setOutput,
	const JString&	symName,
	std::ostream*	symOutput
	)
	const
{
	// headers

	projOutput << kProjectFileSignature << ' ';
	projOutput << kCurrentProjectFileVersion << '\n';

	if (setOutput != nullptr)
	{
		*setOutput << kSettingFileSignature;
		*setOutput << ' ' << kCurrentProjectFileVersion;
	}

	if (symOutput != nullptr)
	{
		*symOutput << kSymbolFileSignature;
		*symOutput << ' ' << kCurrentProjectFileVersion;
	}

	// data

	projOutput << "# project tree\n";
	itsFileTree->StreamOut(projOutput);

	if (setOutput != nullptr)
	{
		*setOutput << ' ';
		GetWindow()->WriteGeometry(*setOutput);
	}

	writeSpace(projOutput, setOutput, symOutput);
	itsFileTable->WriteSetup(projOutput, setOutput);

	projOutput << "# tasks\n";		// expected by ReadTasksFromProjectFile()
	itsCmdMgr->WriteSetup(projOutput);

	writeSpace(projOutput, setOutput, symOutput);
	itsBuildMgr->StreamOut(projOutput, setOutput);

	if (setOutput != nullptr)
	{
		*setOutput << ' ' << itsPrintName;
	}

	writeSpace(projOutput, setOutput, symOutput);
	itsAllFileDirector->StreamOut(projOutput, setOutput, symOutput);

	writeSpace(projOutput, setOutput, symOutput);
	itsSymbolDirector->StreamOut(projOutput, setOutput, symOutput);

	writeSpace(projOutput, setOutput, symOutput);
	itsCTreeDirector->StreamOut(projOutput, setOutput, symOutput, itsDirList);

	writeSpace(projOutput, setOutput, symOutput);
	itsDTreeDirector->StreamOut(projOutput, setOutput, symOutput,  nullptr);

	writeSpace(projOutput, setOutput, symOutput);
	itsGoTreeDirector->StreamOut(projOutput, setOutput, symOutput, nullptr);

	writeSpace(projOutput, setOutput, symOutput);
	itsJavaTreeDirector->StreamOut(projOutput, setOutput, symOutput, nullptr);

	writeSpace(projOutput, setOutput, symOutput);
	itsPHPTreeDirector->StreamOut(projOutput, setOutput, symOutput, nullptr);

	// this must be last so it can be cancelled

	if (setOutput != nullptr)
	{
		*setOutput << ' ';
		GetDocumentManager()->WriteForProject(*setOutput);
	}

	// clean up -- no problem if settings or symbol table can't be written

	if (setOutput != nullptr)
	{
		const bool ok = setOutput->good();
		jdelete setOutput;
		if (!ok && !setName.IsEmpty())
		{
			JRemoveFile(setName);
		}
	}

	if (symOutput != nullptr)
	{
		const bool ok = symOutput->good();
		jdelete symOutput;
		if (!ok && !symName.IsEmpty())
		{
			JRemoveFile(symName);
		}
	}
}

/******************************************************************************
 ReadTasksFromProjectFile (static)

	Returns true if the file is a project file.

 ******************************************************************************/

bool
ProjectDocument::ReadTasksFromProjectFile
	(
	std::istream&				input,
	CommandManager::CmdList*	cmdList
	)
{
	JFileVersion vers;
	const FileStatus status = CanReadFile(input, &vers);
	if (status == kFileReadable && vers < 71)
	{
		JGetUserNotification()->ReportError(JGetString("FileTooOld::ProjectDocument"));
		return true;
	}
	else if (status == kFileReadable)
	{
		bool foundDelimiter;
		JString makeDependCmd;

		JIgnoreUntil(input, "\n# tasks\n", &foundDelimiter);
		if (!foundDelimiter ||
			!CommandManager::ReadCommands(input, &makeDependCmd, 	cmdList))
		{
			JGetUserNotification()->ReportError(JGetString("InvalidProjectFile::ProjectDocument"));
		}
		return true;
	}
	else if (status == kNeedNewerVersion)
	{
		JGetUserNotification()->ReportError(JGetString("NeedNewerVersion::ProjectDocument"));
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 ConvertCompileRunDialogs

 ******************************************************************************/

void
ProjectDocument::ConvertCompileRunDialogs
	(
	std::istream&		projInput,
	const JFileVersion	projVers
	)
{
	itsCmdMgr->ConvertCompileDialog(projInput, projVers, itsBuildMgr, true);
	itsCmdMgr->ConvertRunDialog(projInput, projVers, true);
}

/******************************************************************************
 CanReadFile (static)

 ******************************************************************************/

JXFileDocument::FileStatus
ProjectDocument::CanReadFile
	(
	const JString& fullName
	)
{
	std::ifstream input(fullName.GetBytes());
	JFileVersion actualFileVersion;
	return CanReadFile(input, &actualFileVersion);
}

/******************************************************************************
 CanReadFile (static)

 ******************************************************************************/

JXFileDocument::FileStatus
ProjectDocument::CanReadFile
	(
	std::istream&	input,
	JFileVersion*	actualFileVersion
	)
{
	return DefaultCanReadASCIIFile(input, kProjectFileSignature,
								   kCurrentProjectFileVersion, actualFileVersion);
}

/******************************************************************************
 ReadTemplate (private)

 ******************************************************************************/

void
ProjectDocument::ReadTemplate
	(
	std::istream&			input,
	const JFileVersion	tmplVers,
	const JFileVersion	projVers
	)
{
	itsFileTree->CreateFilesForTemplate(input, tmplVers);
	itsFileTable->ReadSetup(input, projVers, nullptr, 0);

	BuildManager::MakefileMethod makefileMethod = BuildManager::kManual;
	if (tmplVers == 2)
	{
		input >> makefileMethod;
	}
	else if (tmplVers < 2)
	{
		bool shouldWriteMakeFilesFlag;
		input >> JBoolFromString(shouldWriteMakeFilesFlag);

		makefileMethod = (shouldWriteMakeFilesFlag ? BuildManager::kMakemake :
													 BuildManager::kManual);
	}

	JString targetName, depListExpr;
	if (tmplVers <= 2)
	{
		input >> targetName >> depListExpr;
	}

	itsDirList->ReadDirectories(input, projVers);
	itsCTreeDirector->GetCTree()->GetCPreprocessor()->ReadSetup(input, projVers);
	itsBuildMgr->ReadTemplate(input, tmplVers, projVers, makefileMethod,
							  targetName, depListExpr);
	itsCmdMgr->ReadTemplate(input, tmplVers, projVers);
}

/******************************************************************************
 WriteTemplate (private)

 ******************************************************************************/

void
ProjectDocument::WriteTemplate
	(
	const JString& fileName
	)
	const
{
	std::ofstream output(fileName.GetBytes());
	output << kTmplFileSignature;
	output << ' ' << kCurrentProjectTmplVersion;
	output << ' ' << kCurrentProjectFileVersion;

	output << ' ';
	itsFileTree->StreamOut(output);

	// everything above this line is read in the constructor

	output << ' ';
	itsFileTree->SaveFilesInTemplate(output);

	output << ' ';
	itsFileTable->WriteSetup(output, nullptr);

	output << ' ';
	itsDirList->WriteDirectories(output);

	output << ' ';
	itsCTreeDirector->GetCTree()->GetCPreprocessor()->WriteSetup(output);

	output << ' ';
	itsBuildMgr->WriteTemplate(output);

	output << ' ';
	itsCmdMgr->WriteTemplate(output);
}

/******************************************************************************
 GetSettingFileName (static private)

 ******************************************************************************/

JString
ProjectDocument::GetSettingFileName
	(
	const JString& fullName
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);
	name = JCombinePathAndName(path, kDataDirectory);
	JCreateDirectory(name);
	name = JCombinePathAndName(name, kSettingFileName);
	return name;
}

/******************************************************************************
 GetSymbolFileName (static private)

 ******************************************************************************/

JString
ProjectDocument::GetSymbolFileName
	(
	const JString& fullName
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);
	name = JCombinePathAndName(path, kDataDirectory);
	JCreateDirectory(name);
	name = JCombinePathAndName(name, kSymbolFileName);
	return name;
}

/******************************************************************************
 DiscardChanges (virtual protected)

	We only set NeedsSilentSave(), so this will never be called.

 ******************************************************************************/

void
ProjectDocument::DiscardChanges()
{
}

/******************************************************************************
 AddFile

 ******************************************************************************/

void
ProjectDocument::AddFile
	(
	const JString&					fullName,
	const ProjectTable::PathType	pathType
	)
{
	JString name = fullName;
	JPtrArray<JString> list(JPtrArrayT::kForgetAll);
	list.Append(&name);
	itsFileTable->AddFiles(list, pathType);
}

/******************************************************************************
 GetAllFileList

 ******************************************************************************/

FileListTable*
ProjectDocument::GetAllFileList()
	const
{
	return itsAllFileDirector->GetFileListTable();
}

/******************************************************************************
 RefreshVCSStatus

 ******************************************************************************/

void
ProjectDocument::RefreshVCSStatus()
{
	itsFileTable->Refresh();
}

/******************************************************************************
 SetTreePrefs

 ******************************************************************************/

void
ProjectDocument::SetTreePrefs
	(
	const JSize	fontSize,
	const bool	autoMinMILinks,
	const bool	drawMILinksOnTop,
	const bool	raiseWhenSingleMatch,
	const bool	writePrefs
	)
{
	for (auto* director : *itsTreeDirectorList)
	{
		director->SetTreePrefs(fontSize,
							   autoMinMILinks, drawMILinksOnTop,
							   raiseWhenSingleMatch);

		if (writePrefs)
		{
			director->JPrefObject::WritePrefs();
		}
	}
}

/******************************************************************************
 EditMakeConfig

 ******************************************************************************/

void
ProjectDocument::EditMakeConfig()
{
	GetDocumentManager()->SetActiveProjectDocument(this);
	itsBuildMgr->EditMakeConfig();
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
ProjectDocument::Activate()
{
	JXFileDocument::Activate();

	if (IsActive())
	{
		GetDocumentManager()->SetActiveProjectDocument(this);
	}
}

/******************************************************************************
 GetName (virtual)

	Override of JXDocument::GetName().

 ******************************************************************************/

const JString&
ProjectDocument::GetName()
	const
{
	itsDocName = GetFileName();
	if (itsDocName.EndsWith(kProjectFileSuffix))
	{
		JString root, suffix;
		JSplitRootAndSuffix(itsDocName, &root, &suffix);
		itsDocName = root;
	}
	return itsDocName;
}

/******************************************************************************
 GetMenuIcon (virtual)

	Override of JXDocument::GetMenuIcon().

 ******************************************************************************/

bool
ProjectDocument::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	ProjectDocument* doc;
	*icon = GetProjectFileIcon(GetDocumentManager()->GetActiveProjectDocument(&doc) &&
							   doc == const_cast<ProjectDocument*>(this));
	return true;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "jcc_project_window.xpm"
#include "ProjectDocument-File.h"
#include "ProjectDocument-Project.h"
#include "ProjectDocument-Source.h"
#include "ProjectDocument-Preferences.h"

void
ProjectDocument::BuildWindow
	(
	ProjectTree* fileList
	)
{
	auto* treeList = jnew JNamedTreeList(fileList);

// begin JXLayout

	auto* window = jnew JXWindow(this, 510,430, JString::empty);
	window->SetMinSize(150, 150);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "Code_Crusader_Project");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,30);
	assert( menuBar != nullptr );

	itsConfigButton =
		jnew JXTextButton(JGetString("itsConfigButton::ProjectDocument::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 450,0, 60,30);

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kProjectToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 510,380);

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 510,380);

	itsFileTable =
		jnew ProjectTable(this, menuBar, treeList, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 510,380);

	itsUpdateContainer =
		jnew JXWidgetSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 0,410, 510,20);

	itsUpdateLabel =
		jnew JXStaticText(JGetString("itsUpdateLabel::ProjectDocument::JXLayout"), itsUpdateContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 90,20);
	itsUpdateLabel->SetToLabel(false);

	itsUpdateCounter =
		jnew JXStaticText(JGetString("itsUpdateCounter::ProjectDocument::JXLayout"), itsUpdateContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,0, 90,20);
	itsUpdateCounter->SetToLabel(false);

	itsUpdateCleanUpIndicator =
		jnew JXProgressIndicator(itsUpdateContainer,
					JXWidget::kHElastic, JXWidget::kFixedTop, 90,5, 420,10);

// end JXLayout

	AdjustWindowTitle();

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, jcc_project_window);
	window->SetIcon(icon);

	JPoint desktopLoc;
	JCoordinate w,h;
	if (GetPrefsManager()->GetWindowSize(kProjectWindSizeID, &desktopLoc, &w, &h))
	{
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
	}

	ListenTo(itsConfigButton);
	itsConfigButton->SetHint(JGetString("ConfigButtonHint::ProjectDocument"));

	itsUpdateContainer->ClearNeedsInternalFTC();

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("MenuTitle::ProjectDocument_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&ProjectDocument::UpdateFileMenu,
		&ProjectDocument::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	jnew FileHistoryMenu(DocumentManager::kProjectFileHistory,
						  itsFileMenu, kRecentProjectMenuCmd, menuBar);

	jnew FileHistoryMenu(DocumentManager::kTextFileHistory,
						  itsFileMenu, kRecentTextMenuCmd, menuBar);

	itsProjectMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::ProjectDocument_Project"));
	itsProjectMenu->SetMenuItems(kProjectMenuStr);
	itsProjectMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsProjectMenu->AttachHandlers(this,
		&ProjectDocument::UpdateProjectMenu,
		&ProjectDocument::HandleProjectMenu);
	ConfigureProjectMenu(itsProjectMenu);

	itsSourceMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::ProjectDocument_Source"));
	itsSourceMenu->SetMenuItems(kSourceMenuStr);
	itsSourceMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsSourceMenu->AttachHandlers(this,
		&ProjectDocument::UpdateSourceMenu,
		&ProjectDocument::HandleSourceMenu);
	ConfigureSourceMenu(itsSourceMenu);

	itsCmdMenu =
		jnew CommandMenu(this, nullptr, menuBar,
						  JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	menuBar->AppendMenu(itsCmdMenu);
	ListenTo(itsCmdMenu);

	auto* fileListMenu =
		jnew DocumentMenu(menuBar, JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	menuBar->AppendMenu(fileListMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::ProjectDocument_Preferences"));
	itsPrefsMenu->SetMenuItems(kPreferencesMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandlers(this,
		&ProjectDocument::UpdatePrefsMenu,
		&ProjectDocument::HandlePrefsMenu);
	ConfigurePreferencesMenu(itsPrefsMenu);

	JXTextMenu* helpMenu = GetApplication()->CreateHelpMenu(menuBar, "ProjectHelp");

	// must do this after creating widgets

	auto f = std::function(UpgradeToolBarID);
	itsToolBar->LoadPrefs(&f);
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kNewTextEditorCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenSomethingCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kPrintCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsProjectMenu, kSearchFilesCmd);

		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, helpMenu);
	}

	// update pg

	auto* pg = jnew JXProgressDisplay;
	pg->SetItems(nullptr, itsUpdateCounter, itsUpdateCleanUpIndicator, itsUpdateLabel);

	itsUpdatePG = jnew SymbolUpdatePG(pg, kSymbolUpdateScaleFactor,
									  itsToolBar, itsUpdateContainer);
	itsUpdatePG->Hide();
}

/******************************************************************************
 UpgradeToolBarID (static private)

 ******************************************************************************/

void
ProjectDocument::UpgradeToolBarID
	(
	JString* s
	)
{
	if (s->StartsWith("CB") && !CommandMenu::UpgradeToolBarID(s))
	{
		JStringIterator iter(s);
		iter.SkipNext(2);
		iter.RemoveAllPrev();
		*s += "::ProjectDocument";
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
ProjectDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetPrefsManager() &&
		 message.Is(PrefsManager::kFileTypesChanged))
	{
		auto& info = dynamic_cast<const PrefsManager::FileTypesChanged&>(message);
		if (info.AnyChanged())
		{
			itsBuildMgr->ProjectChanged();
			itsSymbolDirector->FileTypesChanged(info);
			for (auto* director : *itsTreeDirectorList)
			{
				director->FileTypesChanged(info);
			}
			UpdateSymbolDatabase();

			itsFileTable->Refresh();	// update icons
		}
	}

	else if (sender == itsFileTree &&
			 (message.Is(JTree::kNodeInserted) ||
			  message.Is(JTree::kNodeRemoved)))
	{
		if (itsProcessNodeMessageFlag)
		{
			ProcessNodeMessage(message);
		}
	}
	else if (sender == itsFileTree && message.Is(JTree::kNodeDeleted))
	{
		itsBuildMgr->ProjectChanged();
	}
	else if (sender == itsFileTree && message.Is(JTree::kNodeChanged))
	{
		ProcessNodeMessage(message);
	}
	else if (sender == itsFileTree && message.Is(JTree::kPrepareForNodeMove))
	{
		itsProcessNodeMessageFlag = false;
	}
	else if (sender == itsFileTree && message.Is(JTree::kNodeMoveFinished))
	{
		itsProcessNodeMessageFlag = true;
		itsBuildMgr->ProjectChanged();
	}

	else if (sender == itsConfigButton && message.Is(JXButton::kPushed))
	{
		EditMakeConfig();
	}

	else
	{
		JXFileDocument::Receive(sender, message);
	}
}

/******************************************************************************
 ProcessNodeMessage (private)

 ******************************************************************************/

void
ProjectDocument::ProcessNodeMessage
	(
	const Message& message
	)
{
	auto& info = dynamic_cast<const JTree::NodeMessage&>(message);
	if (info.GetNode()->GetDepth() == ProjectTable::kFileDepth)
	{
		auto& node = dynamic_cast<const ProjectNode&>(*info.GetNode());
		itsBuildMgr->ProjectChanged(&node);

		if (message.Is(JTree::kNodeChanged))
		{
			SymbolDatabaseNeedsUpdate();
		}
	}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
ProjectDocument::UpdateFileMenu()
{
	const bool canPrint = itsFileTable->GetRowCount() > 0;
	itsFileMenu->SetItemEnabled(kPrintCmd, canPrint);
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
ProjectDocument::HandleFileMenu
	(
	const JIndex index
	)
{
	GetDocumentManager()->SetActiveProjectDocument(this);

	if (!itsFileTable->EndEditing())
	{
		return;
	}

	if (index == kNewTextEditorCmd)
	{
		GetDocumentManager()->NewTextDocument();
	}
	else if (index == kNewTextTemplateCmd)
	{
		GetDocumentManager()->NewTextDocumentFromTemplate();
	}
	else if (index == kNewProjectCmd)
	{
		GetDocumentManager()->NewProjectDocument();
	}

	else if (index == kOpenSomethingCmd)
	{
		GetDocumentManager()->OpenSomething();
	}

	else if (index == kSaveCmd)
	{
		bool onDisk;
		const JString fullName = GetFullName(&onDisk);
		if (onDisk && !JFileWritable(fullName))
		{
			JEditVCS(fullName);
		}
		SaveInCurrentFile();
	}
	else if (index == kSaveAsTemplateCmd)
	{
		SaveAsTemplate();
	}

	else if (index == kPageSetupCmd)
	{
		GetPTTextPrinter()->EditUserPageSetup();
	}
	else if (index == kPrintCmd)
	{
		PTPrinter* p = GetPTTextPrinter();
		p->SetFileName(itsPrintName);
		if (p->ConfirmUserPrintSetup())
		{
			bool onDisk;
			const JString fullName = GetFullName(&onDisk);
			p->SetHeaderName(fullName);

			itsPrintName = p->GetFileName();
			Print(*p);
		}
	}

	else if (index == kCloseCmd)
	{
		Close();
	}
	else if (index == kQuitCmd)
	{
		JXGetApplication()->Quit();
	}
}

/******************************************************************************
 SaveAsTemplate (private)

 ******************************************************************************/

void
ProjectDocument::SaveAsTemplate()
	const
{
	JString origName;
	if (!GetDocumentManager()->GetTemplateDirectory(kProjTemplateDir, true, &origName))
	{
		return;
	}
	origName = JCombinePathAndName(origName, GetFileName());

	auto* dlog = JXSaveFileDialog::Create(JGetString("SaveTmplPrompt::ProjectDocument"), origName);
	if (dlog->DoDialog())
	{
		WriteTemplate(dlog->GetFullName());
	}
}

/******************************************************************************
 Print (private)

 ******************************************************************************/

void
ProjectDocument::Print
	(
	JPTPrinter& p
	)
	const
{
	const JUtf8Byte* map[] =
	{
		"name", GetFileName().GetBytes(),
		"path", GetFilePath().GetBytes()
	};
	JString s = JGetString("PrintHeader::ProjectDocument", map, sizeof(map));

	itsFileTree->Print(&s);

	p.Print(s);
}

/******************************************************************************
 UpdateProjectMenu (private)

 ******************************************************************************/

void
ProjectDocument::UpdateProjectMenu()
{
	itsProjectMenu->SetItemEnabled(kUpdateMakefileCmd,
		itsBuildMgr->GetMakefileMethod() != BuildManager::kManual);

	itsProjectMenu->SetItemEnabled(kShowCTreeCmd,
		!itsCTreeDirector->GetTree()->IsEmpty());
	itsProjectMenu->SetItemEnabled(kShowDTreeCmd,
		!itsDTreeDirector->GetTree()->IsEmpty());
	itsProjectMenu->SetItemEnabled(kShowGoTreeCmd,
		!itsGoTreeDirector->GetTree()->IsEmpty());
	itsProjectMenu->SetItemEnabled(kShowJavaTreeCmd,
		!itsJavaTreeDirector->GetTree()->IsEmpty());
	itsProjectMenu->SetItemEnabled(kShowPHPTreeCmd,
		!itsPHPTreeDirector->GetTree()->IsEmpty());
}

/******************************************************************************
 HandleProjectMenu (private)

 ******************************************************************************/

void
ProjectDocument::HandleProjectMenu
	(
	const JIndex index
	)
{
	GetDocumentManager()->SetActiveProjectDocument(this);

	if (!itsFileTable->EndEditing())
	{
		return;
	}

	if (index == kOpenMakeConfigDlogCmd)
	{
		itsBuildMgr->EditProjectConfig();
	}
	else if (index == kUpdateMakefileCmd)
	{
		itsBuildMgr->UpdateMakefile(nullptr, nullptr, true);
	}

	else if (index == kUpdateSymbolDBCmd)
	{
		UpdateSymbolDatabase();
	}
	else if (index == kShowSymbolBrowserCmd)
	{
		itsSymbolDirector->Activate();
	}
	else if (index == kShowCTreeCmd)
	{
		itsCTreeDirector->Activate();
	}
	else if (index == kShowDTreeCmd)
	{
		itsDTreeDirector->Activate();
	}
	else if (index == kShowGoTreeCmd)
	{
		itsGoTreeDirector->Activate();
	}
	else if (index == kShowJavaTreeCmd)
	{
		itsJavaTreeDirector->Activate();
	}
	else if (index == kShowPHPTreeCmd)
	{
		itsPHPTreeDirector->Activate();
	}
	else if (index == kViewManPageCmd)
	{
		GetViewManPageDialog()->Activate();
	}

	else if (index == kEditSearchPathsCmd)
	{
		EditSearchPaths();
	}
	else if (index == kShowFileListCmd)
	{
		itsAllFileDirector->Activate();
	}
	else if (index == kFindFileCmd)
	{
		GetFindFileDialog()->Activate();
	}
	else if (index == kSearchFilesCmd)
	{
		GetSearchTextDialog()->Activate();
	}
	else if (index == kDiffFilesCmd)
	{
		GetDiffFileDialog()->Activate();
	}
}

/******************************************************************************
 EditSearchPaths

 ******************************************************************************/

void
ProjectDocument::EditSearchPaths
	(
	const JPtrArray<JString>* dirList
	)
{
	auto* dlog = jnew EditSearchPathsDialog(*itsDirList);

	if (dirList != nullptr)
	{
		dlog->AddDirectories(*dirList);
	}

	if (dlog->DoDialog())
	{
		DirList pathList;
		dlog->GetPathList(&pathList);
		if (pathList != *itsDirList)
		{
			*itsDirList = pathList;
			UpdateSymbolDatabase();
		}
	}
}

/******************************************************************************
 UpdateSourceMenu (private)

 ******************************************************************************/

void
ProjectDocument::UpdateSourceMenu()
{
	ProjectTable::SelType selType;
	bool single;
	JIndex index;
	const bool hasSelection = itsFileTable->GetSelectionType(&selType, &single, &index);
	if (hasSelection && selType == ProjectTable::kFileSelection)
	{
		itsSourceMenu->SetItemText(kOpenFilesCmd, JGetString("OpenFilesItemText::ProjectDocument"));

		itsSourceMenu->EnableItem(kRemoveSelCmd);
		itsSourceMenu->EnableItem(kOpenFilesCmd);
		itsSourceMenu->EnableItem(kOpenComplFilesCmd);
		itsSourceMenu->SetItemEnabled(kEditPathCmd, single);
		itsSourceMenu->SetItemEnabled(kEditSubprojConfigCmd,
			single && (itsFileTable->GetProjectNode(index))->GetType() == kLibraryNT);
		itsSourceMenu->EnableItem(kDiffSmartCmd);
		itsSourceMenu->EnableItem(kDiffVCSCmd);
		itsSourceMenu->EnableItem(kShowLocationCmd);
	}
	else if (hasSelection)
	{
		itsSourceMenu->SetItemText(kOpenFilesCmd, JGetString("EditGroupNameItemText::ProjectDocument"));

		itsSourceMenu->EnableItem(kRemoveSelCmd);
		itsSourceMenu->SetItemEnabled(kOpenFilesCmd, single);
		itsSourceMenu->DisableItem(kOpenComplFilesCmd);
		itsSourceMenu->DisableItem(kEditPathCmd);
		itsSourceMenu->DisableItem(kEditSubprojConfigCmd);
		itsSourceMenu->EnableItem(kDiffSmartCmd);
		itsSourceMenu->EnableItem(kDiffVCSCmd);
		itsSourceMenu->EnableItem(kShowLocationCmd);
	}
	else
	{
		itsSourceMenu->SetItemText(kOpenFilesCmd, JGetString("OpenFilesItemText::ProjectDocument"));

		itsSourceMenu->DisableItem(kRemoveSelCmd);
		itsSourceMenu->DisableItem(kOpenFilesCmd);
		itsSourceMenu->DisableItem(kOpenComplFilesCmd);
		itsSourceMenu->DisableItem(kEditPathCmd);
		itsSourceMenu->DisableItem(kEditSubprojConfigCmd);
		itsSourceMenu->DisableItem(kDiffSmartCmd);
		itsSourceMenu->DisableItem(kDiffVCSCmd);
		itsSourceMenu->DisableItem(kShowLocationCmd);
	}

	itsSourceMenu->SetItemEnabled(kCloseAllTextCmd,
								 GetDocumentManager()->HasTextDocuments());
	itsSourceMenu->SetItemEnabled(kSaveAllTextCmd,
								 GetDocumentManager()->TextDocumentsNeedSave());
}

/******************************************************************************
 HandleSourceMenu (private)

 ******************************************************************************/

void
ProjectDocument::HandleSourceMenu
	(
	const JIndex index
	)
{
	GetDocumentManager()->SetActiveProjectDocument(this);

	if (!itsFileTable->EndEditing())
	{
		return;
	}

	if (index == kNewGroupCmd)
	{
		itsFileTable->NewGroup();
	}
	else if (index == kAddFilesCmd)
	{
		itsFileTable->AddFiles();
	}
	else if (index == kAddDirTreeCmd)
	{
		itsFileTable->AddDirectoryTree();
	}
	else if (index == kRemoveSelCmd)
	{
		itsFileTable->RemoveSelection();
	}

	else if (index == kOpenFilesCmd)
	{
		itsFileTable->OpenSelection();
	}
	else if (index == kOpenComplFilesCmd)
	{
		itsFileTable->OpenComplementFiles();
	}

	else if (index == kEditPathCmd)
	{
		itsFileTable->EditFilePath();
	}
	else if (index == kEditSubprojConfigCmd)
	{
		itsFileTable->EditSubprojectConfig();
	}

	else if (index == kDiffSmartCmd)
	{
		itsFileTable->PlainDiffSelection();
	}
	else if (index == kDiffVCSCmd)
	{
		itsFileTable->VCSDiffSelection();
	}
	else if (index == kShowLocationCmd)
	{
		itsFileTable->ShowSelectedFileLocations();
	}

	else if (index == kSaveAllTextCmd)
	{
		GetDocumentManager()->SaveTextDocuments(true);
	}
	else if (index == kCloseAllTextCmd)
	{
		GetDocumentManager()->CloseTextDocuments();
	}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
ProjectDocument::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
ProjectDocument::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kProjectPrefsCmd)
	{
		EditProjectPrefs();
	}
	else if (index == kToolBarPrefsCmd)
	{
		itsToolBar->Edit();
	}
	else if (index == kEditFileTypesCmd)
	{
		GetPrefsManager()->EditFileTypes();
	}
	else if (index == kChooseExtEditorsCmd)
	{
		GetDocumentManager()->ChooseEditors();
	}
	else if (index == kShowLocationPrefsCmd)
	{
		(JXGetWebBrowser())->EditPrefs();
	}
	else if (index == kMiscPrefsCmd)
	{
		GetApplication()->EditMiscPrefs();
	}

	else if (index == kSaveWindSizeCmd)
	{
		GetPrefsManager()->SaveWindowSize(kProjectWindSizeID, GetWindow());
	}
}

/******************************************************************************
 EditProjectPrefs (private)

 ******************************************************************************/

void
ProjectDocument::EditProjectPrefs()
{
	auto* dlog =
		jnew EditProjPrefsDialog(theReopenTextFilesFlag,
								 CompileDocument::WillDoubleSpace(),
								 BuildManager::WillRebuildMakefileDaily(),
								 ProjectTable::GetDropFileAction());
	if (dlog->DoDialog())
	{
		dlog->UpdateSettings();
	}
}

/******************************************************************************
 SetProjectPrefs

 ******************************************************************************/

void
ProjectDocument::SetProjectPrefs
	(
	const bool							reopenTextFiles,
	const bool							doubleSpaceCompile,
	const bool							rebuildMakefileDaily,
	const ProjectTable::DropFileAction	dropFileAction
	)
{
	theReopenTextFilesFlag = reopenTextFiles;

	CompileDocument::ShouldDoubleSpace(doubleSpaceCompile);
	BuildManager::ShouldRebuildMakefileDaily(rebuildMakefileDaily);
	ProjectTable::SetDropFileAction(dropFileAction);
}

/******************************************************************************
 ReadStaticGlobalPrefs (static)

 ******************************************************************************/

void
ProjectDocument::ReadStaticGlobalPrefs
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	if (vers >= 51)
	{
		input >> JBoolFromString(theWarnOpenOldVersionFlag);
	}

	if (vers >= 19)
	{
		ProjectTable::DropFileAction action;
		input >> JBoolFromString(theReopenTextFilesFlag) >> action;
		ProjectTable::SetDropFileAction(action);
	}

	if (20 <= vers && vers <= 42)
	{
		bool beepAfterMake, beepAfterCompile;
		input >> JBoolFromString(beepAfterMake)
			  >> JBoolFromString(beepAfterCompile);
	}

	if (37 <= vers && vers <= 42)
	{
		bool raiseBeforeMake, raiseBeforeCompile, raiseBeforeRun;
		input >> JBoolFromString(raiseBeforeMake)
			  >> JBoolFromString(raiseBeforeCompile)
			  >> JBoolFromString(raiseBeforeRun);
	}

	if (vers >= 42)
	{
		bool doubleSpaceCompile;
		input >> JBoolFromString(doubleSpaceCompile);
		CompileDocument::ShouldDoubleSpace(doubleSpaceCompile);
	}

	if (vers >= 46)
	{
		bool rebuildMakefileDaily;
		input >> JBoolFromString(rebuildMakefileDaily);
		BuildManager::ShouldRebuildMakefileDaily(rebuildMakefileDaily);
	}

	if (vers >= 47)
	{
		input >> theAddFilesFilter;
	}
}

/******************************************************************************
 WriteStaticGlobalPrefs (static)

 ******************************************************************************/

void
ProjectDocument::WriteStaticGlobalPrefs
	(
	std::ostream& output
	)
{
	output << JBoolToString(theWarnOpenOldVersionFlag)
		   << JBoolToString(theReopenTextFilesFlag);

	output << ' ' << ProjectTable::GetDropFileAction();

	output << ' ' << JBoolToString(CompileDocument::WillDoubleSpace())
				  << JBoolToString(BuildManager::WillRebuildMakefileDaily());

	output << ' ' << theAddFilesFilter;
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
ProjectDocument::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	if (sender == itsCmdMenu && message->Is(CommandMenu::kGetTargetInfo))
	{
		auto& info = dynamic_cast<CommandMenu::GetTargetInfo&>(*message);
		itsFileTable->GetSelectedFileNames(info.GetFileList());
	}
	else
	{
		JXFileDocument::ReceiveWithFeedback(sender, message);
	}
}

/******************************************************************************
 GetProjectFileSuffix (static)

 ******************************************************************************/

const JUtf8Byte*
ProjectDocument::GetProjectFileSuffix()
{
	return kProjectFileSuffix;
}

/******************************************************************************
 FindSymbol

 ******************************************************************************/

bool
ProjectDocument::FindSymbol
	(
	const JString&		name,
	const JString&		fileName,
	const JXMouseButton	button
	)
{
	return (!itsIsUpdatingFlag &&
			itsSymbolDirector->FindSymbol(name, fileName, button));
}

/******************************************************************************
 SymbolDatabaseNeedsUpdate

	This is used when we expect more user actions that would trigger yet
	another update.

 ******************************************************************************/

void
ProjectDocument::SymbolDatabaseNeedsUpdate()
{
	if (itsDelaySymbolUpdateTask == nullptr)
	{
		itsDelaySymbolUpdateTask = jnew JXFunctionTask(kUpdateCheckInterval, [this]()
		{
			if (!itsIsUpdatingFlag && !CompleterUpdateRunning())
			{
				jdelete itsDelaySymbolUpdateTask;
				itsDelaySymbolUpdateTask = nullptr;
				UpdateSymbolDatabase();
			}
		},
		"ProjectDocument::SymbolDatabaseNeedsUpdate");
		itsDelaySymbolUpdateTask->Start();
	}
}

/******************************************************************************
 UpdateSymbolDatabase

 ******************************************************************************/

void
ProjectDocument::UpdateSymbolDatabase()
{
	if (itsIsUpdatingFlag)
	{
		SymbolDatabaseNeedsUpdate();
		return;
	}

	// don't block when a dialog triggers it

	JXApplication::StartFiber([this]()
	{
		while (CompleterUpdateRunning())
		{
			boost::this_fiber::sleep_for(
				std::chrono::milliseconds(kUpdateCheckInterval));
		}

		SymbolUpdateStarted();
		itsIsUpdatingFlag = true;

		itsUpdateLabel->GetText()->SetText(JString::empty);
		itsFileTable->GetEditMenu()->Deactivate();
		itsProjectMenu->Deactivate();
		itsSourceMenu->Deactivate();

		if (itsAllFileDirector->GetFileListTable()->Update(
				*itsUpdatePG, itsFileTree, *itsDirList, itsSymbolDirector,
				*itsTreeDirectorList) >= kSymbolUpdateSaveFraction)
		{
			HandleFileMenu(kSaveCmd);
		}

		assert( !itsUpdatePG->ProcessRunning() );
		itsIsUpdatingFlag = false;
		SymbolUpdateFinished();

		itsFileTable->GetEditMenu()->Activate();
		itsProjectMenu->Activate();
		itsSourceMenu->Activate();
	},
	"ProjectDocument::UpdateSymbolDatabase");
}

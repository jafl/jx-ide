/******************************************************************************
 CommandManager.cpp

	Stores a list of user-defined commands.

	BASE CLASS = JPrefObject, virtual JBroadcaster

	Copyright © 2001-18 by John Lindal.

 ******************************************************************************/

#include "CommandManager.h"
#include "Command.h"
#include "ProjectDocument.h"
#include "TextDocument.h"
#include "TextEditor.h"
#include "CompileDocument.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXHistoryMenuBase.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jcore/JKLRand.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kDefaultMakeDependCmd = "echo Makefile must be updated manually";
JPtrArray<TextDocument> CommandManager::theExecDocList(JPtrArrayT::kForgetAll);

// setup information

const JFileVersion kCurrentSetupVersion = 4;

	// version  1 removes makeDepend flag, since Makefile now rebuilt once a day
	// version  2 saves menuID
	// version  3 saves isVCS
	// version  4 converts count to keepGoing, for CVS merge

// JBroadcaster message types

const JUtf8Byte* CommandManager::kUpdateCommandMenu = "UpdateCommandMenu::CommandManager";

/******************************************************************************
 Constructor

	Only globals actually invokes Read/WritePrefs().

 ******************************************************************************/

CommandManager::CommandManager
	(
	DocumentManager* docMgr
	)
	:
	JPrefObject(GetPrefsManager(), kGlobalCommandsID),
	itsMakeDependCmd(kDefaultMakeDependCmd),
	itsCompileDoc(nullptr)
{
	itsCmdList = jnew CmdList;
	assert( itsCmdList != nullptr );

	if (docMgr != nullptr)
	{
		InitCommandList();
		ListenTo(docMgr);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CommandManager::~CommandManager()
{
	itsCmdList->DeleteAll();
	jdelete itsCmdList;
}

/******************************************************************************
 GetMakeDependCmdStr

 ******************************************************************************/

bool
CommandManager::GetMakeDependCmdStr
	(
	ProjectDocument*	projDoc,
	const bool		reportError,
	JString*			cmdStr
	)
	const
{
	*cmdStr = itsMakeDependCmd;
	return Substitute(projDoc, reportError, cmdStr);
}

/******************************************************************************
 Substitute

 ******************************************************************************/

bool
CommandManager::Substitute
	(
	ProjectDocument*	projDoc,
	const bool		reportError,
	JString*			cmdStr
	)
	const
{
	assert( projDoc != nullptr );

	return Substitute(cmdStr, projDoc, JString::empty, 0, reportError);
}

/******************************************************************************
 MakeDepend

 ******************************************************************************/

bool
CommandManager::MakeDepend
	(
	ProjectDocument*		projDoc,
	ExecOutputDocument*	compileDoc,
	Command**				resultCmd
	)
{
	assert( projDoc != nullptr );

	JString cmdPath = projDoc->GetFilePath();
	CmdInfo info(&cmdPath, &itsMakeDependCmd, nullptr, nullptr, nullptr, nullptr);

	JPtrArray<JString> emptyFullNameList(JPtrArrayT::kForgetAll);
	JArray<JIndex> emptyLineIndexList;

	Command* cmd = nullptr;
	// subroutines not allowed
	if (Prepare(info, projDoc, emptyFullNameList, emptyLineIndexList, &cmd, nullptr) &&
		cmd->StartMakeProcess(compileDoc))
	{
		if (resultCmd != nullptr)
		{
			*resultCmd = cmd;
		}
		return true;
	}

	if (resultCmd != nullptr)
	{
		*resultCmd = nullptr;
	}
	return false;
}

/******************************************************************************
 Exec (textDoc)

	projDoc can be nullptr

 ******************************************************************************/

void
CommandManager::Exec
	(
	const JIndex		cmdIndex,
	ProjectDocument*	projDoc,
	TextDocument*		textDoc
	)
{
	const CmdInfo info = itsCmdList->GetElement(cmdIndex);
	Exec(info, projDoc, textDoc);
}

// static

void
CommandManager::Exec
	(
	const CmdInfo&		info,
	ProjectDocument*	projDoc,
	TextDocument*		textDoc
	)
{
	bool onDisk;
	JString fullName = textDoc->GetFullName(&onDisk);
	if (!onDisk && JIsAbsolutePath(fullName))
	{
		JString p,n;
		JSplitPathAndName(fullName, &p, &n);
		fullName = n;
	}

	JTextEditor* te        = textDoc->GetTextEditor();
	const JIndex lineIndex = te->VisualLineIndexToCRLineIndex(te->GetLineForChar(te->GetInsertionCharIndex()));

	JPtrArray<JString> fullNameList(JPtrArrayT::kForgetAll);
	fullNameList.Append(&fullName);

	JArray<JIndex> lineIndexList;
	lineIndexList.AppendElement(lineIndex);

	Command* cmd = nullptr;
	FunctionStack fnStack;
	if (Prepare(info, projDoc, fullNameList, lineIndexList, &cmd, &fnStack))
	{
		cmd->Start(info);
	}
}

/******************************************************************************
 Exec (file list)

	projDoc can be nullptr

 ******************************************************************************/

void
CommandManager::Exec
	(
	const JIndex				cmdIndex,
	ProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList
	)
{
	const CmdInfo info = itsCmdList->GetElement(cmdIndex);
	Exec(info, projDoc, fullNameList, lineIndexList);
}

// static

void
CommandManager::Exec
	(
	const CmdInfo&				info,
	ProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList
	)
{
	Command* cmd = nullptr;
	FunctionStack fnStack;
	if (Prepare(info, projDoc, fullNameList, lineIndexList, &cmd, &fnStack))
	{
		cmd->Start(info);
	}
}

/******************************************************************************
 Prepare

	projDoc can be nullptr

 ******************************************************************************/

bool
CommandManager::Prepare
	(
	const JString&				cmdName,
	ProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList,
	Command**					cmd,
	CmdInfo**					returnInfo,
	FunctionStack*				fnStack
	)
{
	*cmd        = nullptr;
	*returnInfo = nullptr;

	CmdInfo info;
	if (FindCommandName(cmdName, &info))
	{
		if (Prepare(info, projDoc, fullNameList, lineIndexList, cmd, fnStack))
		{
			*returnInfo = jnew CmdInfo;
			assert( *returnInfo != nullptr );
			**returnInfo = info.Copy();
		}
		return *cmd != nullptr;
	}
	else if (this != GetCommandManager())
	{
		return GetCommandManager()->Prepare(cmdName, projDoc, fullNameList,
											lineIndexList, cmd, returnInfo, fnStack);
	}
	else
	{
		const JUtf8Byte* map[] =
		{
			"cmd", cmdName.GetBytes()
		};
		const JString msg = JGetString("UnknownCmd::CommandManager", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		return false;
	}
}

// static private

bool
CommandManager::Prepare
	(
	const CmdInfo&				info,
	ProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList,
	Command**					cmd,
	FunctionStack*				fnStack
	)
{
	const bool hasLines = !lineIndexList.IsEmpty();
	assert( !hasLines || lineIndexList.GetElementCount() == fullNameList.GetElementCount() );

	const bool usesFiles = (info.cmd->Contains("$full_name")        ||
							info.cmd->Contains("$relative_name")    ||
							info.cmd->Contains("$file_name")        ||
							info.cmd->Contains("$file_name_root")   ||
							info.cmd->Contains("$file_name_suffix") ||
							info.cmd->Contains("$full_path")        ||
							info.cmd->Contains("$relative_path")    ||
							info.cmd->Contains("$line")             ||
							info.path->GetFirstCharacter() == '@');

	if (usesFiles && fullNameList.IsEmpty())
	{
		JGetUserNotification()->ReportError(JGetString("RequiresFile::CommandManager"));
		return false;
	}

	CmdQueue cmdQueue(JPtrArrayT::kDeleteAll);
	if (!Parse(*info.cmd, &cmdQueue, fnStack))
	{
		return false;
	}

	if (usesFiles && info.oneAtATime)
	{
		const JSize count = fullNameList.GetElementCount();
		JString cmdPath;
		for (JIndex i=1; i<=count; i++)
		{
			cmdPath = *info.path;

			JPtrArray<JString> subFullNameList(JPtrArrayT::kForgetAll);
			subFullNameList.Append(fullNameList.GetElement(i));

			JArray<JIndex> subLineIndexList;
			if (hasLines)
			{
				subLineIndexList.AppendElement(lineIndexList.GetElement(i));
			}

			if (BuildCmdPath(&cmdPath, projDoc, *fullNameList.GetElement(i), true) &&
				ProcessCmdQueue(cmdPath, cmdQueue, info, projDoc,
								subFullNameList, subLineIndexList,
								true, cmd, fnStack))
			{
				(**cmd).MarkEndOfSequence();
			}
			else
			{
				jdelete *cmd;
				*cmd = nullptr;
				break;
			}
		}
	}
	else if (usesFiles && info.path->GetFirstCharacter() == '@')
	{
		// group files in the same directory

		JPtrArray<JString> nameList(fullNameList, JPtrArrayT::kForgetAll);
		JArray<JIndex> lineList(lineIndexList);

		JPtrArray<JString> samePathNameList(JPtrArrayT::kForgetAll);
		JArray<JIndex> samePathLineList;

		JString cmdPath, filePath, p, n;
		while (!nameList.IsEmpty())
		{
			cmdPath = *info.path;
			samePathNameList.RemoveAll();
			samePathLineList.RemoveAll();
			filePath.Clear();

			JSize nameCount = nameList.GetElementCount();
			for (JIndex i=1; i<=nameCount; i++)
			{
				const JString* fullName = nameList.GetElement(i);
				JSplitPathAndName(*fullName, &p, &n);
				if (filePath.IsEmpty() || p == filePath)
				{
					samePathNameList.Append(const_cast<JString*>(fullName));
					nameList.RemoveElement(i);

					if (hasLines)
					{
						samePathLineList.AppendElement(lineList.GetElement(i));
						lineList.RemoveElement(i);
					}

					filePath = p;
					nameCount--;
					i--;
				}
			}

			if (BuildCmdPath(&cmdPath, projDoc, *samePathNameList.GetFirstElement(), true) &&
				ProcessCmdQueue(cmdPath, cmdQueue, info, projDoc,
								samePathNameList, samePathLineList,
								true, cmd, fnStack))
			{
				(**cmd).MarkEndOfSequence();
			}
			else
			{
				jdelete *cmd;
				*cmd = nullptr;
				break;
			}
		}
	}
	else
	{
		JString cmdPath = *info.path;
		if (BuildCmdPath(&cmdPath, projDoc, JString::empty, true))
		{
			ProcessCmdQueue(cmdPath, cmdQueue, info, projDoc,
							fullNameList, lineIndexList,
							true, cmd, fnStack);
		}
	}

	return *cmd != nullptr;
}

/******************************************************************************
 Parse (static private)

	fnStack is used to detect infinite loops.

 ******************************************************************************/

bool
CommandManager::Parse
	(
	const JString&		origCmd,
	CmdQueue*			cmdQueue,
	FunctionStack*	fnStack
	)
{
	if (origCmd.IsEmpty())
	{
		return false;
	}

	JPtrArray<JString> argList(JPtrArrayT::kDeleteAll);
	JParseArgsForExec(origCmd, &argList);

	if (!argList.IsEmpty() && *argList.GetLastElement() != ";")
	{
		argList.Append(JString(";", JString::kNoCopy));	// catch all commands inside loop
	}

	JPtrArray<JString> cmdArgs(JPtrArrayT::kDeleteAll);

	while (!argList.IsEmpty())
	{
		JString* arg = argList.GetFirstElement();
		if (*arg == ";")
		{
			if (!cmdArgs.IsEmpty())
			{
				auto* a = jnew JPtrArray<JString>(cmdArgs, JPtrArrayT::kDeleteAll);
				assert( a != nullptr );
				cmdArgs.RemoveAll();
				cmdQueue->Append(a);
			}

			argList.DeleteElement(1);
		}
		else if (!arg->IsEmpty() && arg->GetFirstCharacter() == '&' &&
				 cmdArgs.IsEmpty())
		{
			if (fnStack == nullptr)
			{
				JGetUserNotification()->ReportError(JGetString("FnsNotAllowed::CommandManager"));
				return false;
			}
			else if (*argList.GetElement(2) != ";")
			{
				JGetUserNotification()->ReportError(JGetString("ArgsNotAllowed::CommandManager"));
				return false;
			}

			auto* a = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
			assert( a != nullptr );
			a->Append(arg);
			cmdQueue->Append(a);

			argList.RemoveElement(1);
		}
		else
		{
			cmdArgs.Append(arg);
			argList.RemoveElement(1);
		}
	}

	return true;
}

/******************************************************************************
 ProcessCmdQueue (static private)

 ******************************************************************************/

bool
CommandManager::ProcessCmdQueue
	(
	const JString&				cmdPath,
	const CmdQueue&			cmdQueue,
	const CmdInfo&				info,
	ProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList,
	const bool				reportError,
	Command**					cmd,
	FunctionStack*			fnStack
	)
{
	JPtrArray<JString> args(JPtrArrayT::kDeleteAll);

	const JSize nameCount = fullNameList.GetElementCount();

	const bool hasLines = !lineIndexList.IsEmpty();
	assert( !hasLines || lineIndexList.GetElementCount() == fullNameList.GetElementCount() );

	const JSize cmdCount = cmdQueue.GetElementCount();
	for (JIndex i=1; i<=cmdCount; i++)
	{
		args.CleanOut();

		const JPtrArray<JString>* cmdArgs = cmdQueue.GetElement(i);
		const JSize argCount = cmdArgs->GetElementCount();
		for (JIndex j=1; j<=argCount; j++)
		{
			const JString* cmdArg = cmdArgs->GetElement(j);

			if (fullNameList.IsEmpty())
			{
				auto* arg = jnew JString(*cmdArg);
				assert( arg != nullptr );

				if (!Substitute(arg, projDoc, JString::empty, 0, reportError))
				{
					jdelete *cmd;
					*cmd = nullptr;
					return false;
				}

				args.Append(arg);
			}
			else
			{
				for (JIndex k=1; k<=nameCount; k++)
				{
					auto* arg = jnew JString(*cmdArg);
					assert( arg != nullptr );

					if (!Substitute(arg, projDoc,
									*(fullNameList.GetElement(k)),
									(hasLines ? lineIndexList.GetElement(k) : 0),
									reportError))
					{
						jdelete *cmd;
						*cmd = nullptr;
						return false;
					}

					args.Append(arg);
					if (!UsesFile(*cmdArg))
					{
						break;
					}
				}
			}
		}

		if (!Add(cmdPath, args, info, projDoc, fullNameList, lineIndexList, cmd, fnStack))
		{
			return false;
		}
	}

	return true;
}

/******************************************************************************
 Add (static private)

 ******************************************************************************/

bool
CommandManager::Add
	(
	const JString&				path,
	const JPtrArray<JString>&	cmdArgs,
	const CmdInfo&				info,
	ProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList,
	Command**					cmd,
	FunctionStack*			fnStack
	)
{
	if (*cmd == nullptr)
	{
		*cmd = jnew Command(path, info.isVCS, info.beepWhenFinished, projDoc);
		assert( *cmd != nullptr );
	}

	if (path != (**cmd).GetPath())
	{
		auto* subCmd = jnew Command(path, info.isVCS, false, projDoc);
		if (subCmd->Add(cmdArgs, fullNameList, lineIndexList, fnStack))
		{
			(**cmd).Add(subCmd, info);
			return true;
		}
		else
		{
			jdelete subCmd;
			jdelete *cmd;
			*cmd = nullptr;
			return false;
		}
	}
	else if ((**cmd).Add(cmdArgs, fullNameList, lineIndexList, fnStack))
	{
		return true;
	}
	else
	{
		jdelete *cmd;
		*cmd = nullptr;
		return false;
	}
}

/******************************************************************************
 BuildCmdPath (static private)

 ******************************************************************************/

bool
CommandManager::BuildCmdPath
	(
	JString*			cmdPath,
	ProjectDocument*	projDoc,
	const JString&		fullName,
	const bool			reportError
	)
{
	const bool onDisk = !fullName.IsEmpty() && JIsAbsolutePath(fullName);

	if (!cmdPath->IsEmpty() && cmdPath->GetFirstCharacter() == '@')
	{
		if (!onDisk)
		{
			if (reportError)
			{
				JGetUserNotification()->ReportError(
					JGetString(onDisk ? "RequiresFile::CommandManager" : "MustSaveText::CommandManager"));
			}
			return false;
		}

		JString p, n;
		JSplitPathAndName(fullName, &p, &n);

		JStringIterator iter(cmdPath);
		iter.RemoveNext();
		iter.Invalidate();
		cmdPath->Prepend(p);

		JCleanPath(cmdPath);
	}

	if (projDoc == nullptr && JIsRelativePath(*cmdPath))
	{
		if (reportError)
		{
			JGetUserNotification()->ReportError(JGetString("RequiresProject::CommandManager"));
		}
		return false;
	}
	else
	{
		JString basePath;
		if (projDoc != nullptr)
		{
			basePath = projDoc->GetFilePath();
		}

		JString fullPath;
		if (!JConvertToAbsolutePath(*cmdPath, basePath, &fullPath) ||
			!JDirectoryExists(fullPath))
		{
			if (reportError)
			{
				const JUtf8Byte* map[] =
				{
					"path", cmdPath->GetBytes()
				};
				const JString msg = JGetString("InvalidPath::CommandManager", map, sizeof(map));
				JGetUserNotification()->ReportError(msg);
			}
			return false;
		}

		*cmdPath = fullPath;
		return true;
	}
}

/******************************************************************************
 Substitute (static private)

 ******************************************************************************/

bool
CommandManager::Substitute
	(
	JString*			arg,
	ProjectDocument*	projDoc,
	const JString&		fullName,
	const JIndex		lineIndex,
	const bool		reportError
	)
{
	const bool onDisk = !fullName.IsEmpty() && JIsAbsolutePath(fullName);

	JString projectPath, projectName, programName;
	if (projDoc != nullptr)
	{
		projectPath = projDoc->GetFilePath();
		projectName = projDoc->GetName();
		programName = projDoc->GetBuildTargetName();
	}
	else if (arg->Contains("$project_path")  ||
			 arg->Contains("$project_name")  ||
			 arg->Contains("$program")       ||
			 arg->Contains("$relative_name") ||
			 arg->Contains("$relative_path"))
	{
		if (reportError)
		{
			JGetUserNotification()->ReportError(JGetString("RequiresProject::CommandManager"));
		}
		return false;
	}

	if (!onDisk && UsesFile(*arg))
	{
		if (reportError)
		{
			JGetUserNotification()->ReportError(
				JGetString(fullName.IsEmpty() ?
					"RequiresFile::CommandManager" :
					"MustSaveText::CommandManager"));
		}
		return false;
	}

	JString fullPath, fileName, relativePath, relativeName,
			fileNameRoot, fileNameSuffix;
	if (onDisk)
	{
		JSplitPathAndName(fullName, &fullPath, &fileName);

		if (!projectPath.IsEmpty())
		{
			relativePath = JConvertToRelativePath(fullPath, projectPath);
			relativeName = JConvertToRelativePath(fullName, projectPath);
		}

		if (JSplitRootAndSuffix(fileName, &fileNameRoot, &fileNameSuffix))
		{
			fileNameSuffix.Prepend(".");
		}
	}

	const JString lineIndexStr(lineIndex, 0);

	const JUtf8Byte* map[] =
	{
		"project_path",     projectPath.GetBytes(),
		"project_name",     projectName.GetBytes(),
		"program",          programName.GetBytes(),

		// remember to update check in Exec()

		"full_name",        fullName.GetBytes(),
		"relative_name",    relativeName.GetBytes(),
		"file_name",        fileName.GetBytes(),
		"file_name_root",   fileNameRoot.GetBytes(),
		"file_name_suffix", fileNameSuffix.GetBytes(),

		"full_path",        fullPath.GetBytes(),
		"relative_path",    relativePath.GetBytes(),

		"line",             lineIndexStr.GetBytes()
	};

	JGetStringManager()->Replace(arg, map, sizeof(map));
	return true;
}

/******************************************************************************
 UsesFile (static private)

 ******************************************************************************/

bool
CommandManager::UsesFile
	(
	const JString& arg
	)
{
	return arg.Contains("$full_name")        ||
		arg.Contains("$relative_name")    ||
		arg.Contains("$file_name")        ||
		arg.Contains("$file_name_root")   ||
		arg.Contains("$file_name_suffix") ||
		arg.Contains("$full_path")        ||
		arg.Contains("$relative_path")    ||
		arg.Contains("$line");
}

/******************************************************************************
 FindCommandName (private)

 ******************************************************************************/

bool
CommandManager::FindCommandName
	(
	const JString&	name,
	CmdInfo*		info
	)
	const
{
	const JSize count = itsCmdList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		*info = itsCmdList->GetElement(i);
		if (*(info->name) == name)
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 AppendCommand

 ******************************************************************************/

void
CommandManager::AppendCommand
	(
	const JString&	path,
	const JString&	cmd,
	const JString&	cmdName,

	const bool	isMake,
	const bool	isVCS,
	const bool	saveAll,
	const bool	oneAtATime,
	const bool	useWindow,
	const bool	raise,
	const bool	beep,

	const JString&	menuText,
	const JString&	menuShortcut,
	const bool	separator
	)
{
	auto* p = jnew JString(path);
	assert( p != nullptr );

	auto* c = jnew JString(cmd);
	assert( c != nullptr );

	auto* cn = jnew JString(cmdName);
	assert( cn != nullptr );

	auto* mt = jnew JString(menuText);
	assert( mt != nullptr );

	auto* ms = jnew JString(menuShortcut);
	assert( ms != nullptr );

	auto* mi = jnew JString;
	assert( mi != nullptr );

	CmdInfo info(p, c, cn,
				 isMake, isVCS, saveAll, oneAtATime,
				 useWindow, raise, beep,
				 mt, ms, mi, separator);
	itsCmdList->AppendElement(info);
}

/******************************************************************************
 AppendMenuItems

 ******************************************************************************/

void
CommandManager::AppendMenuItems
	(
	JXTextMenu*		menu,
	const bool	hasProject
	)
	const
{
	const JSize count = itsCmdList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const CmdInfo info = itsCmdList->GetElement(i);
		menu->AppendItem((info.menuText)->IsEmpty() ? *info.cmd : *info.menuText);
		menu->SetItemNMShortcut(menu->GetItemCount(), *info.menuShortcut);
		menu->SetItemID(menu->GetItemCount(), *info.menuID);
		if (info.separator)
		{
			menu->ShowSeparatorAfter(menu->GetItemCount());
		}

		if (!hasProject && info.isMake)
		{
			menu->DisableItem(menu->GetItemCount());
		}
	}

	menu->ShowSeparatorAfter(menu->GetItemCount());
}

/******************************************************************************
 GetCurrentCmdInfoFileVersion (static)

 ******************************************************************************/

JFileVersion
CommandManager::GetCurrentCmdInfoFileVersion()
{
	return kCurrentSetupVersion;
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CommandManager::ReadSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	if (ReadCommands(input, &itsMakeDependCmd, itsCmdList, &vers) && vers < 2)
	{
		UpdateMenuIDs();
	}
}

/******************************************************************************
 ReadCommands (static)

 ******************************************************************************/

bool
CommandManager::ReadCommands
	(
	std::istream&	input,
	JString*		makeDependCmd,
	CmdList*		cmdList,
	JFileVersion*	returnVers
	)
{
	JFileVersion vers;
	input >> vers;
	if (input.fail() || vers > kCurrentSetupVersion)
	{
		return false;
	}

	input >> *makeDependCmd;

	cmdList->DeleteAll();

	if (vers <= 3)
	{
		JSize count;
		input >> count;

		for (JIndex i=1; i<=count; i++)
		{
			CmdInfo info = ReadCmdInfo(input, vers);
			UpgradeCommand(&info);
			cmdList->AppendElement(info);

			if (input.fail())
			{
				break;
			}
		}
	}
	else
	{
		while (true)
		{
			bool keepGoing;
			input >> JBoolFromString(keepGoing);
			if (input.fail() || !keepGoing)
			{
				break;
			}

			CmdInfo info = ReadCmdInfo(input, vers);
			UpgradeCommand(&info);
			cmdList->AppendElement(info);
		}
	}

	if (returnVers != nullptr)
	{
		*returnVers = vers;
	}
	return true;
}

/******************************************************************************
 ReadCmdInfo (static)

 ******************************************************************************/

CommandManager::CmdInfo
CommandManager::ReadCmdInfo
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	auto* path = jnew JString;
	assert( path != nullptr );
	input >> *path;

	auto* cmd = jnew JString;
	assert( cmd != nullptr );
	input >> *cmd;

	auto* cmdName = jnew JString;
	assert( cmdName != nullptr );
	input >> *cmdName;

	bool isMake, saveAll, oneAtATime, useWindow, raise, beep;
	bool isVCS = false;
	input >> JBoolFromString(isMake);
	if (vers >= 3)
	{
		input >> JBoolFromString(isVCS);
	}
	if (vers == 0)
	{
		bool makeDepend;
		input >> JBoolFromString(makeDepend);
	}
	input >> JBoolFromString(saveAll)
		  >> JBoolFromString(oneAtATime)
		  >> JBoolFromString(useWindow)
		  >> JBoolFromString(raise)
		  >> JBoolFromString(beep);

	auto* menuText = jnew JString;
	assert( menuText != nullptr );
	input >> *menuText;

	auto* menuShortcut = jnew JString;
	assert( menuShortcut != nullptr );
	input >> *menuShortcut;

	auto* menuID = jnew JString;
	assert( menuID != nullptr );
	if (vers >= 2)
	{
		input >> *menuID;
	}

	bool separator;
	input >> JBoolFromString(separator);

	return CmdInfo(path, cmd, cmdName,
				   isMake, isVCS, saveAll, oneAtATime,
				   useWindow, raise, beep,
				   menuText, menuShortcut, menuID, separator);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CommandManager::WriteSetup
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion << '\n';
	output << itsMakeDependCmd << '\n';

	const JSize count = itsCmdList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		output << JBoolToString(true);
		WriteCmdInfo(output, itsCmdList->GetElement(i));
	}

	output << JBoolToString(false) << '\n';
}

/******************************************************************************
 WriteCmdInfo (static)

 ******************************************************************************/

void
CommandManager::WriteCmdInfo
	(
	std::ostream&		output,
	const CmdInfo&	info
	)
{
	output << ' ' << *info.path;
	output << ' ' << *info.cmd;
	output << ' ' << *info.name;

	output << ' ' << JBoolToString(info.isMake)
				  << JBoolToString(info.isVCS)
				  << JBoolToString(info.saveAll)
				  << JBoolToString(info.oneAtATime)
				  << JBoolToString(info.useWindow)
				  << JBoolToString(info.raiseWindowWhenStart)
				  << JBoolToString(info.beepWhenFinished);

	output << ' ' << *info.menuText;
	output << ' ' << *info.menuShortcut;
	output << ' ' << *info.menuID;
	output << ' ' << JBoolToString(info.separator);
	output << '\n';
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
CommandManager::ReadPrefs
	(
	std::istream& input
	)
{
	ReadSetup(input);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CommandManager::WritePrefs
	(
	std::ostream& output
	)
	const
{
	WriteSetup(output);
}

/******************************************************************************
 UpgradeCommand (private static)

 ******************************************************************************/

static const JString svnClient("nps_svn_client", JString::kNoCopy);

static const JRegex svnChangeFullPathPattern =
	"\\bsvn (resolved|revert|commit|add|remove) \\$full_name\\b";

static const JRegex svnChangeRelativePathPattern =
	"\\bsvn (resolved|revert|commit|add|remove)\\b";

void
CommandManager::UpgradeCommand
	(
	CmdInfo* info
	)
{
	if (JProgramAvailable(svnClient) &&
		!info->cmd->Contains(svnClient))
	{
		if (*info->cmd == "svn info $file_name; svn log $file_name" ||
			*info->cmd == "svn info $file_name"                     ||
			*info->cmd == "svn log $file_name"                      ||
			*info->cmd == "svn info $full_name; svn log $full_name" ||
			*info->cmd == "svn info $full_name"                     ||
			*info->cmd == "svn log $full_name")
		{
			*info->cmd                 = "nps_svn_client --info-log $file_name";
			*info->path                = "@";
			info->oneAtATime           = true;
			info->useWindow            = false;
			info->raiseWindowWhenStart = false;
			info->beepWhenFinished     = false;
		}
		else if (svnChangeFullPathPattern.Match(*info->cmd))
		{
			*info->cmd += "; nps_svn_client --refresh-status $full_path";
		}
		else if (svnChangeRelativePathPattern.Match(*info->cmd))
		{
			*info->cmd += "; nps_svn_client --refresh-status .";
		}
		else if (*info->cmd == "svn update; jcc --reload-open" &&
				 (*info->path == "." || *info->path == "./"))
		{
			*info->cmd                 = "nps_svn_client --update .";
			info->useWindow            = false;
			info->raiseWindowWhenStart = false;
			info->beepWhenFinished     = false;
		}
		else if (*info->cmd == "svn status" &&
				 (*info->path == "." || *(info->path) == "./"))
		{
			*info->cmd                 = "nps_svn_client --status .";
			info->useWindow            = false;
			info->raiseWindowWhenStart = false;
			info->beepWhenFinished     = false;
		}
	}
}

/******************************************************************************
 ReadTemplate

 ******************************************************************************/

void
CommandManager::ReadTemplate
	(
	std::istream&				input,
	const JFileVersion		tmplVers,
	const JFileVersion		projVers
	)
{
	if (tmplVers >= 3)
	{
		ReadSetup(input);
	}
}

/******************************************************************************
 WriteTemplate

 ******************************************************************************/

void
CommandManager::WriteTemplate
	(
	std::ostream& output
	)
	const
{
	WriteSetup(output);
}

/******************************************************************************
 InitCommandList (private)

 ******************************************************************************/

struct DefCmd
{
	const JUtf8Byte*	path;
	const JUtf8Byte*	cmd;
	const JUtf8Byte*	name;
	bool			isMake;
	bool			isVCS;
	bool			saveAll;
	bool			oneAtATime;
	bool			useWindow;
	bool			raiseWindowWhenStart;
	bool			beepWhenFinished;
	const JUtf8Byte*	menuTextID;
	const JUtf8Byte*	menuShortcutID;
	bool			separator;
};

static const DefCmd kDefCmd[] =
{
	// make

{ "./", J_MAKE_BINARY_NAME " -kw all",
		"make_default",
		true, false, true, false, true, false, true,
		"DefCmdBuildText::CommandManager", "DefCmdBuildShortcut::CommandManager",
		false },
{ "./", J_MAKE_BINARY_NAME " $relative_path$file_name_root.o",
		"",
		true, false, true, false, true, true, false,
		"DefCmdCompileText::CommandManager", "DefCmdCompileShortcut::CommandManager",
		false },
{ "./", J_MAKE_BINARY_NAME " tidy",
		"",
		true, false, true, false, true, false, false,
		"DefCmdRemoveObjFilesText::CommandManager", "DefCmdRemoveObjFilesShortcut::CommandManager",
		true },

	// run

{ "./", "$program",
		"",
		false, false, false, false, true, true, false,
		"DefCmdRunText::CommandManager", "DefCmdRunShortcut::CommandManager",
		false },
{ "./", "java $program",
		"",
		false, false, false, false, true, true, false,
		"DefCmdRunJavaText::CommandManager", "DefCmdRunJavaShortcut::CommandManager",
		false },
{ "./", "medic $program",
		"",
		false, false, false, false, false, false, false,
		"DefCmdDebugText::CommandManager", "DefCmdDebugShorcut::CommandManager",
		false },
{ "./", "medic -b +$line $file_name",
		"",
		false, false, false, true, false, false, false,
		"DefCmdSetBreakpointText::CommandManager", "DefCmdSetBreakpointShortcut::CommandManager",
		false },
{ "./", "medic -f +$line $file_name",
		"",
		false, false, false, true, false, false, false,
		"DefCmdOpenDebuggerText::CommandManager", "DefCmdOpenDebuggerShortcut::CommandManager",
		true },

	// make & run

{ "./", "&make_default ; $program",
		"",
		false, false, false, false, true, false, false,
		"DefCmdBuildRunText::CommandManager", "DefCmdBuildRunShortcut::CommandManager",
		false },
{ "./", "&make_default ; medic $program",
		"",
		false, false, false, false, false, false, false,
		"DefCmdBuildDebugText::CommandManager", "DefCmdBuildDebugShortcut::CommandManager",
		true },

	// git

{ "@", "git log -p $file_name",
		"",
		false, true, false, true, true, true, false,
		"DefCmdGitInfoText::CommandManager", "DefCmdVCSInfoShortcut::CommandManager",
		false },
{ "@", "git checkout -q $file_name; jcc --reload-open",
		"",
		false, true, true, false, false, false, false,
		"DefCmdGitRevertText::CommandManager", "DefCmdVCSRevertShortcut::CommandManager",
		false },
{ "@", "git commit $file_name",
		"",
		false, true, true, false, true, true, false,
		"DefCmdGitCheckInText::CommandManager", "DefCmdVCSCheckInShortcut::CommandManager",
		false },
{ "@", "git add $file_name",
		"",
		false, true, true, false, false, false, false,
		"DefCmdGitAddText::CommandManager", "DefCmdVCSAddShortcut::CommandManager",
		false },
{ "@", "git rm -f $file_name",
		"",
		false, true, false, false, false, false, false,
		"DefCmdGitRemoveText::CommandManager", "DefCmdVCSRemoveShortcut::CommandManager",
		true },

{ "./", "git pull; jcc --reload-open",
		"",
		false, true, true, false, true, true, false,
		"DefCmdGitUpdateAllText::CommandManager", "DefCmdVCSUpdateAllShortcut::CommandManager",
		false },
{ "./", "git status",
		"",
		false, true, true, false, true, true, false,
		"DefCmdGitDiffAllText::CommandManager", "DefCmdVCSDiffAllShortcut::CommandManager",
		false },
{ "./", "git commit",
		"",
		false, true, true, false, true, true, false,
		"DefCmdGitCommitAllText::CommandManager", "DefCmdVCSCommitAllShortcut::CommandManager",
		true },

	// svn

{ "@", "svn info $file_name; svn log $file_name",
		"",
		false, true, false, true, true, true, false,
		"DefCmdSVNInfoText::CommandManager", "DefCmdVCSInfoShortcut::CommandManager",
		false },
{ "@", "svn resolved $file_name",
		"",
		false, true, true, false, false, false, false,
		"DefCmdSVNResolveText::CommandManager", "DefCmdVCSCheckOutShortcut::CommandManager",
		false },
{ "@", "svn revert $file_name; jcc --reload-open",
		"",
		false, true, true, false, false, false, false,
		"DefCmdSVNRevertText::CommandManager", "DefCmdVCSRevertShortcut::CommandManager",
		false },
{ "@", "svn commit $file_name",
		"",
		false, true, true, false, true, true, false,
		"DefCmdSVNCheckInText::CommandManager", "DefCmdVCSCheckInShortcut::CommandManager",
		false },
{ "@", "svn add $file_name",
		"",
		false, true, true, false, false, false, false,
		"DefCmdSVNAddText::CommandManager", "DefCmdVCSAddShortcut::CommandManager",
		false },
{ "@", "svn remove $file_name",
		"",
		false, true, false, false, false, false, false,
		"DefCmdSVNRemoveText::CommandManager", "DefCmdVCSRemoveShortcut::CommandManager",
		true },

{ "./", "svn update; jcc --reload-open",
		"",
		false, true, true, false, true, true, false,
		"DefCmdSVNUpdateAllText::CommandManager", "DefCmdVCSUpdateAllShortcut::CommandManager",
		false },
{ "./", "svn status",
		"",
		false, true, true, false, true, true, false,
		"DefCmdSVNStatusText::CommandManager", "DefCmdVCSStatusShortcut::CommandManager",
		false },
{ "./", "svn diff --diff-cmd diff -x --brief",
		"",
		false, true, true, false, true, true, false,
		"DefCmdSVNDiffAllText::CommandManager", "DefCmdVCSDiffAllShortcut::CommandManager",
		false },
{ "./", "svn commit",
		"",
		false, true, true, false, true, true, false,
		"DefCmdSVNCommitAllText::CommandManager", "DefCmdVCSCommitAllShortcut::CommandManager",
		true },
/*
{ "./", "snv commit $project_name.jcc",
		"",
		false, true, true, false, true, true, false,
		"DefCmdSVNCheckInProjectText::CommandManager", "DefCmdVCSCheckInProjectShortcut::CommandManager",
		false },
{ "./", "svn add $project_name.jcc",
		"",
		false, true, true, false, false, false, false,
		"DefCmdSVNAddProjectText::CommandManager", "DefCmdVCSAddProjectShortcut::CommandManager",
		true },
*/
	// cvs
/*
{ "@", "cvs edit $file_name",
		"",
		false, true, true, false, false, false, false,
		"DefCmdCVSCheckOutText::CommandManager", "DefCmdVCSCheckOutShortcut::CommandManager",
		false },
{ "@", "cvs unedit $file_name; jcc --reload-open",
		"",
		false, true, true, false, false, false, false,
		"DefCmdCVSRevertText::CommandManager", "DefCmdVCSRevertShortcut::CommandManager",
		false },
{ "@", "cvs commit $file_name",
		"",
		false, true, true, false, true, true, false,
		"DefCmdCVSCheckInText::CommandManager", "DefCmdVCSCheckInShortcut::CommandManager",
		false },
{ "@", "cvs add $file_name",
		"",
		false, true, true, false, false, false, false,
		"DefCmdCVSAddText::CommandManager", "DefCmdVCSAddShortcut::CommandManager",
		false },
{ "@", "cvs remove -f $file_name",
		"",
		false, true, false, false, false, false, false,
		"DefCmdCVSRemoveText::CommandManager", "DefCmdVCSRemoveShortcut::CommandManager",
		true },

{ "./", "cvs -q update -P -d; jcc --reload-open",
		"",
		false, true, true, false, true, true, false,
		"DefCmdCVSUpdateAllText::CommandManager", "DefCmdVCSUpdateAllShortcut::CommandManager",
		false },
{ "./", "cvs -q diff --brief",
		"",
		false, true, true, false, true, true, false,
		"DefCmdCVSDiffAllText::CommandManager", "DefCmdVCSDiffAllShortcut::CommandManager",
		false },
{ "./", "cvs -q commit",
		"",
		false, true, true, false, true, true, false,
		"DefCmdCVSCommitAllText::CommandManager", "DefCmdVCSCommitAllShortcut::CommandManager",
		true },
*//*
{ "./", "cvs commit $project_name.jcc",
		"",
		false, true, true, false, true, true, false,
		"DefCmdCVSCheckInProjectText::CommandManager", "DefCmdVCSCheckInProjectShortcut::CommandManager",
		false },
{ "./", "cvs add $project_name.jcc",
		"",
		false, true, true, false, false, false, false,
		"DefCmdCVSAddProjectText::CommandManager", "DefCmdVCSAddProjectShortcut::CommandManager",
		true },
*/
};

const JSize kDefCmdCount = sizeof(kDefCmd) / sizeof(DefCmd);

void
CommandManager::InitCommandList()
{
	for (const auto& cmd : kDefCmd)
	{
		AppendCommand(JString(cmd.path, JString::kNoCopy),
					  JString(cmd.cmd, JString::kNoCopy),
					  JString(cmd.name, JString::kNoCopy),
					  cmd.isMake, cmd.isVCS, cmd.saveAll,
					  cmd.oneAtATime, cmd.useWindow,
					  cmd.raiseWindowWhenStart,
					  cmd.beepWhenFinished,
					  JGetString(cmd.menuTextID),
					  JGetString(cmd.menuShortcutID),
					  cmd.separator);
	}

	for (JIndex i=1; i<=kDefCmdCount; i++)
	{
		*((itsCmdList->GetElement(i)).menuID) = GetUniqueMenuID();
	}
}

/******************************************************************************
 UpdateMenuIDs

 ******************************************************************************/

void
CommandManager::UpdateMenuIDs()
{
	const JSize count = itsCmdList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		JString* id = (itsCmdList->GetElement(i)).menuID;
		if (id->IsEmpty())
		{
			*id = GetUniqueMenuID();
		}
	}
}

/******************************************************************************
 GetUniqueMenuID (private)

 ******************************************************************************/

JString
CommandManager::GetUniqueMenuID()
{
	JKLRand r;

	JString id;
	while (true)
	{
		id = "::CommandManager";

		for (JIndex i=1; i<=20; i++)
		{
			id.Prepend(JUtf8Character(r.UniformLong(32, 126)));
		}

		bool found = false;

		const JSize count = itsCmdList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
		{
			if (id == *((itsCmdList->GetElement(i)).menuID))
			{
				found = true;
			}
		}

		if (!found)
		{
			break;
		}
	}

	return id;
}

/******************************************************************************
 ConvertCompileDialog

 ******************************************************************************/

void
CommandManager::ConvertCompileDialog
	(
	std::istream&		input,
	const JFileVersion	vers,
	BuildManager*		buildMgr,
	const bool		readWindGeom
	)
{
	if (vers < 3 || 61 < vers)
	{
		return;
	}

	if (vers >= 23 && readWindGeom)
	{
		JXWindow::SkipGeometry(input);
	}

	// path input
	JString path;
	input >> path;

	JString str;
	JPtrArray<JString> itemList(JPtrArrayT::kDeleteAll),
					   nmShortcutList(JPtrArrayT::kDeleteAll);
	if (vers >= 8)
	{
		// path history menu
		JXHistoryMenuBase::ReadSetup(input, &itemList, &nmShortcutList);
	}

	if (vers >= 14)
	{
		input >> itsMakeDependCmd;

		// make depend history menu
		JXHistoryMenuBase::ReadSetup(input, &itemList, &nmShortcutList);

		bool makeDepend;
		input >> JBoolFromString(makeDepend);
	}

	// make command -- at top of menu
	input >> str;

	// make history menu
	JPtrArray<JString> makeList(JPtrArrayT::kDeleteAll);
	JXHistoryMenuBase::ReadSetup(input, &makeList, &nmShortcutList);

	// compile command -- at top of menu
	input >> str;

	// compile history menu
	JPtrArray<JString> compileList(JPtrArrayT::kDeleteAll);
	JXHistoryMenuBase::ReadSetup(input, &compileList, &nmShortcutList);

	bool saveAll;
	input >> JBoolFromString(saveAll);

	if (vers >= 29)
	{
		bool doubleSpace;
		input >> JBoolFromString(doubleSpace);
	}

	buildMgr->ConvertCompileDialog(input, vers);

	UpdateFileMarkers(vers < 26, &itsMakeDependCmd);

	// save commands in menu

	JSize count = makeList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		UpdateFileMarkers(vers < 26, makeList.GetElement(i));
		AppendCommand(path, *makeList.GetElement(i), JString::empty,
					  true, false, true, false, true, false, true,
					  JString::empty, JString::empty, i == count);
	}

	count = compileList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		UpdateFileMarkers(vers < 26, compileList.GetElement(i));
		AppendCommand(path, *compileList.GetElement(i), JString::empty,
					  true, false, true, true, true, true, false,
					  JString::empty, JString::empty, i == count);
	}
}

/******************************************************************************
 ConvertRunDialog

 ******************************************************************************/

void
CommandManager::ConvertRunDialog
	(
	std::istream&			input,
	const JFileVersion	vers,
	const bool		readWindGeom
	)
{
	if (vers < 5 || 61 < vers)
	{
		return;
	}

	if (vers >= 23 && readWindGeom)
	{
		JXWindow::SkipGeometry(input);
	}

	// path input
	JString path;
	input >> path;

	JString str;
	JPtrArray<JString> itemList(JPtrArrayT::kDeleteAll),
					   nmShortcutList(JPtrArrayT::kDeleteAll);
	if (vers >= 8)
	{
		// path history menu
		JXHistoryMenuBase::ReadSetup(input, &itemList, &nmShortcutList);
	}

	// run command -- at top of menu
	input >> str;

	// run history menu
	JPtrArray<JString> runList(JPtrArrayT::kDeleteAll);
	JXHistoryMenuBase::ReadSetup(input, &runList, &nmShortcutList);

	// debug command -- at top of menu
	input >> str;

	// debug history menu
	JPtrArray<JString> debugList(JPtrArrayT::kDeleteAll);
	JXHistoryMenuBase::ReadSetup(input, &debugList, &nmShortcutList);

	bool makeBeforeRun;
	input >> JBoolFromString(makeBeforeRun);

	if (vers >= 39)
	{
		bool useOutputDoc;
		input >> JBoolFromString(useOutputDoc);
	}

	// save commands in menu

	JSize count = runList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		UpdateFileMarkers(vers < 26, runList.GetElement(i));
		AppendCommand(path, *runList.GetElement(i), JString::empty,
					  false, false, false, false, true, true, false,
					  JString::empty, JString::empty, i == count);
	}

	count = debugList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		UpdateFileMarkers(vers < 26, debugList.GetElement(i));
		AppendCommand(path, *debugList.GetElement(i), JString::empty,
					  false, false, false, true, false, false, false,
					  JString::empty, JString::empty, i == count);
	}
}

/******************************************************************************
 UpdateFileMarkers (private)

	Convert %[fpnrl] to $[fpnrl], when upgrading from vers < 26.

 ******************************************************************************/

static const JUtf8Byte* kVarNameList = "fpnrl";

static const JUtf8Byte* kOldVar[] =
{
	"$f", "$p", "$n", "$r", "$l"
};

static const JUtf8Byte* kNewVar[] =
{
	"$full_name", "$full_path", "$file_name", "$file_name_root", "$line"
};

const JSize kVarCount = strlen(kVarNameList);

void
CommandManager::UpdateFileMarkers
	(
	const bool	convertFromAncient,
	JString*		s
	)
{
	if (convertFromAncient)
	{
		JXWebBrowser::ConvertVarNames(s, kVarNameList);
	}

	// convert variable names

	JStringIterator iter(s);
	for (JUnsignedOffset i=0; i<kVarCount; i++)
	{
		iter.MoveTo(kJIteratorStartAtBeginning, 0);
		while (iter.Next(kOldVar[i]))
		{
			iter.ReplaceLastMatch(kNewVar[i]);
		}
	}
}

/******************************************************************************
 GetCompileDoc

 ******************************************************************************/

CompileDocument*
CommandManager::GetCompileDoc
	(
	ProjectDocument* projDoc
	)
{
	if (itsCompileDoc == nullptr)
	{
		itsCompileDoc = jnew CompileDocument(projDoc);
		assert( itsCompileDoc != nullptr );
		ListenTo(itsCompileDoc);
	}

	return itsCompileDoc;
}

/******************************************************************************
 GetOutputDoc (static)

 ******************************************************************************/

ExecOutputDocument*
CommandManager::GetOutputDoc()
{
	const JSize count = theExecDocList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		auto* doc = dynamic_cast<ExecOutputDocument*>(theExecDocList.GetElement(i));
		assert( doc != nullptr );
		if (!doc->ProcessRunning())
		{
			return doc;
		}
	}

	auto* doc = jnew ExecOutputDocument();
	assert( doc != nullptr );
	theExecDocList.Append(doc);
	(GetCommandManager())->ListenTo(doc);
	return doc;
}

/******************************************************************************
 Receive (virtual protected)

	Listen for new active project document.

 ******************************************************************************/

void
CommandManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(DocumentManager::kProjectDocumentActivated))
	{
		UpdateAllCommandMenus();
	}
	else if (message.Is(TextDocument::kSaved))
	{
		DocumentDeleted(sender);
	}
	else
	{
		JBroadcaster::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	Listen for closed ExecOutputDocument

 ******************************************************************************/

void
CommandManager::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (!DocumentDeleted(sender))
	{
		JBroadcaster::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 DocumentDeleted (private)

 ******************************************************************************/

bool
CommandManager::DocumentDeleted
	(
	JBroadcaster* sender
	)
{
	if (sender == itsCompileDoc)
	{
		itsCompileDoc = nullptr;
		return true;
	}

	const JSize count = theExecDocList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (sender == theExecDocList.GetElement(i))
		{
			theExecDocList.RemoveElement(i);
			return true;
		}
	}

	return false;
}

/******************************************************************************
 CmdList functions

 ******************************************************************************/

/******************************************************************************
 DeleteAll

 ******************************************************************************/

void
CommandManager::CmdList::DeleteAll()
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		CmdInfo info = GetElement(i);
		info.Free();
	}
	RemoveAll();
}

/******************************************************************************
 CmdInfo functions

 ******************************************************************************/

/******************************************************************************
 Copy

 ******************************************************************************/

CommandManager::CmdInfo
CommandManager::CmdInfo::Copy()
	const
{
	CmdInfo info = *this;

	info.path = jnew JString(*(this->path));
	assert( info.path != nullptr );

	info.cmd = jnew JString(*(this->cmd));
	assert( info.cmd != nullptr );

	info.name = jnew JString(*(this->name));
	assert( info.name != nullptr );

	info.menuText = jnew JString(*(this->menuText));
	assert( info.menuText != nullptr );

	info.menuShortcut = jnew JString(*(this->menuShortcut));
	assert( info.menuShortcut != nullptr );

	info.menuID = jnew JString(*(this->menuID));
	assert( info.menuID != nullptr );

	return info;
}

/******************************************************************************
 Free

 ******************************************************************************/

void
CommandManager::CmdInfo::Free()
{
	jdelete path;
	path = nullptr;

	jdelete cmd;
	cmd = nullptr;

	jdelete name;
	name = nullptr;

	jdelete menuText;
	menuText = nullptr;

	jdelete menuShortcut;
	menuShortcut = nullptr;

	jdelete menuID;
	menuID = nullptr;
}

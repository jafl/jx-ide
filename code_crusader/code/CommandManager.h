/******************************************************************************
 CommandManager.h

	Copyright © 2001-02 by John Lindal.

 ******************************************************************************/

#ifndef _H_CommandManager
#define _H_CommandManager

#include <jx-af/jcore/JPrefObject.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JStack.h>
#include <jx-af/jcore/JPtrQueue.h>

class JProcess;
class JXTextMenu;
class Command;
class ProjectDocument;
class TextDocument;
class ExecOutputDocument;
class CompileDocument;
class DocumentManager;
class BuildManager;

using CmdQueue      = JPtrArray<JPtrArray<JString> >;
using FunctionStack = JStack<const JUtf8Byte*, JArray<const JUtf8Byte*> >;

class CommandManager : public JPrefObject, virtual public JBroadcaster
{
public:

	struct CmdInfo
	{
		JString*	path;
		JString*	cmd;
		JString*	name;

		bool	isMake;
		bool	isVCS;
		bool	saveAll;
		bool	oneAtATime;
		bool	useWindow;
		bool	raiseWindowWhenStart;
		bool	beepWhenFinished;

		JString*	menuText;
		JString*	menuShortcut;
		JString*	menuID;
		bool	separator;

		CmdInfo()
			:
			path(nullptr), cmd(nullptr), name(nullptr),
			isMake(false), isVCS(false), saveAll(false),
			oneAtATime(true), useWindow(true),
			raiseWindowWhenStart(false), beepWhenFinished(false),
			menuText(nullptr), menuShortcut(nullptr), menuID(nullptr), separator(false)
		{ };

		CmdInfo(JString* p, JString* c, JString* cn,
				JString* mt, JString* ms, JString* mi)
			:
			path(p), cmd(c), name(cn),
			isMake(false), isVCS(false), saveAll(false),
			oneAtATime(true), useWindow(true),
			raiseWindowWhenStart(false), beepWhenFinished(false),
			menuText(mt), menuShortcut(ms), menuID(mi), separator(false)
		{ };

		CmdInfo(JString* p, JString* c, JString* cn,
				const bool mk, const bool vcs,
				const bool sa, const bool oaat,
				const bool uw, const bool raise, const bool beep,
				JString* mt, JString* ms, JString* mi, const bool sep)
			:
			path(p), cmd(c), name(cn),
			isMake(mk), isVCS(vcs), saveAll(sa), oneAtATime(oaat), useWindow(uw),
			raiseWindowWhenStart(raise), beepWhenFinished(beep),
			menuText(mt), menuShortcut(ms), menuID(mi), separator(sep)
		{ };

		CmdInfo	Copy() const;
		void	Free();
	};

	class CmdList : public JArray<CmdInfo>
	{
	public:

		void	DeleteAll();
	};

public:

	CommandManager(DocumentManager* docMgr = nullptr);

	~CommandManager() override;

	void	Exec(const JIndex cmdIndex, ProjectDocument* projDoc,
				 TextDocument* textDoc);
	void	Exec(const JIndex cmdIndex, ProjectDocument* projDoc,
				 const JPtrArray<JString>& fullNameList,
				 const JArray<JIndex>& lineIndexList);

	static void	Exec(const CmdInfo& info, ProjectDocument* projDoc,
					 TextDocument* textDoc);
	static void	Exec(const CmdInfo& info, ProjectDocument* projDoc,
					 const JPtrArray<JString>& fullNameList,
					 const JArray<JIndex>& lineIndexList);

	bool	Prepare(const JString& cmdName, ProjectDocument* projDoc,
					const JPtrArray<JString>& fullNameList,
					const JArray<JIndex>& lineIndexList,
					Command** cmd, CmdInfo** returnInfo,
					FunctionStack* fnStack);

	bool	GetMakeDependCmdStr(ProjectDocument* projDoc, const bool reportError,
								JString* cmdStr) const;
	bool	MakeDepend(ProjectDocument* projDoc,
					   ExecOutputDocument* compileDoc,
					   Command** resultCmd);

	const JString&	GetMakeDependCommand() const;
	void			SetMakeDependCommand(const JString& cmd);

	bool	Substitute(ProjectDocument* projDoc, const bool reportError,
					   JString* cmdStr) const;

	JSize		GetCommandCount() const;
	CmdList*	GetCommandList();

	void	AppendCommand(const JString& path, const JString& cmd,
						  const JString& name,
						  const bool isMake, const bool isVCS,
						  const bool saveAll,
						  const bool oneAtATime, const bool useWindow,
						  const bool raise, const bool beep,
						  const JString& menuText, const JString& menuShortcut,
						  const bool separator);

	void	UpdateAllCommandMenus();
	void	AppendMenuItems(JXTextMenu* menu, const bool hasProject) const;

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	static bool	ReadCommands(std::istream& input,
							 JString* makeDependCmd, CmdList* cmdList,
							 JFileVersion* returnVers = nullptr);

	void	ConvertCompileDialog(std::istream& input, const JFileVersion vers,
							 BuildManager* buildMgr,
							 const bool readWindGeom);
	void	ConvertRunDialog(std::istream& input, const JFileVersion vers,
							 const bool readWindGeom);

	// stored in project template

	void	ReadTemplate(std::istream& input, const JFileVersion tmplVers,
						 const JFileVersion projVers);
	void	WriteTemplate(std::ostream& output) const;

	// used by Command

	CompileDocument*			GetCompileDoc(ProjectDocument* projDoc);
	static ExecOutputDocument*	GetOutputDoc();

	// used by CommandSelection & CommandTable

	static JFileVersion	GetCurrentCmdInfoFileVersion();
	static CmdInfo		ReadCmdInfo(std::istream& input, const JFileVersion vers);
	static void			WriteCmdInfo(std::ostream& output, const CmdInfo& info);

	// used by EditCommandsDialog

	void	UpdateMenuIDs();

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CmdList*			itsCmdList;
	JString				itsMakeDependCmd;
	CompileDocument*	itsCompileDoc;		// not owned

	static JPtrArray<ExecOutputDocument>	theExecDocList;

private:

	void	InitCommandList();
	void	UpdateFileMarkers(const bool convertFromAncient, JString* s);

	static bool	Prepare(const CmdInfo& info, ProjectDocument* projDoc,
						const JPtrArray<JString>& fullNameList,
						const JArray<JIndex>& lineIndexList, Command** cmd,
						FunctionStack* fnStack);
	bool		FindCommandName(const JString& name, CmdInfo* info) const;
	static bool	Parse(const JString& cmd, CmdQueue* cmdQueue,
					  FunctionStack* fnStack);
	static bool	BuildCmdPath(JString* cmdPath, ProjectDocument* projDoc,
							 const JString& fullName, const bool reportError);
	static bool	ProcessCmdQueue(const JString& cmdPath, const CmdQueue& cmdQueue,
								const CmdInfo& info, ProjectDocument* projDoc,
								const JPtrArray<JString>& fullNameList,
								const JArray<JIndex>& lineIndexList,
								const bool reportError,
								Command** cmd, FunctionStack* fnStack);
	static bool	Substitute(JString* arg, ProjectDocument* projDoc,
						   const JString& fullName, const JIndex lineIndex,
						   const bool reportError);
	static bool	Add(const JString& path, const JPtrArray<JString>& cmdArgs,
					const CmdInfo& info, ProjectDocument* projDoc,
					const JPtrArray<JString>& fullNameList,
					const JArray<JIndex>& lineIndexList,
					Command** cmd, FunctionStack* fnStack);
	static bool	UsesFile(const JString& arg);

	JString	GetUniqueMenuID();

	bool	DocumentDeleted(JBroadcaster* sender);

	static void	UpgradeCommand(CmdInfo* info);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kUpdateCommandMenu;

	class UpdateCommandMenu : public JBroadcaster::Message
		{
		public:

			UpdateCommandMenu()
				:
				JBroadcaster::Message(kUpdateCommandMenu)
				{ };
		};
};


/******************************************************************************
 Make depend

 ******************************************************************************/

inline const JString&
CommandManager::GetMakeDependCommand()
	const
{
	return itsMakeDependCmd;
}

inline void
CommandManager::SetMakeDependCommand
	(
	const JString& cmd
	)
{
	itsMakeDependCmd = cmd;
}

/******************************************************************************
 GetCommandCount

 ******************************************************************************/

inline JSize
CommandManager::GetCommandCount()
	const
{
	return itsCmdList->GetElementCount();
}

/******************************************************************************
 GetCommandList

 ******************************************************************************/

inline CommandManager::CmdList*
CommandManager::GetCommandList()
{
	return itsCmdList;
}

/******************************************************************************
 UpdateAllCommandMenus

 ******************************************************************************/

inline void
CommandManager::UpdateAllCommandMenus()
{
	Broadcast(UpdateCommandMenu());
}

#endif

/******************************************************************************
 Command.h

	Copyright © 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_Command
#define _H_Command

#include "CommandManager.h"	// need defn of CmdInfo
#include <jx-af/jcore/JPtrArray-JString.h>

class JProcess;
class ProjectDocument;
class ExecOutputDocument;

class Command : virtual public JBroadcaster
{
public:

	Command(const JString& path, const bool refreshVCSStatusWhenFinished,
			const bool beepWhenFinished, ProjectDocument* projDoc);

	~Command() override;

	void	SetParent(Command* cmd);
	bool	Add(const JPtrArray<JString>& cmdArgs,
				const JPtrArray<JString>& fullNameList,
				const JArray<JIndex>& lineIndexList,
				FunctionStack* fnStack);
	void	Add(Command* subCmd, const CommandManager::CmdInfo& cmdInfo);
	void	MarkEndOfSequence();
	bool	Start(const CommandManager::CmdInfo& info);
	bool	StartMakeProcess(ExecOutputDocument* outputDoc);

	const JString&	GetPath() const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	struct CmdInfo
	{
		JPtrArray<JString>*			cmd;		// can be nullptr
		Command*					cmdObj;		// can be nullptr
		CommandManager::CmdInfo*	cmdInfo;	// nullptr if cmdObj==nullptr
		bool						isMakeDepend;

		CmdInfo()
			:
			cmd(nullptr), cmdObj(nullptr), cmdInfo(nullptr), isMakeDepend(false)
		{ };

		CmdInfo(JPtrArray<JString>* c, Command* o, CommandManager::CmdInfo* i,
				const bool isMD)
			:
			cmd(c), cmdObj(o), cmdInfo(i), isMakeDepend(isMD)
		{ };

		bool	IsEndOfSequence() const;
		bool	IsSubroutine() const;
		void	Free(const bool deleteCmdObj);
	};

private:

	ProjectDocument*	itsProjDoc;			// can be nullptr
	const JString		itsCmdPath;
	JArray<CmdInfo>*	itsCmdList;
	ExecOutputDocument*	itsOutputDoc;		// not owned; can be nullptr
	JString				itsWindowTitle;
	JString				itsDontCloseMsg;
	const bool			itsBeepFlag;
	bool				itsRefreshVCSStatusFlag;
	bool				itsUpdateSymbolDatabaseFlag;
	bool				itsInQueueFlag;
	bool				itsSuccessFlag;
	bool				itsCancelledFlag;
	Command*			itsMakeDependCmd;

	// used for subroutines

	ExecOutputDocument*	itsBuildOutputDoc;	// can be nullptr; ensures single window for subroutines
	ExecOutputDocument*	itsRunOutputDoc;	// can be nullptr; ensures single window for subroutines
	Command*			itsParent;			// can be nullptr; not owned; parent who Start()ed us
	bool				itsCallParentProcessFinishedFlag;

private:

	bool	StartProcess();
	void	ProcessFinished(const bool success, const bool cancelled);

	void	SetCompileDocStrings();

	void	DeleteThis();
	void	FinishWindow(ExecOutputDocument** doc);

	void	ReportInfiniteLoop(const FunctionStack& fnStack,
							   const JIndex startIndex);

	// not allowed

	Command(const Command&) = delete;
	Command& operator=(const Command&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFinished;

	class Finished : public JBroadcaster::Message
		{
		public:

			Finished(const bool success, const bool cancelled)
				:
				JBroadcaster::Message(kFinished),
				itsSuccessFlag(success && !cancelled),
				itsCancelledFlag(cancelled)
				{ };

			bool
			Successful() const
			{
				return itsSuccessFlag;
			};

			bool
			Cancelled() const
			{
				return itsCancelledFlag;
			};

		private:

			const bool	itsSuccessFlag;
			const bool	itsCancelledFlag;
		};
};


/******************************************************************************
 GetPath

 ******************************************************************************/

inline const JString&
Command::GetPath()
	const
{
	return itsCmdPath;
}

/******************************************************************************
 SetParent

 ******************************************************************************/

inline void
Command::SetParent
	(
	Command* cmd
	)
{
	itsParent = cmd;
}

#endif

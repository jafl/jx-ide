/******************************************************************************
 Link.cpp

	Base class for debugger interfaces.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#include "Link.h"
#include "Command.h"
#include "Breakpoint.h"
#include "globals.h"
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* Link::kUserOutput             = "UserOutput::Link";
const JUtf8Byte* Link::kDebugOutput            = "DebugOutput::Link";

const JUtf8Byte* Link::kDebuggerReadyForInput  = "DebuggerReadyForInput::Link";
const JUtf8Byte* Link::kDebuggerBusy           = "DebuggerBusy::Link";
const JUtf8Byte* Link::kDebuggerDefiningScript = "DebuggerDefiningScript::Link";

const JUtf8Byte* Link::kDebuggerStarted        = "DebuggerStarted::Link";
const JUtf8Byte* Link::kDebuggerRestarted      = "DebuggerRestarted::Link";
const JUtf8Byte* Link::kPrepareToLoadSymbols   = "PrepareToLoadSymbols::Link";
const JUtf8Byte* Link::kSymbolsLoaded          = "SymbolsLoaded::Link";
const JUtf8Byte* Link::kSymbolsReloaded        = "SymbolsReloaded::Link";
const JUtf8Byte* Link::kCoreLoaded             = "CoreLoaded::Link";
const JUtf8Byte* Link::kCoreCleared            = "CoreCleared::Link";
const JUtf8Byte* Link::kAttachedToProcess      = "AttachedToProcess::Link";
const JUtf8Byte* Link::kDetachedFromProcess    = "DetachedFromProcess::Link";

const JUtf8Byte* Link::kProgramRunning         = "ProgramRunning::Link";
const JUtf8Byte* Link::kProgramFirstStop       = "ProgramFirstStop::Link";
const JUtf8Byte* Link::kProgramStopped         = "ProgramStopped::Link";
const JUtf8Byte* Link::kProgramStopped2        = "ProgramStopped2::Link";
const JUtf8Byte* Link::kProgramFinished        = "ProgramFinished::Link";

const JUtf8Byte* Link::kBreakpointsChanged     = "BreakpointsChanged::Link";
const JUtf8Byte* Link::kFrameChanged           = "FrameChanged::Link";
const JUtf8Byte* Link::kThreadChanged          = "ThreadChanged::Link";
const JUtf8Byte* Link::kValueChanged           = "ValueChanged::Link";

const JUtf8Byte* Link::kThreadListChanged      = "ThreadListChanged::Link";

const JUtf8Byte* Link::kPlugInMessage          = "PlugInMessage::Link";

/******************************************************************************
 Constructor

 *****************************************************************************/

Link::Link
	(
	const bool*			features,
	const JUtf8Byte*	cmdPromptKey,
	const JUtf8Byte*	scriptPromptKey,
	const JUtf8Byte*	chooseProgInstrKey
	)
	:
	itsFeatures(features),
	itsCommandPromptKey(cmdPromptKey),
	itsScriptPromptKey(scriptPromptKey),
	itsChooseProgramInstructionsKey(chooseProgInstrKey)
{
	// commands are often owned by other objects, who can delete them more reliably
	itsForegroundQ = jnew JPtrArray<Command>(JPtrArrayT::kForgetAll);
	assert( itsForegroundQ != nullptr );

	// commands are often owned by other objects, who can delete them more reliably
	itsBackgroundQ = jnew JPtrArray<Command>(JPtrArrayT::kForgetAll);
	assert( itsBackgroundQ != nullptr );

	itsRunningCommand = nullptr;
	itsLastCommandID  = 0;

	itsFileNameMap = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
	assert( itsFileNameMap != nullptr );
}

/******************************************************************************
 Destructor

 *****************************************************************************/

Link::~Link()
{
	jdelete itsForegroundQ;	// must be last, after objects have deleted their commands
	jdelete itsBackgroundQ;

	jdelete itsFileNameMap;
}

/******************************************************************************
 GetCommandPrompt (virtual)

 ******************************************************************************/

const JString&
Link::GetCommandPrompt()
	const
{
	return JGetString(itsCommandPromptKey);
}

/******************************************************************************
 GetScriptPrompt (virtual)

 ******************************************************************************/

const JString&
Link::GetScriptPrompt()
	const
{
	return JGetString(itsScriptPromptKey);
}

/******************************************************************************
 GetChooseProgramInstructions

 ******************************************************************************/

const JString&
Link::GetChooseProgramInstructions()
	const
{
	return JGetString(itsChooseProgramInstructionsKey);
}

/******************************************************************************
 DeleteOneShotCommands (private)

	*** Only for use by dtor.

 *****************************************************************************/

void
Link::DeleteOneShotCommands()
{
	DeleteOneShotCommands(itsForegroundQ);
	DeleteOneShotCommands(itsBackgroundQ);
}

void
Link::DeleteOneShotCommands
	(
	JPtrArray<Command>* list
	)
{
	const JSize count = list->GetItemCount();
	for (JIndex i=count; i>=1; i--)
	{
		Command* cmd = list->GetItem(i);
		if (cmd->IsOneShot())
		{
			jdelete cmd;		// automatically removed from list
		}
	}
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
Link::SetBreakpoint
	(
	const Breakpoint& bp
	)
{
	SetBreakpoint(bp.GetFileName(), bp.GetLineNumber(),
				  bp.GetAction() == Breakpoint::kRemoveBreakpoint);
}

void
Link::SetBreakpoint
	(
	const Location&	loc,
	const bool		temporary
	)
{
	SetBreakpoint(loc.GetFileName(), loc.GetLineNumber(), temporary);
}

/******************************************************************************
 RemoveBreakpoint

 *****************************************************************************/

void
Link::RemoveBreakpoint
	(
	const Breakpoint& bp
	)
{
	RemoveBreakpoint(bp.GetDebuggerIndex());
}

/******************************************************************************
 OKToSendMultipleCommands (virtual)

 *****************************************************************************/

bool
Link::OKToSendMultipleCommands()
	const
{
	return true;
}

/******************************************************************************
 Send

	Sends the given command to the debugger.  If the command cannot be
	submitted, return false, and nothing is changed.

 *****************************************************************************/

bool
Link::Send
	(
	Command* command
	)
{
	if (command->GetState() != Command::kUnassigned)
	{
		assert( command->GetTransactionID() != 0 );
		return false;
	}
	else
	{
		command->SetTransactionID(GetNextTransactionID());

		if (command->IsBackground())
		{
			itsBackgroundQ->Append(command);
		}
		else
		{
			itsForegroundQ->Append(command);
		}
		RunNextCommand();

		return true;
	}
}

/******************************************************************************
 GetNextTransactionID (protected)

 *****************************************************************************/

JIndex
Link::GetNextTransactionID()
{
	itsLastCommandID++;
	if (itsLastCommandID == 0) // Wrap, even though it isn't likely to ever happen
	{
		itsLastCommandID = 1;
	}
	return itsLastCommandID;
}

/******************************************************************************
 Cancel

	Do not call directly.

 *****************************************************************************/

void
Link::Cancel
	(
	Command* cmd
	)
{
	if (itsRunningCommand == cmd)
	{
		itsRunningCommand = nullptr;
	}
	itsBackgroundQ->Remove(cmd);
	itsForegroundQ->Remove(cmd);
}

/******************************************************************************
 RunNextCommand (protected)

 *****************************************************************************/

void
Link::RunNextCommand()
{
	if (!itsForegroundQ->IsEmpty() && OKToSendMultipleCommands())
	{
		const JSize count = itsForegroundQ->GetItemCount();
		for (JIndex i=1; i<=count; i++)
		{
			Command* command = itsForegroundQ->GetItem(i);
			if (command->GetState() != Command::kExecuting)
			{
				SendMedicCommand(command);
			}
		}
	}
	else if (!itsForegroundQ->IsEmpty())
	{
		Command* command = itsForegroundQ->GetFirstItem();
		if (command->GetState() != Command::kExecuting && OKToSendCommands(false))
		{
			SendMedicCommand(command);
		}
	}
	else if (!itsBackgroundQ->IsEmpty() && OKToSendCommands(true))
	{
		Command* command = itsBackgroundQ->GetFirstItem();
		if (command->GetState() != Command::kExecuting)
		{
			SendMedicCommand(command);
		}
	}
}

/******************************************************************************
 HandleCommandRunning (protected)

 *****************************************************************************/

void
Link::HandleCommandRunning
	(
	const JIndex cmdID
	)
{
	assert( itsRunningCommand == nullptr );

	const JSize fgCount = itsForegroundQ->GetItemCount();
	for (JIndex i=1; i<=fgCount; i++)
	{
		Command* command = itsForegroundQ->GetItem(i);
		if (command->GetTransactionID() == cmdID)
		{
			itsRunningCommand = command;
			itsForegroundQ->RemoveItem(i);
			break;
		}
	}

	if (itsRunningCommand == nullptr && !itsBackgroundQ->IsEmpty())
	{
		Command* command = itsBackgroundQ->GetFirstItem();
		if (command->GetTransactionID() == cmdID)
		{
			itsRunningCommand = command;
			itsBackgroundQ->RemoveItem(1);
		}
	}
}

/******************************************************************************
 CancelAllCommands (protected)

 *****************************************************************************/

void
Link::CancelAllCommands()
{
	if (IsShuttingDown())		// command owners already deleted
	{
		return;
	}

	for (JIndex i=itsForegroundQ->GetItemCount(); i>=1; i--)
	{
		Command* cmd = itsForegroundQ->GetItem(i);
		itsForegroundQ->RemoveItem(i);	// remove first, in case auto-delete
		cmd->Finished(false);

		if (itsRunningCommand == cmd)
		{
			itsRunningCommand = nullptr;
		}
	}

	CancelBackgroundCommands();
}

/******************************************************************************
 CancelBackgroundCommands (protected)

 *****************************************************************************/

void
Link::CancelBackgroundCommands()
{
	if (IsShuttingDown())		// command owners already deleted
	{
		return;
	}

	for (JIndex i=itsBackgroundQ->GetItemCount(); i>=1; i--)
	{
		Command* cmd = itsBackgroundQ->GetItem(i);
		itsBackgroundQ->RemoveItem(i);	// remove first, in case auto-delete
		cmd->Finished(false);

		if (itsRunningCommand == cmd)
		{
			itsRunningCommand = nullptr;
		}
	}
}

/******************************************************************************
 RememberFile

 ******************************************************************************/

void
Link::RememberFile
	(
	const JString& fileName,
	const JString& fullName
	)
{
	if (fullName.IsEmpty())
	{
		itsFileNameMap->SetItem(fileName, nullptr, JPtrArrayT::kDelete);
	}
	else
	{
		itsFileNameMap->SetItem(fileName, fullName, JPtrArrayT::kDelete);
	}
}

/******************************************************************************
 FindFile

 ******************************************************************************/

bool
Link::FindFile
	(
	const JString&	fileName,
	bool*		exists,
	JString*		fullName
	)
	const
{
	const JString* s = nullptr;
	if (JIsAbsolutePath(fileName) && JFileExists(fileName))
	{
		*exists   = true;
		*fullName = fileName;
		return true;
	}
	else if (itsFileNameMap->GetItem(fileName, &s))
	{
		if (s == nullptr)
		{
			*exists = false;
			fullName->Clear();
		}
		else
		{
			*exists   = true;
			*fullName = *s;
		}
		return true;
	}
	else
	{
		*exists = false;
		fullName->Clear();
		return false;
	}

/*	All search paths are unreliable.  See GetFullPathCmd.cpp for more info.

	if (JIsRelativePath(fileName))
	{
		const JSize count = itsPathList->GetItemCount();
		for (JIndex i=1; i<=count; i++)
		{
			*fullName = JCombinePathAndName(*(itsPathList->GetItem(i)), fileName);
			if (JFileExists(*fullName))
			{
				return true;
			}
		}
	}
	else if (JFileExists(fileName))
	{
		*fullName = fileName;
		return true;
	}

	fullName->Clear();
	return false; */
}

/******************************************************************************
 ClearFileNameMap

 ******************************************************************************/

void
Link::ClearFileNameMap()
{
	itsFileNameMap->DeleteAll();
}

/******************************************************************************
 NotifyUser (static)

 *****************************************************************************/

void
Link::NotifyUser
	(
	const JString&	msg,
	const bool	error
	)
{
	GetLink()->Broadcast(UserOutput(msg, error));
}

/******************************************************************************
 Log (static)

 *****************************************************************************/

void
Link::Log
	(
	const JUtf8Byte* log
	)
{
	GetLink()->Broadcast(DebugOutput(JString(log, JString::kNoCopy), kLogType));
}

/******************************************************************************
 Log (static)

 *****************************************************************************/

void
Link::Log
	(
	std::ostringstream& log
	)
{
	std::string logData = log.str();
	Log(logData.data());
}

/******************************************************************************
 Build1DArrayExpressionForCFamilyLanguage (protected)

 *****************************************************************************/

JString
Link::Build1DArrayExpressionForCFamilyLanguage
	(
	const JString&	origExpr,
	const JInteger	index
	)
{
	JString expr = origExpr;

	const JString indexStr(index, 0);	// must use floating point conversion
	if (expr.Contains("$i"))
	{
		const JUtf8Byte* map[] =
		{
			"i", indexStr.GetBytes()
		};
		JGetStringManager()->Replace(&expr, map, sizeof(map));
	}
	else
	{
		if (expr.GetFirstCharacter() != '(' ||
			expr.GetLastCharacter()  != ')')
		{
			expr.Prepend("(");
			expr.Append(")");
		}

		expr += "[";
		expr += indexStr;
		expr += "]";
	}

	return expr;
}
/******************************************************************************
 Build2DArrayExpressionForCFamilyLanguage (protected)

 *****************************************************************************/

JString
Link::Build2DArrayExpressionForCFamilyLanguage
	(
	const JString&	origExpr,
	const JInteger	rowIndex,
	const JInteger	colIndex
	)
{
	JString expr = origExpr;

	const bool usesI = expr.Contains("$i");		// row
	const bool usesJ = expr.Contains("$j");		// col

	const JString iStr(rowIndex, 0);	// must use floating point conversion
	const JString jStr(colIndex, 0);	// must use floating point conversion

	// We have to do both at the same time because otherwise we lose a $.

	if (usesI || usesJ)
	{
		const JUtf8Byte* map[] =
		{
			"i", iStr.GetBytes(),
			"j", jStr.GetBytes()
		};
		JGetStringManager()->Replace(&expr, map, sizeof(map));
	}

	if (!usesI || !usesJ)
	{
		if (expr.GetFirstCharacter() != '(' ||
			expr.GetLastCharacter()  != ')')
		{
			expr.Prepend("(");
			expr.Append(")");
		}

		if (!usesI)
		{
			expr += "[";
			expr += iStr;
			expr += "]";
		}
		if (!usesJ)
		{
			expr += "[";
			expr += jStr;
			expr += "]";
		}
	}

	return expr;
}

/******************************************************************************
 Uncommon functionality, so not pure virtual

 *****************************************************************************/

void
Link::StepOverAssembly()
{
}

void
Link::StepIntoAssembly()
{
}

void
Link::RunUntil
	(
	const JString& addr
	)
{
}

void
Link::SetExecutionPoint
	(
	const JString& addr
	)
{
}

void
Link::BackupOver()
{
}

void
Link::BackupInto()
{
}

void
Link::BackupOut()
{
}

void
Link::BackupContinue()
{
}

%top {
/*
Copyright (C) 2007 by John Lindal.

This scanner reads output from gdb --interpreter=mi2.
*/

#include "Location.h"
#include "Breakpoint.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

#define RETURN(x)	return Token(x, itsData)

#ifndef IN_HEADER
// splitting up square brackets protects them from flex 2.5.33 on macOS
static const JRegex pidRegex1("(?:child|attached) (?:process|Pid|PID|thread) ([""[:digit:]""]+)");
static const JRegex pidRegex2("\\(LWP ([""[:digit:]""]+)");
static const JRegex pidRegex3("process ([""[:digit:]""]+)");
static const JRegex attachRegex1("(?:process|Pid|PID) ([""[:digit:]""]+)");
static const JRegex pingRegex(":Medic debugger ready:([""[:digit:]""]+)");
#endif
%}

%option namespace="gdb::Output" lexer="Scanner"
%option lex="NextToken" token-type="gdb::Output::Scanner::Token"
%option unicode nodefault full freespace

%class {

public:

	enum TokenType
	{
		kEOF = 258,
		kProgramOutput,
		kErrorOutput,
		kCommandOutput,
		kCommandResult,
		kDebugOutput,

		kReadyForInput,
		kDebuggerFinished,

		kPrepareToLoadSymbols,
		kFinishedLoadingSymbolsFromProgram,
		kNoSymbolsInProgram,
		kSymbolsReloaded,

		kCoreChanged,

		kAttachedToProcess,
		kDetachingFromProcess,

		kProgramStarting,
		kBeginGetPID,
		kProgramPID,
		kEndGetPID,

		kProgramStoppedAtLocation,
		kProgramRunning,
		kProgramFinished,
		kProgramKilled,

		kBeginMedicCmd,
		kEndMedicCmd,

		kBeginMedicIgnoreCmd,
		kEndMedicIgnoreCmd,

		kBreakpointsChanged,
		kFrameChanged,
		kThreadChanged,
		kValueChanged,

		kFrameChangedAndProgramStoppedAtLocation,

		kBeginScriptDefinition,
		kPlugInMessage
	};

	struct Data
	{
		JUInt		number;
		Location*	pLocation;
		JString*	pString;
	};

	struct Token
	{
		TokenType	type;
		Data		data;

		Token()
			:
			type(kProgramOutput)
			{ };

		Token(const TokenType t, const Data& d)
			:
			type(t), data(d)
			{ };
	};

public:

	void	Reset();
	void	AppendInput(const JString& string);

	static void	TranslateMIOutput(JString* data);

private:

	JString	itsPartialBuffer;
	JString	itsInputBuffer;

	// "Registers" -- temporary buffers for use in Token

	Data			itsData;
	Location		itsLocation;
	JString			itsString;

private:

	void	AppendInputText(const JString& string);
	void	ExtractCommandId(const JUtf8Byte* str, const JSize len);
}

%x ACQUIRE_PID ATTACHING_TO_PROCESS MEDIC_COMMAND READING_SYMBOLS

WS     ([[:space:]]+)
INT    ([[:digit:]]+)
HEX    (0x[[:xdigit:]]+)
PROMPT "(gdb) "\n?
CMDPFX \\032\\032

%%

%{
/************************************************************************/

	memset(&itsData, 0, sizeof(itsData));

%}

{PROMPT} {
	RETURN(kReadyForInput);
	}

(&\""echo "{CMDPFX}:"Medic debugger ready":{INT}:\\n\"\n)?~\"{CMDPFX}:"Medic debugger ready":{INT}:\"\n(\^done\n)? {

	const JString s(text(), size(), JString::kNoCopy);
	const JStringMatch m = pingRegex.Match(s, JRegex::kIncludeSubmatches);
	assert( !m.IsEmpty() );
	const bool ok = m.GetSubstring(1).ConvertToUInt(&itsData.number);
	assert( ok );
	RETURN(kReadyForInput);
	}

<INITIAL,MEDIC_COMMAND>\^exit\n |
~\"{CMDPFX}:"Medic debugger quit":\"\n {
	RETURN(kDebuggerFinished);
	}

<INITIAL,MEDIC_COMMAND>\^done,reason=\"exited[^"]*\".*\n    |
<INITIAL,MEDIC_COMMAND>\*stopped,reason=\"exited[^"]*\".*\n |
<INITIAL,MEDIC_COMMAND>\*stopped\n                          {

	itsString.Set(text() + 1, size() - 1);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kProgramFinished);
	}

~\"\\n"Program "(exited|terminated).*\"\n  {

	itsString.Set(text() + 4, size() - 6);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kProgramFinished);
	}

~\"{CMDPFX}:"Medic loading file":\"\n {
	start(READING_SYMBOLS);
	RETURN(kPrepareToLoadSymbols);
	}

~\"{CMDPFX}:"Medic loading core":\"\n {
	RETURN(kCoreChanged);
	}

~\"{CMDPFX}:"Medic attaching":\"\n {
	start(ATTACHING_TO_PROCESS);
	RETURN(kDebugOutput);
	}

~\"{CMDPFX}:"Medic detaching":\"\n {
	RETURN(kDetachingFromProcess);
	}

~\"{CMDPFX}:"Medic program starting":\"\n {
	RETURN(kProgramStarting);
	}

~\"{CMDPFX}:"Medic acquire pid":\"\n {
	start(ACQUIRE_PID);
	RETURN(kBeginGetPID);
	}

	/* macOS prints pid when program starts */

~\""[Switching to process "{INT}.*\]\\n\"\n {

	const JString s(text(), size(), JString::kNoCopy);
	const JStringMatch m = pidRegex3.Match(s, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
		{
		const bool ok = m.GetSubstring(1).ConvertToUInt(&itsData.number);
		assert( ok );
		}
	RETURN(kProgramPID);
	}

~\"{CMDPFX}:"Medic program running":\"\n {
	RETURN(kProgramRunning);
	}

~\".*" has changed; re-reading symbols".*\\n\"\n {
	RETURN(kSymbolsReloaded);
	}

~\"{CMDPFX}:"Medic program killed":\"\n {
	RETURN(kProgramKilled);
	}

	/* result of running gdb with --fullname option: first item is full path + name */
	/* on macOS, this only works when switch frames */

~\"{CMDPFX}[^:\0]+:{INT}:{INT}:[^:]+:{HEX}\\n\"\n {

	JIndexRange r(11,11);
	while (text()[r.last-1] != ':')
		{
		r.last++;
		}
	const_cast<JUtf8Byte*>(text())[r.last-1] = '\0';
	itsLocation.SetFileName(JString(text()+r.first-1, JString::kNoCopy));

	r.last++;
	r.first = r.last;
	while (text()[r.last-1] != ':')
		{
		r.last++;
		}
	const_cast<JUtf8Byte*>(text())[r.last-1] = '\0';
	JIndex lineNumber;
	const bool ok = JString::ConvertToUInt(text()+r.first-1, &lineNumber);
	assert( ok );
	itsLocation.SetLineNumber(lineNumber);

	itsData.pLocation = &itsLocation;
	RETURN(kProgramStoppedAtLocation);
	}

	/* macOS result of hitting a breakpoint */

<INITIAL,MEDIC_COMMAND>\^done,reason=.+\n {

	itsString.Set(text() + 1, size() - 1);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kProgramStoppedAtLocation);
	}

~\"{CMDPFX}:"Medic breakpoints changed":\"\n {
	RETURN(kBreakpointsChanged);
	}

	/* frame changed to function that doesn't have a source file */

~\"{CMDPFX}:"Medic frame changed":\"\n/("^done,frame="|~.+\n^done\n) {
	RETURN(kFrameChangedAndProgramStoppedAtLocation);
	}

~\"{CMDPFX}:"Medic frame changed":\"\n {
	RETURN(kFrameChanged);
	}

~\"{CMDPFX}:"Medic thread changed":\"\n {
	RETURN(kThreadChanged);
	}

~\"{CMDPFX}:"Medic value changed":\"\n {
	RETURN(kValueChanged);
	}

~\"{CMDPFX}:"Medic defining script":\"\n {
	RETURN(kBeginScriptDefinition);
	}

~\"{CMDPFX}:"Medic command":{INT}:\"\n {
	ExtractCommandId(text(), size());
	start(MEDIC_COMMAND);
	RETURN(kBeginMedicCmd);
	}

~\"{CMDPFX}:"Medic ignore command":{INT}:\"\n {
	ExtractCommandId(text(), size());
	RETURN(kBeginMedicIgnoreCmd);
	}

~\"{CMDPFX}:"Medic ignore command done":{INT}:\"\n {
	ExtractCommandId(text(), size());
	RETURN(kEndMedicIgnoreCmd);
	}

	/* hook set by plug-in */

~\"\cZ\cY:.+:\"\n {

	itsString.Set(text() + 11, size() - 14);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kPlugInMessage);
	}

	/* user output */

[~@]\".*\"\n {

	itsString.Set(text() + 2, size() - 4);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kProgramOutput);
	}

	/* mi results */

\^error,msg=\""Cannot execute this command while the target is running."\"\n |
\^error,msg=\""Cannot execute command ".+" while target running"\"\n {
	RETURN(kDebugOutput);
	}

\^error,msg=\".*\"\n {

	itsString.Set(text() + 12, size() - 14);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kErrorOutput);
	}

\^running\n\*running.*\n{PROMPT} |
[*+=^].*\n                       {
	RETURN(kDebugOutput);
	}

	/* gdb debug output */

&\".*\"\n {
	RETURN(kDebugOutput);
	}

	/* optimize to slurp as quickly as possible */

[^(^~@&*+=].*\n |
[^(^~@&*+=\n]+  |
[(^~@&*+=\n]    {

	itsString.Set(text(), size());
	itsData.pString = &itsString;
	RETURN(kProgramOutput);
	}

<*><<EOF>> {
	RETURN(kEOF);
	}



	/* Debugger is reading symbols from program */



<READING_SYMBOLS>{

~\"done\.\\n\"\n {

	itsString       = "done.\n";
	itsData.pString = &itsString;
	start(INITIAL);
	RETURN(kFinishedLoadingSymbolsFromProgram);
	}

([~@]\".*\")?\^done\n {
	start(INITIAL);
	RETURN(kFinishedLoadingSymbolsFromProgram);
	}

&\".*"No such file or directory".*\"\n |
~\""(No debugging symbols found in".*\"\n {

	itsData.pString = nullptr;
	start(INITIAL);
	RETURN(kNoSymbolsInProgram);
	}

\^error,msg=\".*\"\n {

	itsString.Set(text() + 12, size() - 14);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	start(INITIAL);
	RETURN(kNoSymbolsInProgram);
	}

[~@]\".*\"\n {

	itsString.Set(text() + 2, size() - 4);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kProgramOutput);
	}

.*\n? {
	RETURN(kDebugOutput);
	}

}



	/* Medic asking debugger for PID of running program */



<ACQUIRE_PID>{

~\".*(child|attached)" "(process|Pid|PID|thread)" "{INT}.*\\n\"\n |
~\".*"(LWP "{INT}.*\\n\"\n                            {

	const JString s(text(), size(), JString::kNoCopy);
	const JStringMatch m1 = pidRegex1.Match(s, JRegex::kIncludeSubmatches),
					   m2 = pidRegex2.Match(s, JRegex::kIncludeSubmatches);
	if (!m1.IsEmpty())
		{
		const bool ok = m1.GetSubstring(1).ConvertToUInt(&itsData.number);
		assert( ok );
		}
	else if (!m2.IsEmpty())
		{
		const bool ok = m2.GetSubstring(1).ConvertToUInt(&itsData.number);
		assert( ok );
		}
	RETURN(kProgramPID);
	}

~\"{CMDPFX}:"Medic acquire pid end":\"\n {
	start(INITIAL);
	RETURN(kEndGetPID);
	}

.*\n? {
	RETURN(kDebugOutput);
	}

}



	/* Debugger attaching to process */



<ATTACHING_TO_PROCESS>{

~\".*(process|Pid|PID)" "{INT}.*\\n\"\n {

	const JString s(text(), size(), JString::kNoCopy);
	const JStringMatch m = attachRegex1.Match(s, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
		{
		const bool ok = m.GetSubstring(1).ConvertToUInt(&itsData.number);
		assert( ok );
		}
	start(READING_SYMBOLS);
	RETURN(kAttachedToProcess);
	}

.*\n? {
	RETURN(kDebugOutput);
	}

}



	/* Medic running Command */



<MEDIC_COMMAND>{

~\"{CMDPFX}:"Medic command done":{INT}:\"\n {
	ExtractCommandId(text(), size());
	start(INITIAL);
	RETURN(kEndMedicCmd);
	}

~\".*\"\n {

	itsString.Set(text() + 2, size() - 4);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kCommandOutput);
	}

\^.*\n {

	itsString.Set(text() + 1, size() - 2);
	itsData.pString = &itsString;
	RETURN(kCommandResult);
	}

@\".*\"\n {

	itsString.Set(text() + 2, size() - 4);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kProgramOutput);
	}

.*\n? {
	RETURN(kDebugOutput);
	}

}

%%

/******************************************************************************
 Reset

 *****************************************************************************/

void
gdb::Output::Scanner::Reset()
{
	start(INITIAL);
}

/******************************************************************************
 AppendInput

 *****************************************************************************/

void
gdb::Output::Scanner::AppendInput
	(
	const JString& string
	)
{
	itsInputBuffer.Clear();

	if (string.GetLastCharacter() == '\n')
		{
		AppendInputText(string);
		}
	else
		{
		JStringIterator iter(string, JStringIterator::kStartAtEnd);
		JUtf8ByteRange r;
		if (iter.Prev("\n"))
			{
			r.Set(1, iter.GetNextByteIndex());
			AppendInputText(JString(string.GetRawBytes(), r, JString::kNoCopy));
			iter.SkipNext();
			}

		if (!iter.AtEnd())
			{
			r.Set(iter.GetNextByteIndex(), string.GetByteCount());
			const JString s(string.GetRawBytes(), r, JString::kNoCopy);

			// Accumulate partial line for later parsing if:
			// 1: the first character is a meta character
			// 2: we have already accumulated something else

			if (strchr("^~@&*+=", s.GetRawBytes()[0]) != nullptr ||
				!itsPartialBuffer.IsEmpty())
				{
				itsPartialBuffer += s;
				}
			else
				{
				itsInputBuffer += s;
				}
			}
		}

	// lexer reads directly from itsInputBuffer

	if (!itsInputBuffer.IsEmpty())
		{
		in() = reflex::Input(itsInputBuffer.GetBytes(), itsInputBuffer.GetByteCount());
		if (has_matcher())
			{
			matcher().set_end(false);
			}
		}
}

/******************************************************************************
 AppendInputText (private)

 *****************************************************************************/

void
gdb::Output::Scanner::AppendInputText
	(
	const JString& string
	)
{
	if (!itsPartialBuffer.IsEmpty())
		{
		itsInputBuffer += itsPartialBuffer;
		itsPartialBuffer.Clear();
		}
	itsInputBuffer += string;
}

/******************************************************************************
 ExtractCommandId (private)

 *****************************************************************************/

void
gdb::Output::Scanner::ExtractCommandId
	(
	const JUtf8Byte*	str,
	const JSize			len
	)
{
	const JUtf8Byte* s = str + len-4;
	while (isdigit(s[-1]))
		{
		s--;
		}
	const_cast<JUtf8Byte*>(str)[size()-3] = '\0';
	const bool ok = JString::ConvertToUInt(s, &itsData.number);
	assert( ok );
}

/******************************************************************************
 TranslateMIOutput (static)

 *****************************************************************************/

void
gdb::Output::Scanner::TranslateMIOutput
	(
	JString* data
	)
{
	JStringIterator iter(data);
	JUtf8Character c;
	while (iter.Next(&c))
		{
		if (c == '\\')
			{
			iter.RemovePrev();

			if (!iter.Next(&c))
				{
				break;
				}

			if (c == 'n')
				{
				iter.SetPrev(JUtf8Character('\n'));
				}
			else if (c == 't')
				{
				iter.SetPrev(JUtf8Character('\t'));
				}
			}
		}
}

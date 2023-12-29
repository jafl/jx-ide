/******************************************************************************
 CtagsUser.cpp

	Provides interface to ctags.

	BASE CLASS = virtual JBroadcaster

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CtagsUser.h"
#include "globals.h"

#include <jx-af/jcore/JProcess.h>
#include <jx-af/jcore/JOutPipeStream.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

CtagsUser::CtagsStatus CtagsUser::itsHasUniversalCtagsFlag = CtagsUser::kUntested;

static const JString kCheckVersionCmd("ctags --version");
static const JRegex versionPattern("^Universal Ctags");

static const JUtf8Byte* kBaseExecCmd =
	"ctags --filter=yes --filter-terminator=\\\f --fields=kzfsS --extras=+g --extras=-{anonymous} ";

const JUtf8Byte kDelimiter = '\f';

// language specific information

struct FTInfo
{
	TextFileType		fileType;
	Language			lang;
	const JUtf8Byte*	cmd;
	const JUtf8Byte*	fnTitleID;
};

static const JUtf8Byte* kOtherLangCmd = "--languages=all";

static const FTInfo kFTInfo[] =		// index on TextFileType
{
{ kUnknownFT,          kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kCSourceFT,          kCLang,           "--language-force=c++"        , "FunctionsMenuTitle::CtagsUser"   },
{ kCHeaderFT,          kCLang,           "--language-force=c++"        , "FunctionsMenuTitle::CtagsUser"   },
{ kOtherSourceFT,      kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kDocumentationFT,    kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kHTMLFT,             kHTMLLang,        "--language-force=html"       , "IdsMenuTitle::CtagsUser"         },
{ kEiffelFT,           kEiffelLang,      "--language-force=eiffel"     , "FeaturesMenuTitle::CtagsUser"    },
{ kFortranFT,          kFortranLang,     "--language-force=fortran"    , "FunctionsMenuTitle::CtagsUser"   },
{ kJavaSourceFT,       kJavaLang,        "--language-force=java"       , "FunctionsMenuTitle::CtagsUser"   },
{ kStaticLibraryFT,    kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kSharedLibraryFT,    kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kExecOutputFT,       kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kManPageFT,          kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kDiffOutputFT,       kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kAssemblyFT,         kAssemblyLang,    "--language-force=asm"        , "LabelsMenuTitle::CtagsUser"      },
{ kPascalFT,           kPascalLang,      "--language-force=pascal"     , "FunctionsMenuTitle::CtagsUser"   },
{ kRatforFT,           kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kExternalFT,         kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kBinaryFT,           kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kModula2ModuleFT,    kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kModula2InterfaceFT, kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kModula3ModuleFT,    kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kModula3InterfaceFT, kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kAWKFT,              kAWKLang,         "--language-force=awk"        , "FunctionsMenuTitle::CtagsUser"   },
{ kCobolFT,            kCobolLang,       "--language-force=cobol"      , "ParagraphsMenuTitle::CtagsUser"  },
{ kLispFT,             kLispLang,        "--language-force=lisp"       , "FunctionsMenuTitle::CtagsUser"   },
{ kPerlFT,             kPerlLang,        "--language-force=perl"       , "SubroutinesMenuTitle::CtagsUser" },
{ kPythonFT,           kPythonLang,      "--language-force=python"     , "FunctionsMenuTitle::CtagsUser"   },
{ kSchemeFT,           kSchemeLang,      "--language-force=scheme"     , "FunctionsMenuTitle::CtagsUser"   },
{ kBourneShellFT,      kBourneShellLang, "--language-force=sh"         , "FunctionsMenuTitle::CtagsUser"   },
{ kTCLFT,              kTCLLang,         "--language-force=tcl"        , "ProceduresMenuTitle::CtagsUser"  },
{ kVimFT,              kVimLang,         "--language-force=vim"        , "FunctionsMenuTitle::CtagsUser"   },
{ kJavaArchiveFT,      kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kPHPFT,              kPHPLang,         "--language-force=html"       , "FunctionsMenuTitle::CtagsUser"   },
{ kASPFT,              kASPLang,         "--language-force=asp"        , "FunctionsMenuTitle::CtagsUser"   },
{ kSearchOutputFT,     kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kMakeFT,             kMakeLang,        "--language-force=make"       , "TargetsMenuTitle::CtagsUser"     },
{ kREXXFT,             kREXXLang,        "--language-force=rexx"       , "SubroutinesMenuTitle::CtagsUser" },
{ kRubyFT,             kRubyLang,        "--language-force=ruby"       , "FunctionsMenuTitle::CtagsUser"   },
{ kLexFT,              kLexLang,         "--language-force=lex"        , "StatesMenuTitle::CtagsUser"      },
{ kCShellFT,           kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kBisonFT,            kBisonLang,       "--language-force=jbison"     , "SymbolsMenuTitle::CtagsUser"     },
{ kBetaFT,             kBetaLang,        "--language-force=beta"       , "FragmentsMenuTitle::CtagsUser"   },
{ kLuaFT,              kLuaLang,         "--language-force=lua"        , "FunctionsMenuTitle::CtagsUser"   },
{ kSLangFT,            kSLangLang,       "--language-force=slang"      , "FunctionsMenuTitle::CtagsUser"   },
{ kSQLFT,              kSQLLang,         "--language-force=sql"        , "ObjectsMenuTitle::CtagsUser"     },
{ kVeraSourceFT,       kVeraLang,        "--language-force=vera"       , "FunctionsMenuTitle::CtagsUser"   },
{ kVeraHeaderFT,       kVeraLang,        "--language-force=vera"       , "FunctionsMenuTitle::CtagsUser"   },
{ kVerilogFT,          kVerilogLang,     "--language-force=verilog"    , "FunctionsMenuTitle::CtagsUser"   },
{ kCSharpFT,           kCSharpLang,      "--language-force=c#"         , "FunctionsMenuTitle::CtagsUser"   },
{ kErlangFT,           kErlangLang,      "--language-force=erlang"     , "FunctionsMenuTitle::CtagsUser"   },
{ kSMLFT,              kSMLLang,         "--language-force=sml"        , "FunctionsMenuTitle::CtagsUser"   },
{ kJavaScriptFT,       kJavaScriptLang,  "--language-force=javascript" , "FunctionsMenuTitle::CtagsUser"   },
{ kAntFT,              kAntLang,         "--language-force=ant"        , "TargetsMenuTitle::CtagsUser"     },
{ kJSPFT,              kJSPLang,         "--language-force=javascript" , "FunctionsMenuTitle::CtagsUser"   },
{ kXMLFT,              kXMLLang,         kOtherLangCmd                 , nullptr                           },
{ kBasicFT,            kBasicLang,       "--language-force=basic"      , "FunctionsMenuTitle::CtagsUser"   },
{ kUnused1FT,          kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kMatlabFT,           kMatlabLang,      "--language-force=matlab"     , "FunctionsMenuTitle::CtagsUser"   },
{ kAdobeFlexFT,        kAdobeFlexLang,   "--language-force=flex"       , "FunctionsMenuTitle::CtagsUser"   },
{ kINIFT,              kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kPropertiesFT,       kOtherLang,       kOtherLangCmd                 , nullptr                           },
{ kGoFT,               kGoLang,          "--language-force=go"         , "FunctionsMenuTitle::CtagsUser"   },
{ kDFT,                kDLang,           "--language-force=d"          , "FunctionsMenuTitle::CtagsUser"   },
};

const JSize kCtagsFTCount = sizeof(kFTInfo) / sizeof(FTInfo);

/******************************************************************************
 Constructor

 ******************************************************************************/

CtagsUser::CtagsUser
	(
	const JUtf8Byte* args
	)
	:
	itsProcess(nullptr),
	itsArgs(args),
	itsCmdPipe(nullptr),
	itsResultFD(ACE_INVALID_HANDLE),
	itsIsActiveFlag(HasUniversalCtags()),
	itsTryRestartFlag(true)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CtagsUser::~CtagsUser()
{
	DeleteProcess();
}

/******************************************************************************
 SetCtagsArgs (protected)

	Set the arguments passed to ctags.

 ******************************************************************************/

void
CtagsUser::SetCtagsArgs
	(
	const JUtf8Byte* args
	)
{
	itsArgs = args;
	DeleteProcess();	// force restart
}

/******************************************************************************
 ProcessFile (protected)

	Runs the specified file through ctags.

 ******************************************************************************/

bool
CtagsUser::ProcessFile
	(
	const JString&		fileName,
	const TextFileType	fileType,
	JString*			result,
	Language*			lang
	)
{
	if (!StartProcess(fileType, lang))
	{
		result->Clear();
		return false;
	}

	*itsCmdPipe << fileName.GetBytes() << std::endl;

	bool found;
	*result = JReadUntil(itsResultFD, kDelimiter, &found);
	if (found)
	{
		return true;
	}
	else if (itsTryRestartFlag)
	{
		DeleteProcess();

		itsTryRestartFlag = false;
		const bool ok = ProcessFile(fileName, fileType, result, lang);
		itsTryRestartFlag = true;
		return ok;
	}
	else
	{
		DeleteProcess();
		itsIsActiveFlag = false;

		if (!InUpdateThread())
		{
			JGetUserNotification()->ReportError(JGetString("ctagsFailure::CtagsUser"));
		}

		return false;
	}
}

/******************************************************************************
 StartProcess (private)

 ******************************************************************************/

bool
CtagsUser::StartProcess
	(
	const TextFileType	fileType,
	Language*			lang
	)
{
	*lang = kOtherLang;

	if (itsIsActiveFlag && itsProcess == nullptr)
	{
		JString cmd(kBaseExecCmd);
		cmd += itsArgs;

		int toFD, fromFD;
		const JError err =
			JProcess::Create(&itsProcess, cmd,
							 kJCreatePipe, &toFD,
							 kJCreatePipe, &fromFD);
		if (err.OK())
		{
			ListenTo(itsProcess);

			itsCmdPipe = jnew JOutPipeStream(toFD, true);

			itsResultFD = fromFD;
			assert( itsResultFD != ACE_INVALID_HANDLE );

			InitCtags(*itsCmdPipe);
		}
		else
		{
			itsIsActiveFlag = false;
			DeleteProcess();
		}
	}

	if (itsProcess != nullptr)
	{
		assert( kCtagsFTCount == kFTCount && kFTInfo[fileType].fileType == fileType );

		*lang = kFTInfo[fileType].lang;
		*itsCmdPipe << kFTInfo[fileType].cmd << std::endl;
	}

	return itsIsActiveFlag;
}

/******************************************************************************
 InitCtags (virtual protected)

	Derived classes can pass additional info to ctags.

 ******************************************************************************/

void
CtagsUser::InitCtags
	(
	std::ostream& output
	)
{
}

/******************************************************************************
 DeleteProcess (protected)

	Closing itsCmdPipe causes ctags to quit automatically.

 ******************************************************************************/

void
CtagsUser::DeleteProcess()
{
	jdelete itsProcess;
	itsProcess = nullptr;

	jdelete itsCmdPipe;
	itsCmdPipe = nullptr;

	close(itsResultFD);
	itsResultFD = ACE_INVALID_HANDLE;
}

/******************************************************************************
 ReadExtensionFlags (protected)

	Parses the extension flags.

 ******************************************************************************/

void
CtagsUser::ReadExtensionFlags
	(
	std::istream&			input,
	JStringPtrMap<JString>*	flags
	)
	const
{
	flags->CleanOut();

	JIgnoreUntil(input, ";\"\t");			// fluff

	JPtrArray<JString> split(JPtrArrayT::kDeleteAll);
	JString data, key;
	JUtf8Byte delimiter = '\t';
	while (delimiter != '\n')
	{
		if (!JReadUntil(input, 2, "\t\n", &data, &delimiter))
		{
			delimiter = '\n';
		}

		auto* value = jnew JString;

		data.Split(":", &split, 2);
		if (split.GetItemCount() == 2)
		{
			key    = *split.GetItem(1);
			*value = *split.GetItem(2);
		}
		else if (data.EndsWith(":"))
		{
			key = *split.GetItem(1);
			value->Clear();
		}
		else
		{
			key    = "kind";
			*value = data;
		}

		if (!key.IsEmpty() && !value->IsEmpty())
		{
			flags->SetItem(key, value, JPtrArrayT::kDelete);
		}
		else
		{
			jdelete value;
		}
	}
}

/******************************************************************************
 DecodeSymbolType (protected)

	Adobe Flash
		f   functions
		c   classes
		m   methods
		p   properties
		v   global variables
		x   mxtags

	Ant
		t   target

	Assembly
		d   defines
		l   labels
		m   macros
		t	types

	ASP
		d	constants
		c	classes
		f   functions
		s   subroutines
		v	variables

	AWK
		f   functions

	Basic
		c	constants
		f   functions
		l   labels
		t   types
		v   variables
		g   enumerations

	Beta
		f	fragment definitions
		p	all patterns [off]
		s	slots (fragment uses)
		v	patterns (only virtual or rebound patterns are recorded)

	Bison (jbison defined in FnMenuUpdater,SymbolList)
		N   non-terminal symbol definition
		n   non-terminal symbol declaration
		t   terminal symbol (token) declaration

	C/C++
		c   classes
		d   macro definitions (and #undef names)
		e   enumerators
		f   function definitions
		g   enumeration names
		l   local variables [off]
		m   class, struct, or union members
		n   namespaces
		p   function prototypes and declarations
		s   structure names
		t   typedefs
		u   union names
		v   variable definitions
		x   extern and forward variable declarations [off]

	C#
		c	classes
		d	macro definitions
		e	enumerators (values inside an enumeration)
		E	events
		f	fields
		g	enumeration names
		i	interfaces
		l	local variables [off]
		m	methods
		n	namespaces
		p	properties
		s	structure names
		t	typedefs

	Cobol
		d	data
		f	file
		g	group
		p   paragraphs
		P	program
		s	section

	Eiffel
		c   classes
		f   features
		l   local entities [off]

	Erlang
		d	macro definitions
		f	functions
		m	modules
		r	record definitions

	FORTRAN
		b   block data
		c   common blocks
		e   entry points
		f   functions
		i   interfaces
		k   type components
		l   labels [off]
		L   local and common block variables [off]
		m   modules
		n   namelists
		p   programs
		s   subroutines
		t   derived types
		v   module variables

	HTML
		I   identifiers

	Java
		c   classes
		e   enum constants
		f   fields
		g   enum types
		i   interfaces
		l   local variables [off]
		m   methods
		p   packages

	JavaScript
		f   functions
		c   classes
		m   methods
		v   global variables

	Lex
		c   start or exclusive condition
		r   regular expression

	Lisp
		f   functions

	Lua
		f	functions

	Makefile
		t   target
		m   macro (variable)

	Matlab
		f   function

	Pascal
		f   functions
		p   procedures

	Perl
		c	constants
		f   formats
		l	labels
	#	p	packages
		s   subroutines
		d   subroutine declarations [off]

	PHP
		c   classes
		i   interfaces
		d   constant definitions
		f   functions
		j   javascript functions

	Python
		c   classes
		f   functions
		m	class member

	REXX
		s   subroutines

	Ruby
		c   classes
		f   methods
		F	singleton methods
		m   mixins

	Scheme
		f   functions
		s   sets

	Bourne Shell
		f   functions

	SLang
		f	functions
		n	namespaces

	SML
		e	exception declarations
		f	function definitions
		c	functor definitions
		s	signature declarations
		r	structure declarations
		t	type definitions
		v	value bindings

	SQL
		c	cursors
		d	prototypes [off]
		f	functions
		F	record fields
		l	local variables [off]
		L	block label
		P	packages
		p	procedures
		r	records
		s	subtypes
		t	tables
		T	triggers
		v	variables
		i   indexes
		e   events
		U   publications
		R   services
		D   domains
		V   views
		n   synonyms
		x   MobiLink Table Scripts
		y   MobiLink Conn Scripts

	TCL
		c	classes
		m	methods
		p   procedures

	Vera
		c	classes
		d	macro definitions
		e	enumerators
		f	function definitions
		g	enumeration names
		m	class, struct, and union members
		p	programs
		P	function prototypes [off]
		t	tasks
		T	typedefs
		v	variable definitions
		x	external variable declarations [off]

	Verilog
		c	constants (define, parameter, specparam)
		e	events
		f	functions
		m	modules
		n	net data types
		p	ports (input, output, inout)
		r	register data types
		t	tasks
	#	v	variables(integer, real, time)
	#	w	wires (supply, tri, wire, wand, ...)

	Vim
		a	autocommand group
		c   user-defined commands
		f   functions
		m   maps
		v	variables

 ******************************************************************************/

CtagsUser::Type
CtagsUser::DecodeSymbolType
	(
	const Language					lang,
	const JUtf8Byte					c,
	const JStringPtrMap<JString>&	flags
	)
	const
{
	if (lang == kAdobeFlexLang)
	{
		switch (c)
		{
			case 'f':  return kAdobeFlexFunctionST;
			case 'c':  return kAdobeFlexClassST;
			case 'm':  return kAdobeFlexMethodST;
			case 'v':  return kAdobeFlexGlobalVariableST;
			case 'x':  return kAdobeFlexMxTagST;
		}
	}

	else if (lang == kAntLang)
	{
		switch (c)
		{
			case 't':  return kAntTargetST;
		}
	}

	else if (lang == kAssemblyLang)
	{
		switch (c)
		{
			case 'd':  return kAssemblyDefineST;
			case 'l':  return kAssemblyLabelST;
			case 'm':  return kAssemblyMacroST;
			case 't':  return kAssemblyTypeST;
		}
	}

	else if (lang == kASPLang)
	{
		switch (c)
		{
			case 'd':  return kASPConstantST;
			case 'c':  return kASPClassST;
			case 'f':  return kASPFunctionST;
			case 's':  return kASPSubroutineST;
			case 'v':  return kASPVariableST;
		}
	}

	else if (lang == kAWKLang)
	{
		switch (c)
		{
			case 'f':  return kAWKFunctionST;
		}
	}

	else if (lang == kBasicLang)
	{
		switch (c)
		{
			case 'c':  return kBasicConstantST;
			case 'f':  return kBasicFunctionST;
			case 'l':  return kBasicLabelST;
			case 't':  return kBasicTypeST;
			case 'v':  return kBasicVariableST;
			case 'g':  return kBasicEnumerationST;
		}
	}

	else if (lang == kBetaLang)
	{
		switch (c)
		{
			case 'f':  return kBetaFragmentST;
			case 'p':  return kBetaPatternST;
			case 's':  return kBetaSlotST;
			case 'v':  return kBetaPatternST;
		}
	}

	else if (lang == kBisonLang)
	{
		switch (c)
		{
			case 'N':  return kBisonNonterminalDefST;
			case 'n':  return kBisonNonterminalDeclST;
			case 't':  return kBisonTerminalDeclST;
		}
	}

	else if (lang == kCLang)
	{
		switch (c)
		{
			case 'c':  return kCClassST;
			case 'd':  return kCMacroST;
			case 'e':  return kCEnumValueST;
			case 'f':  return kCFunctionST;
			case 'g':  return kCEnumST;
			case 'm':  return kCMemberST;
			case 'n':  return kCNamespaceST;
			case 'p':  return kCPrototypeST;
			case 's':  return kCStructST;
			case 't':  return kCTypedefST;
			case 'u':  return kCUnionST;
			case 'v':  return kCVariableST;
			case 'x':  return kCExternVariableST;
		}
	}

	else if (lang == kCSharpLang)
	{
		switch (c)
		{
			case 'c':  return kCSharpClassST;
			case 'd':  return kCSharpMacroST;
			case 'e':  return kCSharpEnumValueST;
			case 'E':  return kCSharpEventST;
			case 'f':  return kCSharpFieldST;
			case 'g':  return kCSharpEnumNameST;
			case 'i':  return kCSharpInterfaceST;
			case 'm':  return kCSharpMethodST;
			case 'n':  return kCSharpNamespaceST;
			case 'p':  return kCSharpPropertyST;
			case 's':  return kCSharpStructNameST;
			case 't':  return kCSharpTypedefST;
		}
	}

	else if (lang == kCobolLang)
	{
		switch (c)
		{
			case 'd':  return kCobolDataST;
			case 'f':  return kCobolFileST;
			case 'g':  return kCobolGroupST;
			case 'p':  return kCobolParagraphST;
			case 'P':  return kCobolProgramST;
			case 's':  return kCobolSectionST;
		}
	}

	else if (lang == kEiffelLang)
	{
		switch (c)
		{
			case 'c':  return kEiffelClassST;
			case 'f':  return kEiffelFeatureST;
		}
	}

	else if (lang == kErlangLang)
	{
		switch (c)
		{
			case 'd':  return kErlangMacroST;
			case 'f':  return kErlangFunctionST;
			case 'm':  return kErlangModuleST;
			case 'r':  return kErlangRecordST;
		}
	}

	else if (lang == kFortranLang)
	{
		switch (c)
		{
			case 'b':  return kFortranBlockDataST;
			case 'c':  return kFortranCommonBlockST;
			case 'e':  return kFortranEntryPointST;
			case 'f':  return kFortranFunctionST;
			case 'i':  return kFortranInterfaceST;
			case 'k':  return kFortranTypeComponentST;
			case 'l':  return kFortranLabelST;
			case 'L':  return kFortranLocalVariableST;
			case 'm':  return kFortranModuleST;
			case 'n':  return kFortranNamelistST;
			case 'p':  return kFortranProgramST;
			case 's':  return kFortranSubroutineST;
			case 't':  return kFortranDerivedTypeST;
			case 'v':  return kFortranModuleVariableST;
		}
	}

	else if (lang == kHTMLLang)
	{
		switch (c)
		{
			case 'a':  return kHTMLIDST;
		}
	}

	else if (lang == kJavaLang || lang == kJSPLang)
	{
		const JString* value;
		if (c == 'm' && flags.GetItem("interface", &value))
		{
			return kJavaPrototypeST;
		}

		switch (c)
		{
			case 'c':  return kJavaClassST;
			case 'e':  return kJavaEnumValueST;
			case 'f':  return kJavaFieldST;
			case 'g':  return kJavaEnumST;
			case 'i':  return kJavaInterfaceST;
			case 'm':  return kJavaMethodST;
			case 'p':  return kJavaPackageST;
		}
	}

	else if (lang == kJavaScriptLang)
	{
		switch (c)
		{
			case 'f':  return kJavaScriptFunctionST;
			case 'c':  return kJavaScriptClassST;
			case 'm':  return kJavaScriptMethodST;
			case 'v':  return kJavaScriptGlobalVariableST;
		}
	}

	else if (lang == kLexLang)
	{
		switch (c)
		{
			case 'c':  return kLexStateST;
			case 'r':  return kLexRegexST;
		}
	}

	else if (lang == kLispLang)
	{
		switch (c)
		{
			case 'f':  return kLispFunctionST;
		}
	}

	else if (lang == kLuaLang)
	{
		switch (c)
		{
			case 'f':  return kLuaFunctionST;
		}
	}

	else if (lang == kMakeLang)
	{
		switch (c)
		{
			case 't':  return kMakeTargetST;
			case 'm':  return kMakeVariableST;
		}
	}

	else if (lang == kMatlabLang)
	{
		switch (c)
		{
			case 'f':  return kMatlabFunctionST;
		}
	}

	else if (lang == kPascalLang)
	{
		switch (c)
		{
			case 'f':  return kPascalFunctionST;
			case 'p':  return kPascalProcedureST;
		}
	}

	else if (lang == kPerlLang)
	{
		switch (c)
		{
			case 'c':  return kPerlConstantST;
			case 'f':  return kPerlFormatST;
			case 'l':  return kPerlLabelST;
			case 'p':  return kPerlPackageST;
			case 's':  return kPerlSubroutineST;
		}
	}

	else if (lang == kPHPLang)
	{
		switch (c)
		{
			case 'c':  return kPHPClassST;
			case 'i':  return kPHPInterfaceST;
			case 'd':  return kPHPDefineST;
			case 'f':  return kPHPFunctionST;
			case 'j':  return kJavaScriptFunctionST;
		}
	}

	else if (lang == kPythonLang)
	{
		switch (c)
		{
			case 'c':  return kPythonClassST;
			case 'f':  return kPythonFunctionST;
			case 'm':  return kPythonClassMemberST;
		}
	}

	else if (lang == kREXXLang)
	{
		switch (c)
		{
			case 's':  return kREXXSubroutineST;
		}
	}

	else if (lang == kRubyLang)
	{
		switch (c)
		{
			case 'c':  return kRubyClassST;
			case 'f':  return kRubyMethodST;
			case 'F':  return kRubySingletonMethodST;
			case 'm':  return kRubyMixinST;
		}
	}

	else if (lang == kSchemeLang)
	{
		switch (c)
		{
			case 'f':  return kSchemeFunctionST;
			case 's':  return kSchemeSetST;
		}
	}

	else if (lang == kBourneShellLang)
	{
		switch (c)
		{
			case 'f':  return kBourneShellFunctionST;
		}
	}

	else if (lang == kSLangLang)
	{
		switch (c)
		{
			case 'f':  return kSLangFunctionST;
			case 'n':  return kSLangNamespaceST;
		}
	}

	else if (lang == kSMLLang)
	{
		switch (c)
		{
			case 'e':  return kSMLExceptionST;
			case 'f':  return kSMLFunctionST;
			case 'c':  return kSMLFunctorST;
			case 's':  return kSMLSignatureST;
			case 'r':  return kSMLStructureST;
			case 't':  return kSMLTypeST;
			case 'v':  return kSMLValueST;
		}
	}

	else if (lang == kSQLLang)
	{
		switch (c)
		{
			case 'c':  return kSQLCursorST;
			case 'd':  return kSQLPrototypeST;
			case 'f':  return kSQLFunctionST;
			case 'F':  return kSQLRecordFieldST;
			case 'l':  return kSQLLocalVariableST;
			case 'L':  return kSQLLabelST;
			case 'P':  return kSQLPackageST;
			case 'p':  return kSQLProcedureST;
			case 'r':  return kSQLRecordST;
			case 's':  return kSQLSubtypeST;
			case 't':  return kSQLTableST;
			case 'T':  return kSQLTriggerST;
			case 'v':  return kSQLVariableST;
			case 'i':  return kSQLIndexST;
			case 'e':  return kSQLEventST;
			case 'U':  return kSQLPublicationST;
			case 'R':  return kSQLServiceST;
			case 'D':  return kSQLDomainST;
			case 'V':  return kSQLViewST;
			case 'n':  return kSQLSynonymST;
			case 'x':  return kSQLMobiLinkTableScriptST;
			case 'y':  return kSQLMobiLinkConnScriptST;
		}
	}

	else if (lang == kTCLLang)
	{
		switch (c)
		{
			case 'c':  return kTCLClassST;
			case 'm':  return kTCLMethodST;
			case 'p':  return kTCLProcedureST;
		}
	}

	else if (lang == kVeraLang)
	{
		switch (c)
		{
			case 'c':  return kVeraClassST;
			case 'd':  return kVeraMacroST;
			case 'e':  return kVeraEnumValueST;
			case 'f':  return kVeraFunctionST;
			case 'g':  return kVeraEnumST;
			case 'm':  return kVeraMemberST;
			case 'p':  return kVeraProgramST;
			case 'P':  return kVeraPrototypeST;
			case 't':  return kVeraTaskST;
			case 'T':  return kVeraTypedefST;
			case 'v':  return kVeraVariableST;
			case 'x':  return kVeraExternalVariableST;
		}
	}

	else if (lang == kVerilogLang)
	{
		switch (c)
		{
			case 'c':  return kVerilogConstantST;
			case 'e':  return kVerilogEventST;
			case 'f':  return kVerilogFunctionST;
			case 'm':  return kVerilogModuleST;
			case 'n':  return kVerilogNetST;
			case 'p':  return kVerilogPortST;
			case 'r':  return kVerilogRegisterST;
			case 't':  return kVerilogTaskST;
//			case 'v':  return kVerilogVariableST;
//			case 'w':  return kVerilogWireST;
		}
	}

	else if (lang == kVimLang)
	{
		switch (c)
		{
			case 'a':  return kVimAutocommandGroupST;
			case 'c':  return kVimUserCommandST;
			case 'f':  return kVimFunctionST;
			case 'm':  return kVimMapST;
			case 'v':  return kVimVariableST;
		}
	}

	return kUnknownST;
}

/******************************************************************************
 IgnoreSymbol (static)

	Fully qualified, global symbols (::qsort) are pointless.

 ******************************************************************************/

bool
CtagsUser::IgnoreSymbol
	(
	const JString& s
	)
{
	return s.StartsWith("::");
}

/******************************************************************************
 Receive (virtual protected)

	If the process dies, we delete the object so the process will
	be restarted next time.

 ******************************************************************************/

void
CtagsUser::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsProcess && message.Is(JProcess::kFinished))
	{
		DeleteProcess();
	}
	else
	{
		JBroadcaster::Receive(sender, message);
	}
}

/******************************************************************************
 HasUniversalCtags (static)

 ******************************************************************************/

// Interrupt routine

// prototypes

static void emptyHandler(int sig);

// functions

void emptyHandler(int sig)
{
}

bool
CtagsUser::HasUniversalCtags()
{
	if (itsHasUniversalCtagsFlag == kUntested)
	{
		itsHasUniversalCtagsFlag = kFailure;

		// this hack is required on Linux kernel 2.3.x (4/19/2000)
		j_sig_func origHandler = signal(SIGCHLD, emptyHandler);

		pid_t pid;

		#if defined _J_SUNOS
		pid_t* ppid = nullptr;
		#else
		pid_t* ppid = &pid;
		#endif

		int fromFD;
		JError err = JExecute(kCheckVersionCmd, ppid,
							  kJIgnoreConnection, nullptr,
							  kJCreatePipe, &fromFD,
							  kJTossOutput, nullptr);
		if (err.OK())
		{
			JString vers;
			JReadAll(fromFD, &vers);

			if (versionPattern.Match(vers))
			{
				itsHasUniversalCtagsFlag = kSuccess;
			}
		}

		if (origHandler != SIG_ERR)
		{
			signal(SIGCHLD, origHandler);
		}
	}

	return itsHasUniversalCtagsFlag == kSuccess;
}

/******************************************************************************
 IsParsed (static)

 ******************************************************************************/

bool
CtagsUser::IsParsed
	(
	const TextFileType type
	)
{
	assert( kCtagsFTCount == kFTCount && kFTInfo[type].fileType == type );

	return kFTInfo[type].lang != kOtherLang;
}

/******************************************************************************
 IsParsedForFunctionMenu (static)

 ******************************************************************************/

bool
CtagsUser::IsParsedForFunctionMenu
	(
	const TextFileType type
	)
{
	assert( kCtagsFTCount == kFTCount && kFTInfo[type].fileType == type );

	return kFTInfo[type].fnTitleID != nullptr;
}

/******************************************************************************
 GetFunctionMenuTitle (static)

 ******************************************************************************/

static const JString kDefFnTitle("Not parsed");

const JString&
CtagsUser::GetFunctionMenuTitle
	(
	const TextFileType type
	)
{
	assert( kCtagsFTCount == kFTCount && kFTInfo[type].fileType == type );

	if (kFTInfo[type].fnTitleID != nullptr)
	{
		return JGetString(kFTInfo[type].fnTitleID);
	}
	else
	{
		return kDefFnTitle;
	}
}

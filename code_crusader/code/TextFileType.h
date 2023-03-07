/******************************************************************************
 TextFileType.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_TextFileType
#define _H_TextFileType

#include <jx-af/jcore/JString.h>

class StylerBase;
class StringCompleter;

// Do not change the file type values once they are assigned because
// they are stored in the prefs file.

enum TextFileType
{
	kUnknownFT = 0,
	kCSourceFT,
	kCHeaderFT,
	kOtherSourceFT,
	kDocumentationFT,
	kHTMLFT,
	kEiffelFT,
	kFortranFT,
	kJavaSourceFT,
	kStaticLibraryFT,
	kSharedLibraryFT,
	kExecOutputFT,		// special
	kManPageFT,			// special
	kDiffOutputFT,		// special
	kAssemblyFT,
	kPascalFT,
	kRatforFT,
	kExternalFT,			// use other program
	kBinaryFT,			// use binary file editor
	kModula2ModuleFT,
	kModula2InterfaceFT,
	kModula3ModuleFT,
	kModula3InterfaceFT,
	kAWKFT,
	kCobolFT,
	kLispFT,
	kPerlFT,
	kPythonFT,
	kSchemeFT,
	kBourneShellFT,
	kTCLFT,
	kVimFT,
	kJavaArchiveFT,
	kPHPFT,
	kASPFT,
	kSearchOutputFT,		// special
	kMakeFT,
	kREXXFT,
	kRubyFT,
	kLexFT,
	kCShellFT,
	kBisonFT,
	kBetaFT,
	kLuaFT,
	kSLangFT,
	kSQLFT,
	kVeraSourceFT,
	kVeraHeaderFT,
	kVerilogFT,
	kCSharpFT,
	kErlangFT,
	kSMLFT,
	kJavaScriptFT,
	kAntFT,
	kJSPFT,
	kXMLFT,
	kBasicFT,
	kShellOutputFT,
	kMatlabFT,
	kAdobeFlexFT,
	kINIFT,
	kPropertiesFT,
	kGoFT,
	kDFT,				// = kLastFT	// special

	// When you add new types, be sure to increment the prefs version!

	kFirstFT = kUnknownFT,
	kLastFT  = kDFT
};

const JSize kFTCount = kLastFT+1;

std::istream& operator>>(std::istream& input, TextFileType& type);
std::ostream& operator<<(std::ostream& output, const TextFileType type);

bool			GetComplementType(const TextFileType inputType, TextFileType* outputType);
const JString&	GetComplementFileTypeName(const TextFileType type);

bool	LangIsCharacterInWord(const TextFileType type, const JUtf8Character& c);

inline bool
HasComplementType
	(
	const TextFileType type
	)
{
	TextFileType complType;
	return GetComplementType(type, &complType);
}

inline bool
IsHeaderType
	(
	const TextFileType type
	)
{
	return (type == kCHeaderFT          ||
			type == kModula2InterfaceFT ||
			type == kModula3InterfaceFT ||
			type == kVeraHeaderFT);
}

inline bool
CanCompile
	(
	const TextFileType type
	)
{
	return (type == kCSourceFT       ||
			type == kCSharpFT        ||
			type == kDFT             ||
			type == kLexFT           ||
			type == kBisonFT         ||
			type == kEiffelFT        ||
			type == kErlangFT        ||
			type == kFortranFT       ||
			type == kRatforFT        ||
			type == kJavaSourceFT    ||
			type == kJSPFT           ||
			type == kGoFT            ||
			type == kPascalFT        ||
			type == kAssemblyFT      ||
			type == kModula2ModuleFT ||
			type == kModula3ModuleFT ||
			type == kCobolFT         ||
			type == kBetaFT          ||
			type == kLuaFT           ||
			type == kVeraSourceFT    ||
			type == kVerilogFT       ||
			type == kBasicFT         ||
			type == kOtherSourceFT);
}

inline bool
IsLibrary
	(
	const TextFileType type
	)
{
	return (type == kStaticLibraryFT ||
			type == kSharedLibraryFT ||
			type == kJavaArchiveFT);
}

inline bool
IncludeInMakeFiles
	(
	const TextFileType type
	)
{
	return CanCompile(type) || IsLibrary(type);
}

inline bool
IncludeInCMakeSource
	(
	const TextFileType type
	)
{
	return type == kCSourceFT || type == kLexFT || type == kBisonFT;
}

inline bool
IncludeInCMakeHeader
	(
	const TextFileType type
	)
{
	return type == kCHeaderFT;
}

inline bool
IncludeInQMakeSource
	(
	const TextFileType type
	)
{
	return type == kCSourceFT || type == kLexFT || type == kBisonFT;
}

inline bool
IncludeInQMakeHeader
	(
	const TextFileType type
	)
{
	return type == kCHeaderFT;
}

inline bool
ExcludeFromFileList
	(
	const TextFileType type
	)
{
	return IsLibrary(type);
}

inline bool
UseCSourceWindowSize
	(
	const TextFileType type
	)
{
	return CanCompile(type)         ||
				type == kDocumentationFT ||
				type == kAntFT           ||
				type == kASPFT           ||
				type == kAWKFT           ||
				type == kBisonFT         ||
				type == kINIFT           ||
				type == kJavaScriptFT    ||
				type == kJSPFT           ||
				type == kLexFT           ||
				type == kLispFT          ||
				type == kMakeFT          ||
				type == kPerlFT          ||
				type == kPHPFT           ||
				type == kPropertiesFT    ||
				type == kPythonFT        ||
				type == kREXXFT          ||
				type == kRubyFT          ||
				type == kSchemeFT        ||
				type == kBourneShellFT   ||
				type == kCShellFT        ||
				type == kSLangFT         ||
				type == kSMLFT           ||
				type == kSQLFT           ||
				type == kTCLFT           ||
				type == kVimFT;
}

inline bool
CanDebug
	(
	const TextFileType type
	)
{
	return CanCompile(type) || type == kCHeaderFT;
}

inline bool
IsCommandOutput
	(
	const TextFileType type
	)
{
	return type == kExecOutputFT || type == kSearchOutputFT;
}

inline bool
DrawRightMargin
	(
	const TextFileType type
	)
{
	return !( IsCommandOutput(type) ||
			  type == kManPageFT    ||
			  type == kDiffOutputFT ||
			  type == kShellOutputFT );
}


// Do not change these values once they are assigned because
// they are stored in the project file.

enum Language
{
	kOtherLang = 0,
	kCLang,
	kEiffelLang,
	kFortranLang,
	kJavaLang,
	kAssemblyLang,
	kAWKLang,
	kCobolLang,
	kLispLang,
	kPerlLang,
	kPHPLang,
	kPythonLang,
	kSchemeLang,
	kBourneShellLang,
	kTCLLang,
	kVimLang,
	kHTMLLang,
	kASPLang,
	kMakeLang,
	kPascalLang,
	kREXXLang,
	kRubyLang,
	kLexLang,
	kCShellLang,
	kBisonLang,
	kBetaLang,
	kLuaLang,
	kSLangLang,
	kSQLLang,
	kVeraLang,
	kVerilogLang,
	kCSharpLang,
	kErlangLang,
	kSMLLang,
	kJavaScriptLang,
	kAntLang,
	kJSPLang,
	kXMLLang,
	kBasicLang,
	kMatlabLang,
	kAdobeFlexLang,
	kINILang,
	kPropertiesLang,
	kGoLang,
	kDLang,		// = kLastLang

	// When you add new types, be sure to increment the prefs version!

	kFirstLang = kOtherLang,
	kLastLang  = kDLang
};

const JSize kLangCount = kLastLang+1;

Language	GetLanguage(const TextFileType type);

bool	GetStyler(const Language lang, StylerBase** styler);
bool	GetCompleter(const Language lang, StringCompleter** completer);

void	ShutdownStylers();
void	ShutdownCompleters();

bool	NameIsQualified(const JString& s);

inline JString::Case
IsCaseSensitive
	(
	const Language lang
	)
{
	return (lang != kEiffelLang  &&
			lang != kFortranLang &&
			lang != kJavaScriptLang ? JString::kCompareCase : JString::kIgnoreCase);
}

inline bool
HasNamespace
	(
	const Language lang
	)
{
	return lang == kCLang          ||	// C++, actually
		   lang == kDLang          ||
		   lang == kGoLang         ||
		   lang == kHTMLLang       ||	// for JavaScript
		   lang == kJavaLang       ||
		   lang == kJavaScriptLang ||
		   lang == kPerlLang       ||
		   lang == kPHPLang        ||
		   lang == kSQLLang;
}

inline bool
GetStyler
	(
	const TextFileType	type,
	StylerBase**		styler
	)
{
	return GetStyler(GetLanguage(type), styler);
}

inline bool
GetCompleter
	(
	const TextFileType	type,
	StringCompleter**	completer
	)
{
	return GetCompleter(GetLanguage(type), completer);
}

#endif

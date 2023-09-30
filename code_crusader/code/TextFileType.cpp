/******************************************************************************
 TextFileType.cpp

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "TextFileType.h"

#include "CStyler.h"
#include "DStyler.h"
#include "GoStyler.h"
#include "HTMLStyler.h"
#include "JavaStyler.h"

#ifdef CODE_CRUSADER

#include "BourneShellStyler.h"
#include "CSharpStyler.h"
#include "CShellStyler.h"
#include "EiffelStyler.h"
#include "INIStyler.h"
#include "JavaScriptStyler.h"
#include "MakeStyler.h"
#include "PerlStyler.h"
#include "PropertiesStyler.h"
#include "PythonStyler.h"
#include "RubyStyler.h"
#include "SQLStyler.h"
#include "TCLStyler.h"

#include "BisonCompleter.h"
#include "BourneShellCompleter.h"
#include "CCompleter.h"
#include "CSharpCompleter.h"
#include "CShellCompleter.h"
#include "DCompleter.h"
#include "EiffelCompleter.h"
#include "FortranCompleter.h"
#include "GoCompleter.h"
#include "HTMLCompleter.h"
#include "JavaCompleter.h"
#include "JavaScriptCompleter.h"
#include "JSPCompleter.h"
#include "LexCompleter.h"
#include "LuaCompleter.h"
#include "MakeCompleter.h"
#include "PascalCompleter.h"
#include "PerlCompleter.h"
#include "PHPCompleter.h"
#include "PythonCompleter.h"
#include "REXXCompleter.h"
#include "RubyCompleter.h"
#include "SQLCompleter.h"
#include "TCLCompleter.h"

#else

#include "StylerStubs.h"
#include <jx-af/jcore/JString.h>

#endif

#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 LangIsCharacterInWord

	Returns true if the character is a special part of a word for the
	given language.

 ******************************************************************************/

bool
LangIsCharacterInWord
	(
	const TextFileType	type,
	const JUtf8Character&	c
	)
{
	if (type == kPerlFT)
	{
		return c == '$' || c == '%' || c == '@';
	}
	else if (type == kAWKFT         ||
			 type == kPHPFT         ||
			 type == kPythonFT      ||
			 type == kBourneShellFT ||
			 type == kCShellFT      ||
			 type == kMakeFT        ||
			 type == kTCLFT)
	{
		return c == '$';
	}
	else if (type == kJavaSourceFT ||
			 type == kSQLFT)
	{
		return c == '@';
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 NameIsQualified

	Returns true if the character is a special part of a word for the
	given language.

 ******************************************************************************/

bool
NameIsQualified
	(
	const JString& s
	)
{
	return s.Contains(":") || s.Contains(".") || s.Contains("\\");
}

/******************************************************************************
 GetNamespaceOperator

	Returns the namespace operator for the given language.  Explodes if
	the language doesn't support namespaces, i.e., !HasNamespace(lang).

 ******************************************************************************/

const JUtf8Byte*
GetNamespaceOperator
	(
	const Language lang
	)
{
	if (lang == kCLang    ||
		lang == kPerlLang ||
		lang == kTCLLang)
	{
		return "::";
	}
	else if (lang == kCSharpLang     ||
			 lang == kDLang          ||
			 lang == kEiffelLang     ||
			 lang == kGoLang         ||
			 lang == kHTMLLang       ||	// for JavaScript
			 lang == kJavaLang       ||
			 lang == kJavaScriptLang ||
			 lang == kLuaLang        ||
			 lang == kPascalLang     ||
			 lang == kPythonLang     ||
			 lang == kRubyLang       ||
			 lang == kSchemeLang)
	{
		return ".";
	}
	else if (lang == kPHPLang)
	{
		return "\\";
	}
	else if (lang == kSQLLang)
	{
		return ":";
	}
	else
	{
		assert( false );
		return "";
	}
}

/******************************************************************************
 Complement file mapping

 ******************************************************************************/

struct ComplMap
{
	TextFileType t1;
	TextFileType t2;
};

static const ComplMap kComplMap[] =
{
{ kCSourceFT,       kCHeaderFT },
{ kModula2ModuleFT, kModula2InterfaceFT },
{ kModula3ModuleFT, kModula3InterfaceFT },
{ kVeraSourceFT,    kVeraHeaderFT },
};

bool
GetComplementType
	(
	const TextFileType	inputType,
	TextFileType*		outputType
	)
{
	for (auto& item : kComplMap)
	{
		if (inputType == item.t1)
		{
			*outputType = item.t2;
			return true;
		}
		if (inputType == item.t2)
		{
			*outputType = item.t1;
			return true;
		}
	}

	*outputType = kUnknownFT;
	return false;
}

struct ComplName
{
	TextFileType		type;
	const JUtf8Byte*	name;
};

static const ComplName kComplName[] =
{
{ kCSourceFT,          "ComplementHeader::global"    },
{ kCHeaderFT,          "ComplementSource::global"    },
{ kModula2ModuleFT,    "ComplementInterface::global" },
{ kModula2InterfaceFT, "ComplementModule::global"    },
{ kModula3ModuleFT,    "ComplementInterface::global" },
{ kModula3InterfaceFT, "ComplementModule::global"    },
{ kVeraSourceFT,       "ComplementHeader::global"    },
{ kVeraHeaderFT,       "ComplementSource::global"    },
};

const JString&
GetComplementFileTypeName
	(
	const TextFileType type
	)
{
	for (const auto& item : kComplName)
	{
		if (type == item.type)
		{
			return JGetString(item.name);
		}
	}

	return JString::empty;
}

/******************************************************************************
 GetLanguage

 ******************************************************************************/

struct FT2Lang
{
	TextFileType	type;
	Language		lang;
};

static const FT2Lang kFT2Lang[] =
{
{ kUnknownFT,          kOtherLang       },
{ kCSourceFT,          kCLang           },
{ kCHeaderFT,          kCLang           },
{ kOtherSourceFT,      kOtherLang       },
{ kDocumentationFT,    kOtherLang       },
{ kHTMLFT,             kHTMLLang        },
{ kEiffelFT,           kEiffelLang      },
{ kFortranFT,          kFortranLang     },
{ kJavaSourceFT,       kJavaLang        },
{ kStaticLibraryFT,    kOtherLang       },
{ kSharedLibraryFT,    kOtherLang       },
{ kExecOutputFT,       kOtherLang       },
{ kManPageFT,          kOtherLang       },
{ kDiffOutputFT,       kOtherLang       },
{ kAssemblyFT,         kAssemblyLang    },
{ kPascalFT,           kPascalLang      },
{ kRatforFT,           kFortranLang     },
{ kExternalFT,         kOtherLang       },
{ kBinaryFT,           kOtherLang       },
{ kModula2ModuleFT,    kOtherLang       },
{ kModula2InterfaceFT, kOtherLang       },
{ kModula3ModuleFT,    kOtherLang       },
{ kModula3InterfaceFT, kOtherLang       },
{ kAWKFT,              kAWKLang         },
{ kCobolFT,            kCobolLang       },
{ kLispFT,             kLispLang        },
{ kPerlFT,             kPerlLang        },
{ kPythonFT,           kPythonLang      },
{ kSchemeFT,           kSchemeLang      },
{ kBourneShellFT,      kBourneShellLang },
{ kTCLFT,              kTCLLang         },
{ kVimFT,              kVimLang         },
{ kJavaArchiveFT,      kOtherLang       },
{ kPHPFT,              kPHPLang         },
{ kASPFT,              kASPLang         },
{ kSearchOutputFT,     kOtherLang       },
{ kMakeFT,             kMakeLang        },
{ kREXXFT,             kREXXLang        },
{ kRubyFT,             kRubyLang        },
{ kLexFT,              kLexLang         },
{ kCShellFT,           kCShellLang      },
{ kBisonFT,            kBisonLang       },
{ kBetaFT,             kBetaLang        },
{ kLuaFT,              kLuaLang         },
{ kSLangFT,            kSLangLang       },
{ kSQLFT,              kSQLLang         },
{ kVeraSourceFT,       kVeraLang        },
{ kVeraHeaderFT,       kVeraLang        },
{ kVerilogFT,          kVerilogLang     },
{ kCSharpFT,           kCSharpLang      },
{ kErlangFT,           kErlangLang      },
{ kSMLFT,              kSMLLang         },
{ kJavaScriptFT,       kJavaScriptLang  },
{ kAntFT,              kAntLang         },
{ kJSPFT,              kJSPLang         },
{ kXMLFT,              kXMLLang         },
{ kBasicFT,            kBasicLang       },
{ kUnused1FT,          kOtherLang       },
{ kMatlabFT,           kMatlabLang      },
{ kAdobeFlexFT,        kAdobeFlexLang   },
{ kINIFT,              kINILang         },
{ kPropertiesFT,       kPropertiesLang  },
{ kGoFT,               kGoLang          },
{ kDFT,                kDLang           },
};

const JSize kFT2LangCount = sizeof(kFT2Lang) / sizeof(FT2Lang);

Language
GetLanguage
	(
	const TextFileType type
	)
{
	assert( kFT2LangCount == kFTCount );
	assert( kFT2Lang[type].type == type );
	return kFT2Lang[type].lang;
}

/******************************************************************************
 GetStyler

 ******************************************************************************/

struct Lang2Styler
{
	Language	lang;
	StylerBase*	(*create)();
	void		(*destroy)();
};

static const Lang2Styler kLang2Styler[] =
{
{ kOtherLang,       nullptr, nullptr },
{ kCLang,           &CStyler::Instance,           &CStyler::Shutdown },
{ kEiffelLang,      &EiffelStyler::Instance,      &EiffelStyler::Shutdown },
{ kFortranLang,     nullptr, nullptr },
{ kJavaLang,        &JavaStyler::Instance,        &JavaStyler::Shutdown },
{ kAssemblyLang,    nullptr, nullptr },
{ kAWKLang,         nullptr, nullptr },
{ kCobolLang,       nullptr, nullptr },
{ kLispLang,        nullptr, nullptr },
{ kPerlLang,        &PerlStyler::Instance,        &PerlStyler::Shutdown },
{ kPHPLang,         &HTMLStyler::Instance,        &HTMLStyler::Shutdown },
{ kPythonLang,      &PythonStyler::Instance,      &PythonStyler::Shutdown },
{ kSchemeLang,      nullptr, nullptr },
{ kBourneShellLang, &BourneShellStyler::Instance, &BourneShellStyler::Shutdown },
{ kTCLLang,         &TCLStyler::Instance,         &TCLStyler::Shutdown },
{ kVimLang,         nullptr, nullptr },
{ kHTMLLang,        &HTMLStyler::Instance,        &HTMLStyler::Shutdown },
{ kASPLang,         nullptr, nullptr },
{ kMakeLang,        &MakeStyler::Instance,        &MakeStyler::Shutdown },
{ kPascalLang,      nullptr, nullptr },
{ kREXXLang,        nullptr, nullptr },
{ kRubyLang,        &RubyStyler::Instance,        &RubyStyler::Shutdown },
{ kLexLang,         nullptr, nullptr },
{ kCShellLang,      &CShellStyler::Instance,      &CShellStyler::Shutdown },
{ kBisonLang,       &CStyler::Instance,           &CStyler::Shutdown },
{ kBetaLang,        nullptr, nullptr },
{ kLuaLang,         nullptr, nullptr },
{ kSLangLang,       nullptr, nullptr },
{ kSQLLang,         &SQLStyler::Instance,         &SQLStyler::Shutdown },
{ kVeraLang,        nullptr, nullptr },
{ kVerilogLang,     nullptr, nullptr },
{ kCSharpLang,      &CSharpStyler::Instance,      &CSharpStyler::Shutdown },
{ kErlangLang,      nullptr, nullptr },
{ kSMLLang,         nullptr, nullptr },
{ kJavaScriptLang,  &JavaScriptStyler::Instance,  &JavaScriptStyler::Shutdown },
{ kAntLang,         &HTMLStyler::Instance,        &HTMLStyler::Shutdown },
{ kJSPLang,         &HTMLStyler::Instance,        &HTMLStyler::Shutdown },
{ kXMLLang,         &HTMLStyler::Instance,        &HTMLStyler::Shutdown },
{ kBasicLang,       nullptr, nullptr },
{ kMatlabLang,      nullptr, nullptr },
{ kAdobeFlexLang,   nullptr, nullptr },
{ kINILang,         &INIStyler::Instance,         &INIStyler::Shutdown },
{ kPropertiesLang,  &PropertiesStyler::Instance,  &PropertiesStyler::Shutdown },
{ kGoLang,          &GoStyler::Instance,          &GoStyler::Shutdown },
{ kDLang,           &DStyler::Instance,           &DStyler::Shutdown },
};

const JSize kLang2StylerCount = sizeof(kLang2Styler) / sizeof(Lang2Styler);

bool
GetStyler
	(
	const Language	lang,
	StylerBase**	styler
	)
{
	assert( kLang2StylerCount == kLangCount );
	assert( kLang2Styler[lang].lang == lang );

	if (kLang2Styler[lang].create != nullptr)
	{
		*styler = kLang2Styler[lang].create();
		return *styler != nullptr;
	}
	else
	{
		*styler = nullptr;
		return false;
	}
}

void
ShutdownStylers()
{
	for (auto& item : kLang2Styler)
	{
		if (item.destroy != nullptr)
		{
			item.destroy();
		}
	}
}

#ifdef CODE_CRUSADER

/******************************************************************************
 GetCompleter

 ******************************************************************************/

struct Lang2Completer
{
	Language			lang;
	StringCompleter*	(*create)();
	void				(*destroy)();
};

static const Lang2Completer kLang2Completer[] =
{
{ kOtherLang,       nullptr, nullptr },
{ kCLang,           &CCompleter::Instance,           &CCompleter::Shutdown },
{ kEiffelLang,      &EiffelCompleter::Instance,      &EiffelCompleter::Shutdown },
{ kFortranLang,     &FortranCompleter::Instance,     &FortranCompleter::Shutdown },
{ kJavaLang,        &JavaCompleter::Instance,        &JavaCompleter::Shutdown },
{ kAssemblyLang,    nullptr, nullptr },
{ kAWKLang,         nullptr, nullptr },
{ kCobolLang,       nullptr, nullptr },
{ kLispLang,        nullptr, nullptr },
{ kPerlLang,        &PerlCompleter::Instance,        &PerlCompleter::Shutdown },
{ kPHPLang,         &PHPCompleter::Instance,         &PHPCompleter::Shutdown },
{ kPythonLang,      &PythonCompleter::Instance,      &PythonCompleter::Shutdown },
{ kSchemeLang,      nullptr, nullptr },
{ kBourneShellLang, &BourneShellCompleter::Instance, &BourneShellCompleter::Shutdown },
{ kTCLLang,         &TCLCompleter::Instance,         &TCLCompleter::Shutdown },
{ kVimLang,         nullptr, nullptr },
{ kHTMLLang,        &HTMLCompleter::Instance,        &HTMLCompleter::Shutdown },
{ kASPLang,         nullptr, nullptr },
{ kMakeLang,        &MakeCompleter::Instance,        &MakeCompleter::Shutdown },
{ kPascalLang,      &PascalCompleter::Instance,      &PascalCompleter::Shutdown },
{ kREXXLang,        &REXXCompleter::Instance,        &REXXCompleter::Shutdown },
{ kRubyLang,        &RubyCompleter::Instance,        &RubyCompleter::Shutdown },
{ kLexLang,         &LexCompleter::Instance,         &LexCompleter::Shutdown },
{ kCShellLang,      &CShellCompleter::Instance,      &CShellCompleter::Shutdown },
{ kBisonLang,       &BisonCompleter::Instance,       &BisonCompleter::Shutdown },
{ kBetaLang,        nullptr, nullptr },
{ kLuaLang,         &LuaCompleter::Instance,         &LuaCompleter::Shutdown },
{ kSLangLang,       nullptr, nullptr },
{ kSQLLang,         &SQLCompleter::Instance,         &SQLCompleter::Shutdown },
{ kVeraLang,        nullptr, nullptr },
{ kVerilogLang,     nullptr, nullptr },
{ kCSharpLang,      &CSharpCompleter::Instance,      &CSharpCompleter::Shutdown },
{ kErlangLang,      nullptr, nullptr },
{ kSMLLang,         nullptr, nullptr },
{ kJavaScriptLang,  &JavaScriptCompleter::Instance,  &JavaScriptCompleter::Shutdown },
{ kAntLang,         nullptr, nullptr },
{ kJSPLang,         &JSPCompleter::Instance,         &JSPCompleter::Shutdown },
{ kXMLLang,         &HTMLCompleter::Instance,        &HTMLCompleter::Shutdown },
{ kBasicLang,       nullptr, nullptr },
{ kMatlabLang,      nullptr, nullptr },
{ kAdobeFlexLang,   nullptr, nullptr },
{ kINILang,         nullptr, nullptr },
{ kPropertiesLang,  nullptr, nullptr },
{ kGoLang,          &GoCompleter::Instance,          &GoCompleter::Shutdown },
{ kDLang,           &DCompleter::Instance,           &DCompleter::Shutdown },
};

const JSize kLang2CompleterCount = sizeof(kLang2Completer) / sizeof(Lang2Completer);

bool
GetCompleter
	(
	const Language		lang,
	StringCompleter**	completer
	)
{
	assert( kLang2CompleterCount == kLangCount );
	assert( kLang2Completer[lang].lang == lang );

	if (kLang2Completer[lang].create != nullptr)
	{
		*completer = kLang2Completer[lang].create();
		return *completer != nullptr;
	}
	else
	{
		*completer = nullptr;
		return false;
	}
}

void
ShutdownCompleters()
{
	for (auto& item : kLang2Completer)
	{
		if (item.destroy != nullptr)
		{
			item.destroy();
		}
	}
}

#endif

/******************************************************************************
 Stream operators

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&		input,
	TextFileType&	type
	)
{
	long temp;
	input >> temp;
	type = (TextFileType) temp;
	assert( kFirstFT <= type && type <= kLastFT );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&				output,
	const TextFileType	type
	)
{
	output << (long) type;
	return output;
}

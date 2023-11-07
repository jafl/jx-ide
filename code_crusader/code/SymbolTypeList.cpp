/******************************************************************************
 SymbolTypeList.cpp

	BASE CLASS = JContainer, JPrefObject

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "SymbolTypeList.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXImageCache.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;

// JBroadcaster message types

const JUtf8Byte* SymbolTypeList::kVisibilityChanged = "VisibilityChanged::SymbolTypeList";
const JUtf8Byte* SymbolTypeList::kStyleChanged      = "StyleChanged::SymbolTypeList";

/******************************************************************************
 Constructor

 ******************************************************************************/

SymbolTypeList::SymbolTypeList
	(
	JXDisplay* display
	)
	:
	JContainer(),
	JPrefObject(GetPrefsManager(), kSymbolTypeListID)
{
	CreateSymTypeList(display);
	JPrefObject::ReadPrefs();
	InstallCollection(itsSymbolTypeList);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SymbolTypeList::~SymbolTypeList()
{
	jdelete itsSymbolTypeList;
}

/******************************************************************************
 SkipSetup (static)

	Ignore the old data in the project file.

 ******************************************************************************/

void
SymbolTypeList::SkipSetup
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	if (vers >= 74)
	{
		return;
	}

	if (vers >= 71)
	{
		input >> std::ws;
		JIgnoreLine(input);
	}

	if (vers <= 71)
	{
		JSize typeCount;
		input >> typeCount;

		for (JIndex i=1; i<=typeCount; i++)
		{
			long type;
			input >> type;

			bool visible;
			input >> JBoolFromString(visible);

			JFontStyle style;
			input >> JBoolFromString(style.bold);
			input >> JBoolFromString(style.italic);
			input >> style.underlineCount;
			input >> JBoolFromString(style.strike);

			JRGB color;
			input >> color;
		}
	}
	else
	{
		while (true)
		{
			bool keepGoing;
			input >> JBoolFromString(keepGoing);
			if (!keepGoing)
			{
				break;
			}

			JIgnoreLine(input);
		}
	}
}

/******************************************************************************
 ReadPrefs (virtual protected)

	We don't broadcast because this is called long before SymbolList is
	loaded.

 ******************************************************************************/

void
SymbolTypeList::ReadPrefs
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

	JSize typeCount;
	input >> typeCount;

	for (JIndex i=1; i<=typeCount; i++)
	{
		long type;
		input >> type;

		const JIndex j      = FindType((SymbolList::Type) type);
		SymbolTypeInfo info = itsSymbolTypeList->GetItem(j);

		input >> JBoolFromString(info.visible);
		input >> JBoolFromString(info.style.bold);
		input >> JBoolFromString(info.style.italic);
		input >> info.style.underlineCount;
		input >> JBoolFromString(info.style.strike);

		JRGB color;
		input >> color;
		info.style.color = JColorManager::GetColorID(color);

		itsSymbolTypeList->SetItem(j, info);
	}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
SymbolTypeList::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	const JSize typeCount = itsSymbolTypeList->GetItemCount();
	output << ' ' << typeCount;

	for (JIndex i=1; i<=typeCount; i++)
	{
		const SymbolTypeInfo info = itsSymbolTypeList->GetItem(i);
		output << ' ' << (long) info.type;
		output << ' ' << JBoolToString(info.visible);

		output << ' ' << JBoolToString(info.style.bold);
		output << ' ' << JBoolToString(info.style.italic);
		output << ' ' << info.style.underlineCount;
		output << ' ' << JBoolToString(info.style.strike);
		output << ' ' << JColorManager::GetRGB(info.style.color);
	}
}

/******************************************************************************
 FindType (private)

	This does not return bool because the list must contain all types.

 ******************************************************************************/

JIndex
SymbolTypeList::FindType
	(
	const SymbolList::Type type
	)
	const
{
	const SymbolTypeInfo* info = itsSymbolTypeList->GetCArray();

	const JSize count = itsSymbolTypeList->GetItemCount();
	for (JUnsignedOffset i=0; i<count; i++)
	{
		if (info[i].type == type)
		{
			return i+1;
		}
	}

	assert_msg( 0, "SymbolTypeList::FindType couldn't find type" );
	return 0;
}

/******************************************************************************
 CreateSymTypeList (private)

 ******************************************************************************/

#define ADD(C,L) \
	itsSymbolTypeList->AppendItem(SymbolTypeInfo( \
		SymbolList::C, L, JFontStyle(), nullptr));

#define ADD_S(C,L,S) \
	itsSymbolTypeList->AppendItem(SymbolTypeInfo( \
		SymbolList::C, L, S, nullptr));

#define ADD_I(C,L,I) \
	itsSymbolTypeList->AppendItem(SymbolTypeInfo( \
		SymbolList::C, L, JFontStyle(), I));

#define ADD_SI(C,L,S,I) \
	itsSymbolTypeList->AppendItem(SymbolTypeInfo( \
		SymbolList::C, L, S, I));

void
SymbolTypeList::CreateSymTypeList
	(
	JXDisplay* display
	)
{
	LoadIcons(display);

	const JFontStyle bold(true, false, 0, false),
					 italic(false, true, 0, false),
					 bold_gray(true, false, 0, false, JColorManager::GetGrayColor(50));

	itsSymbolTypeList = jnew JArray<SymbolTypeInfo>(512);
	assert( itsSymbolTypeList != nullptr );

	ADD_S (kUnknownST, kOtherLang, JFontStyle(JColorManager::GetGrayColor(50)))

	ADD_I(kAdobeFlexFunctionST,       kAdobeFlexLang, itsFunctionIcon)
	ADD_S(kAdobeFlexClassST,          kAdobeFlexLang, bold)
	ADD_I(kAdobeFlexMethodST,         kAdobeFlexLang, itsFunctionIcon)
	ADD_I(kAdobeFlexGlobalVariableST, kAdobeFlexLang, itsVariableIcon)
	ADD  (kAdobeFlexMxTagST,          kAdobeFlexLang)

	ADD(kAntTargetST, kAntLang)

	ADD_I(kAssemblyLabelST,  kAssemblyLang, itsAssemblyLabelIcon)
	ADD_I(kAssemblyDefineST, kAssemblyLang, itsCMacroIcon)
	ADD_I(kAssemblyMacroST,  kAssemblyLang, itsCMacroIcon)
	ADD  (kAssemblyTypeST,   kAssemblyLang)

	ADD_I(kASPConstantST,   kASPLang, itsCMacroIcon)
	ADD_S(kASPClassST,      kASPLang, bold)
	ADD_I(kASPFunctionST,   kASPLang, itsFunctionIcon)
	ADD_I(kASPSubroutineST, kASPLang, itsFunctionIcon)
	ADD_I(kASPVariableST,   kASPLang, itsVariableIcon)

	ADD_I(kAWKFunctionST, kAWKLang, itsFunctionIcon)

	ADD_I(kBasicConstantST,    kBasicLang, itsCMacroIcon)
	ADD_I(kBasicFunctionST,    kBasicLang, itsFunctionIcon)
	ADD_I(kBasicLabelST,       kBasicLang, itsAssemblyLabelIcon)
	ADD  (kBasicTypeST,        kBasicLang)
	ADD_I(kBasicVariableST,    kBasicLang, itsVariableIcon)
	ADD_I(kBasicEnumerationST, kBasicLang, itsCEnumIcon)

	ADD(kBetaFragmentST, kBetaLang)
	ADD(kBetaPatternST,  kBetaLang)
	ADD(kBetaSlotST,     kBetaLang)

	ADD_I(kBisonNonterminalDefST,  kBisonLang, itsBisonNontermDefIcon)
	ADD_I(kBisonNonterminalDeclST, kBisonLang, itsBisonNontermDeclIcon)
	ADD_I(kBisonTerminalDeclST,    kBisonLang, itsBisonTermDeclIcon)

	ADD_SI(kCClassST,          kCLang, bold, itsCClassIcon)
	ADD_SI(kCMacroST,          kCLang, JFontStyle(JColorManager::GetBlueColor()), itsCMacroIcon)
	ADD_I (kCEnumValueST,      kCLang, itsMemberIcon)
	ADD_I (kCFunctionST,       kCLang, itsFunctionIcon)
	ADD_I (kCEnumST,           kCLang, itsCEnumIcon)
	ADD_I (kCNamespaceST,      kCLang, itsCNamespaceIcon)
	ADD_SI(kCStructST,         kCLang, bold_gray, itsCStructIcon)
	ADD_SI(kCTypedefST,        kCLang, italic, itsCTypedefIcon)
	ADD_I (kCUnionST,          kCLang, itsCUnionIcon)
	ADD_I (kCVariableST,       kCLang, itsVariableIcon)
	ADD_SI(kCPrototypeST,      kCLang, italic, itsPrototypeIcon)
	ADD_I (kCMemberST,         kCLang, itsMemberIcon)
	ADD_I (kCExternVariableST, kCLang, itsVariableIcon)

	ADD_SI(kCSharpClassST,      kCSharpLang, bold, itsCSharpClassIcon)
	ADD_SI(kCSharpMacroST,      kCSharpLang, JFontStyle(JColorManager::GetBlueColor()), itsCMacroIcon)
	ADD_I (kCSharpEnumValueST,  kCSharpLang, itsMemberIcon)
	ADD_I (kCSharpEventST,      kCSharpLang, itsCSharpEventIcon)
	ADD_I (kCSharpFieldST,      kCSharpLang, itsVariableIcon)
	ADD_I (kCSharpEnumNameST,   kCSharpLang, itsCEnumIcon)
	ADD_SI(kCSharpInterfaceST,  kCSharpLang, bold_gray, itsCSharpInterfaceIcon)
	ADD_I (kCSharpMethodST,     kCSharpLang, itsFunctionIcon)
	ADD_I (kCSharpNamespaceST,  kCSharpLang, itsCSharpNamespaceIcon)
	ADD_I (kCSharpPropertyST,   kCSharpLang, itsMemberIcon)
	ADD_I (kCSharpStructNameST, kCSharpLang, itsCSharpStructIcon)
	ADD_SI(kCSharpTypedefST,    kCSharpLang, italic, itsCTypedefIcon)

	ADD_I(kCobolParagraphST, kCobolLang, itsFunctionIcon)
	ADD_I(kCobolDataST,      kCobolLang, itsVariableIcon)
	ADD  (kCobolFileST,      kCobolLang)
	ADD  (kCobolGroupST,     kCobolLang)
	ADD  (kCobolProgramST,   kCobolLang)
	ADD  (kCobolSectionST,   kCobolLang)

	ADD_SI(kEiffelClassST,   kEiffelLang, bold, itsEiffelClassIcon)
	ADD_I (kEiffelFeatureST, kEiffelLang, itsFunctionIcon)

	ADD_SI(kErlangMacroST,    kErlangLang, JFontStyle(JColorManager::GetBlueColor()), itsCMacroIcon)
	ADD_I (kErlangFunctionST, kErlangLang, itsFunctionIcon)
	ADD   (kErlangModuleST,   kErlangLang)
	ADD   (kErlangRecordST,   kErlangLang)

	ADD   (kFortranBlockDataST,      kFortranLang)
	ADD_I (kFortranCommonBlockST,    kFortranLang, itsFortranCommonBlockIcon)
	ADD   (kFortranEntryPointST,     kFortranLang)
	ADD_I (kFortranFunctionST,       kFortranLang, itsFunctionIcon)
	ADD   (kFortranInterfaceST,      kFortranLang)
	ADD   (kFortranModuleST,         kFortranLang)
	ADD   (kFortranNamelistST,       kFortranLang)
	ADD_I (kFortranProgramST,        kFortranLang, itsFunctionIcon)
	ADD_I (kFortranSubroutineST,     kFortranLang, itsFunctionIcon)
	ADD   (kFortranDerivedTypeST,    kFortranLang)
	ADD   (kFortranTypeComponentST,  kFortranLang)
	ADD   (kFortranLabelST,          kFortranLang)
	ADD_I (kFortranLocalVariableST,  kFortranLang, itsVariableIcon)
	ADD_I (kFortranModuleVariableST, kFortranLang, itsVariableIcon)

	ADD(kHTMLIDST, kHTMLLang)

	ADD_SI(kJavaClassST,     kJavaLang, bold, itsJavaClassIcon)
	ADD_I (kJavaEnumValueST, kJavaLang, itsMemberIcon)
	ADD_I (kJavaFieldST,     kJavaLang, itsMemberIcon)
	ADD_I (kJavaEnumST,      kJavaLang, itsCEnumIcon)
	ADD_SI(kJavaInterfaceST, kJavaLang, bold_gray, itsJavaInterfaceIcon)
	ADD_I (kJavaMethodST,    kJavaLang, itsFunctionIcon)
	ADD_SI(kJavaPrototypeST, kJavaLang, italic, itsPrototypeIcon)
	ADD_I (kJavaPackageST,   kJavaLang, itsJavaPackageIcon)

	ADD_I (kJavaScriptFunctionST,       kJavaScriptLang, itsFunctionIcon)
	ADD_SI(kJavaScriptClassST,          kJavaScriptLang, bold, itsJavaScriptClassIcon)
	ADD_I (kJavaScriptMethodST,         kJavaScriptLang, itsFunctionIcon)
	ADD_I (kJavaScriptGlobalVariableST, kJavaScriptLang, itsVariableIcon)

	ADD(kLexStateST, kLexLang)
	ADD(kLexRegexST, kLexLang)

	ADD_I(kLispFunctionST, kLispLang, itsFunctionIcon)

	ADD_I(kLuaFunctionST, kLuaLang, itsFunctionIcon)

	ADD(kMakeVariableST, kMakeLang)
	ADD(kMakeTargetST,   kMakeLang)

	ADD_I(kMatlabFunctionST, kMatlabLang, itsFunctionIcon)

	ADD_I(kPascalFunctionST,  kPascalLang, itsFunctionIcon)
	ADD_I(kPascalProcedureST, kPascalLang, itsFunctionIcon)

	ADD_I(kPerlSubroutineST, kPerlLang, itsFunctionIcon)
	ADD  (kPerlPackageST,    kPerlLang)
	ADD_I(kPerlConstantST,   kPerlLang, itsVariableIcon)
	ADD  (kPerlLabelST,      kPerlLang)
	ADD  (kPerlFormatST,     kPerlLang)

	ADD_SI(kPHPClassST,     kPHPLang, bold, itsPHPClassIcon)
	ADD_SI(kPHPInterfaceST, kPHPLang, bold_gray, itsPHPInterfaceIcon)
	ADD_I (kPHPFunctionST,  kPHPLang, itsFunctionIcon)
	ADD_SI(kPHPDefineST,    kPHPLang, JFontStyle(JColorManager::GetBlueColor()), itsCMacroIcon)

	ADD_SI(kPythonClassST,       kPythonLang, bold, itsPythonClassIcon)
	ADD_I (kPythonFunctionST,    kPythonLang, itsFunctionIcon)
	ADD_I (kPythonClassMemberST, kPythonLang, itsMemberIcon)

	ADD_I(kREXXSubroutineST, kREXXLang, itsFunctionIcon)

	ADD_S(kRubyClassST,           kRubyLang, bold)
	ADD_I(kRubyMethodST,          kRubyLang, itsFunctionIcon)
	ADD_I(kRubySingletonMethodST, kRubyLang, itsFunctionIcon)
	ADD_S(kRubyMixinST,           kRubyLang, bold)

	ADD_I(kSchemeFunctionST, kSchemeLang, itsFunctionIcon)
	ADD  (kSchemeSetST,      kSchemeLang)

	ADD_I(kBourneShellFunctionST, kBourneShellLang, itsFunctionIcon)

	ADD_I(kSLangFunctionST,  kSLangLang, itsFunctionIcon)
	ADD  (kSLangNamespaceST, kSLangLang)

	ADD   (kSMLExceptionST,   kSMLLang)
	ADD_I (kSMLFunctionST,    kSMLLang, itsFunctionIcon)
	ADD_SI(kSMLFunctorST,     kSMLLang, JFontStyle(JColorManager::GetGrayColor(50)), itsFunctionIcon)
	ADD   (kSMLSignatureST,   kSMLLang)
	ADD   (kSMLStructureST,   kSMLLang)
	ADD   (kSMLTypeST,        kSMLLang)
	ADD_I (kSMLValueST,       kSMLLang, itsVariableIcon)

	ADD  (kSQLCursorST,              kSQLLang)
	ADD_I(kSQLPrototypeST,           kSQLLang, itsPrototypeIcon)
	ADD_I(kSQLFunctionST,            kSQLLang, itsFunctionIcon)
	ADD  (kSQLRecordFieldST,         kSQLLang)
	ADD_I(kSQLLocalVariableST,       kSQLLang, itsVariableIcon)
	ADD  (kSQLLabelST,               kSQLLang)
	ADD  (kSQLPackageST,             kSQLLang)
	ADD_I(kSQLProcedureST,           kSQLLang, itsFunctionIcon)
	ADD  (kSQLRecordST,              kSQLLang)
	ADD  (kSQLSubtypeST,             kSQLLang)
	ADD  (kSQLTableST,               kSQLLang)
	ADD  (kSQLTriggerST,             kSQLLang)
	ADD_I(kSQLVariableST,            kSQLLang, itsVariableIcon)
	ADD  (kSQLIndexST,               kSQLLang)
	ADD  (kSQLEventST,               kSQLLang)
	ADD  (kSQLPublicationST,         kSQLLang)
	ADD  (kSQLServiceST,             kSQLLang)
	ADD  (kSQLDomainST,              kSQLLang)
	ADD  (kSQLViewST,                kSQLLang)
	ADD  (kSQLSynonymST,             kSQLLang)
	ADD  (kSQLMobiLinkTableScriptST, kSQLLang)
	ADD  (kSQLMobiLinkConnScriptST,  kSQLLang)

	ADD  (kVeraClassST,	           kVeraLang)
	ADD  (kVeraMacroST,            kVeraLang)
	ADD_I(kVeraEnumValueST,        kVeraLang, itsMemberIcon)
	ADD_I(kVeraFunctionST,         kVeraLang, itsFunctionIcon)
	ADD_I(kVeraEnumST,             kVeraLang, itsCEnumIcon)
	ADD_I(kVeraMemberST,           kVeraLang, itsFunctionIcon)
	ADD  (kVeraProgramST,          kVeraLang)
	ADD_I(kVeraPrototypeST,        kVeraLang, itsPrototypeIcon)
	ADD  (kVeraTaskST,             kVeraLang)
	ADD_I(kVeraTypedefST,          kVeraLang, itsCTypedefIcon)
	ADD_I(kVeraVariableST,         kVeraLang, itsVariableIcon)
	ADD_I(kVeraExternalVariableST, kVeraLang, itsVariableIcon)

	ADD_I(kVerilogFunctionST, kVerilogLang, itsFunctionIcon)
	ADD  (kVerilogModuleST,   kVerilogLang)
	ADD_I(kVerilogConstantST, kVerilogLang, itsVariableIcon)
	ADD  (kVerilogPortST,     kVerilogLang)
	ADD  (kVerilogRegisterST, kVerilogLang)
	ADD  (kVerilogTaskST,     kVerilogLang)
	ADD_I(kVerilogVariableST, kVerilogLang, itsVariableIcon)
	ADD  (kVerilogWireST,     kVerilogLang)
	ADD  (kVerilogEventST,    kVerilogLang)
	ADD  (kVerilogNetST,      kVerilogLang)

	ADD_I(kTCLProcedureST, kTCLLang, itsFunctionIcon)
	ADD  (kTCLClassST,     kTCLLang)
	ADD_I(kTCLMethodST,    kTCLLang, itsFunctionIcon)

	ADD_I(kVimFunctionST,         kVimLang, itsFunctionIcon)
	ADD_I(kVimVariableST,         kVimLang, itsVariableIcon)
	ADD  (kVimAutocommandGroupST, kVimLang)
	ADD_I(kVimUserCommandST,      kVimLang, itsCMacroIcon)
	ADD  (kVimMapST,              kVimLang)
}

/******************************************************************************
 LoadIcons (private)

 ******************************************************************************/

#include "jcc_sym_bison_nonterm_def.xpm"
#include "jcc_sym_bison_nonterm_decl.xpm"
#include "jcc_sym_bison_term_decl.xpm"
#include "jcc_sym_c_class.xpm"
#include "jcc_sym_c_struct.xpm"
#include "jcc_sym_c_enum.xpm"
#include "jcc_sym_c_union.xpm"
#include "jcc_sym_c_macro.xpm"
#include "jcc_sym_c_typedef.xpm"
#include "jcc_sym_c_namespace.xpm"
#include "jcc_sym_c_sharp_class.xpm"
#include "jcc_sym_c_sharp_event.xpm"
#include "jcc_sym_c_sharp_interface.xpm"
#include "jcc_sym_c_sharp_namespace.xpm"
#include "jcc_sym_c_sharp_struct.xpm"
#include "jcc_sym_eiffel_class.xpm"
#include "jcc_sym_fortran_common_blk.xpm"
#include "jcc_sym_java_class.xpm"
#include "jcc_sym_java_interface.xpm"
#include "jcc_sym_java_package.xpm"
#include "jcc_sym_java_script_class.xpm"
#include "jcc_sym_asm_label.xpm"
#include "jcc_sym_python_class.xpm"
#include "jcc_sym_php_class.xpm"
#include "jcc_sym_php_interface.xpm"
#include "jcc_sym_prototype.xpm"
#include "jcc_sym_function.xpm"
#include "jcc_sym_variable.xpm"
#include "jcc_sym_member.xpm"

void
SymbolTypeList::LoadIcons
	(
	JXDisplay* display
	)
{
	JXImageCache* c = display->GetImageCache();

	// C

	itsCClassIcon     = c->GetImage(jcc_sym_c_class);
	itsCStructIcon    = c->GetImage(jcc_sym_c_struct);
	itsCEnumIcon      = c->GetImage(jcc_sym_c_enum);
	itsCUnionIcon     = c->GetImage(jcc_sym_c_union);
	itsCMacroIcon     = c->GetImage(jcc_sym_c_macro);
	itsCTypedefIcon   = c->GetImage(jcc_sym_c_typedef);
	itsCNamespaceIcon = c->GetImage(jcc_sym_c_namespace);

	// C#

	itsCSharpClassIcon     = c->GetImage(jcc_sym_c_sharp_class);
	itsCSharpEventIcon     = c->GetImage(jcc_sym_c_sharp_event);
	itsCSharpInterfaceIcon = c->GetImage(jcc_sym_c_sharp_interface);
	itsCSharpNamespaceIcon = c->GetImage(jcc_sym_c_sharp_namespace);
	itsCSharpStructIcon    = c->GetImage(jcc_sym_c_sharp_struct);

	// Eiffel

	itsEiffelClassIcon = c->GetImage(jcc_sym_eiffel_class);

	// FORTRAN

	itsFortranCommonBlockIcon = c->GetImage(jcc_sym_fortran_common_block);

	// Java

	itsJavaClassIcon     = c->GetImage(jcc_sym_java_class);
	itsJavaInterfaceIcon = c->GetImage(jcc_sym_java_interface);
	itsJavaPackageIcon   = c->GetImage(jcc_sym_java_package);

	// JavaScript

	itsJavaScriptClassIcon = c->GetImage(jcc_sym_java_script_class);

	// Assembly

	itsAssemblyLabelIcon = c->GetImage(jcc_sym_asm_label);

	// Python

	itsPythonClassIcon = c->GetImage(jcc_sym_python_class);

	// PHP

	itsPHPClassIcon     = c->GetImage(jcc_sym_php_class);
	itsPHPInterfaceIcon = c->GetImage(jcc_sym_php_interface);

	// Bison

	itsBisonNontermDefIcon  = c->GetImage(jcc_sym_bison_nonterm_def);
	itsBisonNontermDeclIcon = c->GetImage(jcc_sym_bison_nonterm_decl);
	itsBisonTermDeclIcon    = c->GetImage(jcc_sym_bison_term_decl);

	// shared

	itsPrototypeIcon = c->GetImage(jcc_sym_prototype);
	itsFunctionIcon  = c->GetImage(jcc_sym_function);
	itsVariableIcon  = c->GetImage(jcc_sym_variable);
	itsMemberIcon    = c->GetImage(jcc_sym_member);
}

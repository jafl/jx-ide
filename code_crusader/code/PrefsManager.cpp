/******************************************************************************
 PrefsManager.cpp

	BASE CLASS = JXPrefsManager

	Copyright © 1997-2001 John Lindal.

 ******************************************************************************/

#include "PrefsManager.h"
#include "EditFileTypesDialog.h"
#include "EditMacroDialog.h"
#include "EditCRMDialog.h"
#include "ProjectDocument.h"
#include "TextDocument.h"
#include "TextEditor.h"
#include "CharActionManager.h"
#include "MacroManager.h"
#include "sharedUtil.h"
#include "globals.h"
#include "fileVersions.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXChooseSaveFile.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXSharedPrefsManager.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JSubstitute.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kScriptDir = "scripts";
static const JUtf8Character kNameRegexMarker("*");

const JFileVersion kCurrentTextColorVers = 1;

// version  1 appends kRightMarginColorIndex

// JBroadcaster message types

const JUtf8Byte* PrefsManager::kFileTypesChanged = "FileTypesChanged::PrefsManager";
const JUtf8Byte* PrefsManager::kTextColorChanged = "TextColorChanged::PrefsManager";

/******************************************************************************
 Constructor

 ******************************************************************************/

PrefsManager::PrefsManager
	(
	bool* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, true)
{
	itsFileTypeList = nullptr;
	itsMacroList    = nullptr;
	itsCRMList      = nullptr;

	itsExecOutputWordWrapFlag  = true;
	itsUnknownTypeWordWrapFlag = true;

	itsFileTypesDialog = nullptr;
	itsMacroDialog     = nullptr;
	itsCRMDialog       = nullptr;

	*isNew = JPrefsManager::UpgradeData();

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	csf->SetPrefInfo(this, kgCSFSetupID);
	csf->JPrefObject::ReadPrefs();

	ReadColors();
	ReadStaticGlobalPrefs(kCurrentPrefsFileVersion);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PrefsManager::~PrefsManager()
{
	SaveAllBeforeDestruct();

	JSize count = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		itsFileTypeList->GetElement(i).Free();
	}

	jdelete itsFileTypeList;

	count = itsMacroList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		itsMacroList->GetElement(i).Free();
	}

	jdelete itsMacroList;

	count = itsCRMList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		itsCRMList->GetElement(i).Free();
	}

	jdelete itsCRMList;
}

/******************************************************************************
 SaveAllBeforeDestruct (virtual protected)

 ******************************************************************************/

void
PrefsManager::SaveAllBeforeDestruct()
{
	WriteStaticGlobalPrefs();

	WriteData(*itsMacroList);
	WriteData(*itsCRMList);
	WriteFileTypeInfo();
	WriteColors();

	SetData(kProgramVersionID, GetVersionNumberStr());

	JXPrefsManager::SaveAllBeforeDestruct();
}

/******************************************************************************
 GetJCCVersionStr

 ******************************************************************************/

JString
PrefsManager::GetJCCVersionStr()
	const
{
	std::string data;
	if (GetData(kProgramVersionID, &data))
	{
		return JString(data);
	}
	else
	{
		return JString("< 0.13.0");		// didn't exist before this version
	}
}

/******************************************************************************
 GetExpirationTimeStamp

 ******************************************************************************/

bool
PrefsManager::GetExpirationTimeStamp
	(
	time_t* t
	)
	const
{
	std::string data;
	if (GetData(kExpireTimeStampID, &data))
	{
		std::istringstream input(data);
		input >> *t;
		return !input.fail();
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 SetExpirationTimeStamp

 ******************************************************************************/

void
PrefsManager::SetExpirationTimeStamp
	(
	const time_t t
	)
{
	std::ostringstream data;
	data << t;

	SetData(kExpireTimeStampID, data);
}

/******************************************************************************
 EditFileTypes

 ******************************************************************************/

void
PrefsManager::EditFileTypes()
{
	assert( itsFileTypesDialog == nullptr );

	itsFileTypesDialog =
		jnew EditFileTypesDialog(GetApplication(), *itsFileTypeList,
								  *itsMacroList, *itsCRMList,
								  itsExecOutputWordWrapFlag,
								  itsUnknownTypeWordWrapFlag);
	assert( itsFileTypesDialog != nullptr );
	itsFileTypesDialog->BeginDialog();
	ListenTo(itsFileTypesDialog);
}

/******************************************************************************
 UpdateFileTypes (private)

 ******************************************************************************/

void
PrefsManager::UpdateFileTypes
	(
	const EditFileTypesDialog& dlog
	)
{
	JPtrArray< JPtrArray<JString> > origSuffixList(JPtrArrayT::kDeleteAll);
	for (JUnsignedOffset i=0; i<kFTCount; i++)
	{
		auto* list = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert( list != nullptr );
		origSuffixList.Append(list);

		GetFileSuffixes((TextFileType) i, list);
	}

	dlog.GetFileTypeInfo(itsFileTypeList, &itsExecOutputWordWrapFlag,
						 &itsUnknownTypeWordWrapFlag);

	itsFileTypeList->SetCompareFunction(CompareFileTypeSpecAndLength);
	itsFileTypeList->Sort();

	FileTypesChanged msg;
	JPtrArray<JString> suffixes(JPtrArrayT::kDeleteAll);
	for (JUnsignedOffset i=0; i<kFTCount; i++)
	{
		GetFileSuffixes((TextFileType) i, &suffixes);

		msg.SetChanged((TextFileType) i,
			!JSameStrings(*origSuffixList.GetElement(i+1), suffixes, JString::kCompareCase));
	}

	Broadcast(msg);

	if (msg.AnyChanged())
	{
		WriteSharedPrefs(true);
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
PrefsManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileTypesDialog &&
		message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			UpdateFileTypes(*itsFileTypesDialog);
		}
		itsFileTypesDialog = nullptr;
	}

	else if (sender == itsMacroDialog &&
		message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			UpdateMacros(*itsMacroDialog);
		}
		itsMacroDialog = nullptr;
	}

	else if (sender == itsCRMDialog &&
		message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			UpdateCRMRuleLists(*itsCRMDialog);
		}
		itsCRMDialog = nullptr;
	}

	else
	{
		JXPrefsManager::Receive(sender, message);
	}
}

/******************************************************************************
 addNewSuffixes (local)

 ******************************************************************************/

struct NewSuffixInfo
{
	const JUtf8Byte*	suffix;
	bool			found;
};

void
addNewSuffixes
	(
	const JUtf8Byte*		macroName,		// can be nullptr
	const JUtf8Byte*		origCRMName,	// can be nullptr -- uses macroName
	const TextFileType	type,
	NewSuffixInfo*		newInfo,
	const JSize				newSize,

	JArray<PrefsManager::FileTypeInfo>*			fileTypeList,
	JArray<PrefsManager::MacroSetInfo>*			macroList,
	const JArray<PrefsManager::CRMRuleListInfo>&	crmList
	)
{
	const JSize newCount = newSize / sizeof(NewSuffixInfo);

	// if suffix already exists, don't change it

	const JSize ftCount = fileTypeList->GetElementCount();
	for (JUnsignedOffset i=0; i<newCount; i++)
	{
		for (JIndex j=1; j<=ftCount; j++)
		{
			const PrefsManager::FileTypeInfo ftInfo = fileTypeList->GetElement(j);
			if (*(ftInfo.suffix) == newInfo[i].suffix)
			{
				newInfo[i].found = true;
				break;
			}
		}
	}

	// install new suffixes

	const JIndex macroID = PrefsManager::FindMacroName(macroName, macroList, true);

	const JUtf8Byte* crmName =
		(JString::IsEmpty(origCRMName) ? macroName : origCRMName);

	const JIndex crmID = PrefsManager::FindCRMRuleListName(crmName, crmList);

	for (JUnsignedOffset i=0; i<newCount; i++)
	{
		if (!newInfo[i].found)
		{
			auto* suffix = jnew JString(newInfo[i].suffix);
			assert( suffix != nullptr );

			auto* complSuffix = jnew JString;
			assert( complSuffix != nullptr );

			fileTypeList->InsertSorted(
				PrefsManager::FileTypeInfo(
					suffix, nullptr, nullptr, type, macroID, crmID,
					true, nullptr, false, complSuffix, nullptr));
		}
	}
}

/******************************************************************************
 addNewExternalSuffixes (local)

 ******************************************************************************/

struct NewExternalSuffixInfo
{
	const JUtf8Byte*	suffix;
	const JUtf8Byte*	cmd;
	bool			found;
};

void
addNewExternalSuffixes
	(
	NewExternalSuffixInfo*				newInfo,
	const JSize								newCount,
	JArray<PrefsManager::FileTypeInfo>*	fileTypeList
	)
{
	const JSize ftCount = fileTypeList->GetElementCount();
	for (JUnsignedOffset i=0; i<newCount; i++)
	{
		for (JIndex j=1; j<=ftCount; j++)
		{
			const PrefsManager::FileTypeInfo ftInfo = fileTypeList->GetElement(j);
			if (*(ftInfo.suffix) == newInfo[i].suffix)
			{
				newInfo[i].found = true;
				break;
			}
		}
	}

	for (JUnsignedOffset i=0; i<newCount; i++)
	{
		if (!newInfo[i].found)
		{
			auto* suffix = jnew JString(newInfo[i].suffix);
			assert( suffix != nullptr );

			auto* complSuffix = jnew JString;
			assert( complSuffix != nullptr );

			auto* editCmd = jnew JString(newInfo[i].cmd);
			assert( editCmd != nullptr );

			PrefsManager::FileTypeInfo info(
				suffix, nullptr, nullptr, kExternalFT,
				kEmptyMacroID, kEmptyCRMRuleListID,
				true, nullptr, false, complSuffix, editCmd);
			info.CreateRegex();
			fileTypeList->InsertSorted(info);
		}
	}
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
PrefsManager::UpgradeData
	(
	const bool		isNew,
	const JFileVersion	currentVersion
	)
{
	std::string data;

	//
	// upgrades that must be done in order
	//

	// initialize suffixes

	if (isNew)
	{
		std::ostringstream sourceSuffixData;
		sourceSuffixData << 7;
		sourceSuffixData << ' ' << JString(".c")   << ' ' << JString(".cc");
		sourceSuffixData << ' ' << JString(".cpp") << ' ' << JString(".cxx");
		sourceSuffixData << ' ' << JString(".c++") << ' ' << JString(".C");
		sourceSuffixData << ' ' << JString(".tmpl");
		SetData(10000, sourceSuffixData);

		std::ostringstream headerSuffixData;
		headerSuffixData << 6;
		headerSuffixData << ' ' << JString(".h")   << ' ' << JString(".hh");
		headerSuffixData << ' ' << JString(".hpp") << ' ' << JString(".hxx");
		headerSuffixData << ' ' << JString(".h++") << ' ' << JString(".H");
		SetData(10001, headerSuffixData);
	}

	// add default font

	if (currentVersion < 5)
	{
		SetDefaultFont(JFontManager::GetDefaultMonospaceFontName(),
					   JFontManager::GetDefaultMonospaceFontSize());
	}

	// collect file suffixes

	if (!isNew && currentVersion < 6)
	{
		const JIndex origID[] = { 2, 3, 16, 17 };
		for (JUnsignedOffset i=0; i<4; i++)
		{
			GetData(origID[i], &data);
			RemoveData(origID[i]);
			SetData(10000 + i, data);
		}
	}

	// cmds for using external editor

	if (!isNew && currentVersion == 0)
	{
		std::string oldData;
		GetData(kDocMgrID, &oldData);
		std::ostringstream newData;
		newData << 0 << ' ' << JBoolToString(true) << ' ' << oldData.c_str();
		SetData(kDocMgrID, newData);
	}
	else if (4 <= currentVersion && currentVersion <= 6)
	{
		if (GetData(4, &data))
		{
			RemoveData(4);
			SetData(kPrintPlainTextID, data);
		}
	}

	// remove PS,EPS print setup for tree

	if (!isNew && currentVersion < 8)
	{
		RemoveData(1);
		RemoveData(8);
	}

	// add in misc file type info

	if (currentVersion < 9)
	{
		if (GetData(10003, &data))
		{
			SetData(10004, data);
		}
		if (GetData(10002, &data))
		{
			SetData(10003, data);
		}
		if (GetData(20003, &data))
		{
			SetData(20004, data);
		}

		std::ostringstream otherSourceSuffixData;
		otherSourceSuffixData << 0;
		SetData(10002, otherSourceSuffixData);

		bool codeWrap = false;
		if (GetData(kTextDocID, &data))
		{
			std::istringstream dataStream(data);
			JFileVersion vers = 0;
			dataStream >> vers;
			if (!dataStream.eof() && !dataStream.fail() && vers == 1)
			{
				bool breakCodeCROnly;
				dataStream >> JBoolFromString(breakCodeCROnly);
				codeWrap = !breakCodeCROnly;
			}
		}

		std::ostringstream codeData;
		codeData << JBoolToString(codeWrap);
		SetData(30000, codeData);
		SetData(30001, codeData);
		SetData(30002, codeData);

		std::ostringstream otherData;
		otherData << JBoolToString(true);
		SetData(30003, otherData);
		SetData(30004, otherData);
		SetData(39998, otherData);
		SetData(39999, otherData);
	}

	if (!IDValid(10003))
	{
		std::ostringstream docSuffixData;
		docSuffixData << 1 << ' ' << JString(".doc");
		SetData(10003, docSuffixData);
	}

	if (!IDValid(10004))
	{
		std::ostringstream htmlSuffixData;
		htmlSuffixData << 5;
		htmlSuffixData << ' ' << JString(".html") << ' ' << JString(".HTML");
		htmlSuffixData << ' ' << JString(".htm")  << ' ' << JString(".HTM");
		htmlSuffixData << ' ' << JString(".jsp");
		SetData(10004, htmlSuffixData);
	}

	if (currentVersion < 12)
	{
		// move styler data into main block of ID's

		if (GetData(20000, &data))
		{
			RemoveData(20000);
			SetData(kCStyleID, data);
		}

		if (GetData(20004, &data))
		{
			RemoveData(20004);
			SetData(kHTMLStyleID, data);
		}

		ConvertFromSuffixLists();
	}
	else
	{
		CreateAndReadData(&itsMacroList, currentVersion);
		ReadFileTypeInfo(currentVersion);
	}

	if (currentVersion < 13)
	{
		CreateCRMRuleLists();
	}
	else
	{
		CreateAndReadData(&itsCRMList, currentVersion);
	}

	if (currentVersion < 14)
	{
		itsFileTypeList->Sort();
	}

	if (currentVersion < 18)
	{
		NewSuffixInfo kEiffelSuffix[] =
	{
		{ ".e", false }
	};
		addNewSuffixes("Eiffel",  nullptr, kEiffelFT, kEiffelSuffix, sizeof(kEiffelSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kFortranSuffix[] =
	{
		{ ".f",   false },
		{ ".for", false },
		{ ".ftn", false },
		{ ".f77", false },
		{ ".f90", false },
		{ ".f95", false },
		{ ".F",   false },
		{ ".FOR", false },
		{ ".FTN", false },
		{ ".F77", false },
		{ ".F90", false },
		{ ".F95", false }
	};
		addNewSuffixes("FORTRAN", nullptr, kFortranFT, kFortranSuffix, sizeof(kFortranSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kJavaSuffix[] =
	{
		{ ".java", false },
		{ ".jws",  false }
	};
		addNewSuffixes("Java", nullptr, kJavaSourceFT, kJavaSuffix, sizeof(kJavaSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kStaticLibrarySuffix[] =
	{
		{ ".a", false }
	};
		addNewSuffixes(nullptr, nullptr, kStaticLibraryFT, kStaticLibrarySuffix, sizeof(kStaticLibrarySuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kSharedLibrarySuffix[] =
	{
		{ ".so",    false },
		{ ".dylib", false }
	};
		addNewSuffixes(nullptr, nullptr, kSharedLibraryFT, kSharedLibrarySuffix, sizeof(kSharedLibrarySuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 23)
	{
		RemoveData(47);
	}

	if (currentVersion < 24)
	{
		NewSuffixInfo kAssemblySuffix[] =
	{
		{ ".asm", false },
		{ ".s",   false },
		{ ".S",   false }
	};
		addNewSuffixes("Assembly", nullptr, kAssemblyFT, kAssemblySuffix, sizeof(kAssemblySuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kPascalSuffix[] =
	{
		{ ".p",   false },
		{ ".pas", false }
	};
		addNewSuffixes("Pascal", nullptr, kPascalFT, kPascalSuffix, sizeof(kPascalSuffix),   itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kRatforSuffix[] =
	{
		{ ".r", false }
	};
		addNewSuffixes("FORTRAN", nullptr, kRatforFT, kRatforSuffix, sizeof(kRatforSuffix),   itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 27)
	{
		RemoveData(18);
		RemoveData(20);

		if (GetData(19, &data))
		{
			RemoveData(19);
			ConvertHTMLSuffixesToFileTypes(data);
		}
		else
		{
#ifdef _J_OSX
			NewExternalSuffixInfo kImageSuffix[] =
		{
			{ ".gif", "open $f", false },
			{ ".jpg", "open $f", false }
		};
#else
			NewExternalSuffixInfo kImageSuffix[] =
		{
			{ ".gif", "eog $f", false },
			{ ".jpg", "eog $f", false }
		};
#endif
			addNewExternalSuffixes(kImageSuffix, sizeof(kImageSuffix)/sizeof(NewExternalSuffixInfo), itsFileTypeList);
		}
	}

	if (currentVersion < 28)
	{
		NewExternalSuffixInfo kFDesignSuffix[] =
	{
		{ ".fd", "jfdesign $f", false }
	};
		addNewExternalSuffixes(kFDesignSuffix, sizeof(kFDesignSuffix)/sizeof(NewExternalSuffixInfo), itsFileTypeList);
	}

	if (currentVersion < 29)
	{
		if (GetData(0, &data))
		{
			RemoveData(0);
			SetData(kCStyleID, data);
		}
	}

	if (currentVersion < 30)
	{
		NewSuffixInfo kModula2ModuleSuffix[] =
	{
		{ ".md", false }
	};
		addNewSuffixes("Modula-2", nullptr, kModula2ModuleFT, kModula2ModuleSuffix, sizeof(kModula2ModuleSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kModula2InterfaceSuffix[] =
	{
		{ ".mi", false }
	};
		addNewSuffixes("Modula-2", nullptr, kModula2InterfaceFT, kModula2InterfaceSuffix, sizeof(kModula2InterfaceSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kModula3ModuleSuffix[] =
	{
		{ ".m3", false }
	};
		addNewSuffixes("Modula-3", nullptr, kModula3ModuleFT, kModula3ModuleSuffix, sizeof(kModula3ModuleSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kModula3InterfaceSuffix[] =
	{
		{ ".i3", false }
	};
		addNewSuffixes("Modula-3", nullptr, kModula3InterfaceFT, kModula3InterfaceSuffix, sizeof(kModula3InterfaceSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 31)
	{
		NewSuffixInfo kAWKSuffix[] =
	{
		{ ".awk", false }
	};
		addNewSuffixes("AWK", "unix script", kAWKFT, kAWKSuffix, sizeof(kAWKSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kCobolSuffix[] =
	{
		{ ".cob", false },
		{ ".COB", false }
	};
		addNewSuffixes("Cobol", nullptr, kCobolFT, kCobolSuffix, sizeof(kCobolSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kLispSuffix[] =
	{
		{ ".cl",    false },
		{ ".clisp", false },
		{ ".el",    false },
		{ ".lisp",  false },
		{ ".lsp",   false }
	};
		addNewSuffixes("Lisp", nullptr, kLispFT, kLispSuffix, sizeof(kLispSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kPerlSuffix[] =
	{
		{ ".pl",   false },
		{ ".pm",   false },
		{ ".perl", false },
		{ ".plx",  false }
	};
		addNewSuffixes("Perl", "unix script", kPerlFT, kPerlSuffix, sizeof(kPerlSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kPythonSuffix[] =
	{
		{ ".py",     false },
		{ ".python", false }
	};
		addNewSuffixes("Python", "unix script", kPythonFT, kPythonSuffix, sizeof(kPythonSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kSchemeSuffix[] =
	{
		{ ".sch",    false },
		{ ".scheme", false },
		{ ".scm",    false },
		{ ".sm",     false },
		{ ".SCM",    false },
		{ ".SM",     false }
	};
		addNewSuffixes("Scheme", nullptr, kSchemeFT, kSchemeSuffix, sizeof(kSchemeSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kBourneShellSuffix[] =
	{
		{ ".sh",   false },
		{ ".SH",   false },
		{ ".bsh",  false },
		{ ".bash", false },
		{ ".ksh",  false }
	};
		addNewSuffixes("Bourne Shell", "unix script", kBourneShellFT, kBourneShellSuffix, sizeof(kBourneShellSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kTCLSuffix[] =
	{
		{ ".tcl",  false },
		{ ".tk",   false },
		{ ".wish", false }
	};
		addNewSuffixes("TCL", "unix script", kTCLFT, kTCLSuffix, sizeof(kTCLSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kVIMSuffix[] =
	{
		{ ".vim", false }
	};
		addNewSuffixes("Vim", "unix script", kVimFT, kVIMSuffix, sizeof(kVIMSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 32)
	{
		NewSuffixInfo kJavaArchiveSuffix[] =
	{
		{ ".jar", false }
	};
		addNewSuffixes(nullptr, nullptr, kJavaArchiveFT, kJavaArchiveSuffix, sizeof(kJavaArchiveSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 33)
	{
		NewSuffixInfo kPHPSuffix[] =
	{
		{ ".php",   false },
		{ ".php3",  false },
		{ ".phtml", false }
	};
		addNewSuffixes("HTML", "PHP", kPHPFT, kPHPSuffix, sizeof(kPHPSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 34)
	{
		NewSuffixInfo kASPSuffix[] =
	{
		{ ".asp", false },
		{ ".asa", false }
	};
		addNewSuffixes("ASP", nullptr, kASPFT, kASPSuffix, sizeof(kASPSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 35)
	{
		NewSuffixInfo kMakeSuffix[] =
	{
		{ ".mak",         false },
		{ "Makefile",     false },
		{ "makefile",     false },
		{ "Make.header",  false }
	};
		addNewSuffixes("Make", "unix script", kMakeFT, kMakeSuffix, sizeof(kMakeSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 36)
	{
		NewSuffixInfo kREXXSuffix[] =
	{
		{ ".cmd",  false },
		{ ".rexx", false },
		{ ".rx",   false }
	};
		addNewSuffixes("REXX", "C", kREXXFT, kREXXSuffix, sizeof(kREXXSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kRubySuffix[] =
	{
		{ ".rb", false }
	};
		addNewSuffixes("Ruby", "unix script", kRubyFT, kRubySuffix, sizeof(kRubySuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 38)
	{
		NewSuffixInfo kLexSuffix[] =
	{
		{ ".l", false }
	};
		addNewSuffixes("Lex", "C", kLexFT, kLexSuffix, sizeof(kLexSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 39)
	{
		NewSuffixInfo kCShellSuffix[] =
	{
		{ ".csh",   false },
		{ ".CSH",   false },
		{ ".tcsh",  false }
	};
		addNewSuffixes("C shell", "unix script", kCShellFT, kCShellSuffix, sizeof(kCShellSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 41)
	{
		NewSuffixInfo kBisonSuffix[] =
	{
		{ ".y", false }
	};
		addNewSuffixes("Bison", "C", kBisonFT, kBisonSuffix, sizeof(kBisonSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 44)
	{
		NewSuffixInfo kBetaSuffix[] =
	{
		{ ".bet", false }
	};
		addNewSuffixes("Beta", "Beta", kBetaFT, kBetaSuffix, sizeof(kBetaSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kLuaSuffix[] =
	{
		{ ".lua", false }
	};
		addNewSuffixes("Lua", "Lua",  kLuaFT, kLuaSuffix, sizeof(kLuaSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kSLangSuffix[] =
	{
		{ ".sl", false }
	};
		addNewSuffixes("SLang", nullptr, kSLangFT, kSLangSuffix, sizeof(kSLangSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kSQLSuffix[] =
	{
		{ ".sql",  false },
		{ ".ddl",  false }
	};
		addNewSuffixes("SQL", "SQL", kSQLFT, kSQLSuffix, sizeof(kSQLSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kVeraSourceSuffix[] =
	{
		{ ".vr",  false },
		{ ".vri", false }
	};
		addNewSuffixes("Vera", "Vera", kVeraSourceFT, kVeraSourceSuffix, sizeof(kVeraSourceSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kVeraHeaderSuffix[] =
	{
		{ ".vrh", false }
	};
		addNewSuffixes("Vera", "Vera", kVeraHeaderFT, kVeraHeaderSuffix, sizeof(kVeraHeaderSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kVerilogSuffix[] =
	{
		{ ".v", false }
	};
		addNewSuffixes("Verilog", nullptr, kVerilogFT, kVerilogSuffix, sizeof(kVerilogSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 45)
	{
		RemoveData(14);
		RemoveData(15);
	}

	if (currentVersion < 49)
	{
		JIndex macroID = FindMacroName("C#", itsMacroList, true);
		JIndex macroIndex;
		if (FindMacroID(*itsMacroList, macroID, &macroIndex))
		{
			const MacroSetInfo macroInfo = itsMacroList->GetElement(macroIndex);
			if (macroInfo.macro->GetMacroList().IsEmpty())
			{
				AddDefaultCSharpMacros(macroInfo.macro);
			}
		}

		NewSuffixInfo kCSharpSuffix[] =
	{
		{ ".cs", false }
	};
		addNewSuffixes("C#", nullptr, kCSharpFT, kCSharpSuffix, sizeof(kCSharpSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kErlangSuffix[] =
	{
		{ ".erl", false }
	};
		addNewSuffixes("Erlang", nullptr, kErlangFT, kErlangSuffix, sizeof(kErlangSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kSMLSuffix[] =
	{
		{ ".sml", false }
	};
		addNewSuffixes("SML", nullptr, kSMLFT, kSMLSuffix, sizeof(kSMLSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 50)
	{
		NewSuffixInfo kJavaScriptSuffix[] =
	{
		{ ".js",   false },
		{ ".json", false }
	};
		addNewSuffixes("JavaScript", "Java", kJavaScriptFT, kJavaScriptSuffix, sizeof(kJavaScriptSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 52)
	{
		NewSuffixInfo kAntSuffix[] =
	{
		{ "build.xml", false }
	};
		addNewSuffixes("Ant", "XML", kAntFT, kAntSuffix, sizeof(kAntSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 53)
	{
		itsFileTypeList->Sort();
	}

	if (currentVersion < 56)
	{
		NewSuffixInfo kJSPSuffix[] =
	{
		{ ".jsp",   false },
		{ ".jspf",  false }
	};
		addNewSuffixes("JSP", "Java", kJSPFT, kJSPSuffix, sizeof(kJSPSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 57)
	{
		JIndex macroID = FindMacroName("XML", itsMacroList, true);
		JIndex macroIndex;
		if (FindMacroID(*itsMacroList, macroID, &macroIndex))
		{
			const MacroSetInfo macroInfo = itsMacroList->GetElement(macroIndex);
			if (macroInfo.macro->GetMacroList().IsEmpty())
			{
				AddDefaultXMLMacros(macroInfo.macro);
			}
			if (macroInfo.action->GetActionMap().IsEmpty())
			{
				AddDefaultXMLActions(macroInfo.action);
			}
		}
	}

	if (currentVersion < 58)
	{
		NewSuffixInfo kXMLSuffix[] =
	{
		{ ".xml",  false },
		{ ".xsd",  false },
		{ ".wsdl", false },
		{ ".dtd",  false }
	};
		addNewSuffixes("XML", nullptr, kXMLFT, kXMLSuffix, sizeof(kXMLSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 59)
	{
		NewSuffixInfo kBasicSuffix[] =
	{
		{ ".bas", false },
		{ ".bi",  false },
		{ ".bb",  false },
		{ ".pb",  false }
	};
		addNewSuffixes("Basic", nullptr, kBasicFT, kBasicSuffix, sizeof(kBasicSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 62)
	{
#ifdef _J_OSX
		NewExternalSuffixInfo kImageSuffix[] =
	{
		{ ".png", "open $f", false }
	};
#else
		NewExternalSuffixInfo kImageSuffix[] =
	{
		{ ".png", "eog $f", false }
	};
#endif
		addNewExternalSuffixes(kImageSuffix, sizeof(kImageSuffix)/sizeof(NewExternalSuffixInfo), itsFileTypeList);
	}

	if (currentVersion < 63)
	{
		NewSuffixInfo kMatlabSuffix[] =
	{
		{ ".m", false }
	};
		addNewSuffixes("Matlab", "C", kMatlabFT, kMatlabSuffix, sizeof(kMatlabSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kAdobeFlexSuffix[] =
	{
		{ ".as",   false },
		{ ".mxml", false }
	};
		addNewSuffixes("Adobe Flash", "C", kAdobeFlexFT, kAdobeFlexSuffix, sizeof(kAdobeFlexSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 64)
	{
		JString name;
		JSize size;
		GetDefaultFont(&name, &size);
		if (name != "Courier")
		{
			SetDefaultFont(JFontManager::GetDefaultMonospaceFontName(),
						   JFontManager::GetDefaultMonospaceFontSize());
		}
	}

	if (currentVersion < 65)
	{
		itsFileTypeList->Sort();

		NewExternalSuffixInfo kCoreSuffix[] =
	{
		{ "core.*", "medic -c $f", false }
	};
		addNewExternalSuffixes(kCoreSuffix, sizeof(kCoreSuffix)/sizeof(NewExternalSuffixInfo), itsFileTypeList);
	}

	if (currentVersion < 66)
	{
		NewSuffixInfo kINISuffix[] =
	{
		{ ".ini", false }
	};
		addNewSuffixes("INI", nullptr, kINIFT, kINISuffix, sizeof(kINISuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 67)
	{
		NewSuffixInfo kPropertiesSuffix[] =
	{
		{ ".properties", false }
	};
		addNewSuffixes("Properties", "UNIX script", kPropertiesFT, kPropertiesSuffix, sizeof(kPropertiesSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	if (currentVersion < 68)
	{
		CreateDCRMRuleList();

		NewSuffixInfo kDSuffix[] =
	{
		{ ".d", false }
	};
		addNewSuffixes("D", nullptr, kDFT, kDSuffix, sizeof(kDSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		NewSuffixInfo kGoSuffix[] =
	{
		{ ".go", false }
	};
		addNewSuffixes("Go", nullptr, kGoFT, kGoSuffix, sizeof(kGoSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
	}

	//
	// upgrades that can be done in any order
	//

	ReadStaticGlobalPrefs(currentVersion);
	WriteStaticGlobalPrefs();

	// Copy Source Window geometry to Header Window, Other Text Window, and
	// Exec Output Window if they don't already exist.

	if (IDValid(kSourceWindSizeID))
	{
		GetData(kSourceWindSizeID, &data);
		if (!IDValid(kHeaderWindSizeID))
		{
			SetData(kHeaderWindSizeID, data);
		}
		if (!IDValid(kOtherTextWindSizeID))
		{
			SetData(kOtherTextWindSizeID, data);
		}
		if (!IDValid(kExecOutputWindSizeID))
		{
			SetData(kExecOutputWindSizeID, data);
		}
		if (!IDValid(kSearchOutputWindSizeID))
		{
			SetData(kSearchOutputWindSizeID, data);
		}
	}
}

/******************************************************************************
 ConvertFromSuffixLists (private)

	[10000,10004], [30000,30004], 39998, 39999 -> kFileTypeListID

	40000, 40004, 50000, 50004 read in and deleted.

 ******************************************************************************/

static const JUtf8Byte* kOrigMacroName[] =
{
	"C/C++", "Eiffel", "FORTRAN", "HTML", "Java"
};

static const JUnsignedOffset kOrigMacroID[] =
{
	0, 100, 100, 4, 100
};

const JSize kOrigMacroCount = sizeof(kOrigMacroName)/sizeof(JUtf8Byte*);

static const TextFileType kOrigFileType[] =
{
	kCSourceFT, kCHeaderFT, kOtherSourceFT, kDocumentationFT, kHTMLFT
};

static const JUnsignedOffset kOrigFTMacroMap[] =
{
	kFirstMacroID, kFirstMacroID, kEmptyMacroID, kEmptyMacroID, kFirstMacroID+3
};

const JSize kOrigFileTypeCount = sizeof(kOrigFileType)/sizeof(TextFileType);

void
PrefsManager::ConvertFromSuffixLists()
{
std::string data;

	assert( itsMacroList == nullptr && itsFileTypeList == nullptr );

	itsMacroList = CreateMacroList();

	for (JUnsignedOffset i=0; i<kOrigMacroCount; i++)
	{
		auto* actionMgr = jnew CharActionManager;
		assert( actionMgr != nullptr );
		if (GetData(50000 + kOrigMacroID[i], &data))
		{
			RemoveData(50000 + kOrigMacroID[i]);
			std::istringstream dataStream(data);
			actionMgr->ReadSetup(dataStream);
		}

		auto* macroMgr = jnew MacroManager;
		assert( macroMgr != nullptr );
		if (GetData(40000 + kOrigMacroID[i], &data))
		{
			RemoveData(40000 + kOrigMacroID[i]);
			std::istringstream dataStream(data);
			macroMgr->ReadSetup(dataStream);
		}
		else if (i == 0)
		{
			AddDefaultCMacros(macroMgr);
		}
		else if (i == 1)
		{
			AddDefaultEiffelMacros(macroMgr);
		}
		else if (i == 2)
		{
			AddDefaultFortranMacros(macroMgr);
		}
		else if (i == 3)
		{
			AddDefaultHTMLMacros(macroMgr);
		}
		else
		{
			assert( i == 4 );
			AddDefaultJavaMacros(macroMgr);
		}

		auto* name = jnew JString(kOrigMacroName[i]);
		assert( name != nullptr );

		itsMacroList->AppendElement(
			MacroSetInfo(kFirstMacroID + i, name, actionMgr, macroMgr));
	}

	itsFileTypeList = CreateFileTypeList();

	JPtrArray<JString> suffixList(JPtrArrayT::kForgetAll);
	for (JUnsignedOffset i=0; i<kOrigFileTypeCount; i++)
	{
		GetStringList(10000+i, &suffixList);
		RemoveData(10000+i);

		const bool ok = GetData(30000+i, &data);
		assert( ok );
		RemoveData(30000+i);
		std::istringstream dataStream(data);
		bool wordWrap;
		dataStream >> JBoolFromString(wordWrap);

		const JSize count = suffixList.GetElementCount();
		for (JIndex j=1; j<=count; j++)
		{
			auto* complSuffix = jnew JString;
			assert( complSuffix != nullptr );
			InitComplementSuffix(*(suffixList.GetElement(j)), complSuffix);

			itsFileTypeList->AppendElement(
				FileTypeInfo(suffixList.GetElement(j), nullptr, nullptr, kOrigFileType[i],
							 kOrigFTMacroMap[i], kEmptyCRMRuleListID,
							 true, nullptr, wordWrap, complSuffix, nullptr));
		}

		suffixList.RemoveAll();		// avoid DeleteAll() in GetStringList()
	}

	itsFileTypeList->Sort();

{
	const bool ok = GetData(39998, &data);
	assert( ok );
	RemoveData(39998);
	std::istringstream dataStream(data);
	dataStream >> JBoolFromString(itsExecOutputWordWrapFlag);
}

{
	const bool ok = GetData(39999, &data);
	assert( ok );
	RemoveData(39999);
	std::istringstream dataStream(data);
	dataStream >> JBoolFromString(itsUnknownTypeWordWrapFlag);
}
}

/******************************************************************************
 Default macros (private)

 ******************************************************************************/

void
PrefsManager::AddDefaultCMacros
	(
	MacroManager* mgr
	)
	const
{
	mgr->AddMacro("/*",  "  */\\l\\l\\l");
	mgr->AddMacro("#\"", "\\binclude \"\"\\l");
	mgr->AddMacro("#<",  "\\binclude <>\\l");

	mgr->AddMacro("if",   " ()\\n\\t{\\n}\\u\\u\\l");
	mgr->AddMacro("ei",   "\\blse if ()\\n\\t{\\n}\\u\\u\\r\\r\\r\\r");
	mgr->AddMacro("el",   "se\\n\\t{\\n}\\u\\n");

	mgr->AddMacro("for",    " (;;)\\n\\t{\\n}\\u\\u");
	mgr->AddMacro("while",  " ()\\n\\t{\\n}\\u\\u\\r\\r");
	mgr->AddMacro("do",     "\\n\\t{\\n}\\nwhile ();\\l\\l");
	mgr->AddMacro("switch", " ()\\n\\t{\\ndefault:\\n\\tbreak;\\n\\b}\\u\\u\\u\\u\\r\\r\\r");
	mgr->AddMacro("try",    "\\n\\t{\\n}\\n\\b"
							"catch (...)\\n\\t{\\n}\\u\\u\\u\\u\\n");
}

void
PrefsManager::AddDefaultEiffelMacros
	(
	MacroManager* mgr
	)
	const
{
}

void
PrefsManager::AddDefaultFortranMacros
	(
	MacroManager* mgr
	)
	const
{
}

void
PrefsManager::AddDefaultHTMLMacros
	(
	MacroManager* mgr
	)
	const
{
	mgr->AddMacro("<a",   " href=\"\"></a>\\l\\l\\l\\l\\l\\l");
	mgr->AddMacro("<img", " src=\"\" />\\l\\l\\l\\l");

	mgr->AddMacro("<ul",  ">\\n<li></li>\\n</ul>\\u\\l");
	mgr->AddMacro("<ol",  ">\\n<li></li>\\n</ol>\\u\\l");
	mgr->AddMacro("<dl",  ">\\n<dt></dt>\\n<dd></dd>\\n</dl>\\u\\u\\l");

	mgr->AddMacro("<!--", "  -->\\l\\l\\l\\l");

	// PHP

	mgr->AddMacro("<?",   "  ?>\\l\\l\\l");
	mgr->AddMacro("<?=",  "  ?>\\l\\l\\l");

	// JSP

	mgr->AddMacro("<%",   "  %>\\l\\l\\l");
	mgr->AddMacro("<%=",  "  %>\\l\\l\\l");
	mgr->AddMacro("<%@",  "  %>\\l\\l\\l");
	mgr->AddMacro("<%!",  "  %>\\l\\l\\l");
	mgr->AddMacro("<%--", "  --%>\\l\\l\\l\\l\\l");
}

void
PrefsManager::AddDefaultJavaMacros
	(
	MacroManager* mgr
	)
	const
{
	mgr->AddMacro("/*",   "  */\\l\\l\\l");
	mgr->AddMacro("/**",  "  */\\l\\l\\l");

	mgr->AddMacro("if",   " ()\\n\\t{\\n}\\u\\u\\l");
	mgr->AddMacro("ei",   "\\blse if ()\\n\\t{\\n}\\u\\u\\r\\r\\r\\r");
	mgr->AddMacro("el",   "se\\n\\t{\\n}\\u\\n");

	mgr->AddMacro("for",    " (;;)\\n\\t{\\n}\\u\\u");
	mgr->AddMacro("while",  " ()\\n\\t{\\n}\\u\\u\\r\\r");
	mgr->AddMacro("do",     "\\n\\t{\\n}\\nwhile ();\\l\\l");
	mgr->AddMacro("switch", " ()\\n\\t{\\ndefault:\\n\\tbreak;\\n\\b}\\u\\u\\u\\u\\r\\r\\r");
	mgr->AddMacro("try",    "\\n\\t{\\n}\\n\\b"
							"catch (Exception e)\\n\\t{\\n}\\n\\b"
							"final\\n\\t{\\n}\\u\\u\\u\\u\\u\\u\\u\\n");
}

void
PrefsManager::AddDefaultCSharpMacros
	(
	MacroManager* mgr
	)
	const
{
	mgr->AddMacro("/*",  "  */\\l\\l\\l");
	mgr->AddMacro("/**", "  */\\l\\l\\l");
	mgr->AddMacro("#\"", "\\binclude \"\"\\l");
	mgr->AddMacro("#<",  "\\binclude <>\\l");

	mgr->AddMacro("if",   " ()\\n\\t{\\n}\\u\\u\\l");
	mgr->AddMacro("ei",   "\\blse if ()\\n\\t{\\n}\\u\\u\\r\\r\\r\\r");
	mgr->AddMacro("el",   "se\\n\\t{\\n}\\u\\n");

	mgr->AddMacro("for",    " (;;)\\n\\t{\\n}\\u\\u");
	mgr->AddMacro("while",  " ()\\n\\t{\\n}\\u\\u\\r\\r");
	mgr->AddMacro("do",     "\\n\\t{\\n}\\nwhile ();\\l\\l");
	mgr->AddMacro("foreach"," ( in )\\n\\t{\\n}\\u\\u\\r\\r\\r\\r");
	mgr->AddMacro("switch", " ()\\n\\t{\\ndefault:\\n\\tbreak;\\n\\b}\\u\\u\\u\\u\\r\\r\\r");
	mgr->AddMacro("try",    "\\n\\t{\\n}\\n\\b"
							"catch (...)\\n\\t{\\n}\\u\\u\\u\\u\\n");
}

void
PrefsManager::AddDefaultXMLMacros
	(
	MacroManager* mgr
	)
	const
{
	mgr->AddMacro("<!--", "  -->\\l\\l\\l\\l");
}

void
PrefsManager::AddDefaultXMLActions
	(
	CharActionManager* mgr
	)
	const
{
	mgr->SetAction(JUtf8Character('>'),  JString("$(xml-auto-close $t)", JString::kNoCopy));
}

/******************************************************************************
 InitComplementSuffix (private)

 ******************************************************************************/

static const JUtf8Byte* kInitCSourceSuffix[] =
{
	".c", ".cc", ".cpp", ".cxx", ".C", ".tmpl"
};

const JSize kInitCSourceSuffixCount = sizeof(kInitCSourceSuffix) / sizeof(JUtf8Byte*);

static const JUtf8Byte* kInitCHeaderSuffix[] =
{
	".h", ".hh", ".hpp", ".hxx", ".H", ".h"
};

const JSize kInitCHeaderSuffixCount = sizeof(kInitCHeaderSuffix) / sizeof(JUtf8Byte*);

void
PrefsManager::InitComplementSuffix
	(
	const JString&	suffix,
	JString*		complSuffix
	)
	const
{
	assert( kInitCSourceSuffixCount == kInitCHeaderSuffixCount );

	for (JUnsignedOffset i=0; i<kInitCSourceSuffixCount; i++)
	{
		if (suffix == kInitCSourceSuffix[i])
		{
			*complSuffix = kInitCHeaderSuffix[i];
			break;
		}
		if (suffix == kInitCHeaderSuffix[i])
		{
			*complSuffix = kInitCSourceSuffix[i];
			break;
		}
	}
}

/******************************************************************************
 FindMacroName (static private)

	Returns the id of the macro set with the given name.  Can create a new
	one if the name doesn't exist.

 ******************************************************************************/

JIndex
PrefsManager::FindMacroName
	(
	const JUtf8Byte*		macroName,
	JArray<MacroSetInfo>*	macroList,
	const bool			create
	)
{
	if (JString::IsEmpty(macroName))
	{
		return kEmptyMacroID;
	}

	// search for macroName

	JIndex maxID = kEmptyMacroID;

	const JSize macroCount = macroList->GetElementCount();
	for (JIndex i=1; i<=macroCount; i++)
	{
		const MacroSetInfo macroInfo = macroList->GetElement(i);
		maxID = JMax(maxID, macroInfo.id);
		if (JString::Compare(macroName, *(macroInfo.name), JString::kIgnoreCase) == 0)
		{
			return macroInfo.id;
		}
	}

	if (!create)
	{
		return kEmptyMacroID;
	}

	// create a new macro set

	const JIndex macroID = maxID+1;

	auto* name = jnew JString(macroName);
	assert( name != nullptr );

	auto* actionMgr = jnew CharActionManager;
	assert( actionMgr != nullptr );

	auto* macroMgr = jnew MacroManager;
	assert( macroMgr != nullptr );

	macroList->InsertSorted(
		MacroSetInfo(macroID, name, actionMgr, macroMgr));

	return macroID;
}

/******************************************************************************
 FindCRMRuleListName (static private)

	Returns the id of the CRM rule list with the given name.

 ******************************************************************************/

JIndex
PrefsManager::FindCRMRuleListName
	(
	const JUtf8Byte*				crmName,
	const JArray<CRMRuleListInfo>&	crmList
	)
{
	if (JString::IsEmpty(crmName))
	{
		return kEmptyCRMRuleListID;
	}

	const JSize crmCount = crmList.GetElementCount();
	for (JIndex i=1; i<=crmCount; i++)
	{
		const CRMRuleListInfo crmInfo = crmList.GetElement(i);
		if (JString::Compare(crmName, *(crmInfo.name), JString::kIgnoreCase) == 0)
		{
			return crmInfo.id;
		}
	}

	return kEmptyCRMRuleListID;
}

/******************************************************************************
 ConvertHTMLSuffixesToFileTypes (private)

 ******************************************************************************/

void
PrefsManager::ConvertHTMLSuffixesToFileTypes
	(
	const std::string& data
	)
{
	std::istringstream dataStream(data);
	JSize count;
	dataStream >> count;
	JString suffix;
	for (JIndex i=1; i<=count; i++)
	{
		bool found = false;
		dataStream >> suffix;

		const JSize ftCount = itsFileTypeList->GetElementCount();
		for (JIndex j=1; j<=ftCount; j++)
		{
			const FileTypeInfo info = itsFileTypeList->GetElement(j);
			if (*(info.suffix) == suffix)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			#ifdef _J_OSX
			const JUtf8Byte* cmd = "open $f";
			#else
			const JUtf8Byte* cmd = "eog $f";
			#endif

			FileTypeInfo info(jnew JString(suffix), nullptr, nullptr, kExternalFT,
							  kEmptyMacroID, kEmptyCRMRuleListID,
							  true, nullptr, true, jnew JString, jnew JString(cmd));
			assert( info.suffix != nullptr && info.complSuffix != nullptr && info.editCmd != nullptr );
			itsFileTypeList->InsertSorted(info);
		}
	}
}

/******************************************************************************
 CreateFileTypeList (private)

 ******************************************************************************/

JArray<PrefsManager::FileTypeInfo>*
PrefsManager::CreateFileTypeList()
{
	auto* list = jnew JArray<FileTypeInfo>(256);
	assert( list != nullptr );
	list->SetSortOrder(JListT::kSortAscending);
	list->SetCompareFunction(CompareFileTypeSpecAndLength);
	return list;
}

/******************************************************************************
 ReadFileTypeInfo (private)

 ******************************************************************************/

void
PrefsManager::ReadFileTypeInfo
	(
	const JFileVersion vers
	)
{
	assert( itsFileTypeList == nullptr );

	itsFileTypeList = CreateFileTypeList();

	std::string listData;
	bool ok = GetData(kFileTypeListID, &listData);
	assert( ok );
	std::istringstream listStream(listData);

	JSize count;
	listStream >> count;

	for (JIndex i=1; i<=count; i++)
	{
		FileTypeInfo info;

		info.suffix = jnew JString;
		assert( info.suffix != nullptr );

		info.complSuffix = jnew JString;
		assert( info.complSuffix != nullptr );

		listStream >> *(info.suffix) >> info.type;
		listStream >> info.macroID >> JBoolFromString(info.wordWrap);

		if (vers >= 13)
		{
			listStream >> info.crmID >> *(info.complSuffix);
		}
		else
		{
			info.crmID = kEmptyCRMRuleListID;	// set by CreateCRMRuleLists()
			InitComplementSuffix(*(info.suffix), info.complSuffix);
		}

		if (vers >= 26)
		{
			bool hasEditCmd;
			listStream >> JBoolFromString(hasEditCmd);
			if (hasEditCmd)
			{
				info.editCmd = jnew JString;
				assert( info.editCmd != nullptr );
				listStream >> *(info.editCmd);
			}
		}

		if (vers >= 40)
		{
			bool hasScriptPath;
			listStream >> JBoolFromString(hasScriptPath);
			if (hasScriptPath)
			{
				info.scriptPath = jnew JString;
				assert( info.scriptPath != nullptr );
				listStream >> *(info.scriptPath);
			}
		}

		info.CreateRegex();
		itsFileTypeList->AppendElement(info);
	}

	std::string wrapData;
	ok = GetData(kMiscWordWrapID, &wrapData);
	assert( ok );
	std::istringstream wrapStream(wrapData);
	wrapStream >> JBoolFromString(itsExecOutputWordWrapFlag)
			   >> JBoolFromString(itsUnknownTypeWordWrapFlag);
}

/******************************************************************************
 WriteFileTypeInfo (private)

	Writes out the suffix info.

 ******************************************************************************/

void
PrefsManager::WriteFileTypeInfo()
{
	std::ostringstream listStream;

	const JSize count = itsFileTypeList->GetElementCount();
	listStream << count;

	for (JIndex i=1; i<=count; i++)
	{
		FileTypeInfo info = itsFileTypeList->GetElement(i);
		listStream << ' ' << *(info.suffix) << ' ' << info.type;
		listStream << ' ' << info.macroID << ' ' << JBoolToString(info.wordWrap);
		listStream << ' ' << info.crmID << ' ' << *(info.complSuffix);

		if (info.editCmd != nullptr)
		{
			listStream << ' ' << JBoolToString(true) << ' ' << *(info.editCmd);
		}
		else
		{
			listStream << ' ' << JBoolToString(false);
		}

		if (info.scriptPath != nullptr)
		{
			listStream << ' ' << JBoolToString(true) << ' ' << *(info.scriptPath);
		}
		else
		{
			listStream << ' ' << JBoolToString(false);
		}
	}

	SetData(kFileTypeListID, listStream);

	std::ostringstream wrapStream;
	wrapStream << JBoolToString(itsExecOutputWordWrapFlag)
			   << JBoolToString(itsUnknownTypeWordWrapFlag);
	SetData(kMiscWordWrapID, wrapStream);
}

/******************************************************************************
 FileTypeInfo::CreateRegex

 ******************************************************************************/

void
PrefsManager::FileTypeInfo::CreateRegex()
{
	jdelete nameRegex;
	nameRegex = nullptr;

	jdelete contentRegex;
	contentRegex = nullptr;

	if (suffix->GetFirstCharacter() == kContentRegexMarker)
	{
		contentRegex = jnew JRegex(*suffix);
		assert( contentRegex != nullptr );
		contentRegex->SetSingleLine(true);
	}
	else if (suffix->Contains(kNameRegexMarker))
	{
		JString s;
		const bool ok = JDirInfo::BuildRegexFromWildcardFilter(*suffix, &s);
		assert( ok );

		nameRegex = jnew JRegex(s);
		assert( nameRegex != nullptr );
	}
}

/******************************************************************************
 FileTypeInfo::IsPlainSuffix

 ******************************************************************************/

bool
PrefsManager::FileTypeInfo::IsPlainSuffix()
	const
{
	return nameRegex == nullptr && contentRegex == nullptr;
}

/******************************************************************************
 FileTypeInfo::Free

 ******************************************************************************/

void
PrefsManager::FileTypeInfo::Free()
{
	jdelete suffix;
	suffix = nullptr;

	jdelete nameRegex;
	nameRegex = nullptr;

	jdelete contentRegex;
	contentRegex = nullptr;

	jdelete scriptPath;
	scriptPath = nullptr;

	jdelete complSuffix;
	complSuffix = nullptr;

	jdelete editCmd;
	editCmd = nullptr;
}

/******************************************************************************
 CompareFileTypeSpec (static)

 ******************************************************************************/

JListT::CompareResult
PrefsManager::CompareFileTypeSpec
	(
	const FileTypeInfo& i1,
	const FileTypeInfo& i2
	)
{
	const JUtf8Character c1 = (i1.suffix)->GetFirstCharacter();
	const JUtf8Character c2 = (i2.suffix)->GetFirstCharacter();

	if (c1 == kContentRegexMarker && c2 != kContentRegexMarker)
	{
		return JListT::kFirstLessSecond;
	}
	else if (c1 != kContentRegexMarker && c2 == kContentRegexMarker)
	{
		return JListT::kFirstGreaterSecond;
	}
	else
	{
		return JCompareStringsCaseInsensitive(i1.suffix, i2.suffix);
	}
}

/******************************************************************************
 CompareFileTypeSpecAndLength (static private)

	We check the regexes first because we assume the content or a full name
	match is more reliable than the suffix.

 ******************************************************************************/

JListT::CompareResult
PrefsManager::CompareFileTypeSpecAndLength
	(
	const FileTypeInfo& i1,
	const FileTypeInfo& i2
	)
{
	const JUtf8Character c1 = i1.suffix->GetFirstCharacter();
	const JUtf8Character c2 = i2.suffix->GetFirstCharacter();

	if (c1 == kContentRegexMarker && c2 != kContentRegexMarker)
	{
		return JListT::kFirstLessSecond;
	}
	else if (c1 != kContentRegexMarker && c2 == kContentRegexMarker)
	{
		return JListT::kFirstGreaterSecond;
	}
	else if (i1.suffix->Contains(kNameRegexMarker) &&
			 !i2.suffix->Contains(kNameRegexMarker))
	{
		return JListT::kFirstLessSecond;
	}
	else if (!i1.suffix->Contains(kNameRegexMarker) &&
			 i2.suffix->Contains(kNameRegexMarker))
	{
		return JListT::kFirstGreaterSecond;
	}
	else
	{
		return JCompareStringsCaseInsensitive(i1.suffix, i2.suffix);
	}
}

/******************************************************************************
 Read/Write/Edit macro sets

 ******************************************************************************/

#define DataType      MacroSetInfo
#define ListVar       itsMacroList
#define CreateListFn  CreateMacroList
#define CompareNameFn CompareMacroNames
#define MainDataID    kMacroSetListID
#define EmptyDataID   kEmptyMacroID
#define FirstDataID   kFirstMacroID
#define FindIDFn      FindMacroID
#define FTStructIDVar macroID
#define EditDataFn    EditMacros
#define EditDataArg   MacroManager
#define EditDataSel   macro
#define UpdateDataFn  UpdateMacros
#define DialogClass   EditMacroDialog
#define DialogVar     itsMacroDialog
#define ExtractDataFn GetMacroList
#define CopyConstr    jnew CharActionManager(*(origInfo.action)), \
					  jnew MacroManager(*(origInfo.macro))
#define PtrCheck      newInfo.action != nullptr && \
					  newInfo.macro  != nullptr
#define Destr         jdelete info.action; \
					  jdelete info.macro
#include "PrefsManagerData.th"
#undef DataType
#undef ListVar
#undef CreateListFn
#undef CompareNameFn
#undef MainDataID
#undef EmptyDataID
#undef FirstDataID
#undef FindIDFn
#undef FTStructIDVar
#undef EditDataFn
#undef EditDataArg
#undef EditDataSel
#undef UpdateDataFn
#undef DialogClass
#undef DialogVar
#undef ExtractDataFn
#undef CopyConstr
#undef PtrCheck
#undef Destr

/******************************************************************************
 MacroSetInfo::CreateAndRead

 ******************************************************************************/

void
PrefsManager::MacroSetInfo::CreateAndRead
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	action = jnew CharActionManager;
	assert( action != nullptr );
	action->ReadSetup(input);

	macro = jnew MacroManager;
	assert( macro != nullptr );
	macro->ReadSetup(input);
}

/******************************************************************************
 MacroSetInfo::Write

 ******************************************************************************/

void
PrefsManager::MacroSetInfo::Write
	(
	std::ostream& output
	)
{
	action->WriteSetup(output);
	output << ' ';
	macro->WriteSetup(output);
}

/******************************************************************************
 MacroSetInfo::Free

 ******************************************************************************/

void
PrefsManager::MacroSetInfo::Free()
{
	jdelete name;
	jdelete action;
	jdelete macro;
}

/******************************************************************************
 CreateCRMRuleLists (private)

	Create default CRM rule lists and assign to file types.  Also create
	default content file types.

 ******************************************************************************/

// CRM rule lists

const JSize kMaxInitCRMRuleCount = 5;	// D

struct InitCRMInfo
{
	const JUtf8Byte*	name;
	JSize				count;
	const JUtf8Byte*	first   [ kMaxInitCRMRuleCount ];
	const JUtf8Byte*	rest    [ kMaxInitCRMRuleCount ];
	const JUtf8Byte*	replace [ kMaxInitCRMRuleCount ];
};

static const InitCRMInfo kInitCRM[] =
{
{ "ASP", 1,
	  { "[[:space:]]*('+[[:space:]]*)+" },
	  { "[[:space:]]*('+[[:space:]]*)+" },
	  { "$0"                            }
},

{ "Beta", 2,
	  { "([[:space:]]*)\\((\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",    },
	  { "[[:space:]]*(\\*+\\)?[[:space:]]*)+",  "[[:space:]]*(\\*+\\)?[[:space:]]*)+" },
	  { "$1 $2",                                "$0",                                 }
},

{ "C", 2,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",  },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+" },
	  { "$1 *",                               "$0",                               }
},

{ "C++", 3,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*" },
	  { "$1 *",                               "$0",                                "$0"                         }
},

{ "C#", 3,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*///?[[:space:]]*" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*///?[[:space:]]*" },
	  { "$1 *",                               "$0",                                "$0"                           }
},

{ "Eiffel", 1,
	  { "[[:space:]]*--[[:space:]]*" },
	  { "[[:space:]]*--[[:space:]]*" },
	  { "$0"                         }
},

{ "E-mail", 1,
	  { "[[:space:]]*(>+[[:space:]]*)+" },
	  { "[[:space:]]*(>+[[:space:]]*)+" },
	  { "$0"                            }
},

{ "FORTRAN", 1,
	  { "[Cc][[:space:]]*" },
	  { "[Cc][[:space:]]*" },
	  { "$0"               }
},

{ "INI", 1,
	  { "[[:space:]]*(;+[[:space:]]*)+" },
	  { "[[:space:]]*(;+[[:space:]]*)+" },
	  { "$0"                            }
},

{ "Java", 3,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*" },
	  { "$1 *",                               "$0",                                "$0"                         }
},

{ "JSP", 3,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*" },
	  { "$1 *",                               "$0",                                "$0"                         }
},

{ "Lisp", 1,
	  { "[[:space:]]*;(;+[[:space:]]*)+" },
	  { "[[:space:]]*;(;+[[:space:]]*)+" },
	  { "$0"                             }
},

{ "Lua", 1,
	  { "[[:space:]]*--[[:space:]]*" },
	  { "[[:space:]]*--[[:space:]]*" },
	  { "$0"                         }
},

{ "Outline", 2,
	  { "(\\t*)\\*(\\t*)", "([[:space:]]*)\\*([[:space:]]*)" },
	  { "[[:space:]]*",    "[[:space:]]*"                    },
	  { "$1$2",            "$1 $2"                           }
},

{ "PHP", 4,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*", "[[:space:]]*(#+[[:space:]]*)+" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*", "[[:space:]]*(#+[[:space:]]*)+" },
	  { "$1 *",                               "$0",                                "$0"                        , "$0"                            }
},

{ "SQL", 4,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*--[[:space:]]*", "[[:space:]]*(#+[[:space:]]*)+" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*--[[:space:]]*", "[[:space:]]*(#+[[:space:]]*)+" },
	  { "$1 *",                               "$0",                                "$0",                         "$0"                            }
},

{ "UNIX script", 1,
	  { "[[:space:]]*(#+[[:space:]]*)+" },
	  { "[[:space:]]*(#+[[:space:]]*)+" },
	  { "$0"                            }
},

{ "Vera", 1,
	  { "[[:space:]]*//[[:space:]]*" },
	  { "[[:space:]]*//[[:space:]]*" },
	  { "$0"                         }
},
};

void
createCRMRuleList
	(
	JArray<PrefsManager::CRMRuleListInfo>*	list,
	const InitCRMInfo&							info
	)
{
	auto* name = jnew JString(info.name);
	assert( name != nullptr );

	auto* ruleList = jnew JStyledText::CRMRuleList;
	assert( ruleList != nullptr );

	for (JUnsignedOffset j=0; j<info.count; j++)
	{
		ruleList->AppendElement(JStyledText::CRMRule(
			JString(info.first[j],   JString::kNoCopy),
			JString(info.rest[j],    JString::kNoCopy),
			JString(info.replace[j], JString::kNoCopy)));
	}

	list->AppendElement(
		PrefsManager::CRMRuleListInfo(
			kFirstCRMRuleListID + list->GetElementCount(),
			name, ruleList));
}

// regex file types

struct FTRegexInfo
{
	const JUtf8Byte*	pattern;
	TextFileType		type;
	const JUtf8Byte*	macroName;
	const JUtf8Byte*	crmName;
	bool				wrap;
};

static const FTRegexInfo kFTRegexInfo[] =
{
	// AWK
{ "^#![[:space:]]*/[^[:space:]]+/awk([[:space:]]|$)",
	  kAWKFT, "AWK", "UNIX script", false },

	// Perl
{ "^#![[:space:]]*/[^[:space:]]+/perl[-_]?([0-9.]*)([[:space:]]|$)",
	  kPerlFT, "Perl", "UNIX script", false },

	// Python
{ "^#![[:space:]]*/[^[:space:]]+/python([[:space:]]|$)",
	  kPythonFT, "Python", "UNIX script", false },

	// Ruby
{ "^#![[:space:]]*/[^[:space:]]+/ruby([[:space:]]|$)",
	  kRubyFT, "Ruby", "UNIX script", false },

	// Shell
{ "^#![[:space:]]*/[^[:space:]]+/(ba|k|z)?sh([[:space:]]|$)",
	  kBourneShellFT, "Bourne shell", "UNIX script", false },

	// C shell
{ "^#![[:space:]]*/[^[:space:]]+/t?csh([[:space:]]|$)",
	  kCShellFT, "C shell", "UNIX script", false },

	// TCL
{ "^#![[:space:]]*/[^[:space:]]+/tcl([[:space:]]|$)",
	  kTCLFT, "TCL", "UNIX script", false },

	// TCL/Tk
{ "^#![[:space:]]*/[^[:space:]]+/wish([[:space:]]|$)",
	  kTCLFT, "TCL", "UNIX script", false },

	// XPM source
{ "^/\\* XPM \\*/(\\n|$)",
	  kCHeaderFT, "C/C++", "C", false },

	// email
{ "^From[[:space:]]+.*.{3}[[:space:]]+.{3}[[:space:]]+[[:digit:]]+"
	  "[[:space:]]+([[:digit:]]+:)+[[:digit:]]+[[:space:]]+(...[[:space:]])?"
	  "[[:digit:]]{4}",
	  kUnknownFT, nullptr, "E-mail", true }
};

void
PrefsManager::CreateCRMRuleLists()
{
	assert( itsCRMList == nullptr );

	// create CRM rule lists

	itsCRMList = CreateCRMList();

	for (auto& info : kInitCRM)
	{
		createCRMRuleList(itsCRMList, info);
	}

	// set CRM rule list for each original suffix

	const JSize ftCount = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=ftCount; i++)
	{
		FileTypeInfo info = itsFileTypeList->GetElement(i);
		if (*(info.suffix) == ".c" ||
			*(info.suffix) == ".l" ||
			*(info.suffix) == ".y")
		{
			info.crmID = FindCRMRuleListName("C", *itsCRMList);
		}
		else if (info.type == kCSourceFT || info.type == kCHeaderFT)
		{
			info.crmID = FindCRMRuleListName("C++", *itsCRMList);
		}
		itsFileTypeList->SetElement(i, info);
	}

	// add content regex file types

	for (auto& ftrInfo : kFTRegexInfo)
	{
		auto* suffix = jnew JString(ftrInfo.pattern);
		assert( suffix != nullptr );

		auto* complSuffix = jnew JString;
		assert( complSuffix != nullptr );

		const JIndex macroID = FindMacroName(ftrInfo.macroName, itsMacroList, true);
		const JIndex crmID   = FindCRMRuleListName(ftrInfo.crmName, *itsCRMList);

		FileTypeInfo info(suffix, nullptr, nullptr, ftrInfo.type, macroID, crmID,
						  true, nullptr, ftrInfo.wrap, complSuffix, nullptr);
		info.CreateRegex();
		assert( info.contentRegex != nullptr );
		itsFileTypeList->InsertSorted(info);
	}
}

/******************************************************************************
 CreateDCRMRuleList (private)

 ******************************************************************************/

static const InitCRMInfo kInitDCRM =
{
	"D", 5,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*", "([[:space:]]*)/(\\++[[:space:]]*)+", "[[:space:]]*(\\++[[:space:]]*)+"   },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*", "[[:space:]]*(\\++/?[[:space:]]*)+",  "[[:space:]]*(\\++/?[[:space:]]*)+" },
	  { "$1 *",                               "$0",                                "$0"                        , "$1 +",                               "$0"                                }
};

void
PrefsManager::CreateDCRMRuleList()
{
	createCRMRuleList(itsCRMList, kInitDCRM);
}

/******************************************************************************
 Read/Write/Edit CRM rule lists

 ******************************************************************************/

#define DataType      CRMRuleListInfo
#define ListVar       itsCRMList
#define CreateListFn  CreateCRMList
#define CompareNameFn CompareCRMNames
#define MainDataID    kCRMRuleSetListID
#define EmptyDataID   kEmptyCRMRuleListID
#define FirstDataID   kFirstCRMRuleListID
#define FindIDFn      FindCRMRuleListID
#define FTStructIDVar crmID
#define EditDataFn    EditCRMRuleLists
#define EditDataArg   JStyledText::CRMRuleList
#define EditDataSel   list
#define UpdateDataFn  UpdateCRMRuleLists
#define DialogClass   EditCRMDialog
#define DialogVar     itsCRMDialog
#define ExtractDataFn GetCRMRuleLists
#define CopyConstr    jnew JStyledText::CRMRuleList(*(origInfo.list))
#define PtrCheck      newInfo.list != nullptr
#define Destr         (info.list)->DeleteAll(); \
					  jdelete info.list
#include "PrefsManagerData.th"
#undef DataType
#undef ListVar
#undef CreateListFn
#undef CompareNameFn
#undef MainDataID
#undef EmptyDataID
#undef FirstDataID
#undef FindIDFn
#undef FTStructIDVar
#undef EditDataFn
#undef EditDataArg
#undef EditDataSel
#undef UpdateDataFn
#undef DialogClass
#undef DialogVar
#undef ExtractDataFn
#undef CopyConstr
#undef PtrCheck
#undef Destr

/******************************************************************************
 CRMRuleListInfo::CreateAndRead

 ******************************************************************************/

void
PrefsManager::CRMRuleListInfo::CreateAndRead
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	list = jnew JStyledText::CRMRuleList;
	assert( list != nullptr );

	JSize ruleCount;
	input >> ruleCount;

	JString first, rest, replace;
	for (JIndex j=1; j<=ruleCount; j++)
	{
		input >> first >> rest >> replace;
		list->AppendElement(JStyledText::CRMRule(first, rest, replace));
	}
}

/******************************************************************************
 CRMRuleListInfo::Write

 ******************************************************************************/

void
PrefsManager::CRMRuleListInfo::Write
	(
	std::ostream& output
	)
{
	const JSize ruleCount = list->GetElementCount();
	output << ruleCount;

	for (const auto& r : *list)
	{
		output << ' ' << r.first->GetPattern();
		output << ' ' << r.rest->GetPattern();
		output << ' ' << *r.replace;
	}
}

/******************************************************************************
 CRMRuleListInfo::Free

 ******************************************************************************/

void
PrefsManager::CRMRuleListInfo::Free()
{
	jdelete name;

	list->DeleteAll();
	jdelete list;
}

/******************************************************************************
 Colors

 ******************************************************************************/

JColorID
PrefsManager::GetColor
	(
	const JIndex index
	)
	const
{
	assert( ColorIndexValid(index) );
	return itsColor [ index-1 ];
}

void
PrefsManager::SetColor
	(
	const JIndex		index,
	const JColorID	color
	)
{
	assert( ColorIndexValid(index) );

	if (color != itsColor [ index-1 ])
	{
		itsColor [ index-1 ] = color;

		if (index == kTextColorIndex)
		{
			Broadcast(TextColorChanged());
		}
	}
}

// private

void
PrefsManager::ReadColors()
{
	bool ok[ kColorCount ];

	std::string data;
	if (GetData(kTextColorID, &data))
	{
		std::istringstream dataStream(data);

		JFileVersion vers;
		dataStream >> vers;
		assert( vers <= kCurrentTextColorVers );

		JRGB color[ kColorCount ];
		for (JUnsignedOffset i=0; i<kColorCount; i++)
		{
			if (vers == 0 && i == kRightMarginColorIndex-1)
			{
				ok[i] = false;
			}
			else
			{
				dataStream >> color[i];
				itsColor[i] = JColorManager::GetColorID(color[i]);
				ok[i]       = true;
			}
		}
	}
	else
	{
		for (bool& v : ok)
		{
			v = false;
		}
	}

	// set unallocated colors to the default values

	const JColorID defaultColor[] =
	{
		JColorManager::GetBlackColor(),
		JColorManager::GetWhiteColor(),
		JColorManager::GetRedColor(),
		JColorManager::GetDefaultSelectionColor(),
		JColorManager::GetBlueColor(),
		JColorManager::GetGrayColor(70)
	};
	assert( sizeof(defaultColor)/sizeof(JColorID) == kColorCount );

	for (JUnsignedOffset i=0; i<kColorCount; i++)
	{
		if (!ok[i])
		{
			itsColor[i] = defaultColor[i];
		}
	}
}

void
PrefsManager::WriteColors()
{
	std::ostringstream data;
	data << kCurrentTextColorVers;

	for (JColorID color : itsColor)
	{
		data << ' ' << JColorManager::GetRGB(color);
	}

	SetData(kTextColorID, data);
}

/******************************************************************************
 Default font

 ******************************************************************************/

void
PrefsManager::GetDefaultFont
	(
	JString*	name,
	JSize*		size
	)
	const
{
	std::string data;
	const bool ok = GetData(kDefFontID, &data);
	assert( ok );

	std::istringstream dataStream(data);
	dataStream >> *name >> *size;
}

void
PrefsManager::SetDefaultFont
	(
	const JString&	name,
	const JSize		size
	)
{
	std::ostringstream data;
	data << JString(name) << ' ' << size;
	SetData(kDefFontID, data);
}

/******************************************************************************
 Emulator

 ******************************************************************************/

Emulator
PrefsManager::GetEmulator()
	const
{
	std::string data;
	if (!GetData(kEmulatorID, &data))
	{
		return kNoEmulator;
	}

	std::istringstream dataStream(data);

	Emulator type;
	dataStream >> type;
	return type;
}

void
PrefsManager::SetEmulator
	(
	const Emulator type
	)
{
	std::ostringstream data;
	data << type;
	SetData(kEmulatorID, data);
}

/******************************************************************************
 Global text settings

 ******************************************************************************/

void
PrefsManager::ReadStaticGlobalPrefs
	(
	const JFileVersion vers
	)
	const
{
	std::string data;
	if (GetData(kStaticGlobalID, &data))
	{
		std::istringstream dataStream(data);
		TextDocument::ReadStaticGlobalPrefs(dataStream, vers);
		ProjectDocument::ReadStaticGlobalPrefs(dataStream, vers);
	}
}

void
PrefsManager::WriteStaticGlobalPrefs()
{
	std::ostringstream data;
	TextDocument::WriteStaticGlobalPrefs(data);
	data << ' ';
	ProjectDocument::WriteStaticGlobalPrefs(data);

	SetData(kStaticGlobalID, data);
}

/******************************************************************************
 Program state

	RestoreProgramState() returns true if there was any state to restore.

	ForgetProgramState() is required because we have to save state before
	anything is closed.  If the close fails, we shouldn't save the state.

 ******************************************************************************/

bool
PrefsManager::RestoreProgramState()
{
	std::string data;
	if (GetData(kDocMgrStateID, &data))
	{
		const bool saveAutoDock = JXWindow::WillAutoDockNewWindows();
		JXWindow::ShouldAutoDockNewWindows(false);

		std::istringstream dataStream(data);
		const bool restored =
			GetDocumentManager()->RestoreState(dataStream);
		RemoveData(kDocMgrStateID);

		JXWindow::ShouldAutoDockNewWindows(saveAutoDock);
		return restored;
	}
	else
	{
		return false;
	}
}

void
PrefsManager::SaveProgramState()
{
	MDIServer* mdi;
	if (!GetMDIServer(&mdi))
	{
		return;
	}

	std::ostringstream data;
	if (GetDocumentManager()->SaveState(data))
	{
		SetData(kDocMgrStateID, data);
	}
	else
	{
		RemoveData(kDocMgrStateID);
	}
}

void
PrefsManager::ForgetProgramState()
{
	RemoveData(kDocMgrStateID);
}

/******************************************************************************
 GetWindowSize

 ******************************************************************************/

bool
PrefsManager::GetWindowSize
	(
	const JPrefID&	id,
	JPoint*			desktopLoc,
	JCoordinate*	width,
	JCoordinate*	height
	)
	const
{
	std::string data;
	if (GetData(id, &data))
	{
		std::istringstream dataStream(data);
		dataStream >> *desktopLoc >> *width >> *height;
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 SaveWindowSize

 ******************************************************************************/

void
PrefsManager::SaveWindowSize
	(
	const JPrefID&	id,
	JXWindow*		window
	)
{
	std::ostringstream data;
	data << window->GetDesktopLocation();
	data << ' ' << window->GetFrameWidth();
	data << ' ' << window->GetFrameHeight();

	SetData(id, data);

	// When the first editor window size is saved, set it for all of them.

	const JIndex i = id.GetID();
	if (i == kSourceWindSizeID ||
		i == kHeaderWindSizeID ||
		i == kOtherTextWindSizeID)
	{
		if (!IDValid(kSourceWindSizeID))
		{
			SetData(kSourceWindSizeID, data);
		}
		if (!IDValid(kHeaderWindSizeID))
		{
			SetData(kHeaderWindSizeID, data);
		}
		if (!IDValid(kOtherTextWindSizeID))
		{
			SetData(kOtherTextWindSizeID, data);
		}
		if (!IDValid(kExecOutputWindSizeID))
		{
			SetData(kExecOutputWindSizeID, data);
		}
		if (!IDValid(kSearchOutputWindSizeID))
		{
			SetData(kSearchOutputWindSizeID, data);
		}
	}
}

/******************************************************************************
 EditWithOtherProgram

	Returns true if the file should be opened via another program.
	This only checks the file suffix types since the file isn't open.

 ******************************************************************************/

bool
PrefsManager::EditWithOtherProgram
	(
	const JString&	fileName,
	JString*		cmd
	)
	const
{
	JIndex i;
	if (GetFileType(fileName, &i) == kExternalFT)
	{
		const FileTypeInfo info = itsFileTypeList->GetElement(i);
		assert( info.editCmd != nullptr );
		*cmd = *(info.editCmd);
		if (cmd->IsEmpty())
		{
			return false;
		}

		const JUtf8Byte* map[] =
		{
			"f", fileName.GetBytes()
		};
		JGetStringManager()->Replace(cmd, map, sizeof(map));
		return true;
	}
	else
	{
		cmd->Clear();
		return false;
	}
}

/******************************************************************************
 GetFileType

	This only checks the file suffix types since the file isn't open.

 ******************************************************************************/

TextFileType
PrefsManager::GetFileType
	(
	const JString& fileName
	)
	const
{
	JIndex i;
	return GetFileType(fileName, &i);
}

// private

TextFileType
PrefsManager::GetFileType
	(
	const JString&	fileName,
	JIndex*			index
	)
	const
{
	const JString name = CleanFileName(fileName);

	const JSize count = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		FileTypeInfo info = itsFileTypeList->GetElement(i);
		if ((info.nameRegex != nullptr && info.nameRegex->Match(name)) ||
			(info.IsPlainSuffix() && name.EndsWith(*(info.suffix))))
		{
			*index = i;
			return info.type;
		}
	}

	*index = 0;
	return kUnknownFT;
}

/******************************************************************************
 GetFileType

	Any or all of action, macro, crm can come back nullptr.

 ******************************************************************************/

TextFileType
PrefsManager::GetFileType
	(
	const TextDocument&		doc,
	CharActionManager**		actionMgr,
	MacroManager**			macroMgr,
	JStyledText::CRMRuleList**	crmRuleList,
	JString*					scriptPath,
	bool*						wordWrap
	)
	const
{
	*actionMgr   = nullptr;
	*macroMgr    = nullptr;
	*crmRuleList = nullptr;

	scriptPath->Clear();

	const TextFileType origType = doc.GetFileType();
	if (origType == kExecOutputFT || origType == kShellOutputFT)
	{
		*wordWrap = itsExecOutputWordWrapFlag;
		return origType;
	}
	else if (origType == kSearchOutputFT)
	{
		*wordWrap = true;
		return origType;
	}
	else if (origType == kManPageFT)
	{
		*wordWrap = false;
		return origType;
	}
	else if (origType == kDiffOutputFT)
	{
		*wordWrap = itsUnknownTypeWordWrapFlag;
		return origType;
	}

	JIndex index;
	if (CalcFileType(doc, &index))
	{
		const FileTypeInfo info = itsFileTypeList->GetElement(index);

		JIndex index;
		if (FindMacroID(*itsMacroList, info.macroID, &index))
		{
			const MacroSetInfo macroInfo = itsMacroList->GetElement(index);
			*actionMgr = macroInfo.action;
			*macroMgr  = macroInfo.macro;
		}

		if (FindCRMRuleListID(*itsCRMList, info.crmID, &index))
		{
			const CRMRuleListInfo crmInfo = itsCRMList->GetElement(index);
			*crmRuleList = crmInfo.list;
		}

		JString sysDir, userDir;
		if (info.scriptPath != nullptr && GetScriptPaths(&sysDir, &userDir))
		{
			*scriptPath =
				JCombinePathAndName(info.isUserScript ? userDir : sysDir,
									*(info.scriptPath));
		}

		*wordWrap = info.wordWrap;
		return info.type;
	}
	else
	{
		*wordWrap = itsUnknownTypeWordWrapFlag;
		return kUnknownFT;
	}
}

/******************************************************************************
 CalcFileType (private)

	Returns an index into itsFileTypeList.

 ******************************************************************************/

bool
PrefsManager::CalcFileType
	(
	const TextDocument&	doc,
	JIndex*					index
	)
	const
{
	const JString fileName       = CleanFileName(doc.GetFileName());
	const bool checkSuffixes = doc.ExistsOnDisk();

	const JSize count = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		FileTypeInfo info   = itsFileTypeList->GetElement(i);
		const JString& text = doc.GetTextEditor()->GetText()->GetText();
		if (info.contentRegex != nullptr)
		{
			// JRegex is so slow on a large file (even with the ^ anchor!)
			// that we must avoid running it at all costs.

			if (info.literalRange.IsNothing())
			{
				info.literalRange = GetLiteralPrefixRange(*(info.suffix));
				itsFileTypeList->SetElement(i, info);
			}

			if (JString::CompareMaxNBytes(
					text.GetBytes(), info.suffix->GetBytes()+1,
					info.literalRange.GetCount(), JString::kCompareCase) == 0 &&
				info.contentRegex->Match(text))
			{
				*index = i;
				return true;
			}
		}
		else if ((info.nameRegex != nullptr && info.nameRegex->Match(fileName)) ||
				 (info.IsPlainSuffix() &&
				  checkSuffixes && fileName.EndsWith(*(info.suffix))))
		{
			*index = i;
			return true;
		}
	}

	*index = 0;
	return false;
}

/******************************************************************************
 GetLiteralPrefixRange (static)

 ******************************************************************************/

JUtf8ByteRange
PrefsManager::GetLiteralPrefixRange
	(
	const JString& regexStr
	)
{
	assert( regexStr.GetFirstCharacter() == kContentRegexMarker );

	JStringIterator iter(regexStr, kJIteratorStartAfter, 1);
	JUtf8Character c;
	while (iter.Next(&c, kJIteratorStay) && c != '\0' && c != '\\' &&
		   !JRegex::NeedsBackslashToBeLiteral(c))
	{
		iter.SkipNext();
	}

	if (c == '?' && iter.GetPrevCharacterIndex() > 2)
	{
		iter.SkipPrev();
	}

	return JUtf8ByteRange(2, iter.GetPrevByteIndex());
}

/******************************************************************************
 GetScriptPaths (static)

 ******************************************************************************/

bool
PrefsManager::GetScriptPaths
	(
	JString* sysDir,
	JString* userDir
	)
{
	return JXGetProgramDataDirectories(kScriptDir, sysDir, userDir);
}

/******************************************************************************
 FindMacroID (static)

 ******************************************************************************/

bool
PrefsManager::FindMacroID
	(
	const JArray<MacroSetInfo>&	list,
	const JIndex				id,
	JIndex*						index
	)
{
	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const MacroSetInfo info = list.GetElement(i);
		if (info.id == id)
		{
			*index = i;
			return true;
		}
	}

	*index = 0;
	return false;
}

/******************************************************************************
 FindCRMRuleListID (static)

 ******************************************************************************/

bool
PrefsManager::FindCRMRuleListID
	(
	const JArray<CRMRuleListInfo>&	list,
	const JIndex					id,
	JIndex*							index
	)
{
	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const CRMRuleListInfo info = list.GetElement(i);
		if (info.id == id)
		{
			*index = i;
			return true;
		}
	}

	*index = 0;
	return false;
}

/******************************************************************************
 GetFileSuffixes

 ******************************************************************************/

void
PrefsManager::GetFileSuffixes
	(
	const TextFileType	type,
	JPtrArray<JString>*		list
	)
	const
{
	list->DeleteAll();

	const JSize count = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const FileTypeInfo info = itsFileTypeList->GetElement(i);

		if (info.type == type && info.IsPlainSuffix())
		{
			list->Append(*info.suffix);
		}
	}
}

/******************************************************************************
 GetAllFileSuffixes

 ******************************************************************************/

void
PrefsManager::GetAllFileSuffixes
	(
	JPtrArray<JString>* list
	)
	const
{
	list->DeleteAll();

	const JSize count = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const FileTypeInfo info = itsFileTypeList->GetElement(i);

		if (info.IsPlainSuffix())
		{
			list->Append(*info.suffix);
		}
	}
}

/******************************************************************************
 GetDefaultComplementSuffix

	If the given name ends with a suffix for the given file type, it
	is removed.  Then the default complement file's suffix is appended
	and the result is returned.  *index contains the value that must be
	passed to SetDefaultComplementSuffix().

 ******************************************************************************/

static const TextFileType kDefComplSuffixType[] =
{
	kCSourceFT,
	kCHeaderFT, 
	kModula2ModuleFT,
	kModula2InterfaceFT,
	kModula3ModuleFT,
	kModula3InterfaceFT,
	kVeraSourceFT,
	kVeraHeaderFT
};

static const JUtf8Byte* kDefComplSuffixStr[] =
{
	".h",
	".c", 
	".mi",
	".md",
	".i3",
	".m3",
	".vrh",
	".vr"
};

const JSize kDefComplSuffixCount = sizeof(kDefComplSuffixType) / sizeof(TextFileType);

JString
PrefsManager::GetDefaultComplementSuffix
	(
	const JString&			name,
	const TextFileType	type,
	JIndex*					index
	)
	const
{
	const JSize count = itsFileTypeList->GetElementCount();
	JString root, suffix;
	for (JIndex i=1; i<=count; i++)
	{
		FileTypeInfo info = itsFileTypeList->GetElement(i);

		if (info.type == type && info.IsPlainSuffix() &&
			name.EndsWith(*info.suffix) &&
			name.GetCharacterCount() > info.suffix->GetCharacterCount())
		{
			for (JUnsignedOffset j=0; j<kDefComplSuffixCount; j++)
			{
				if (info.type == kDefComplSuffixType[j] && info.complSuffix->IsEmpty())
				{
					*info.complSuffix = kDefComplSuffixStr[j];
					itsFileTypeList->SetElement(i, info);
					break;
				}
			}

			JSplitRootAndSuffix(name, &root, &suffix);
			root  += *info.complSuffix;
			*index = i;
			return root;
		}
	}

	*index = 0;
	return name;
}

/******************************************************************************
 SetDefaultComplementSuffix

 ******************************************************************************/

void
PrefsManager::SetDefaultComplementSuffix
	(
	const JIndex	index,
	const JString&	name
	)
{
	FileTypeInfo info = itsFileTypeList->GetElement(index);

	JString root;
	if (JSplitRootAndSuffix(name, &root, info.complSuffix))
	{
		info.complSuffix->Prepend(JUtf8Character('.'));
		*info.complSuffix = CleanFileName(*info.complSuffix);
	}

	itsFileTypeList->SetElement(index, info);
}

/******************************************************************************
 FileMatchesSuffix (static)

 ******************************************************************************/

bool
PrefsManager::FileMatchesSuffix
	(
	const JString&				fileName,
	const JPtrArray<JString>&	suffixList
	)
{
	const JSize count = suffixList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JString& suffix = *(suffixList.GetElement(i));
		if (fileName.EndsWith(suffix))
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 CleanFileName (private)

	Removes the path and strips trailing ~ and #

 ******************************************************************************/

JString
PrefsManager::CleanFileName
	(
	const JString& name
	)
	const
{
	JString p, n;
	if (!name.IsEmpty())	// might be untitled document
	{
		JSplitPathAndName(name, &p, &n);

		JStringIterator iter(&n, kJIteratorStartAtEnd);
		JUtf8Character c;
		while (iter.Prev(&c) && (c == '~' || c == '#'))
		{
			iter.RemoveNext();
		}
	}

	return n;
}

/******************************************************************************
 Get/SetStringList (private)

 ******************************************************************************/

void
PrefsManager::GetStringList
	(
	const JPrefID&		id,
	JPtrArray<JString>*	list
	)
	const
{
	std::string data;
	const bool ok = GetData(id, &data);
	assert( ok );

	std::istringstream dataStream(data);
	dataStream >> *list;
}

void
PrefsManager::SetStringList
	(
	const JPrefID&				id,
	const JPtrArray<JString>&	list
	)
{
	std::ostringstream data;
	data << list;
	SetData(id, data);
}

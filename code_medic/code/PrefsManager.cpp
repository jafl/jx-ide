/******************************************************************************
 PrefsManager.cpp

	BASE CLASS = public JXPrefsManager

	Copyright (C) 1998 by Glenn W. Bach.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include "PrefsManager.h"
#include "EditPrefsDialog.h"
#include "globals.h"
#include "fileVersions.h"

#include "CStyler.h"
#include "FnMenuUpdater.h"
#include "sharedUtil.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStringHistoryMenu.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/j2dplot/JX2DPlotEPSPrinter.h>
#include <jx-af/jx/JXPTPrinter.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/JXSearchTextDialog.h>
#include <jx-af/jx/JXWebBrowser.h>

#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

// from CBPrefsManager
const JFileVersion kCurrentTextColorVers = 0;

const JSize kStackLineMaxDefault = 100;

// JBroadcaster messages

const JUtf8Byte* PrefsManager::kFileTypesChanged      = "FileTypesChanged::PrefsManager";
const JUtf8Byte* PrefsManager::kCustomCommandsChanged = "CustomCommandsChanged::PrefsManager";
const JUtf8Byte* PrefsManager::kTextColorChanged      = "TextColorChanged::PrefsManager";

/******************************************************************************
 Constructor

 *****************************************************************************/

PrefsManager::PrefsManager
	(
	bool* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, true, kgCSFSetupID)
{
	*isNew = JPrefsManager::UpgradeData();

	ReadColors();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

PrefsManager::~PrefsManager()
{
	SaveAllBeforeDestruct();
}

/******************************************************************************
 SaveAllBeforeDestruct (virtual protected)

 ******************************************************************************/

void
PrefsManager::SaveAllBeforeDestruct()
{
	WriteColors();
	SaveSearchPrefs();

	SetData(kProgramVersionID, GetVersionNumberStr());

	JXPrefsManager::SaveAllBeforeDestruct();
}

/******************************************************************************
 GetMedicVersionStr

 ******************************************************************************/

JString
PrefsManager::GetMedicVersionStr()
	const
{
	std::string data;
	if (GetData(kProgramVersionID, &data))
	{
		return JString(data);
	}
	else
	{
		return JString("< 0.5.0");		// didn't exist before this version
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
 EditPrefs

 ******************************************************************************/

void
PrefsManager::EditPrefs()
{
	JString gdbCmd = GetGDBCommand();
	JString jvmCmd = GetJVMCommand();

	JString editFileCmd, editFileLineCmd;
	GetEditFileCmds(&editFileCmd, &editFileLineCmd);

	JPtrArray<JString> cSourceSuffixes(JPtrArrayT::kDeleteAll),
					   cHeaderSuffixes(JPtrArrayT::kDeleteAll),
					   javaSuffixes(JPtrArrayT::kDeleteAll),
					   phpSuffixes(JPtrArrayT::kDeleteAll),
					   fortranSuffixes(JPtrArrayT::kDeleteAll),
					   dSuffixes(JPtrArrayT::kDeleteAll),
					   goSuffixes(JPtrArrayT::kDeleteAll);
	GetSuffixes(kCSourceSuffixID, &cSourceSuffixes);
	GetSuffixes(kCHeaderSuffixID, &cHeaderSuffixes);
	GetSuffixes(kJavaSuffixID, &javaSuffixes);
	GetSuffixes(kPHPSuffixID, &phpSuffixes);
	GetSuffixes(kFortranSuffixID, &fortranSuffixes);
	GetSuffixes(kDSuffixID, &dSuffixes);
	GetSuffixes(kGoSuffixID, &goSuffixes);

	auto* dlog =
		jnew EditPrefsDialog(gdbCmd, jvmCmd, editFileCmd, editFileLineCmd,
							 cSourceSuffixes, cHeaderSuffixes,
							 javaSuffixes, phpSuffixes, fortranSuffixes,
							 dSuffixes, goSuffixes);
	if (dlog->DoDialog())
	{
		dlog->GetPrefs(&gdbCmd, &jvmCmd,
					   &editFileCmd, &editFileLineCmd,
					   &cSourceSuffixes, &cHeaderSuffixes,
					   &javaSuffixes, &phpSuffixes, &fortranSuffixes,
					   &dSuffixes, &goSuffixes);

		SetGDBCommand(gdbCmd);
		SetJVMCommand(jvmCmd);
		GetLink()->ChangeDebugger();

		SetEditFileCmds(editFileCmd, editFileLineCmd);

		SetSuffixes(kCSourceSuffixID, cSourceSuffixes);
		SetSuffixes(kCHeaderSuffixID, cHeaderSuffixes);
		SetSuffixes(kJavaSuffixID, javaSuffixes);
		SetSuffixes(kPHPSuffixID, phpSuffixes);
		SetSuffixes(kFortranSuffixID, fortranSuffixes);
		SetSuffixes(kDSuffixID, dSuffixes);
		SetSuffixes(kGoSuffixID, goSuffixes);

		Broadcast(FileTypesChanged());
	}
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
PrefsManager::UpgradeData
	(
	const bool			isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
	{
		std::ostringstream cSourceSuffixData;
		cSourceSuffixData << 7;
		cSourceSuffixData << ' ' << JString(".c")   << ' ' << JString(".cc");
		cSourceSuffixData << ' ' << JString(".cpp") << ' ' << JString(".cxx");
		cSourceSuffixData << ' ' << JString(".c++") << ' ' << JString(".C");
		cSourceSuffixData << ' ' << JString(".tmpl");
		SetData(kCSourceSuffixID, cSourceSuffixData);

		std::ostringstream cHeaderSuffixData;
		cHeaderSuffixData << 6;
		cHeaderSuffixData << ' ' << JString(".h")   << ' ' << JString(".hh");
		cHeaderSuffixData << ' ' << JString(".hpp") << ' ' << JString(".hxx");
		cHeaderSuffixData << ' ' << JString(".h++") << ' ' << JString(".H");
		SetData(kCHeaderSuffixID, cHeaderSuffixData);

		SetEditFileCmds(CODE_CRUSADER_BINARY " $f", CODE_CRUSADER_BINARY " +$l $f");

		std::ostringstream gdbCmdData;
		gdbCmdData << JString("gdb");
		SetData(kGDBCommandID, gdbCmdData);
	}
	else
	{
		JString cmd = GetGDBCommand();
		if (cmd.EndsWith(" -f"))
		{
			JStringIterator iter(&cmd, JStringIterator::kStartAtEnd);
			iter.RemovePrev(3);
			cmd.TrimWhitespace();	// invalidates iter
			SetGDBCommand(cmd);
		}
	}

	if (currentVersion < 2)
	{
		SetDefaultFont(JFontManager::GetDefaultMonospaceFontName(),
					   JFontManager::GetDefaultMonospaceFontSize());
	}
	else
	{
		JString name;
		JSize size;
		GetDefaultFont(&name, &size);
		if (name == "6x13")
		{
			SetDefaultFont(JFontManager::GetDefaultMonospaceFontName(),
						   JFontManager::GetDefaultMonospaceFontSize());
		}
	}

	if (currentVersion < 3)
	{
		RemoveData(33);
	}

	if (!isNew && currentVersion < 4)
	{
		JString editFileCmd, editFileLineCmd;
		GetEditFileCmds(&editFileCmd, &editFileLineCmd);
		JXWebBrowser::ConvertVarNames(&editFileCmd, "fl");
		JXWebBrowser::ConvertVarNames(&editFileLineCmd, "fl");
		SetEditFileCmds(editFileCmd, editFileLineCmd);
	}

	if (currentVersion < 5)
	{
		RemoveData(34);
	}

	if (currentVersion < 6)
	{
		std::ostringstream javaSuffixData;
		javaSuffixData << 1;
		javaSuffixData << ' ' << JString(".java");
		SetData(kJavaSuffixID, javaSuffixData);

		std::ostringstream fortranSuffixData;
		fortranSuffixData << 12;
		fortranSuffixData << ' ' << JString(".f")   << ' ' << JString(".F");
		fortranSuffixData << ' ' << JString(".for") << ' ' << JString(".FOR");
		fortranSuffixData << ' ' << JString(".ftn") << ' ' << JString(".FTN");
		fortranSuffixData << ' ' << JString(".f77") << ' ' << JString(".F77");
		fortranSuffixData << ' ' << JString(".f90") << ' ' << JString(".F90");
		fortranSuffixData << ' ' << JString(".f95") << ' ' << JString(".F95");
		SetData(kFortranSuffixID, fortranSuffixData);
	}

	if (currentVersion < 7)
	{
		std::string data;
		if (GetData(kFileListSetupID, &data))
		{
			std::ostringstream newData;
			newData << 0 << ' ' << data.c_str();
			SetData(kFileListSetupID, newData);
		}
	}

	if (currentVersion < 8)
	{
		std::ostringstream data;
#ifdef _J_MACOS
		data << (long) kLLDBType;
#else
		data << (long) kGDBType;
#endif
		SetData(kDebuggerTypeID, data);
	}

	if (currentVersion < 9)
	{
		RemoveData(kJVMCommandID);

		std::ostringstream jvmCmdData;
		jvmCmdData << JString("java");
		SetData(kJVMCommandID, jvmCmdData);
	}

	if (currentVersion < 10)
	{
		std::ostringstream phpSuffixData;
		phpSuffixData << 2;
		phpSuffixData << ' ' << JString(".php");
		phpSuffixData << ' ' << JString(".inc");
		SetData(kPHPSuffixID, phpSuffixData);
	}
}

/******************************************************************************
 Debugger type

 ******************************************************************************/

PrefsManager::DebuggerType
PrefsManager::GetDebuggerType()
	const
{
	std::string data;
	const bool ok = GetData(kDebuggerTypeID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	long type;
	dataStream >> type;
	return (DebuggerType) type;
}

bool
PrefsManager::SetDebuggerType
	(
	const DebuggerType type
	)
{
	if (type != GetDebuggerType())
	{
		std::ostringstream data;
		data << (long) type;

		SetData(kDebuggerTypeID, data);

		StartDebugger();
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 gdb command

 ******************************************************************************/

JString
PrefsManager::GetGDBCommand()
	const
{
	std::string data;
	const bool ok = GetData(kGDBCommandID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JString cmd;
	dataStream >> cmd;
	return cmd;
}

void
PrefsManager::SetGDBCommand
	(
	const JString& command
	)
{
	std::ostringstream data;
	data << ' ' << command;

	SetData(kGDBCommandID, data);
}

/******************************************************************************
 JVM command

 ******************************************************************************/

JString
PrefsManager::GetJVMCommand()
	const
{
	std::string data;
	const bool ok = GetData(kJVMCommandID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JString cmd;
	dataStream >> cmd;
	return cmd;
}

void
PrefsManager::SetJVMCommand
	(
	const JString& command
	)
{
	std::ostringstream data;
	data << ' ' << command;

	SetData(kJVMCommandID, data);
}

/******************************************************************************
 Stack line max

 ******************************************************************************/

JSize
PrefsManager::GetStackLineMax()
	const
{
	std::string data;
	if (GetData(kStackLineMaxID, &data))
	{
		std::istringstream dataStream(data);

		JSize max;
		dataStream >> max;
		return max;
	}
	else
	{
		return kStackLineMaxDefault;
	}
}

void
PrefsManager::SetStackLineMax
	(
	const JSize max
	)
{
	std::ostringstream data;
	data << max;

	SetData(kStackLineMaxID, data);
}

/******************************************************************************
 GetFileType

 ******************************************************************************/

struct SuffixTypeMap
{
	long			id;
	TextFileType	type;
};

static const SuffixTypeMap kSuffixTypeMap[] =
{
{ kCSourceSuffixID, kCSourceFT    },
{ kCHeaderSuffixID, kCHeaderFT    },
{ kJavaSuffixID,    kJavaSourceFT },
{ kFortranSuffixID, kFortranFT    },
{ kPHPSuffixID,     kPHPFT        }
};

TextFileType
PrefsManager::GetFileType
	(
	const JString& fileName
	)
	const
{
	JPtrArray<JString> suffixList(JPtrArrayT::kDeleteAll);
	for (const auto& suffix : kSuffixTypeMap)
	{
		if (GetSuffixes(suffix.id, &suffixList))
		{
			const JSize count = suffixList.GetItemCount();
			for (JIndex j=1; j<=count; j++)
			{
				if (fileName.EndsWith(*suffixList.GetItem(j)))
				{
					return suffix.type;
				}
			}
		}
	}

	return kUnknownFT;
}

/******************************************************************************
 File suffixes (private)

 ******************************************************************************/

bool
PrefsManager::GetSuffixes
	(
	const JPrefID&		id,
	JPtrArray<JString>*	suffixList
	)
	const
{
	suffixList->DeleteAll();

	if (IDValid(id))
	{
		std::string data;
		GetData(id, &data);
		std::istringstream dataStream(data);
		dataStream >> *suffixList;
	}

	return !suffixList->IsEmpty();
}

void
PrefsManager::SetSuffixes
	(
	const JPrefID&				id,
	const JPtrArray<JString>&	suffixList
	)
{
	std::ostringstream data;
	data << suffixList;
	SetData(id, data);
}

/******************************************************************************
 Edit file cmds

 ******************************************************************************/

void
PrefsManager::GetEditFileCmds
	(
	JString*	editFileCmd,
	JString*	editFileLineCmd
	)
	const
{
	std::string data;
	const bool ok = GetData(kEditFileCmdID, &data);
	assert( ok );

	std::istringstream dataStream(data);
	dataStream >> *editFileCmd >> *editFileLineCmd;
}

void
PrefsManager::SetEditFileCmds
	(
	const JString& editFileCmd,
	const JString& editFileLineCmd
	)
{
	std::ostringstream data;
	data << editFileCmd << ' ' << editFileLineCmd;

	SetData(kEditFileCmdID, data);
}

/******************************************************************************
 Custom command list

 ******************************************************************************/

void
PrefsManager::GetCmdList
	(
	JPtrArray<JString>*	cmdList
	)
	const
{
	std::string data;
	if (GetData(kCmdListID, &data))
	{
		std::istringstream dataStream(data);
		dataStream >> *cmdList;
	}
}

void
PrefsManager::SetCmdList
	(
	const JPtrArray<JString>& cmdList
	)
{
	std::ostringstream data;
	data << cmdList;

	SetData(kCmdListID, data);
	Broadcast(CustomCommandsChanged());
}

/******************************************************************************
 Printer setup

 ******************************************************************************/

void
PrefsManager::ReadPrinterSetup
	(
	JXPSPrinter* printer
	)
{
	std::string data;
	if (GetData(kPSPrinterSetupID, &data))
	{
		std::istringstream dataStream(data);
		printer->ReadXPSSetup(dataStream);
	}
}

void
PrefsManager::WritePrinterSetup
	(
	JXPSPrinter* printer
	)
{
	std::ostringstream data;
	printer->WriteXPSSetup(data);
	SetData(kPSPrinterSetupID, data);
}

void
PrefsManager::ReadPrinterSetup
	(
	JX2DPlotEPSPrinter* printer
	)
{
	std::string data;
	if (GetData(kPlotEPSPrinterSetupID, &data))
	{
		std::istringstream dataStream(data);
		printer->ReadX2DEPSSetup(dataStream);
	}
}

void
PrefsManager::WritePrinterSetup
	(
	JX2DPlotEPSPrinter* printer
	)
{
	std::ostringstream data;
	printer->WriteX2DEPSSetup(data);
	SetData(kPlotEPSPrinterSetupID, data);
}

void
PrefsManager::ReadPrinterSetup
	(
	JXPTPrinter* printer
	)
{
	std::string data;
	if (GetData(kPTPrinterSetupID, &data))
	{
		std::istringstream dataStream(data);
		printer->ReadXPTSetup(dataStream);
	}
}

void
PrefsManager::WritePrinterSetup
	(
	JXPTPrinter* printer
	)
{
	std::ostringstream data;
	printer->WriteXPTSetup(data);
	SetData(kPTPrinterSetupID, data);
}

/******************************************************************************
 History menu

 ******************************************************************************/

void
PrefsManager::ReadHistoryMenuSetup
	(
	JXStringHistoryMenu* menu
	)
{
	std::string data;
	if (GetData(kHistoryMenuID, &data))
	{
		std::istringstream dataStream(data);
		menu->ReadSetup(dataStream);
	}
}

void
PrefsManager::WriteHistoryMenuSetup
	(
	JXStringHistoryMenu* menu
	)
{
	std::ostringstream data;
	menu->WriteSetup(data);
	SetData(kHistoryMenuID, data);
}

/******************************************************************************
 Window size

 ******************************************************************************/

void
PrefsManager::GetWindowSize
	(
	const JPrefID	id,
	JXWindow*		window,
	const bool	skipDocking
	)
	const
{
	std::string data;
	if (GetData(id, &data))
	{
		std::istringstream dataStream(data);
		window->ReadGeometry(dataStream, skipDocking);
		window->Deiconify();	// never iconic since never initially visible
	}
}

void
PrefsManager::SaveWindowSize
	(
	const JPrefID	id,
	JXWindow*		window
	)
{
	std::ostringstream data;
	window->WriteGeometry(data);
	SetData(id, data);
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
	data << name << ' ' << size;
	SetData(kDefFontID, data);
}

/******************************************************************************
 Tab char count

 ******************************************************************************/

JSize
PrefsManager::GetTabCharCount()
	const
{
	std::string data;
	if (GetData(kTabCharCountID, &data))
	{
		JSize count;
		std::istringstream dataStream(data);
		dataStream >> count;
		return count;
	}
	else
	{
		return 4;
	}
}

void
PrefsManager::SetTabCharCount
	(
	const JSize	count
	)
{
	std::ostringstream data;
	data << count;
	SetData(kTabCharCountID, data);
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

// private

void
PrefsManager::ReadColors()
{
	JRGB color[ kColorCount ];

	std::string data;
	const bool hasColors = GetData(kTextColorID, &data);
	if (hasColors)
	{
		std::istringstream dataStream(data);

		JFileVersion vers;
		dataStream >> vers;
		assert( vers <= kCurrentTextColorVers );

		for (auto& c : color)
		{
			dataStream >> c;
		}
	}

	SetColorList(hasColors, color);
}

void
PrefsManager::SetColorList
	(
	const bool	hasColors,
	JRGB			colorList[]
	)
{
	bool ok[ kColorCount ];
	if (hasColors)
	{
		for (JUnsignedOffset i=0; i<kColorCount; i++)
		{
			itsColor[i] = JColorManager::GetColorID(colorList[i]);
			ok[i]       = true;
		}
	}
	else
	{
		for (auto& b : ok)
		{
			b = false;
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

	for (auto id : itsColor)
	{
		data << ' ' << JColorManager::GetRGB(id);
	}

	SetData(kTextColorID, data);
}

/******************************************************************************
 Search dialog

 ******************************************************************************/

void
PrefsManager::LoadSearchPrefs()
{
	std::string data;
	if (GetData(kSearchTextDialogPrefsID, &data))
	{
		std::istringstream dataStream(data);
		JXGetSearchTextDialog()->ReadSetup(dataStream);
	}
}

void
PrefsManager::SaveSearchPrefs()
{
	std::ostringstream data;
	JXGetSearchTextDialog()->WriteSetup(data);

	SetData(kSearchTextDialogPrefsID, data);
}

/******************************************************************************
 SyncWithCodeCrusader

 ******************************************************************************/

void
PrefsManager::SyncWithCodeCrusader()
{
	JString fontName;
	JSize size, tabCharCount;
	bool sort, includeNS, pack, openOnTop;
	JRGB colorList[kColorCount];
	JPtrArray<JString> cSourceSuffixes(JPtrArrayT::kDeleteAll),
					   cHeaderSuffixes(JPtrArrayT::kDeleteAll),
					   fortranSuffixList(JPtrArrayT::kDeleteAll),
					   javaSuffixList(JPtrArrayT::kDeleteAll),
					   phpSuffixList(JPtrArrayT::kDeleteAll),
					   dSuffixList(JPtrArrayT::kDeleteAll),
					   goSuffixList(JPtrArrayT::kDeleteAll);
	if (ReadSharedPrefs(&fontName, &size, &tabCharCount, &sort, &includeNS, &pack,
						&openOnTop, kColorCount, colorList,
						&cSourceSuffixes, &cHeaderSuffixes,
						&fortranSuffixList, &javaSuffixList,
						&phpSuffixList, &dSuffixList, &goSuffixList))
	{
		SetDefaultFont(fontName, size);
		SetTabCharCount(tabCharCount);
		SetColorList(true, colorList);
		SetSuffixes(kCSourceSuffixID, cSourceSuffixes);
		SetSuffixes(kCHeaderSuffixID, cHeaderSuffixes);

		if (!fortranSuffixList.IsEmpty())
		{
			SetSuffixes(kFortranSuffixID, fortranSuffixList);
		}
		if (!javaSuffixList.IsEmpty())
		{
			SetSuffixes(kJavaSuffixID, javaSuffixList);
		}
		if (!phpSuffixList.IsEmpty())
		{
			SetSuffixes(kPHPSuffixID, phpSuffixList);
		}
		if (!dSuffixList.IsEmpty())
		{
			SetSuffixes(kDSuffixID, dSuffixList);
		}
		if (!goSuffixList.IsEmpty())
		{
			SetSuffixes(kGoSuffixID, goSuffixList);
		}

		FnMenuUpdater* updater = GetFnMenuUpdater();
		updater->ShouldSortFnNames(sort);
		updater->ShouldIncludeNamespace(includeNS);
		updater->ShouldPackFnNames(pack);

		JXGetSearchTextDialog()->SetFont(JFontManager::GetFont(fontName, size));
	}
}

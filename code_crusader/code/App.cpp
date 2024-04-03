/******************************************************************************
 App.cpp

	BASE CLASS = JXApplication, JPrefObject

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#include "App.h"
#include "AboutDialog.h"
#include "ProjectDocument.h"
#include "EditMiscPrefsDialog.h"
#include "ViewManPageDialog.h"
#include "globals.h"
#include "stringData.h"
#include "sharedUtil.h"
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXDockManager.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXTipOfTheDayDialog.h>
#include <jx-af/jcore/JLatentPG.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jWebUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kDefaultSysIncludeDir[] =
{
#ifdef _J_UNIX
	"/usr/include/",
	"/usr/local/include/",
#endif
};

static const JUtf8Byte* kExtraSysIncludeDir[] =
{
#ifdef _J_MACOS
	"/usr/X11/include/",
	HOMEBREW_INCLUDE_ROOT,
#endif
};

// Application signature (MDI, prefs)

static const JUtf8Byte* kAppSignature = "jxcb";

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

App::App
	(
	int*		argc,
	char*		argv[],
	const bool	useMDI,
	bool*		displayAbout,
	JString*	prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, "Code_Crusader", kDefaultStringData),
	JPrefObject(nullptr, kAppID)
{
	itsWarnBeforeQuitFlag = false;

	itsSystemIncludeDirs = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsSystemIncludeDirs != nullptr );

	*displayAbout = CreateGlobals(this, useMDI) && useMDI;	// ensure CreateGlobals() is always called
	SetPrefInfo(GetPrefsManager(), kAppID);
	JPrefObject::ReadPrefs();

	if (!*displayAbout)
	{
		*prevVersStr = GetPrefsManager()->GetJCCVersionStr();
		if (*prevVersStr == GetVersionNumberStr())
		{
			prevVersStr->Clear();
		}
		else
		{
			*displayAbout = true;
		}
	}
	else
	{
		prevVersStr->Clear();
	}

	GetSystemIncludeDirectories();

	// Write shared prefs, if they don't exist.
	// (must be done after everything created)

	WriteSharedPrefs(false);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

App::~App()
{
	jdelete itsSystemIncludeDirs;

	if (HasPrefsManager())
	{
		JPrefObject::WritePrefs();
	}
	DeleteGlobals();
}

/******************************************************************************
 Close (virtual protected)

 ******************************************************************************/

bool
App::Close()
{
	if (GetViewManPageDialog()->IsActive())
	{
		// --man with no args must leave window open
		return false;
	}

	return JXApplication::Close();
}

/******************************************************************************
 Quit (virtual)

 ******************************************************************************/

void
App::Quit()
{
	if (!HasPrefsManager() ||
		!itsWarnBeforeQuitFlag ||
		JGetUserNotification()->AskUserNo(JGetString("AskQuit::App")))
	{
		if (!IsQuitting() && HasPrefsManager())
		{
			GetPrefsManager()->SaveProgramState();
		}

		// close these first so they remember all open text documents

		if (GetDocumentManager()->CloseProjectDocuments())
		{
			JXApplication::Quit();
		}
	}
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be nullptr.

 ******************************************************************************/

void
App::DisplayAbout
	(
	const bool		showLicense,
	const JString&	prevVersStr
	)
{
	StartFiber([showLicense, prevVersStr]()
	{
		if (!showLicense || JGetUserNotification()->AcceptLicense())
		{
			auto* dlog = jnew AboutDialog(prevVersStr);
			dlog->DoDialog();

			if (showLicense && prevVersStr.IsEmpty() &&
				JGetUserNotification()->AskUserYes(
					JGetString("StartupTips::JXAskInitDockAll")))
			{
				JXGetDockManager()->DockAll();
			}

			JCheckForNewerVersion(GetPrefsManager(), kVersionCheckID);
		}
		else
		{
			ForgetPrefsManager();
			JXGetApplication()->Quit();
		}
	});
}

/******************************************************************************
 EditMiscPrefs

 ******************************************************************************/

void
App::EditMiscPrefs()
{
	auto* dlog = jnew EditMiscPrefsDialog();
	if (dlog->DoDialog())
	{
		dlog->UpdateSettings();
	}
}

/******************************************************************************
 CreateHelpMenu

 ******************************************************************************/

#include "App-Help.h"

JXTextMenu*
App::CreateHelpMenu
	(
	JXMenuBar*			menuBar,
	const JUtf8Byte*	sectionName
	)
{
	JXTextMenu* menu = menuBar->AppendTextMenu(JGetString("MenuTitle::App_Help"));
	menu->SetMenuItems(kHelpMenuStr);
	menu->SetUpdateAction(JXMenu::kDisableNone);
	ConfigureHelpMenu(menu);

	ListenTo(menu, std::function([this, sectionName](const JXMenu::ItemSelected& msg)
	{
		HandleHelpMenu(sectionName, msg.GetIndex());
	}));

	return menu;
}

/******************************************************************************
 AppendHelpMenuToToolBar

 ******************************************************************************/

void
App::AppendHelpMenuToToolBar
	(
	JXToolBar*	toolBar,
	JXTextMenu* menu
	)
{
	toolBar->NewGroup();
	toolBar->AppendButton(menu, kHelpTOCCmd);
	toolBar->AppendButton(menu, kHelpWindowCmd);
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
App::HandleHelpMenu
	(
	const JUtf8Byte*	windowSectionName,
	const JIndex		index
	)
{
	if (index == kHelpAboutCmd)
	{
		DisplayAbout();
	}

	else if (index == kHelpTOCCmd)
	{
		JXGetHelpManager()->ShowTOC();
	}
	else if (index == kHelpOverviewCmd)
	{
		JXGetHelpManager()->ShowSection("OverviewHelp");
	}
	else if (index == kHelpTutorialCmd)
	{
		JXGetHelpManager()->ShowSection("ProjectTutorialHelp");
	}
	else if (index == kHelpWindowCmd)
	{
		JXGetHelpManager()->ShowSection(windowSectionName);
	}

	else if (index == kTipCmd)
	{
		auto* dlog = jnew JXTipOfTheDayDialog;
		dlog->DoDialog();
	}

	else if (index == kHelpChangeLogCmd)
	{
		JXGetHelpManager()->ShowChangeLog();
	}
	else if (index == kHelpCreditsCmd)
	{
		JXGetHelpManager()->ShowCredits();
	}
}

/******************************************************************************
 FindFile

	Search for the specified file.  Notifies the user if it can't find it.

 ******************************************************************************/

bool
App::FindFile
	(
	const JString&		fileName,
	const JString::Case	caseSensitive,
	JString*			fullName
	)
	const
{
	bool cancelled = false;

	const bool relative = JIsRelativePath(fileName);
	if (!relative && JFileExists(fileName))
	{
		*fullName = fileName;
		return true;
	}
	else if (relative)
	{
		DirInfoList searchPaths;
		CollectSearchPaths(&searchPaths);
		const JSize dirCount = searchPaths.GetItemCount(),
					sysCount = itsSystemIncludeDirs->GetItemCount();

		bool found = false;

		JLatentPG pg;

		const JUtf8Byte* map[] =
		{
			"name", fileName.GetBytes()
		};
		const JString msg = JGetString("FileSearch::App", map, sizeof(map));
		pg.FixedLengthProcessBeginning(dirCount+sysCount, msg, true, true);

		JString path, newName;
		for (JIndex i=1; i<=dirCount; i++)
		{
			const DirInfo info = searchPaths.GetItem(i);
			if (!info.recurse)
			{
				*fullName = JCombinePathAndName(*(info.path), fileName);
				if (JFileExists(*fullName))
				{
					found = true;
					break;
				}
			}
			else if (JSearchSubdirs(*(info.path), fileName, true, caseSensitive,
									&path, &newName, nullptr, &cancelled))
			{
				*fullName = JCombinePathAndName(path, newName);
				found     = true;
				break;
			}
			else if (cancelled)
			{
				break;
			}

			if (!pg.IncrementProgress())
			{
				cancelled = true;
				break;
			}
		}

		if (!found && !cancelled)
		{
			// We have to search system paths last because these are always
			// last on the compiler search path.

			for (JIndex i=1; i<=sysCount; i++)
			{
				if (JSearchSubdirs(*itsSystemIncludeDirs->GetItem(i), fileName, true, caseSensitive,
								   &path, &newName, nullptr, &cancelled))
				{
					*fullName = JCombinePathAndName(path, newName);
					found     = true;
					break;
				}
				else if (cancelled)
				{
					break;
				}

				if (!pg.IncrementProgress())
				{
					cancelled = true;
					break;
				}
			}
		}

		pg.ProcessFinished();
		searchPaths.DeleteAll();
		if (found)
		{
			return true;
		}
	}

	if (!cancelled)
	{
		const JUtf8Byte* map[] =
		{
			"name", fileName.GetBytes()
		};
		const JString msg = JGetString("FileNotFound::App", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
	}

	fullName->Clear();
	return false;
}

/******************************************************************************
 GetSystemIncludeDirectories (private)

 ******************************************************************************/

void
App::GetSystemIncludeDirectories()
{
	for (const auto* s : kExtraSysIncludeDir)
	{
		if (JDirectoryExists(JString(s, JString::kNoCopy)))
		{
			auto* p = jnew JString(s);
			itsSystemIncludeDirs->Append(p);
		}
	}

	int pid, fd, inFD;
	const JError err = JExecute("gcc -Wp,-v -x c++ -fsyntax-only -", &pid,
								kJCreatePipe, &inFD,
								kJCreatePipe, &fd,
								kJAttachToFromFD);
	if (!err.OK())
	{
		for (const auto* s : kDefaultSysIncludeDir)
		{
			if (JDirectoryExists(JString(s, JString::kNoCopy)))
			{
				auto* p = jnew JString(s);
				itsSystemIncludeDirs->Append(p);
			}
		}
		return;
	}

	close(inFD);	// must pass open fd to process and then explicitly close, to avoid a long wait

	JString s;
	while (true)
	{
		s = JReadUntil(fd, '\n');
		if (s.IsEmpty())
		{
			break;
		}

		if (s.GetFirstCharacter() == ' ')
		{
			s.TrimWhitespace();
			if (!s.Contains(" "))
			{
				itsSystemIncludeDirs->Append(s);
			}
		}
	}
}

/******************************************************************************
 CollectSearchPaths (private)

	It's a hopeless mess to listen to all Trees and keep the list of paths
	up to date, so we build it every time it is needed.

 ******************************************************************************/

void
App::CollectSearchPaths
	(
	DirInfoList* searchPaths
	)
	const
{
	searchPaths->DeleteAll();
	searchPaths->SetCompareFunction(DirInfo::ComparePathNames);

	JPtrArray<ProjectDocument>* docList =
		GetDocumentManager()->GetProjectDocList();

	const JSize docCount = docList->GetItemCount();
	JString truePath;
	bool recurse;
	for (JIndex j=1; j<=docCount; j++)
	{
		ProjectDocument* doc   = docList->GetItem(j);
		const DirList& dirList = doc->GetDirectories();
		const JSize count      = dirList.GetItemCount();
		for (JIndex i=1; i<=count; i++)
		{
			if (dirList.GetTruePath(i, &truePath, &recurse))
			{
				DirInfo newInfo(jnew JString(truePath), recurse);
				assert( newInfo.path != nullptr );
				newInfo.projIndex = j;

				bool found;
				const JIndex index =
					searchPaths->SearchSortedOTI(newInfo, JListT::kAnyMatch, &found);
				if (found)
				{
					// compute OR of recurse flags

					DirInfo existingInfo = searchPaths->GetItem(index);
					if (newInfo.recurse && !existingInfo.recurse)
					{
						existingInfo.recurse = true;
						searchPaths->SetItem(index, existingInfo);
					}
					jdelete newInfo.path;
				}
				else
				{
					searchPaths->InsertItemAtIndex(index, newInfo);
				}
			}
		}
	}

	searchPaths->SetCompareFunction(DirInfo::CompareProjIndex);
	searchPaths->Sort();
}

/******************************************************************************
 FindAndViewFile

	If we can find the specified file, tell text editor to display it.
	If lineRange is not empty, displays the specified lines of the file.

 ******************************************************************************/

bool
App::FindAndViewFile
	(
	const JString&		fileName,
	const JIndexRange	lineRange,		// not reference because of default value
	const JString::Case	caseSensitive
	)
	const
{
	JString fullName;
	if (FindFile(fileName, caseSensitive, &fullName))
	{
		GetDocumentManager()->OpenSomething(fullName, lineRange);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
App::ReadPrefs
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

	input >> JBoolFromString(itsWarnBeforeQuitFlag);
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
App::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ' << JBoolToString(itsWarnBeforeQuitFlag);
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
App::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);

	if (reason != JXDocumentManager::kAssertFired)
	{
		JPrefObject::WritePrefs();
		GetPrefsManager()->SaveProgramState();
//		GetDocumentManager()->Save
	}

	::CleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 GetAppSignature (static)

	Required for call to WillBeMDIServer() *before* app is constructed.

 ******************************************************************************/

const JUtf8Byte*
App::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
App::InitStrings()
{
	JGetStringManager()->Register(kAppSignature, kDefaultStringData);
}

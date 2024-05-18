/******************************************************************************
 SaveNewProjectDialog.cpp

	BASE CLASS = JXSaveFileDialog, JPrefObject

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "SaveNewProjectDialog.h"
#include "ProjectDocument.h"
#include "BuildManager.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXSaveFileInput.h>
#include <jx-af/jx/JXPathInput.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXPathHistoryMenu.h>
#include <jx-af/jx/JXCurrentPathMenu.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXNewDirButton.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version  1 stores itsProjectTemplate

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

SaveNewProjectDialog*
SaveNewProjectDialog::Create
	(
	const JString&	prompt,
	const JString&	startName,
	const JString&	fileFilter,
	const JString&	message
	)
{
	auto* dlog = jnew SaveNewProjectDialog(fileFilter);
	dlog->BuildWindow(startName, prompt, message);
	dlog->BuildTemplateMenu();
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

SaveNewProjectDialog::SaveNewProjectDialog
	(
	const JString& fileFilter
	)
	:
	JXSaveFileDialog(fileFilter),
	JPrefObject(GetPrefsManager(), kNewProjectCSFID),
	itsMakefileMethod(BuildManager::kMakemake)
{
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SaveNewProjectDialog::~SaveNewProjectDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 GetMakefileMethod

 ******************************************************************************/

BuildManager::MakefileMethod
SaveNewProjectDialog::GetMakefileMethod()
	const
{
	return (BuildManager::MakefileMethod) itsMethodRG->GetSelectedItem();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "SaveNewProjectDialog-Template.h"

void
SaveNewProjectDialog::BuildWindow
	(
	const JString&	startName,
	const JString&	prompt,
	const JString&	message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,500, JString::empty);

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::SaveNewProjectDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	pathLabel->SetToLabel(false);

	auto* pathHistory =
		jnew JXPathHistoryMenu(1, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::SaveNewProjectDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	filterLabel->SetToLabel(false);

	auto* filterHistory =
		jnew JXStringHistoryMenu(1, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	auto* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::SaveNewProjectDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != nullptr );

	auto* currPathMenu =
		jnew JXCurrentPathMenu("/", window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,140);
	assert( scrollbarSet != nullptr );

	auto* upButton =
		jnew JXTextButton(JGetString("upButton::SaveNewProjectDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	auto* homeButton =
		jnew JXTextButton(JGetString("homeButton::SaveNewProjectDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	auto* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::SaveNewProjectDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != nullptr );

	auto* newDirButton =
		jnew JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( newDirButton != nullptr );

	auto* saveButton =
		jnew JXTextButton(JGetString("saveButton::SaveNewProjectDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	saveButton->SetShortcuts(JGetString("saveButton::shortcuts::SaveNewProjectDialog::JXLayout"));

	auto* promptLabel =
		jnew JXStaticText(JString::empty, window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,290, 180,20);
	promptLabel->SetToLabel(false);

	auto* fileNameInput =
		jnew JXSaveFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,310, 180,20);
	assert( fileNameInput != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::SaveNewProjectDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	itsTemplateMenu =
		jnew JXTextMenu(JGetString("itsTemplateMenu::SaveNewProjectDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,350, 270,30);

	itsMethodRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 160,390, 132,106);

	auto* manualRB =
		jnew JXTextRadioButton(BuildManager::kManual, JGetString("manualRB::SaveNewProjectDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 110,20);
	assert( manualRB != nullptr );

	auto* makefileTitle =
		jnew JXStaticText(JGetString("makefileTitle::SaveNewProjectDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,420, 140,20);
	makefileTitle->SetToLabel(false);

	auto* makemakeRB =
		jnew JXTextRadioButton(BuildManager::kMakemake, JGetString("makemakeRB::SaveNewProjectDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 110,20);
	assert( makemakeRB != nullptr );

	auto* cmakeRB =
		jnew JXTextRadioButton(BuildManager::kCMake, JGetString("cmakeRB::SaveNewProjectDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 110,20);
	assert( cmakeRB != nullptr );

	auto* qmakeRB =
		jnew JXTextRadioButton(BuildManager::kQMake, JGetString("qmakeRB::SaveNewProjectDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 110,20);
	assert( qmakeRB != nullptr );

	auto* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	pathInput->SetIsRequired(true);
	pathInput->ShouldAllowInvalidPath(false);
	pathInput->ShouldRequireWritable(false);

	auto* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != nullptr );

// end JXLayout

	SetObjects(scrollbarSet, promptLabel, prompt, fileNameInput,
			   pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   saveButton, cancelButton, upButton, homeButton,
			   desktopButton, newDirButton,
			   showHiddenCB, currPathMenu, startName, message);

	itsMethodRG->SelectItem(itsMakefileMethod);
}

/******************************************************************************
 BuildTemplateMenu (private)

 ******************************************************************************/

void
SaveNewProjectDialog::BuildTemplateMenu()
{
	itsTemplateIndex = kNoTemplateCmd;

	itsTemplateMenu->SetMenuItems(kTemplateMenuStr);
	itsTemplateMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsTemplateMenu);
	ConfigureTemplateMenu(itsTemplateMenu);

	JPtrArray<JString> menuText(JPtrArrayT::kDeleteAll);
	menuText.SetCompareFunction(JCompareStringsCaseInsensitive);
	menuText.SetSortOrder(JListT::kSortAscending);

	JString sysDir, userDir;
	if (JXGetProgramDataDirectories(ProjectDocument::GetTemplateDirectoryName(),
									&sysDir, &userDir))
	{
		JString* menuTextStr = nullptr;
		BuildTemplateMenuItems(sysDir,  false, &menuText, &menuTextStr);
		BuildTemplateMenuItems(userDir, true,  &menuText, &menuTextStr);

		const JSize count = menuText.GetItemCount();
		JString itemText, nmShortcut;
		for (JIndex i=1; i<=count; i++)
		{
			// We have to extract user/sys here because otherwise we would
			// have to keep extra state while building the sorted list.

			itemText = *menuText.GetItem(i);

			JPtrArray<JString> s(JPtrArrayT::kDeleteAll);
			itemText.Split(" (", &s, 2);
			assert( s.GetItemCount() == 2 );

			itemText = *s.GetItem(1);

			nmShortcut = *s.GetItem(2);
			nmShortcut.Prepend("(");

			itsTemplateMenu->AppendItem(itemText, JXMenu::kRadioType);
			itsTemplateMenu->SetItemNMShortcut(itsTemplateMenu->GetItemCount(), nmShortcut);

			// mark item corresponding to initial template selection

			if (menuText.GetItem(i) == menuTextStr)
			{
				itsTemplateIndex = itsTemplateMenu->GetItemCount();
			}
		}
	}

	// after selecting initial template

	itsTemplateMenu->SetToPopupChoice(true, itsTemplateIndex);
	UpdateMakefileMethod();
}

/******************************************************************************
 BuildTemplateMenuItems (private)

	Builds a sorted list of the valid template files below the given path.

 ******************************************************************************/

void
SaveNewProjectDialog::BuildTemplateMenuItems
	(
	const JString&		path,
	const bool			isUserPath,
	JPtrArray<JString>*	menuText,
	JString**			menuTextStr
	)
	const
{
	JDirInfo* info = nullptr;
	if (JDirInfo::Create(path, &info))
	{
		info->ShowDirs(false);

		const JSize count = info->GetEntryCount();
		JString fullName, templateType;
		for (JIndex i=1; i<=count; i++)
		{
			fullName = (info->GetEntry(i)).GetFullName();
			if (ProjectDocument::GetProjectTemplateType(fullName, &templateType))
			{
				auto* s = jnew JString((info->GetEntry(i)).GetName());
				if (isUserPath)
				{
					*s += JGetString("UserTemplateMarker::SaveNewProjectDialog");
				}
				else
				{
					*s += JGetString("SysTemplateMarker::SaveNewProjectDialog");
				}

				menuText->InsertSorted(s);

				// save item corresponding to initial template selection

				if (JSameDirEntry(itsProjectTemplate, fullName))
				{
					*menuTextStr = s;
				}
			}
		}
	}
}

/******************************************************************************
 GetProjectTemplate

 ******************************************************************************/

bool
SaveNewProjectDialog::GetProjectTemplate
	(
	JString* fullName
	)
	const
{
	if (itsTemplateIndex == kNoTemplateCmd)
	{
		fullName->Clear();
		return false;
	}
	else
	{
		JString nmShortcut;
		const bool ok = itsTemplateMenu->GetItemNMShortcut(itsTemplateIndex, &nmShortcut);
		assert( ok );

		JString sysDir, userDir;
		JXGetProgramDataDirectories(ProjectDocument::GetTemplateDirectoryName(),
									&sysDir, &userDir);

		*fullName = JCombinePathAndName(
			nmShortcut == JGetString("UserTemplateMarker::SaveNewProjectDialog") ? userDir : sysDir,
			itsTemplateMenu->GetItemText(itsTemplateIndex));
		return true;
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SaveNewProjectDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsTemplateMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		itsTemplateMenu->CheckItem(itsTemplateIndex);
	}
	else if (sender == itsTemplateMenu && message.Is(JXMenu::kItemSelected))
	{
		auto& selection = dynamic_cast<const JXMenu::ItemSelected&>(message);
		itsTemplateIndex = selection.GetIndex();
		UpdateMakefileMethod();
	}

	else
	{
		JXSaveFileDialog::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateMakefileMethod (private)

 ******************************************************************************/

void
SaveNewProjectDialog::UpdateMakefileMethod()
{
	itsMethodRG->SetActive(itsTemplateIndex == kNoTemplateCmd );
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
SaveNewProjectDialog::OKToDeactivate()
{
	if (!JXSaveFileDialog::OKToDeactivate())
	{
		return false;
	}
	else if (Cancelled())
	{
		return true;
	}

	const BuildManager::MakefileMethod method = GetMakefileMethod();
	if (method == BuildManager::kManual)
	{
		return true;
	}

	// ask if OK to replace Make.files

	if (method == BuildManager::kMakemake)
	{
		const JString makeFilesName = BuildManager::GetMakeFilesName(GetPath());
		if (JFileExists(makeFilesName) &&
			!OKToReplaceFile(makeFilesName, JGetString("Name")))
		{
			return false;
		}
	}

	// ask if OK to replace CMakeLists.txt

	if (method == BuildManager::kCMake)
	{
		JString projRoot, projSuffix;
		JSplitRootAndSuffix(GetFileName(), &projRoot, &projSuffix);

		const JString cmakeInputName = BuildManager::GetCMakeInputName(GetPath());
		if (JFileExists(cmakeInputName) &&
			!OKToReplaceFile(cmakeInputName, JGetString("Name")))
		{
			return false;
		}
	}

	// ask if OK to replace .pro

	if (method == BuildManager::kQMake)
	{
		JString projRoot, projSuffix;
		JSplitRootAndSuffix(GetFileName(), &projRoot, &projSuffix);

		const JString qmakeInputName = BuildManager::GetQMakeInputName(GetPath(), projRoot);
		if (JFileExists(qmakeInputName) &&
			!OKToReplaceFile(qmakeInputName, JGetString("Name")))
		{
			return false;
		}
	}

	// ask if OK to replace existing Makefile

	JPtrArray<JString> makefileList(JPtrArrayT::kDeleteAll);
	BuildManager::GetMakefileNames(GetPath(), &makefileList);

	const JSize count = makefileList.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JString* fullName = makefileList.GetItem(i);
		if (JFileExists(*fullName) &&
			!OKToReplaceFile(*fullName, BuildManager::GetMakefileMethodName(GetMakefileMethod())))
		{
			return false;
		}
	}

	return true;
}

/******************************************************************************
 OKToReplaceFile (private)

 ******************************************************************************/

bool
SaveNewProjectDialog::OKToReplaceFile
	(
	const JString& fullName,
	const JString& programName
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);

	const JUtf8Byte* map[] =
	{
		"file",   name.GetBytes(),
		"method", programName.GetBytes()
	};
	const JString msg = JGetString("WarnFileExists::SaveNewProjectDialog", map, sizeof(map));

	return JGetUserNotification()->AskUserNo(msg);
}

/******************************************************************************
 ReadPrefs (virtual)

	Intentional override of JXChooseSaveFile::ReadPrefs().

 ******************************************************************************/

void
SaveNewProjectDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
	{
		if (vers >= 1)
		{
			input >> itsProjectTemplate;
		}

		input >> itsMakefileMethod;
	}
}

/******************************************************************************
 WritePrefs (virtual)

	Intentional override of JXChooseSaveFile::WritePrefs().

 ******************************************************************************/

void
SaveNewProjectDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << itsProjectTemplate;
	output << ' ' << itsMakefileMethod;
	output << ' ';
}

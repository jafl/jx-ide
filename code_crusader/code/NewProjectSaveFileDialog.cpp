/******************************************************************************
 NewProjectSaveFileDialog.cpp

	BASE CLASS = JXSaveFileDialog

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "NewProjectSaveFileDialog.h"
#include "ProjectDocument.h"
#include "BuildManager.h"
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
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

// Template menu

static const JUtf8Byte* kTemplateMenuStr = "None %r %l";

enum
{
	kNoTemplateCmd = 1
};

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

NewProjectSaveFileDialog*
NewProjectSaveFileDialog::Create
	(
	JXDirector*								supervisor,
	JDirInfo*								dirInfo,
	const JString&							fileFilter,
	const JString&							templateFile,
	const BuildManager::MakefileMethod	method,
	const JString&							origName,
	const JString&							prompt,
	const JString&							message
	)
{
	auto* dlog =
		jnew NewProjectSaveFileDialog(supervisor, dirInfo, fileFilter, method);
	assert( dlog != nullptr );
	dlog->BuildWindow(origName, prompt, message);
	dlog->BuildTemplateMenu(templateFile);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

NewProjectSaveFileDialog::NewProjectSaveFileDialog
	(
	JXDirector*								supervisor,
	JDirInfo*								dirInfo,
	const JString&							fileFilter,
	const BuildManager::MakefileMethod	method
	)
	:
	JXSaveFileDialog(supervisor, dirInfo, fileFilter)
{
	itsMakefileMethod = method;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

NewProjectSaveFileDialog::~NewProjectSaveFileDialog()
{
}

/******************************************************************************
 GetMakefileMethod

	itsMakefileMethod is only stored for use by BuildWindow().

 ******************************************************************************/

BuildManager::MakefileMethod
NewProjectSaveFileDialog::GetMakefileMethod()
	const
{
	return (BuildManager::MakefileMethod) itsMethodRG->GetSelectedItem();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
NewProjectSaveFileDialog::BuildWindow
	(
	const JString& origName,
	const JString& prompt,
	const JString& message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,500, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,140);
	assert( scrollbarSet != nullptr );

	auto* saveButton =
		jnew JXTextButton(JGetString("saveButton::NewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( saveButton != nullptr );
	saveButton->SetShortcuts(JGetString("saveButton::NewProjectSaveFileDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::NewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	auto* homeButton =
		jnew JXTextButton(JGetString("homeButton::NewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::NewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != nullptr );
	pathLabel->SetToLabel();

	auto* fileNameInput =
		jnew JXSaveFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,310, 180,20);
	assert( fileNameInput != nullptr );

	auto* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != nullptr );

	auto* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::NewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != nullptr );

	auto* promptLabel =
		jnew JXStaticText(JGetString("promptLabel::NewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,290, 180,20);
	assert( promptLabel != nullptr );
	promptLabel->SetToLabel();

	auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::NewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != nullptr );
	filterLabel->SetToLabel();

	auto* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != nullptr );

	auto* makefileTitle =
		jnew JXStaticText(JGetString("makefileTitle::NewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,420, 140,20);
	assert( makefileTitle != nullptr );
	makefileTitle->SetToLabel();

	itsMethodRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 160,390, 132,106);
	assert( itsMethodRG != nullptr );

	auto* manualRB =
		jnew JXTextRadioButton(BuildManager::kManual, JGetString("manualRB::NewProjectSaveFileDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 110,20);
	assert( manualRB != nullptr );

	auto* qmakeRB =
		jnew JXTextRadioButton(BuildManager::kQMake, JGetString("qmakeRB::NewProjectSaveFileDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 110,20);
	assert( qmakeRB != nullptr );

	auto* makemakeRB =
		jnew JXTextRadioButton(BuildManager::kMakemake, JGetString("makemakeRB::NewProjectSaveFileDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 110,20);
	assert( makemakeRB != nullptr );

	auto* pathHistory =
		jnew JXPathHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	auto* filterHistory =
		jnew JXStringHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	auto* upButton =
		jnew JXTextButton(JGetString("upButton::NewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	auto* newDirButton =
		jnew JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( newDirButton != nullptr );

	auto* currPathMenu =
		jnew JXCurrentPathMenu(JString("/", JString::kNoCopy), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != nullptr );

	itsTemplateMenu =
		jnew JXTextMenu(JGetString("itsTemplateMenu::NewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,350, 270,30);
	assert( itsTemplateMenu != nullptr );

	auto* cmakeRB =
		jnew JXTextRadioButton(BuildManager::kCMake, JGetString("cmakeRB::NewProjectSaveFileDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 110,20);
	assert( cmakeRB != nullptr );

	auto* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::NewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != nullptr );

// end JXLayout

	SetObjects(scrollbarSet, promptLabel, prompt, fileNameInput, origName,
			   pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   saveButton, cancelButton, upButton, homeButton,
			   desktopButton, newDirButton,
			   showHiddenCB, currPathMenu, message);

	itsMethodRG->SelectItem(itsMakefileMethod);
}

/******************************************************************************
 BuildTemplateMenu (private)

 ******************************************************************************/

void
NewProjectSaveFileDialog::BuildTemplateMenu
	(
	const JString& templateFile
	)
{
	itsTemplateIndex = kNoTemplateCmd;

	itsTemplateMenu->SetMenuItems(kTemplateMenuStr);
	itsTemplateMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsTemplateMenu);

	JPtrArray<JString> menuText(JPtrArrayT::kDeleteAll);
	menuText.SetCompareFunction(JCompareStringsCaseInsensitive);
	menuText.SetSortOrder(JListT::kSortAscending);

	JString sysDir, userDir;
	if (JXGetProgramDataDirectories(ProjectDocument::GetTemplateDirectoryName(),
									&sysDir, &userDir))
	{
		JString* menuTextStr = nullptr;
		BuildTemplateMenuItems(sysDir,  false, &menuText, templateFile, &menuTextStr);
		BuildTemplateMenuItems(userDir, true,  &menuText, templateFile, &menuTextStr);

		const JSize count = menuText.GetElementCount();
		JString itemText, nmShortcut;
		for (JIndex i=1; i<=count; i++)
		{
			// We have to extract user/sys here because otherwise we would
			// have to keep extra state while building the sorted list.

			itemText = *menuText.GetElement(i);

			JPtrArray<JString> s(JPtrArrayT::kDeleteAll);
			itemText.Split(" (", &s, 2);
			assert( s.GetElementCount() == 2 );

			itemText = *s.GetElement(1);

			nmShortcut = *s.GetElement(2);
			nmShortcut.Prepend("(");

			itsTemplateMenu->AppendItem(itemText, JXMenu::kRadioType);
			itsTemplateMenu->SetItemNMShortcut(itsTemplateMenu->GetItemCount(), nmShortcut);

			// mark item corresponding to initial template selection

			if (menuText.GetElement(i) == menuTextStr)
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
NewProjectSaveFileDialog::BuildTemplateMenuItems
	(
	const JString&		path,
	const bool		isUserPath,
	JPtrArray<JString>*	menuText,
	const JString&		templateFile,
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
				assert( s != nullptr );

				if (isUserPath)
				{
					*s += JGetString("UserTemplateMarker::NewProjectSaveFileDialog");
				}
				else
				{
					*s += JGetString("SysTemplateMarker::NewProjectSaveFileDialog");
				}

				menuText->InsertSorted(s);

				// save item corresponding to initial template selection

				if (JSameDirEntry(templateFile, fullName))
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
NewProjectSaveFileDialog::GetProjectTemplate
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
			nmShortcut == JGetString("UserTemplateMarker::NewProjectSaveFileDialog") ? userDir : sysDir,
			itsTemplateMenu->GetItemText(itsTemplateIndex));
		return true;
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
NewProjectSaveFileDialog::Receive
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
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsTemplateIndex = selection->GetIndex();
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
NewProjectSaveFileDialog::UpdateMakefileMethod()
{
	itsMethodRG->SetActive(itsTemplateIndex == kNoTemplateCmd );
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
NewProjectSaveFileDialog::OKToDeactivate()
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
		JString projFileName, projRoot, projSuffix;
		GetFileName(&projFileName);
		JSplitRootAndSuffix(projFileName, &projRoot, &projSuffix);

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
		JString projFileName, projRoot, projSuffix;
		GetFileName(&projFileName);
		JSplitRootAndSuffix(projFileName, &projRoot, &projSuffix);

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

	const JSize count = makefileList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JString* fullName = makefileList.GetElement(i);
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
NewProjectSaveFileDialog::OKToReplaceFile
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
	const JString msg = JGetString("WarnFileExists::NewProjectSaveFileDialog", map, sizeof(map));

	return JGetUserNotification()->AskUserNo(msg);
}

/******************************************************************************
 NewProjectCSF.cpp

	BASE CLASS = JXChooseSaveFile, JPrefObject

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "NewProjectCSF.h"
#include "NewProjectSaveFileDialog.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version  1 stores itsProjectTemplate

/******************************************************************************
 Constructor

 ******************************************************************************/

NewProjectCSF::NewProjectCSF()
	:
	JXChooseSaveFile()
{
	itsSaveDialog     = nullptr;
	itsMakefileMethod = BuildManager::kMakemake;

	SetPrefInfo(GetPrefsManager(), kNewProjectCSFID);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

NewProjectCSF::~NewProjectCSF()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 CreateSaveFileDialog (virtual protected)

 ******************************************************************************/

JXSaveFileDialog*
NewProjectCSF::CreateSaveFileDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const JString&	origName,
	const JString&	prompt,
	const JString&	message
	)
{
	itsSaveDialog =
		NewProjectSaveFileDialog::Create(supervisor, dirInfo, fileFilter,
										   itsProjectTemplate, itsMakefileMethod,
										   origName, prompt, message);
	return itsSaveDialog;
}

/******************************************************************************
 Receive (protected)

	Listen for response from itsChooseDialog.

 ******************************************************************************/

void
NewProjectCSF::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsSaveDialog && message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			itsSaveDialog->GetProjectTemplate(&itsProjectTemplate);
			itsMakefileMethod = itsSaveDialog->GetMakefileMethod();
		}
		itsSaveDialog = nullptr;
	}

	JXChooseSaveFile::Receive(sender, message);
}

/******************************************************************************
 ReadPrefs (virtual)

	Intentional override of JXChooseSaveFile::ReadPrefs().

 ******************************************************************************/

void
NewProjectCSF::ReadPrefs
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
NewProjectCSF::WritePrefs
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

/******************************************************************************
 RelPathCSF.cpp

	BASE CLASS = JXChooseSaveFile

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "RelPathCSF.h"
#include "RPChooseFileDialog.h"
#include "RPChoosePathDialog.h"
#include "ProjectDocument.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

	We take ProjectDocument instead of a file path against the day when
	"Save as..." is supported.

 ******************************************************************************/

RelPathCSF::RelPathCSF
	(
	ProjectDocument* doc
	)
	:
	JXChooseSaveFile()
{
	itsFileDialog = nullptr;
	itsPathDialog = nullptr;
	itsPathType   = kProjectRelative;
	itsDoc        = doc;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RelPathCSF::~RelPathCSF()
{
}

/******************************************************************************
 ChooseRelFile

 ******************************************************************************/

bool
RelPathCSF::ChooseRelFile
	(
	const JString&	prompt,
	const JString&	instructions,
	const JString&	origName,
	JString*		name
	)
{
	const JString fullOrigName = PrepareForChoose(origName);
	if (JXChooseSaveFile::ChooseFile(prompt, instructions, fullOrigName, name))
	{
		*name = ConvertToRelativePath(*name);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 CreateChooseFileDialog (virtual protected)

 ******************************************************************************/

JXChooseFileDialog*
RelPathCSF::CreateChooseFileDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const bool	allowSelectMultiple,
	const JString&	origName,
	const JString&	message
	)
{
	itsFileDialog =
		RPChooseFileDialog::Create(supervisor, dirInfo, fileFilter,
									 allowSelectMultiple,
									 itsPathType, origName, message);
	return itsFileDialog;
}

/******************************************************************************
 ChooseRelRPath

 ******************************************************************************/

bool
RelPathCSF::ChooseRelRPath
	(
	const JString&	prompt,
	const JString&	instructions,
	const JString&	origPath,
	JString*		newPath
	)
{
	const JString startPath = PrepareForChoose(origPath);
	if (JXChooseSaveFile::ChooseRPath(prompt, instructions, startPath, newPath))
	{
		*newPath = ConvertToRelativePath(*newPath);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 ChooseRelRWPath

 ******************************************************************************/

bool
RelPathCSF::ChooseRelRWPath
	(
	const JString&	prompt,
	const JString&	instructions,
	const JString&	origPath,
	JString*		newPath
	)
{
	const JString startPath = PrepareForChoose(origPath);
	if (JXChooseSaveFile::ChooseRWPath(prompt, instructions, startPath, newPath))
	{
		*newPath = ConvertToRelativePath(*newPath);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 PrepareForChoose (private)

 ******************************************************************************/

JString
RelPathCSF::PrepareForChoose
	(
	const JString& origName
	)
{
	CalcPathType(origName, &itsPathType);

	JString startPath;
	if (origName.IsEmpty())
	{
		startPath = GetProjectPath();
	}
	else if (!origName.IsEmpty() &&
			 !JConvertToAbsolutePath(origName, GetProjectPath(), &startPath))
	{
		startPath = origName;
		if (JIsRelativePath(startPath))
		{
			startPath.Prepend(GetProjectPath());
		}
	}

	return startPath;
}

/******************************************************************************
 CalcPathType

 ******************************************************************************/

// private -- returns void to avoid conflict with static version

void
RelPathCSF::CalcPathType
	(
	const JString&	path,
	PathType*		type
	)
	const
{
	if (!path.IsEmpty())
	{
		*type = CalcPathType(path);
	}
}

// static

RelPathCSF::PathType
RelPathCSF::CalcPathType
	(
	const JString& path
	)
{
	assert( !path.IsEmpty() );

	if (path.GetFirstCharacter() == '~')
	{
		return kHomeRelative;
	}
	else if (JIsAbsolutePath(path))
	{
		return kAbsolutePath;
	}
	else
	{
		return kProjectRelative;
	}
}

/******************************************************************************
 CreateChoosePathDialog (virtual protected)

 ******************************************************************************/

JXChoosePathDialog*
RelPathCSF::CreateChoosePathDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const bool	selectOnlyWritable,
	const JString&	message
	)
{
	itsPathDialog =
		RPChoosePathDialog::Create(supervisor, dirInfo, fileFilter,
									 selectOnlyWritable,
									 itsPathType, message);
	return itsPathDialog;
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
RelPathCSF::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileDialog && message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			itsPathType = itsFileDialog->GetPathType();
		}
		itsFileDialog = nullptr;
	}

	else if (sender == itsPathDialog && message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			itsPathType = itsPathDialog->GetPathType();
		}
		itsPathDialog = nullptr;
	}

	JXChooseSaveFile::Receive(sender, message);
}

/******************************************************************************
 GetProjectPath

 ******************************************************************************/

const JString&
RelPathCSF::GetProjectPath()
	const
{
	return itsDoc->GetFilePath();
}

/******************************************************************************
 ConvertToRelativePath

 ******************************************************************************/

JString
RelPathCSF::ConvertToRelativePath
	(
	const JString& fullPath
	)
{
	return ConvertToRelativePath(fullPath, GetProjectPath(), itsPathType);
}

/******************************************************************************
 ConvertToRelativePath (static)

 ******************************************************************************/

JString
RelPathCSF::ConvertToRelativePath
	(
	const JString&	fullPath,
	const JString&	projPath,
	const PathType	pathType
	)
{
	if (!JFileExists(fullPath) && !JDirectoryExists(fullPath))
	{
		return fullPath;
	}

	if (pathType == kProjectRelative)
	{
		return JConvertToRelativePath(fullPath, projPath);
	}
	else if (pathType == kHomeRelative)
	{
		JString path, trueHome;
		if (JGetHomeDirectory(&path) &&
			JGetTrueName(path, &trueHome))
		{
			path = JConvertToRelativePath(fullPath, trueHome);
			if (path.BeginsWith("." ACE_DIRECTORY_SEPARATOR_STR))
			{
				JStringIterator iter(&path);
				iter.SetNext(JUtf8Character('~'));
			}
			else if (JIsRelativePath(path))
			{
				path.Prepend("~" ACE_DIRECTORY_SEPARATOR_STR);
			}
			return path;
		}
	}

	return fullPath;
}

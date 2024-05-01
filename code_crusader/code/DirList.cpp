/******************************************************************************
 DirList.cpp

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "DirList.h"
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DirList::DirList()
	:
	JContainer()
{
	DirListX();

	AddPath("./", true);
}

// private

void
DirList::DirListX()
{
	itsDirList = jnew DirInfoList;
	itsDirList->SetSortOrder(JListT::kSortAscending);
	itsDirList->SetCompareFunction(DirInfo::ComparePathNames);

	InstallCollection(itsDirList);
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

DirList::DirList
	(
	const DirList& source
	)
	:
	JContainer(),
	itsBasePath(source.itsBasePath)
{
	DirListX();
	CopyPaths(source);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DirList::~DirList()
{
	itsDirList->DeleteAll();
	jdelete itsDirList;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

DirList&
DirList::operator=
	(
	const DirList& source
	)
{
	if (this == &source)
	{
		return *this;
	}

	JContainer::operator=(source);

	itsBasePath = source.itsBasePath;
	CopyPaths(source);

	return *this;
}

/******************************************************************************
 CopyPaths (private)

 ******************************************************************************/

void
DirList::CopyPaths
	(
	const DirList& source
	)
{
	itsDirList->DeleteAll();

	const JSize count = (source.itsDirList)->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const DirInfo info = (source.itsDirList)->GetItem(i);
		AddPath(*(info.path), info.recurse);
	}
}

/******************************************************************************
 GetPath

 ******************************************************************************/

const JString&
DirList::GetPath
	(
	const JIndex	index,
	bool*		recurse
	)
	const
{
	const DirInfo info = itsDirList->GetItem(index);
	*recurse             = info.recurse;
	return *(info.path);
}

/******************************************************************************
 GetFullPath

 ******************************************************************************/

bool
DirList::GetFullPath
	(
	const JIndex	index,
	JString*		fullPath,
	bool*		recurse
	)
	const
{
	const DirInfo info = itsDirList->GetItem(index);
	*recurse             = info.recurse;
	return JConvertToAbsolutePath(*(info.path), itsBasePath, fullPath);
}

/******************************************************************************
 GetTruePath

 ******************************************************************************/

bool
DirList::GetTruePath
	(
	const JIndex	index,
	JString*		truePath,
	bool*		recurse
	)
	const
{
	JString path;
	return GetFullPath(index, &path, recurse) &&
				JGetTrueName(path, truePath);
}

/******************************************************************************
 AddPath

 ******************************************************************************/

void
DirList::AddPath
	(
	const JString&	path,
	const bool	recurse
	)
{
	DirInfo info(jnew JString(path), recurse);
	itsDirList->InsertSorted(info);
}

/******************************************************************************
 SetBasePath

	This is used if a relative path is stored.  It must be an absolute path.

 ******************************************************************************/

void
DirList::SetBasePath
	(
	const JString& path
	)
{
	assert( JIsAbsolutePath(path) );
	itsBasePath = path;
}

/******************************************************************************
 Contains

	Returns true if the given item is contained in our list of paths.

 ******************************************************************************/

bool
DirList::Contains
	(
	const JString& p
	)
	const
{
	if (p.IsEmpty())
	{
		return false;
	}

	JString path = p, name;
	if (path.GetLastCharacter() != ACE_DIRECTORY_SEPARATOR_CHAR)
	{
		JSplitPathAndName(p, &path, &name);
	}

	const JSize count = GetItemCount();
	JString truePath;
	bool recurse;
	for (JIndex i=1; i<=count; i++)
	{
		if (GetTruePath(i, &truePath, &recurse) &&
			(( recurse && path.StartsWith(truePath)) ||
			 (!recurse && path == truePath)))
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 ReadDirectories

 ******************************************************************************/

void
DirList::ReadDirectories
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	itsDirList->DeleteAll();

	if (vers >= 71)
	{
		input >> std::ws;
		JIgnoreLine(input);
	}

	if (vers <= 71)
	{
		JSize dirCount;
		input >> dirCount;

		for (JIndex i=1; i<=dirCount; i++)
		{
			ReadDirectory(input, vers);
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

			ReadDirectory(input, vers);
		}
	}
}

/******************************************************************************
 ReadDirectory (private)

 ******************************************************************************/

void
DirList::ReadDirectory
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	JString path;
	bool recurse = true;

	input >> path;
	if (vers >= 21)
	{
		input >> JBoolFromString(recurse);
	}
	AddPath(path, recurse);
}

/******************************************************************************
 WriteDirectories

 ******************************************************************************/

void
DirList::WriteDirectories
	(
	std::ostream& output
	)
	const
{
	output << "# search paths\n";

	const JSize dirCount = GetItemCount();
	for (JIndex i=1; i<=dirCount; i++)
	{
		output << JBoolToString(true);

		const DirInfo info = itsDirList->GetItem(i);
		output << ' ' << *(info.path);
		output << ' ' << JBoolToString(info.recurse);
		output << '\n';
	}

	output << JBoolToString(false) << '\n';
}

/******************************************************************************
 operator==

	We know that DirList objects are always sorted.

 ******************************************************************************/

bool
operator==
	(
	const DirList& l1,
	const DirList& l2
	)
{
	if (l1.GetBasePath() != l2.GetBasePath())
	{
		return false;
	}

	bool sameDirs = false;
	const JSize count = l1.GetItemCount();
	if (count == l2.GetItemCount())
	{
		sameDirs = true;
		for (JIndex i=1; i<=count; i++)
		{
			bool r1, r2;
			if (l1.GetPath(i, &r1) != l2.GetPath(i, &r2) ||
				r1 != r2)
			{
				sameDirs = false;
				break;
			}
		}
	}

	return sameDirs;
}

/******************************************************************************
 DeleteAll

 ******************************************************************************/

void
DirInfoList::DeleteAll()
{
	const JSize count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		jdelete (GetItem(i)).path;
	}
	RemoveAll();
}

/******************************************************************************
 ComparePathNames (static)

	UNIX path names are case-sensitive.

 ******************************************************************************/

std::weak_ordering
DirInfo::ComparePathNames
	(
	const DirInfo& i1,
	const DirInfo& i2
	)
{
	return JCompareStringsCaseSensitive(i1.path, i2.path);
}

/******************************************************************************
 CompareProjIndex (static)

 ******************************************************************************/

std::weak_ordering
DirInfo::CompareProjIndex
	(
	const DirInfo& i1,
	const DirInfo& i2
	)
{
	std::weak_ordering r = i1.projIndex <=> i2.projIndex;
	if (r == std::weak_ordering::equivalent)
	{
		r = ComparePathNames(i1, i2);
	}
	return r;
}

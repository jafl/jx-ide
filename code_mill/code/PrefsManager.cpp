/******************************************************************************
 PrefsManager.cpp

	<Description>

	BASE CLASS = public JXPrefsManager

	Copyright (C) 2002 by Glenn W. Bach.

 *****************************************************************************/

#include "PrefsManager.h"
#include "PrefsDialog.h"
#include "globals.h"
#include <time.h>
#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 0;

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
	SetData(kProgramVersionID, GetVersionNumberStr());

	JXPrefsManager::SaveAllBeforeDestruct();
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
	if (isNew)
	{
		SetData(kProgramVersionID, GetVersionNumberStr());
	}
}

/******************************************************************************
 GetPrevVersionStr

 ******************************************************************************/

JString
PrefsManager::GetPrevVersionStr()
	const
{
	std::string data;
	const bool ok = GetData(kProgramVersionID, &data);
	assert( ok );
	return JString(data);
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
PrefsManager::EditPrefs()
{
	auto* dlog =
		jnew PrefsDialog(
			GetHeaderComment(),
			GetSourceComment(),
			GetConstructorComment(),
			GetDestructorComment(),
			GetFunctionComment());
	assert( dlog != nullptr );

	if (dlog->DoDialog())
	{
		JString header;
		JString source;
		JString constructor;
		JString destructor;
		JString function;

		dlog->GetValues(&header, &source,
				  &constructor, &destructor,
				  &function);

		SetHeaderComment(header);
		SetSourceComment(source);
		SetConstructorComment(constructor);
		SetDestructorComment(destructor);
		SetFunctionComment(function);
	}
}

/******************************************************************************
 GetHeaderComment

 ******************************************************************************/

JString
PrefsManager::GetHeaderComment
	(
	const JString& classname
	)
	const
{
	JString comment;
	std::string data;
	if (GetData(kHeaderCommentID, &data))
	{
		std::istringstream is(data);
		is >> comment;
	}
	else
	{
		comment	= JGetString("CLASS_HEADER_COMMENT");
	}
		
	if (!classname.IsEmpty())
	{
		JString copyright = GetCopyright(true);
		const JUtf8Byte* map[] =
		{
			"class",     classname.GetBytes(),
			"copyright", copyright.GetBytes()
		};
		JGetStringManager()->Replace(&comment, map, sizeof(map));
	}

	return comment;
}

/******************************************************************************
 SetHeaderComment

 ******************************************************************************/

void
PrefsManager::SetHeaderComment
	(
	const JString& comment
	)
{
	std::ostringstream os;
	os << comment;
	SetData(kHeaderCommentID, os);
}

/******************************************************************************
 GetAuthor

 ******************************************************************************/

JString
PrefsManager::GetAuthor()
	const
{
	std::string data;
	if (GetData(kAuthorID, &data))
	{
		std::istringstream is(data);
		JString author;
		is >> author;
		return author;
	}
	else
	{
		return JString::empty;
	}
}

/******************************************************************************
 SetAuthor

 ******************************************************************************/

void
PrefsManager::SetAuthor
	(
	const JString& author
	)
{
	std::ostringstream os;
	os << author;
	SetData(kAuthorID, os);
}

/******************************************************************************
 GetYear

 ******************************************************************************/

JString
PrefsManager::GetYear()
	const
{
	std::string data;
	if (GetData(kYearID, &data))
	{
		std::istringstream is(data);
		JString year;
		is >> year;
		return year;
	}
	else
	{
		time_t now;
		time(&now);

		struct tm local_time;
		localtime_r(&now, &local_time);

		return JString((JUInt64) local_time.tm_year + 1900);
	}
}

/******************************************************************************
 SetYear

 ******************************************************************************/

void
PrefsManager::SetYear
	(
	const JString& year
	)
{
	std::ostringstream os;
	os << year;
	SetData(kYearID, os);
}


/******************************************************************************
 GetCopyright

 ******************************************************************************/

JString
PrefsManager::GetCopyright
	(
	const bool replaceVars
	)
	const
{
	JString copyright;
	std::string data;
	if (GetData(kCopyrightID, &data))
	{
		std::istringstream is(data);
		is >> copyright;
	}
	else
	{
		copyright	= JGetString("CLASS_COPYRIGHT");
	}
		
	if (replaceVars)
	{
		JString author = GetAuthor();
		JString year   = GetYear();
		const JUtf8Byte* map[] =
		{
			"year",   year.GetBytes(),
			"author", author.GetBytes()
		};
		JGetStringManager()->Replace(&copyright, map, sizeof(map));
	}

	return copyright;
}

/******************************************************************************
 SetCopyright

 ******************************************************************************/

void
PrefsManager::SetCopyright
	(
	const JString& copyright
	)
{
	std::ostringstream os;
	os << copyright;
	SetData(kCopyrightID, os);
}

/******************************************************************************
 GetSourceComment

 ******************************************************************************/

JString
PrefsManager::GetSourceComment
	(
	const JString& classname,
	const JString& base
	)
	const
{
	JString comment;
	std::string data;
	if (GetData(kSourceCommentID, &data))
	{
		std::istringstream is(data);
		is >> comment;
	}
	else
	{
		comment	= JGetString("CLASS_SOURCE_COMMENT");
	}
		
	if (!classname.IsEmpty())
	{
		JString copyright = GetCopyright(true);
		const JUtf8Byte* map[] =
		{
			"class",     classname.GetBytes(),
			"base",      base.GetBytes(),
			"copyright", copyright.GetBytes()
		};
		JGetStringManager()->Replace(&comment, map, sizeof(map));
	}

	return comment;
}

/******************************************************************************
 SetSourceComment

 ******************************************************************************/

void
PrefsManager::SetSourceComment
	(
	const JString& comment
	)
{
	std::ostringstream os;
	os << comment;
	SetData(kSourceCommentID, os);
}

/******************************************************************************
 GetConstructorComment

 ******************************************************************************/

JString
PrefsManager::GetConstructorComment()
	const
{
	JString comment;
	std::string data;
	if (GetData(kConstructorCommentID, &data))
	{
		std::istringstream is(data);
		is >> comment;
	}
	else
	{
		comment	= JGetString("CLASS_CONSTRUCTOR_COMMENT");
	}

	return comment;
}

/******************************************************************************
 SetConstructorComment

 ******************************************************************************/

void
PrefsManager::SetConstructorComment
	(
	const JString& comment
	)
{
	std::ostringstream os;
	os << comment;
	SetData(kConstructorCommentID, os);
}

/******************************************************************************
 GetDestructorComment

 ******************************************************************************/

JString
PrefsManager::GetDestructorComment()
	const
{
	JString comment;
	std::string data;
	if (GetData(kDestructorCommentID, &data))
	{
		std::istringstream is(data);
		is >> comment;
	}
	else
	{
		comment	= JGetString("CLASS_DESTRUCTOR_COMMENT");
	}
		
	return comment;
}

/******************************************************************************
 SetDestructorComment

 ******************************************************************************/

void
PrefsManager::SetDestructorComment
	(
	const JString& comment
	)
{
	std::ostringstream os;
	os << comment;
	SetData(kDestructorCommentID, os);
}

/******************************************************************************
 GetFunctionComment

 ******************************************************************************/

JString
PrefsManager::GetFunctionComment
	(
	const JString& fnName,
	const JString& access
	)
	const
{
	JString comment;
	std::string data;
	if (GetData(kFunctionCommentID, &data))
	{
		std::istringstream is(data);
		is >> comment;
	}
	else
	{
		comment	= JGetString("CLASS_FUNCTION_COMMENT");
	}

	if (!fnName.IsEmpty())
	{
		const JUtf8Byte* map[] =
		{
			"function", fnName.GetBytes(),
			"access",   access.GetBytes()
		};
		JGetStringManager()->Replace(&comment, map, sizeof(map));
	}
			
	return comment;
}

/******************************************************************************
 SetFunctionComment

 ******************************************************************************/

void
PrefsManager::SetFunctionComment
	(
	const JString& comment
	)
{
	std::ostringstream os;
	os << comment;
	SetData(kFunctionCommentID, os);
}

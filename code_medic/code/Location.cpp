/******************************************************************************
 Location.cpp

	BASE CLASS = none

	Copyright (C) 2001 John Lindal.

 *****************************************************************************/

#include <Location.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

Location::Location()
	:
	itsLineNumber(0)
{
}

Location::Location
	(
	const JString&	fileName,
	const JIndex	lineNumber
	)
	:
	itsFileName(fileName),
	itsFileID(fileName),
	itsLineNumber(lineNumber)
{
	if (!itsFileName.IsEmpty() && JIsRelativePath(itsFileName))
	{
		JString p, n;
		JSplitPathAndName(itsFileName, &p, &n);
		itsFileName = n;
	}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

Location::~Location()
{
}

/******************************************************************************
 Copy constructor

 *****************************************************************************/

Location::Location
	(
	const Location& source
	)
{
	*this = source;
}

/******************************************************************************
 Assignment

 *****************************************************************************/

Location&
Location::operator=
	(
	const Location& source
	)
{
	if (this != &source)
	{
		itsFileName      = source.itsFileName;
		itsFileID        = source.itsFileID;
		itsLineNumber    = source.itsLineNumber;
		itsFunctionName  = source.itsFunctionName;
		itsMemoryAddress = source.itsMemoryAddress;
	}

	return *this;
}

/******************************************************************************
 Comparison

 *****************************************************************************/

bool
operator==
	(
	const Location& lhs,
	const Location& rhs
	)
{
	return lhs.itsFileID     == rhs.itsFileID &&
				lhs.itsLineNumber == rhs.itsLineNumber;
}

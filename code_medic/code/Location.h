/******************************************************************************
 Location.h

	Copyright (C) 2001 John Lindal.

 *****************************************************************************/

#ifndef _H_Location
#define _H_Location

#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JFileID.h>

class Location
{
	friend bool operator==(const Location& lhs, const Location& rhs);
	friend bool operator!=(const Location& lhs, const Location& rhs);

public:

	Location();
	Location(const JString& fileName, const JIndex lineNumber);
	Location(const Location& source);

	~Location();

	Location& operator=(const Location& source);

	const JString&	GetFileName() const;
	const JFileID&	GetFileID() const;
	void			SetFileName(const JString& fileName);

	JIndex	GetLineNumber() const;
	void	SetLineNumber(const JIndex lineNumber);

	// not included in comparators

	const JString&	GetFunctionName() const;
	void			SetFunctionName(const JString& fnName);

	const JString&	GetMemoryAddress() const;
	void			SetMemoryAddress(const JString& addr);

private:

	JString	itsFileName;
	JFileID	itsFileID;
	JIndex	itsLineNumber;

	JString	itsFunctionName;
	JString	itsMemoryAddress;
};


/******************************************************************************
 File name

 *****************************************************************************/

inline const JString&
Location::GetFileName() const
{
	return itsFileName;
}

inline void
Location::SetFileName
	(
	const JString& fileName
	)
{
	itsFileName = fileName;
	itsFileID.SetFileName(fileName);
}

/******************************************************************************
 GetFileID

 *****************************************************************************/

inline const JFileID&
Location::GetFileID() const
{
	return itsFileID;
}

/******************************************************************************
 Line number

 *****************************************************************************/

inline JIndex
Location::GetLineNumber() const
{
	return itsLineNumber;
}

inline void
Location::SetLineNumber
	(
	const JIndex lineNumber
	)
{
	itsLineNumber = lineNumber;
}

/******************************************************************************
 Function name

 *****************************************************************************/

inline const JString&
Location::GetFunctionName() const
{
	return itsFunctionName;
}

inline void
Location::SetFunctionName
	(
	const JString& fnName
	)
{
	itsFunctionName = fnName;
}

/******************************************************************************
 Memory address

 *****************************************************************************/

inline const JString&
Location::GetMemoryAddress() const
{
	return itsMemoryAddress;
}

inline void
Location::SetMemoryAddress
	(
	const JString& addr
	)
{
	itsMemoryAddress = addr;
}

/******************************************************************************
 Comparison

 *****************************************************************************/

inline bool
operator!=
	(
	const Location& lhs,
	const Location& rhs
	)
{
	return !(lhs == rhs);
}

#endif

/******************************************************************************
 MemberFunction.h

	Interface for the MemberFunction class

	Copyright (C) 2002 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_MemberFunction
#define _H_MemberFunction

#include <jx-af/jcore/JPtrArray-JString.h>

class MemberFunction
{
public:

	MemberFunction();
	~MemberFunction();

	const JString&	GetFnName() const;
	void			SetFnName(const JString& str);

	bool			IsProtected() const;
	void			ShouldBeProtected(const bool protect);

	bool			IsRequired() const;
	void			ShouldBeRequired(const bool require);

	bool			IsUsed() const;
	void			ShouldBeUsed(const bool use);

	const JString&	GetSignature() const;
	void			SetSignature(const JString& str);

	const JString&	GetReturnType() const;
	void			SetReturnType(const JString& type);

	bool			IsConst() const;
	void			ShouldBeConst(const bool isConst);

	static std::weak_ordering
		CompareFunction(MemberFunction* const & f1, MemberFunction* const & f2);

protected:

private:

	JString	itsFnName;
	bool	itsIsProtected;
	bool	itsIsRequired;	// if base class has pure virtual function
	bool	itsIsUsed;
	JString	itsSignature;
	JString	itsReturnType;
	bool	itsIsConst;

private:

	// not allowed

	MemberFunction(const MemberFunction&) = delete;
	MemberFunction& operator=(const MemberFunction&) = delete;
};


/******************************************************************************
 GetFnName (public)

 ******************************************************************************/

inline const JString&
MemberFunction::GetFnName()
	const
{
	return itsFnName;
}

/******************************************************************************
 SetFnName (public)

 ******************************************************************************/

inline void
MemberFunction::SetFnName
	(
	const JString& name
	)
{
	itsFnName = name;
}

/******************************************************************************
 IsProtected (public)

 ******************************************************************************/

inline bool
MemberFunction::IsProtected()
	const
{
	return itsIsProtected;
}

/******************************************************************************
 ShouldBeProtected (public)

 ******************************************************************************/

inline void
MemberFunction::ShouldBeProtected
	(
	const bool protect
	)
{
	itsIsProtected	= protect;
}

/******************************************************************************
 IsRequired (public)

 ******************************************************************************/

inline bool
MemberFunction::IsRequired()
	const
{
	return itsIsRequired;
}

/******************************************************************************
 ShouldBeRequired (public)

 ******************************************************************************/

inline void
MemberFunction::ShouldBeRequired
	(
	const bool require
	)
{
	itsIsRequired	= require;
}

/******************************************************************************
 IsUsed (public)

 ******************************************************************************/

inline bool
MemberFunction::IsUsed()
	const
{
	return itsIsUsed;
}

/******************************************************************************
 ShouldBeUsed (public)

 ******************************************************************************/

inline void
MemberFunction::ShouldBeUsed
	(
	const bool use
	)
{
	itsIsUsed = use;
}

/******************************************************************************
 GetSignature (public)

 ******************************************************************************/

inline const JString&
MemberFunction::GetSignature()
	const
{
	return itsSignature;
}

/******************************************************************************
 GetReturnType

 ******************************************************************************/

inline const JString&
MemberFunction::GetReturnType()
	const
{
	return itsReturnType;
}

/******************************************************************************
 SetReturnType

 ******************************************************************************/

inline void
MemberFunction::SetReturnType
	(
	const JString& type
	)
{
	itsReturnType = type;
}

/******************************************************************************
 IsConst

 ******************************************************************************/

inline bool
MemberFunction::IsConst()
	const
{
	return itsIsConst;
}

/******************************************************************************
 ShouldBeConst

 ******************************************************************************/

inline void
MemberFunction::ShouldBeConst
	(
	const bool isConst
	)
{
	itsIsConst = isConst;
}

#endif

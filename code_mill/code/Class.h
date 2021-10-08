/******************************************************************************
 Class.h

	Interface for the Class class

	Copyright (C) 2002 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_Class
#define _H_Class

// Superclass Header
#include <jx-af/jcore/JPtrArray.h>

#include <MemberFunction.h>

class Link;

class Class : public JPtrArray<MemberFunction>
{
public:

	Class();
	~Class();

	JSize			GetBaseClassCount() const;
	void			GetBaseClass(const JIndex index, JString* classname, JString* filename) const;
	void			AddBaseClass(const JString& classname, const JString& filename);

	const JString&	GetClassName() const;
	void			SetClassName(const JString& classname);

	const JString&	GetFileName() const;
	void			SetFileName(const JString& filename);

	JSize	GetAncestorCount() const;
	void	GetAncestor(const JIndex index, JString* classname, JString* filename) const;
	void	AddAncestor(const JString& classname, const JString& filename);

	void	Populate();

	void	WritePublicInterface(std::ostream& os);
	void	WriteProtectedInterface(std::ostream& os);
	void	WritePublic(std::ostream& os, const bool interface = false);
	void	WriteProtected(std::ostream& os, const bool interface = false);
	
protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	
private:

	Link*	itsLink;

	JPtrArray<JString>*		itsBaseClasses;
	JPtrArray<JString>*		itsBaseClassFiles;

	JPtrArray<JString>*		itsAncestors;
	JPtrArray<JString>*		itsAncestorFiles;

	JString itsFileName;
	JString	itsClassName;

private:

	void WriteFunction(std::ostream& os, MemberFunction* fn, const bool interface);

	// not allowed

	Class(const Class&) = delete;
	Class& operator=(const Class&) = delete;
};


/******************************************************************************
 GetClassName (public)

 ******************************************************************************/

inline const JString&
Class::GetClassName()
	const
{
	return itsClassName;
}

/******************************************************************************
 SetClassName (public)

 ******************************************************************************/

inline void
Class::SetClassName
	(
	const JString& classname
	)
{
	itsClassName = classname;
}

/******************************************************************************
 GetFileName (public)

 ******************************************************************************/

inline const JString&
Class::GetFileName()
	const
{
	return itsFileName;
}

/******************************************************************************
 SetFileName (public)

 ******************************************************************************/

inline void
Class::SetFileName
	(
	const JString& filename
	)
{
	itsFileName = filename;
}

#endif

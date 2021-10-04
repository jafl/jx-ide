/******************************************************************************
 PrefsManager.h

	Copyright (C) 2002 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_PrefsManager
#define _H_PrefsManager

#include <jx-af/jx/JXPrefsManager.h>

class JString;
class PrefsDialog;

// Preferences -- do not change ID's once they are assigned

enum
{
	kProgramVersionID = 1,
	kgCSFSetupID,
	kMainDirectorID,
	kMainToolBarID,
	kHeaderCommentID,
	kAuthorID,
	kCopyrightID,
	kSourceCommentID,
	kConstructorCommentID,
	kDestructorCommentID,
	kFunctionCommentID,
	kYearID,
	kVersionCheckID
};

class PrefsManager : public JXPrefsManager
{
public:

	PrefsManager(bool* isNew);

	virtual	~PrefsManager();

	JString GetPrevVersionStr() const;
	void	EditPrefs();

	JString	GetHeaderComment(const JString& classname = JString::empty) const;
	void	SetHeaderComment(const JString& comment);

	JString	GetAuthor() const;
	void	SetAuthor(const JString& author);

	JString	GetYear() const;
	void	SetYear(const JString& year);

	JString	GetCopyright(const bool replaceVars = false) const;
	void	SetCopyright(const JString& copyright);

	JString	GetSourceComment(const JString& classname = JString::empty, const JString& base = JString::empty) const;
	void	SetSourceComment(const JString& comment);

	JString	GetConstructorComment() const;
	void	SetConstructorComment(const JString& comment);

	JString	GetDestructorComment() const;
	void	SetDestructorComment(const JString& comment);

	JString	GetFunctionComment(const JString& fnname = JString::empty, const JString& access = JString::empty) const;
	void	SetFunctionComment(const JString& comment);

protected:

	virtual void	UpgradeData(const bool isNew, const JFileVersion currentVersion) override;
	virtual void	SaveAllBeforeDestruct() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	PrefsDialog*	itsDialog;
};

#endif
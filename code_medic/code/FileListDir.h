/******************************************************************************
 FileListDir.h

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_FileListDir
#define _H_FileListDir

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JXTextMenu;
class JXTextButton;
class JXFileListSet;
class JXFileListTable;
class Link;
class CommandDirector;
class GetSourceFileList;

class FileListDir : public JXWindowDirector, public JPrefObject
{
public:

	FileListDir(CommandDirector* supervisor);

	virtual	~FileListDir();

	JXFileListTable*	GetTable();

	virtual const JString&	GetName() const override;
	virtual bool		GetMenuIcon(const JXImage** icon) const override;

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	Link*					itsLink;
	CommandDirector*		itsCommandDir;
	GetSourceFileList*	itsCmd;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsActionsMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout

	JXFileListSet* itsFileListSet;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateWindowTitle(const JString& binaryName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);
};

#endif

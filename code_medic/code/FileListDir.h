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
class GetSourceFileListCmd;

class FileListDir : public JXWindowDirector, public JPrefObject
{
public:

	FileListDir(CommandDirector* supervisor);

	~FileListDir() override;

	JXFileListTable*	GetTable();

	const JString&	GetName() const override;
	bool			GetMenuIcon(const JXImage** icon) const override;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	Link*					itsLink;
	CommandDirector*		itsCommandDir;
	GetSourceFileListCmd*	itsCmd;

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

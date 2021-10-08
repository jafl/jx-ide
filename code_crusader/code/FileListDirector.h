/******************************************************************************
 FileListDirector.h

	Interface for the FileListDirector class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_FileListDirector
#define _H_FileListDirector

#include <jx-af/jx/JXWindowDirector.h>

class JXTextMenu;
class JXToolBar;
class JXFileListSet;
class FileListTable;
class ProjectDocument;
class CommandMenu;

class FileListDirector : public JXWindowDirector
{
public:

	FileListDirector(ProjectDocument* supervisor);
	FileListDirector(std::istream& projInput, const JFileVersion projVers,
					   std::istream* setInput, const JFileVersion setVers,
					   std::istream* symInput, const JFileVersion symVers,
					   ProjectDocument* supervisor, const bool subProject);

	~FileListDirector();

	FileListTable*	GetFileListTable() const;

	void	StreamOut(std::ostream& projOutput,
					  std::ostream* setOutput, std::ostream* symOutput) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	ProjectDocument*	itsProjDoc;
	JXFileListSet*		itsFLSet;
	FileListTable*		itsFLTable;

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsListMenu;
	JXTextMenu*		itsProjectMenu;
	CommandMenu*	itsCmdMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

private:

	void	FileListDirectorX(ProjectDocument* projDoc);
	void	BuildWindow();
	void	AdjustWindowTitle();

	void	OpenSelectedFiles() const;

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateListMenu();
	void	HandleListMenu(const JIndex index);

	void	UpdateProjectMenu();
	void	HandleProjectMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);
};


/******************************************************************************
 GetFileListTable

 ******************************************************************************/

inline FileListTable*
FileListDirector::GetFileListTable()
	const
{
	return itsFLTable;
}

#endif

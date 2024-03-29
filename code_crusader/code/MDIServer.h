/******************************************************************************
 MDIServer.h

	Interface for the MDIServer class

	Copyright © 1997-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_MDIServer
#define _H_MDIServer

#include <jx-af/jx/JXMDIServer.h>
#include <jx-af/jcore/JPrefObject.h>

class MDIServer : public JXMDIServer, public JPrefObject
{
public:

	MDIServer();

	~MDIServer() override;

	static void	PrintCommandLineHelp();

	// called by EditGenPrefsDialog

	void	GetStartupOptions(bool* createEditor, bool* createProject,
							  bool* reopenLast, bool* chooseFile) const;
	void	SetStartupOptions(const bool createEditor, const bool createProject,
							  const bool reopenLast, const bool chooseFile);

protected:

	void	HandleMDIRequest(const JString& dir,
							 const JPtrArray<JString>& argList) override;

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

private:

	bool	itsCreateEditorFlag;
	bool	itsCreateProjectFlag;
	bool	itsReopenLastFlag;
	bool	itsChooseFileFlag;

private:

	void	DisplayManPage(JIndex* index, const JPtrArray<JString>& argList);
	void	DisplayFileDiffs(JIndex* index, const JPtrArray<JString>& argList,
							 const bool silent);
	void	DisplayVCSDiffs(const JUtf8Byte* type, JIndex* index,
							const JPtrArray<JString>& argList,
							const bool silent);
};


/******************************************************************************
 Startup options

 ******************************************************************************/

inline void
MDIServer::GetStartupOptions
	(
	bool* createEditor,
	bool* createProject,
	bool* reopenLast,
	bool* chooseFile
	)
	const
{
	*createEditor  = itsCreateEditorFlag;
	*createProject = itsCreateProjectFlag;
	*reopenLast    = itsReopenLastFlag;
	*chooseFile    = itsChooseFileFlag;
}

inline void
MDIServer::SetStartupOptions
	(
	const bool createEditor,
	const bool createProject,
	const bool reopenLast,
	const bool chooseFile
	)
{
	itsCreateEditorFlag  = createEditor;
	itsCreateProjectFlag = createProject;
	itsReopenLastFlag    = reopenLast;
	itsChooseFileFlag    = chooseFile;
}

#endif

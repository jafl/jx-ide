/******************************************************************************
 BuildManager.h

	Copyright © 1997-2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_BuildManager
#define _H_BuildManager

#include <jx-af/jcore/JBroadcaster.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jTime.h>

class Command;
class ProjectDocument;
class ExecOutputDocument;
class ProjectConfigDialog;
class ProjectNode;

class BuildManager : virtual public JBroadcaster
{
public:

	enum MakefileMethod		// remember to update kMakefileMethodName
	{
		// Do not change values once assigned because they are stored in files

		kManual,
		kMakemake,
		kQMake,
		kCMake
	};

public:

	BuildManager(ProjectDocument* doc, const MakefileMethod method = kMakemake,
				   const bool needWriteMakeFiles = true,
				   const JString& targetName = JString::empty,
				   const JString& depListExpr = JString::empty);
	BuildManager(std::istream& projInput, const JFileVersion projVers,
				   std::istream* setInput, const JFileVersion setVers,
				   ProjectDocument* doc);

	~BuildManager();

	void	CreateMakeFiles(const MakefileMethod method);
	bool	UpdateMakeFiles(const bool reportError = true);
	bool	UpdateMakefile(ExecOutputDocument* compileDoc = nullptr,
						   Command** cmd = nullptr,
						   const bool force = false);

	void	EditProjectConfig();
	bool	EditMakeConfig();

	const JString&	GetBuildTargetName() const;
	void			SetBuildTargetName(const JString& targetName);

	MakefileMethod	GetMakefileMethod() const;

	static const JString&	GetSubProjectBuildSuffix();

	static bool	WillRebuildMakefileDaily();
	static void	ShouldRebuildMakefileDaily(const bool rebuild);

	// stored in project

	void	StreamOut(std::ostream& projOutput, std::ostream* setOutput) const;

	// stored in project template

	void	ReadTemplate(std::istream& input, const JFileVersion tmplVers,
						 const JFileVersion projVers, const MakefileMethod method,
						 const JString& targetName, const JString& depListExpr);
	void	WriteTemplate(std::ostream& output) const;

	// called by ProjectDocument

	void	ProjectChanged(const ProjectNode* node = nullptr);

	// called by CommandManager

	void	ConvertCompileDialog(std::istream& input, const JFileVersion vers);

	// called by NewProjectSaveFileDialog

	static JString	GetMakefileMethodName(const MakefileMethod method);

	static JString	GetMakeFilesName(const JString& path);
	static JString	GetCMakeInputName(const JString& path, const JString& projName);
	static JString	GetQMakeInputName(const JString& path, const JString& projName);
	static void		GetMakefileNames(const JString& path, JPtrArray<JString>* list);

	// called by ProjectConfigDialog

	static bool	UpdateMakeDependCmd(const MakefileMethod oldMethod,
									const MakefileMethod newMethod,
									JString* cmd);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	struct ModTime
	{
		time_t	makeHeaderModTime;
		time_t	makeFilesModTime;
		time_t	cmakeHeaderModTime;
		time_t	cmakeInputModTime;
		time_t	qmakeHeaderModTime;
		time_t	qmakeInputModTime;

		ModTime()
			:
			makeHeaderModTime(0), makeFilesModTime(0),
			cmakeHeaderModTime(0), cmakeInputModTime(0),
			qmakeHeaderModTime(0), qmakeInputModTime(0)
		{ };
	};

private:

	MakefileMethod	itsMakefileMethod;
	bool			itsNeedWriteMakeFilesFlag;	// true => Make.files changed
	JString			itsTargetName;				// name of target in Make.files
	JString			itsDepListExpr;				// "literal:" at end of Make.files
	JString			itsSubProjectBuildCmd;
	JString			itsMakePath;				// path where we last looked for make files
	ModTime			itsModTime;					// last success
	ModTime			itsNewModTime;				// when process started
	time_t			itsLastMakefileUpdateTime;

	ProjectDocument*		itsProjDoc;			// owns us
	Command*				itsMakeDependCmd;	// not owned; nullptr unless running
	ProjectConfigDialog*	itsProjectConfigDialog;

	static bool	itsRebuildMakefileDailyFlag;

private:

	void	UpdateProjectConfig();

	void	PrintTargetName(std::ostream& output) const;
	bool	SaveOpenFile(const JString& fileName);

	void	CreateMakemakeFiles(const JString& makeHeaderText,
								const JString& makeFilesText,
								const bool readingTemplate);
	void	CreateCMakeFiles(const JString& cmakeHeaderText,
							 const bool readingTemplate);
	void	CreateQMakeFiles(const JString& qmakeHeaderText,
							 const bool readingTemplate);
	void	RecreateMakeHeaderFile();

	bool	WriteSubProjectBuildFile(const bool reportError);

	void	UpdateMakeHeader(const JString& fileName,
							 const JPtrArray<JString>& libFileList,
							 const JPtrArray<JString>& libProjPathList) const;
	void	UpdateMakeFiles(const JString& fileName, const JString& text) const;

	void	WriteCMakeInput(const JString& inputFileName,
							const JString& src, const JString& hdr,
							const JString& outputFileName) const;
	void	WriteQMakeInput(const JString& inputFileName,
							const JString& src, const JString& hdr,
							const JString& outputFileName) const;

	bool	MakeFilesChanged() const;
	bool	CMakeHeaderChanged() const;
	bool	QMakeHeaderChanged() const;
	ModTime	SaveMakeFileModTimes();
	bool	MakefileExists() const;

	void	GetMakefileNames(JPtrArray<JString>* list) const;
	void	GetMakemakeFileNames(JString* makeHeaderName,
								 JString* makeFilesName) const;
	void	GetCMakeFileNames(JString* cmakeHeaderName,
							  JString* cmakeInputName) const;
	void	GetQMakeFileNames(JString* qmakeHeaderName,
							  JString* qmakeInputName) const;

	void	ReadSetup(std::istream& projInput, const JFileVersion projVers,
					  std::istream* setInput, const JFileVersion setVers,
					  const JString& projPath);

	// not allowed

	BuildManager(const BuildManager&) = delete;
	BuildManager& operator=(const BuildManager&) = delete;
};

std::istream& operator>>(std::istream& input, BuildManager::MakefileMethod& method);
std::ostream& operator<<(std::ostream& output, const BuildManager::MakefileMethod method);


/******************************************************************************
 Build target name

 ******************************************************************************/

inline const JString&
BuildManager::GetBuildTargetName()
	const
{
	return itsTargetName;
}

inline void
BuildManager::SetBuildTargetName
	(
	const JString& targetName
	)
{
	itsTargetName = targetName;
}

/******************************************************************************
 Build target name

 ******************************************************************************/

inline BuildManager::MakefileMethod
BuildManager::GetMakefileMethod()
	const
{
	return itsMakefileMethod;
}

/******************************************************************************
 Reopen text files when open project (static)

 ******************************************************************************/

inline bool
BuildManager::WillRebuildMakefileDaily()
{
	return itsRebuildMakefileDailyFlag;
}

inline void
BuildManager::ShouldRebuildMakefileDaily
	(
	const bool rebuild
	)
{
	itsRebuildMakefileDailyFlag = rebuild;
}

#endif

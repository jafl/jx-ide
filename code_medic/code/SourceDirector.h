/******************************************************************************
 SourceDirector.h

	Interface for the SourceDirector class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_SourceDirector
#define _H_SourceDirector

#include <jx-af/jx/JXWindowDirector.h>
#include "Location.h"

class JXMenuBar;
class JXTextMenu;
class JXFileNameDisplay;
class JXToolBar;
class Link;
class CommandDirector;
class LineIndexTable;
class SourceText;
class DisplaySourceForMainCmd;
class GetAssemblyCmd;
class FunctionMenu;

class SourceDirector : public JXWindowDirector
{
public:

	enum Type
	{
		kSourceType,
		kMainSourceType,
		kMainAsmType,
		kAsmType
	};

public:

	SourceDirector(CommandDirector* commandDir, const Type type);

	static SourceDirector*	Create(CommandDirector* commandDir,
									   const JString& fileOrFn,
									   const Type type);

	void	CreateWindowsMenu();

	~SourceDirector() override;

	CommandDirector*	GetCommandDirector();

	bool	IsMainSourceWindow() const;
	bool	GetFileName(const JString** fileName) const;
	JIndex	GetCurrentExecLine() const;

	bool	GetFunctionName(const JString** fnName) const;

	void	DisplayFile(const JString& fileName, const JIndex lineNumber = 0,
						const bool markLine = true);
	void	DisplayDisassembly(const Location& loc);
	void	DisplayLine(const JSize lineNumber, const bool markLine = true);
	void	ClearDisplay();

	Type			GetType() const;
	const JString&	GetName() const override;
	bool			GetMenuIcon(const JXImage** icon) const override;

	// called by GDBGetAssemblyCmd

	const Location&	GetDisassemblyLocation() const;
	void			DisplayDisassembly(JPtrArray<JString>* addrList,
									   const JString& instText);

protected:

	SourceDirector(CommandDirector* commandDir, const JString& fileOrFn,
					 const Type type);

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	Link*				itsLink;
	CommandDirector*	itsCommandDir;
	const Type			itsType;
	LineIndexTable*		itsTable;
	SourceText*			itsText;
	JString				itsCurrentFile;			// can be empty
	JString				itsCurrentFn;			// can be empty
	Location			itsAsmLocation;			// used while waiting for GetAssemblyCmd
	JString				itsWindowTitlePrefix;	// only used by main src window

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsDebugMenu;
	FunctionMenu*	itsFnMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

	DisplaySourceForMainCmd*	itsSrcMainCmd;
	GetAssemblyCmd*				itsGetAssemblyCmd;	// can be nullptr

// begin JXLayout

	JXMenuBar*         itsMenuBar;
	JXToolBar*         itsToolBar;
	JXFileNameDisplay* itsFileDisplay;

// end JXLayout

private:

	void	SourceViewDirectorX(CommandDirector* commandDir);
	void	BuildWindow();
	void	UpdateFileType();
	void	UpdateWindowTitle(const JString& binaryName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);
};


/******************************************************************************
 GetCommandDirector

 ******************************************************************************/

inline CommandDirector*
SourceDirector::GetCommandDirector()
{
	return itsCommandDir;
}

/******************************************************************************
 GetType

 ******************************************************************************/

inline SourceDirector::Type
SourceDirector::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 IsMainSourceWindow

 ******************************************************************************/

inline bool
SourceDirector::IsMainSourceWindow()
	const
{
	return itsType != kSourceType && itsType != kAsmType;
}

/******************************************************************************
 GetFileName

 ******************************************************************************/

inline bool
SourceDirector::GetFileName
	(
	const JString** fileName
	)
	const
{
	*fileName = &itsCurrentFile;
	return !itsCurrentFile.IsEmpty();
}

/******************************************************************************
 GetFunctionName

 ******************************************************************************/

inline bool
SourceDirector::GetFunctionName
	(
	const JString** fnName
	)
	const
{
	*fnName = &itsCurrentFn;
	return !itsCurrentFn.IsEmpty();
}

/******************************************************************************
 GetDisassemblyLocation

 ******************************************************************************/

inline const Location&
SourceDirector::GetDisassemblyLocation()
	const
{
	return itsAsmLocation;
}

#endif

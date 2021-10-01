/******************************************************************************
 CompileDocument.h

	Interface for the CompileDocument class

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_CompileDocument
#define _H_CompileDocument

#include "ExecOutputDocument.h"

class ProjectDocument;

class CompileDocument : public ExecOutputDocument
{
public:

	CompileDocument(ProjectDocument* projDoc);

	virtual ~CompileDocument();

	virtual void	SetConnection(JProcess* p, const int inFD, const int outFD,
								  const JString& windowTitle,
								  const JString& dontCloseMsg,
								  const JString& execDir,
								  const JString& execCmd,
								  const bool showPID) override;

	virtual void	OpenPrevListItem() override;
	virtual void	OpenNextListItem() override;

	virtual void	ConvertSelectionToFullPath(JString* fileName) const override;

	static bool	WillDoubleSpace();
	static void		ShouldDoubleSpace(const bool ds);

protected:

	virtual void		AppendText(const JString& text) override;
	virtual bool	ProcessFinished(const JProcess::Finished& info) override;
	virtual bool	NeedsFormattedData() const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	ProjectDocument*	itsProjDoc;			// nullptr is allowed
	JString				itsPrevLine;
	bool			itsHasErrorsFlag;	// true => found errors
	JXTextMenu*			itsErrorMenu;		// not owned

	static bool	theDoubleSpaceFlag;

private:

	void	UpdateErrorMenu();
	void	HandleErrorMenu(const JIndex index);

	void		ShowFirstError();
	bool	ShowPrevError();
	bool	ShowNextError();

	// not allowed

	CompileDocument(const CompileDocument& source);
	const CompileDocument& operator=(const CompileDocument& source);
};


/******************************************************************************
 Double spaced output (static)

 ******************************************************************************/

inline bool
CompileDocument::WillDoubleSpace()
{
	return theDoubleSpaceFlag;
}

inline void
CompileDocument::ShouldDoubleSpace
	(
	const bool ds
	)
{
	theDoubleSpaceFlag = ds;
}

#endif

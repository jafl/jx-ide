/******************************************************************************
 DebugDir.h

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_DebugDir
#define _H_DebugDir

#include <jx-af/jx/JXWindowDirector.h>
#include <fstream>

class JXTextButton;
class JXStaticText;
class Link;

class DebugDir : public JXWindowDirector
{
public:

	DebugDir(JXDirector* supervisor);

	virtual ~DebugDir();

	virtual const JString&	GetName() const override;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	Link*			itsLink;
	JXStaticText*	itsText;
	std::ofstream	itsFile;

// begin JXLayout

	JXTextButton* itsCopyButton;

// end JXLayout

private:

	void	BuildWindow();
};

#endif

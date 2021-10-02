/******************************************************************************
 RegistersDir.h

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#ifndef _H_RegistersDir
#define _H_RegistersDir

#include <jx-af/jx/JXWindowDirector.h>

class JXTextMenu;
class JXStaticText;
class GetRegisters;
class CommandDirector;

class RegistersDir : public JXWindowDirector
{
public:

	RegistersDir(CommandDirector* supervisor);

	virtual	~RegistersDir();

	virtual void			Activate() override;
	virtual bool			Deactivate() override;
	virtual const JString&	GetName() const override;
	virtual bool			GetMenuIcon(const JXImage** icon) const override;

	void	Update(const JString& data);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CommandDirector*	itsCommandDir;
	JXStaticText*		itsWidget;
	GetRegisters*		itsCmd;			// can be nullptr
	bool				itsShouldUpdateFlag;
	bool				itsNeedsUpdateFlag;

	JXTextMenu*			itsFileMenu;
	JXTextMenu*			itsHelpMenu;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();
	void	UpdateWindowTitle(const JString& binaryName);
	void	Update();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);
};

#endif

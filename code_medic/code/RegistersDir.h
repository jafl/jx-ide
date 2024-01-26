/******************************************************************************
 RegistersDir.h

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#ifndef _H_RegistersDir
#define _H_RegistersDir

#include <jx-af/jx/JXWindowDirector.h>

class JXTextMenu;
class JXStaticText;
class GetRegistersCmd;
class CommandDirector;

class RegistersDir : public JXWindowDirector
{
public:

	RegistersDir(CommandDirector* supervisor);

	~RegistersDir() override;

	void			Activate() override;
	bool			Deactivate() override;
	const JString&	GetName() const override;
	bool			GetMenuIcon(const JXImage** icon) const override;

	void	Update(const JString& data);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CommandDirector*	itsCommandDir;
	GetRegistersCmd*	itsCmd;			// can be nullptr
	bool				itsShouldUpdateFlag;
	bool				itsNeedsUpdateFlag;

	JXTextMenu*			itsFileMenu;

// begin JXLayout

	JXStaticText* itsWidget;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateWindowTitle(const JString& binaryName);
	void	Update();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);
};

#endif

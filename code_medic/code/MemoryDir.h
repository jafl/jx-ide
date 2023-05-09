/******************************************************************************
 MemoryDir.h

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#ifndef _H_MemoryDir
#define _H_MemoryDir

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JString.h>

class JXTextButton;
class JXTextMenu;
class JXStaticText;
class Link;
class GetMemoryCmd;
class ArrayExprInput;
class ArrayIndexInput;
class CommandDirector;

class MemoryDir : public JXWindowDirector
{
public:

	// remember to sync itsDisplayTypeMenu and *GetMemoryCmd::kCommandName

	enum DisplayType
	{
		kHexByte = 1,
		kHexShort,
		kHexWord,
		kHexLong,
		kChar,
		kAsm
	};

public:

	MemoryDir(CommandDirector* supervisor, const JString& expr);
	MemoryDir(std::istream& input, const JFileVersion vers,
				CommandDirector* supervisor);

	~MemoryDir() override;

	void	Activate() override;
	bool	Deactivate() override;
	bool	GetMenuIcon(const JXImage** icon) const override;

	void	StreamOut(std::ostream& output);

	const JString&	GetExpression(DisplayType* type, JSize* count);
	void			SetDisplayType(const DisplayType type);
	void			Update(const JString& data);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	Link*				itsLink;
	CommandDirector*	itsCommandDir;
	JXStaticText*		itsWidget;
	JString				itsExpr;
	DisplayType			itsDisplayType;
	JSize				itsItemCount;
	GetMemoryCmd*		itsCmd;
	bool				itsShouldUpdateFlag;
	bool				itsNeedsUpdateFlag;
	bool				itsWaitingForReloadFlag;

	JXTextMenu*			itsFileMenu;
	JXTextMenu*			itsActionMenu;

// begin JXLayout

	ArrayExprInput*  itsExprInput;
	JXTextMenu*      itsDisplayTypeMenu;
	ArrayIndexInput* itsItemCountInput;

// end JXLayout

private:

	void	MemoryDirX(CommandDirector* supervisor);
	void	BuildWindow();
	void	UpdateWindowTitle();
	void	UpdateDisplayTypeMenu();
	void	Update();

	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);
};


/******************************************************************************
 GetExpression

 *****************************************************************************/

inline const JString&
MemoryDir::GetExpression
	(
	DisplayType*	type,
	JSize*			count
	)
{
	*type  = itsDisplayType;
	*count = itsItemCount;
	return itsExpr;
}

#endif

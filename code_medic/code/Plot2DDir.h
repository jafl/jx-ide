/******************************************************************************
 Plot2DDir.h

	Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#ifndef _H_Plot2DDir
#define _H_Plot2DDir

#include <jx-af/jx/JXWindowDirector.h>

class JStringTableData;
class JXTextButton;
class JXTextMenu;
class JXColHeaderWidget;
class JX2DPlotWidget;
class Link;
class CommandDirector;
class Plot2DExprTable;
class Plot2DCmd;

class Plot2DDir : public JXWindowDirector
{
public:

	Plot2DDir(CommandDirector* supervisor, const JString& expr);
	Plot2DDir(std::istream& input, const JFileVersion vers,
				CommandDirector* supervisor);

	virtual	~Plot2DDir();

	void		Activate() override;
	bool	GetMenuIcon(const JXImage** icon) const override;

	void	StreamOut(std::ostream& output);

	JString	GetXExpression(const JIndex curveIndex, const JInteger i) const;
	JString	GetYExpression(const JIndex curveIndex, const JInteger i) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	Link*						itsLink;
	CommandDirector*			itsCommandDir;
	JXColHeaderWidget*			itsColHeader;
	JStringTableData*			itsExprData;
	Plot2DExprTable*			itsExprTable;
	bool						itsShouldUpdateFlag;
	JPtrArray<Plot2DCmd>*	itsUpdateCmdList;
	JPtrArray<JArray<JFloat> >*	itsXData;
	JPtrArray<JArray<JFloat> >*	itsYData;
	bool						itsWaitingForReloadFlag;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsActionMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout

	JX2DPlotWidget* itsPlotWidget;
	JXTextButton*   itsAddPlotButton;
	JXTextButton*   itsDuplicatePlotButton;
	JXTextButton*   itsRemovePlotButton;

// end JXLayout

private:

	void	Plot2DDirX1(CommandDirector* supervisor);
	void	Plot2DDirX2();
	void	BuildWindow();
	void	UpdateWindowTitle();
	void	UpdateButtons();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	void	ShouldUpdate(const bool update);
	void	UpdateAll();
	void	Update(const JIndex first, const JIndex last);

	void	DuplicateSelectedPlots();
	void	RemoveSelectedPlots();
};

#endif

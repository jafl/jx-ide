/******************************************************************************
 TECaretInputBase.h

	Interface for the TECaretInputBase class

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_TECaretInputBase
#define _H_TECaretInputBase

#include <jx-af/jx/JXIntegerInput.h>

class JXStaticText;

class TECaretInputBase : public JXIntegerInput
{
public:

	TECaretInputBase(JXStaticText* label, JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	~TECaretInputBase() override;

	JXTEBase*	GetTE();
	void		SetTE(JXTEBase* te);
	void		ShouldOptimizeUpdate(const bool optimize);

	void	HandleKeyPress(const JUtf8Character& c,
						   const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	void			ShouldAct(const bool act);
	virtual void	Act(JXTEBase* te, const JIndex value) = 0;
	virtual JIndex	GetValue(JXTEBase* te) const = 0;
	virtual JIndex	GetValue(const JTextEditor::CaretLocationChanged& info) const = 0;

	void		BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	JCoordinate	GetFTCMinContentSize(const bool horizontal) const override;

	void	HandleFocusEvent() override;
	void	HandleUnfocusEvent() override;
	bool	OKToUnfocus() override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTEBase*	itsTE;				// not owned
	JInteger	itsOrigValue;
	bool		itsShouldActFlag;
	bool		itsOptimizeUpdateFlag;

	JXStaticText*	itsLabel;		// not owned

private:

	void	Center();
};


/******************************************************************************
 GetTE

 ******************************************************************************/

inline JXTEBase*
TECaretInputBase::GetTE()
{
	return itsTE;
}

/******************************************************************************
 ShouldOptimizeUpdate

 ******************************************************************************/

inline void
TECaretInputBase::ShouldOptimizeUpdate
	(
	const bool optimize
	)
{
	itsOptimizeUpdateFlag = optimize;
}

/******************************************************************************
 ShouldAct (protected)

 ******************************************************************************/

inline void
TECaretInputBase::ShouldAct
	(
	const bool act
	)
{
	itsShouldActFlag = act;
}

#endif

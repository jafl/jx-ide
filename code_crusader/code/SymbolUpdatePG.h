/******************************************************************************
 SymbolUpdatePG.h

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_SymbolUpdatePG
#define _H_SymbolUpdatePG

#include <jx-af/jcore/JProgressDisplay.h>

class SymbolUpdatePG : public JProgressDisplay
{
public:

	SymbolUpdatePG(std::ostream& link, const JSize scaleFactor);

	virtual ~SymbolUpdatePG();

	virtual bool	IncrementProgress(const JString& message = JString::empty) override;
	virtual bool	IncrementProgress(const JSize delta) override;
	virtual bool	IncrementProgress(const JString& message, const JSize delta) override;
	virtual void		ProcessFinished() override;
	virtual void		DisplayBusyCursor() override;

	void	SetScaleFactor(const JSize scaleFactor);

protected:

	virtual void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
									 const JString& message, const bool allowCancel,
									 const bool allowBackground) override;

	virtual bool	CheckForCancel() override;

private:

	std::ostream&	itsLink;
	JSize			itsScaleFactor;

private:

	// not allowed

	SymbolUpdatePG(const SymbolUpdatePG& source);
	const SymbolUpdatePG& operator=(const SymbolUpdatePG& source);
};


/******************************************************************************
 SetScaleFactor

 ******************************************************************************/

inline void
SymbolUpdatePG::SetScaleFactor
	(
	const JSize scaleFactor
	)
{
	itsScaleFactor = scaleFactor;
}

#endif

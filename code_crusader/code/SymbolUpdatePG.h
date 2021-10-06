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

	bool	IncrementProgress(const JString& message = JString::empty) override;
	bool	IncrementProgress(const JSize delta) override;
	bool	IncrementProgress(const JString& message, const JSize delta) override;
	void		ProcessFinished() override;
	void		DisplayBusyCursor() override;

	void	SetScaleFactor(const JSize scaleFactor);

protected:

	void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
									 const JString& message, const bool allowCancel,
									 const bool allowBackground) override;

	bool	CheckForCancel() override;

private:

	std::ostream&	itsLink;
	JSize			itsScaleFactor;
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

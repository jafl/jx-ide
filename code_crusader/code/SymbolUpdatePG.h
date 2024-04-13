/******************************************************************************
 SymbolUpdatePG.h

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_SymbolUpdatePG
#define _H_SymbolUpdatePG

#include <jx-af/jcore/JLatentPG.h>

class JXContainer;

class SymbolUpdatePG : public JLatentPG
{
public:

	SymbolUpdatePG(JProgressDisplay* pg, const JSize scaleFactor,
					JXContainer* widget, JXContainer* container);

	~SymbolUpdatePG() override;

	bool	ProcessContinuing() override;
	void	ProcessFinished() override;

	void	Cancel();
	void	Hide();

protected:

	void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
							 const JString& message, const bool allowCancel,
							 const bool modal) override;

private:

	JXContainer*	itsWidget;
	JXContainer*	itsContainer;
	bool			itsCancelFlag;
};


/******************************************************************************
 Cancel

 ******************************************************************************/

inline void
SymbolUpdatePG::Cancel()
{
	itsCancelFlag = true;
}

#endif

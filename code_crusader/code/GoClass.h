/******************************************************************************
 GoClass.h

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#ifndef _H_GoClass
#define _H_GoClass

#include "Class.h"

class GoClass : public Class
{
public:

	GoClass(const JString& name, const DeclareType declType,
			  const JFAID_t fileID, Tree* tree,
			  const bool isPublic);
	GoClass(std::istream& input, const JFileVersion vers, Tree* tree);

	virtual ~GoClass();

	virtual void	ViewSource() const override;
	virtual void	ViewHeader() const override;

	virtual void	StreamOut(std::ostream& output) const override;

protected:

	virtual Class*	NewGhost(const JString& name, Tree* tree) override;

	virtual void	AdjustNameStyle(JFontStyle* style) const override;

private:

	bool	itsIsPublicFlag;
};

#endif

/******************************************************************************
 PHPClass.h

	Copyright © 2014 John Lindal.

 ******************************************************************************/

#ifndef _H_PHPClass
#define _H_PHPClass

#include "Class.h"

class PHPClass : public Class
{
public:

	PHPClass(const JString& name, const DeclareType declType,
			   const JFAID_t fileID, Tree* tree,
			   const bool isFinal);
	PHPClass(std::istream& input, const JFileVersion vers, Tree* tree);

	virtual ~PHPClass();

	virtual void	ViewSource() const override;
	virtual void	ViewHeader() const override;

	virtual void	StreamOut(std::ostream& output) const override;

protected:

	virtual Class*	NewGhost(const JString& name, Tree* tree) override;

	virtual void	AdjustNameStyle(JFontStyle* style) const override;

private:

	bool	itsIsFinalFlag;
};

#endif

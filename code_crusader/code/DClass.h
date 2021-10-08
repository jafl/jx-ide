/******************************************************************************
 DClass.h

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#ifndef _H_DClass
#define _H_DClass

#include "Class.h"

class DClass : public Class
{
public:

	DClass(const JString& name, const DeclareType declType,
			 const JFAID_t fileID, Tree* tree, const bool isFinal);
	DClass(std::istream& input, const JFileVersion vers, Tree* tree);

	~DClass() override;

	void	ViewSource() const override;
	void	ViewHeader() const override;

	void	StreamOut(std::ostream& output) const override;

protected:

	Class*	NewGhost(const JString& name, Tree* tree) override;

	void	AdjustNameStyle(JFontStyle* style) const override;

private:

	bool	itsIsFinalFlag;
};

#endif

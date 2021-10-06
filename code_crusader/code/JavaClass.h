/******************************************************************************
 JavaClass.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_JavaClass
#define _H_JavaClass

#include "Class.h"

class JavaClass : public Class
{
public:

	JavaClass(const JString& name, const DeclareType declType,
				const JFAID_t fileID, Tree* tree,
				const bool isPublic, const bool isFinal);
	JavaClass(std::istream& input, const JFileVersion vers, Tree* tree);

	virtual ~JavaClass();

	void	ViewSource() const override;
	void	ViewHeader() const override;

	void	StreamOut(std::ostream& output) const override;

protected:

	Class*	NewGhost(const JString& name, Tree* tree) override;

	void	AdjustNameStyle(JFontStyle* style) const override;

private:

	bool	itsIsPublicFlag;
	bool	itsIsFinalFlag;
};

#endif

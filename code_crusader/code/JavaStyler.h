/******************************************************************************
 JavaStyler.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JavaStyler
#define _H_JavaStyler

#include "StylerBase.h"
#include "JavaScanner.h"

class JavaStyler : public StylerBase, public TextScanner::Java::Scanner
{
public:

	static StylerBase*	Instance();
	static void				Shutdown();

	virtual ~JavaStyler();

protected:

	JavaStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	static JavaStyler*	itsSelf;
};

#endif

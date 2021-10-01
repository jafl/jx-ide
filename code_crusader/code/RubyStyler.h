/******************************************************************************
 RubyStyler.h

	Copyright © 2003 by John Lindal.

 ******************************************************************************/

#ifndef _H_RubyStyler
#define _H_RubyStyler

#include "StylerBase.h"
#include "RubyScanner.h"

class RubyStyler : public StylerBase, public TextScanner::Ruby::Scanner
{
public:

	static StylerBase*	Instance();
	static void				Shutdown();

	virtual ~RubyStyler();

protected:

	RubyStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static RubyStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JStyledText::TextRange& tokenRange);
	void	StyleEmbeddedVariables(const Token& token);
};

#endif

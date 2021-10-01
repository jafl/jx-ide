/******************************************************************************
 JavaScriptStyler.h

	Copyright © 2006 by John Lindal.

 ******************************************************************************/

#ifndef _H_JavaScriptStyler
#define _H_JavaScriptStyler

#include "StylerBase.h"
#include "JavaScriptScanner.h"

class JavaScriptStyler : public StylerBase, public TextScanner::JavaScript::Scanner
{
public:

	static StylerBase*	Instance();
	static void				Shutdown();

	virtual ~JavaScriptStyler();

protected:

	JavaScriptStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static JavaScriptStyler*	itsSelf;

private:

	void	StyleEmbeddedVariables(const Token& token);
};

#endif

/******************************************************************************
 HTMLStyler.h

	Copyright © 2001-2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_HTMLStyler
#define _H_HTMLStyler

#include "StylerBase.h"
#include "HTMLScanner.h"

class HTMLStyler : public StylerBase, public TextScanner::HTML::Scanner
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();

	~HTMLStyler();

protected:

	HTMLStyler();

	void	Scan(const JStyledText::TextIndex& startIndex,
				 std::istream& input, const TokenExtra& initData) override;
	void	UpgradeTypeList(const JFileVersion vers,
							JArray<JFontStyle>* typeStyles) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

	const JString&	GetScannedText() const override;

private:

	JString	itsLatestTagName;

	static HTMLStyler*	itsSelf;

private:

	void		InitMustacheTypeStyles();
	void		InitPHPTypeStyles();
	void		InitJSPTypeStyles();
	void		InitJavaScriptTypeStyles();
	void		ExtendCheckRangeForString(const JCharacterRange& tokenRange);
	void		ExtendCheckRangeForLanguageStartEnd(const TokenType tokenType, const JCharacterRange& tokenRange);
	JFontStyle	GetTagStyle(const JUtf8ByteRange& tokenRange, const JIndex typeIndex);
	bool		GetXMLStyle(const JString& tagName, JFontStyle* style);
	void		StyleEmbeddedPHPVariables(const Token& token);
	void		StyleEmbeddedJSVariables(const Token& token);

	static JCharacterRange	MatchAt(const Token& token, JStringIterator& iter,
									const JRegex& pattern);
};

#endif

/******************************************************************************
 CStyler.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CStyler
#define _H_CStyler

#include "StylerBase.h"
#include "CScanner.h"

class CStyler : public StylerBase, public TextScanner::C::Scanner
{
public:

	static StylerBase*	Instance();
	static void				Shutdown();

	virtual ~CStyler();

protected:

	CStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	PreexpandCheckRange(const JString& text,
										const JRunArray<JFont>& styles,
										const JCharacterRange& modifiedRange,
										const bool deletion,
										JStyledText::TextRange* checkRange) override;

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	static CStyler*	itsSelf;

private:

	bool	SlurpPPComment(JStyledText::TextRange* totalRange);
};

#endif

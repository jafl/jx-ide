/******************************************************************************
 DStyler.h

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#ifndef _H_DStyler
#define _H_DStyler

#include "StylerBase.h"
#include "DScanner.h"

class DStyler : public StylerBase, public TextScanner::D::Scanner
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();

	~DStyler() override;

protected:

	DStyler();

	void	Scan(const JStyledText::TextIndex& startIndex,
				 std::istream& input, const TokenExtra& initData) override;

	void	UpgradeTypeList(const JFileVersion vers,
							JArray<JFontStyle>* typeStyles) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	static DStyler*	itsSelf;
};

#endif

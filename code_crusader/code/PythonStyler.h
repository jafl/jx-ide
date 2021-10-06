/******************************************************************************
 PythonStyler.h

	Copyright © 2004 by John Lindal.

 ******************************************************************************/

#ifndef _H_PythonStyler
#define _H_PythonStyler

#include "StylerBase.h"
#include "PythonScanner.h"

class PythonStyler : public StylerBase, public TextScanner::Python::Scanner
{
public:

	static StylerBase*	Instance();
	static void				Shutdown();

	virtual ~PythonStyler();

protected:

	PythonStyler();

	void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static PythonStyler*	itsSelf;
};

#endif

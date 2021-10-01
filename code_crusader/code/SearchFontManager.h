/******************************************************************************
 SearchFontManager.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_SearchFontManager
#define _H_SearchFontManager

#include <jx-af/jcore/JFontManager.h>

class SearchFontManager : public JFontManager
{
public:

	SearchFontManager();

	virtual ~SearchFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames) override;
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) override;
	virtual bool	GetFontSizes(const JString& name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) override;

protected:

	virtual JSize	GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) override;

	virtual JSize	GetCharWidth(const JFontID fontID, const JUtf8Character& c) override;
	virtual JSize	GetStringWidth(const JFontID fontID, const JString& str) override;

	virtual bool	IsExact(const JFontID id) override;
	virtual bool	HasGlyphForCharacter(const JFontID id, const JUtf8Character& c) override;
	virtual bool	GetSubstituteFontName(const JFont& f, const JUtf8Character& c, JString* name) override;

private:

	// not allowed

	SearchFontManager(const SearchFontManager& source);
	const SearchFontManager& operator=(const SearchFontManager& source);
};

#endif

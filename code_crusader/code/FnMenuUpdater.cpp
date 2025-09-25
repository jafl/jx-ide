/******************************************************************************
 FnMenuUpdater.cpp

	Uses ctags to extract function names from a source file and build
	a menu to allow the user to jump to their definitions.

	BASE CLASS = JPrefObject, CtagsUser

	Copyright © 1999-2001 by John Lindal.

 ******************************************************************************/

#include "FnMenuUpdater.h"
#include "sharedUtil.h"
#include "ctagsRegex.h"
#include "globals.h"
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/jXConstants.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// sort=no requires so qualified tag comes after unqualified version

static const JUtf8Byte* kCtagsArgs =
	"--format=2 --excmd=number --sort=no --extras=+qr --fields=+r "
	"--ant-kinds=t "
	"--asm-kinds=l "
	"--asp-kinds=fs "
	"--awk-kinds=f "
	"--beta-kinds=f "
	CtagsBisonDef
	CtagsBisonNonterminalDef
	"--c-kinds=f "
	"--cobol-kinds=p "
	"--eiffel-kinds=f "
	"--fortran-kinds=psfl "
	"--html-kinds=I --css-kinds= "
	"--java-kinds=m "
	"--lex-types=c "
	"--lisp-kinds=f "
	"--lua-kinds=f "
	"--make-kinds=t "
	"--pascal-kinds=fp "
	"--perl-kinds=s --pod-kinds= "
	"--php-kinds=f "
	"--python-kinds=f "
	"--rexx-kinds=s "
	"--ruby-kinds=f "
	"--scheme-kinds=f "
	"--sh-kinds=f "
	"--slang-kinds=f "
	"--sql-kinds=fptvT "
	"--tcl-kinds=p "
	"--vera-kinds=f "
	"--verilog-kinds=f "
	"--vim-kinds=f "
	"--c#-kinds=m "
	"--erlang-kinds=f "
	"--sml-kinds=fc "
	"--typescript-kinds=fm "
	"--basic-kinds=f "
	"--matlab-kinds=f "
	"--flex-kinds=fm ";

// setup information

const JFileVersion kCurrentSetupVersion = 2;

// version  2 adds itsIncludeNSFlag
// version  1 adds itsPackFlag

/******************************************************************************
 Constructor

 ******************************************************************************/

FnMenuUpdater::FnMenuUpdater()
	:
	JPrefObject(GetPrefsManager(), kFnMenuUpdaterID),
	CtagsUser(kCtagsArgs)
{
	itsSortFlag      = true;
	itsIncludeNSFlag = false;
	itsPackFlag      = false;
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FnMenuUpdater::~FnMenuUpdater()
{
}

/******************************************************************************
 UpdateMenu

 ******************************************************************************/

void
FnMenuUpdater::UpdateMenu
	(
	const JString&		fileName,
	const TextFileType	fileType,
	const bool			sort,
	const bool			includeNS,
	const bool			pack,
	JXTextMenu*			menu,
	JArray<JIndex>*		lineIndexList,
	JArray<Language>*	lineLangList
	)
{
	menu->RemoveAllItems();

	JPtrArray<JString> fnNameList(JPtrArrayT::kDeleteAll);
	fnNameList.SetCompareFunction(JCompareStringsCaseInsensitive);
	fnNameList.SetSortOrder(JListT::kSortAscending);

	lineIndexList->RemoveAll();
	lineIndexList->SetCompareFunction(std::function([](const JIndex& i1, const JIndex& i2)
	{
		return i1 <=> i2;
	}));
	lineIndexList->SetSortOrder(JListT::kSortAscending);

	lineLangList->RemoveAll();

	if (pack)
	{
		menu->SetDefaultFontSize(JFontManager::GetDefaultFontSize()-2, false);
		menu->CompressHeight(true);
	}
	else
	{
		menu->SetDefaultFontSize(JFontManager::GetDefaultFontSize(), false);
		menu->CompressHeight(false);
	}

	JString data;
	Language lang;
	if (ProcessFile(fileName, fileType, &data, &lang))
	{
		icharbufstream input(data.GetRawBytes(), data.GetByteCount());
		ReadFunctionList(input, GetLanguage(fileType), sort, includeNS,
						 &fnNameList, lineIndexList, lineLangList);
	}

	// build menu

	for (auto* s : fnNameList)
	{
		menu->AppendItem(*s);
	}
}

/******************************************************************************
 ReadFunctionList (private)

 ******************************************************************************/

void
FnMenuUpdater::ReadFunctionList
	(
	std::istream&		input,
	const Language		lang,
	const bool			sort,
	const bool			includeNS,
	JPtrArray<JString>*	fnNameList,
	JArray<JIndex>*		lineIndexList,
	JArray<Language>*	lineLangList
	)
{
	// build symbol list

	const bool hasNS = HasNamespace(lang);
	JString fnName;
	JStringPtrMap<JString> flags(JPtrArrayT::kDeleteAll);
	while (true)
	{
		input >> std::ws;
		fnName = JReadUntil(input, '\t');	// fn name
		if (input.eof() || input.fail())
		{
			break;
		}

		JIgnoreUntil(input, '\t');			// file name

		JIndex lineIndex;
		input >> lineIndex;					// line index

		ReadExtensionFlags(input, &flags);

		if (IgnoreSymbol(fnName))
		{
			continue;
		}

		// only keep grouping tags for lex

		JString *type, *roles;
		if (!flags.GetItem("kind", &type) ||
			(lang == kLexLang && *type == "c" &&
			 flags.GetItem("roles", &roles) && !roles->Contains("grouping")))
		{
			continue;
		}

		// toss qualified or unqualified version, except for HTML id's

		if (*type == "I")
		{
			fnName.Prepend("#");
		}
		else if (hasNS && !includeNS && NameIsQualified(fnName))
		{
			continue;
		}

		// save symbol

		JIndex i;
		if (sort)
		{
			i = fnNameList->GetInsertionSortIndex(&fnName);
		}
		else
		{
			i = lineIndexList->GetInsertionSortIndex(lineIndex);
		}
		fnNameList->InsertAtIndex(i, fnName);
		lineIndexList->InsertItemAtIndex(i, lineIndex);
		lineLangList->InsertItemAtIndex(i, lang);
	}

	// filter

	if (hasNS && includeNS)
	{
		JSize count = fnNameList->GetItemCount();
		for (JIndex i=1; i<=count; i++)
		{
			const JString* fnName = fnNameList->GetItem(i);
			if (NameIsQualified(*fnName))
			{
				const JIndex lineIndex = lineIndexList->GetItem(i);
				for (JIndex j=1; j<=count; j++)
				{
					if (j != i && lineIndexList->GetItem(j) == lineIndex)
					{
						fnNameList->DeleteItem(j);
						lineIndexList->RemoveItem(j);
						lineLangList->RemoveItem(j);
						count--;
						if (j < i)
						{
							i--;
						}
						break;
					}
				}
			}
		}
	}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
FnMenuUpdater::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
	{
		input >> JBoolFromString(itsSortFlag);

		if (vers >= 2)
		{
			input >> JBoolFromString(itsIncludeNSFlag);
		}

		if (vers >= 1)
		{
			input >> JBoolFromString(itsPackFlag);
		}
	}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
FnMenuUpdater::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;
	output << ' ' << JBoolToString(itsSortFlag);
	output << ' ' << JBoolToString(itsIncludeNSFlag);
	output << ' ' << JBoolToString(itsPackFlag);
}

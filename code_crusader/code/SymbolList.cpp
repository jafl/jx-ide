/******************************************************************************
 SymbolList.cpp

	Uses ctags to maintain a list of symbols for a project.

	BASE CLASS = JContainer, CtagsUser

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "SymbolList.h"
#include "ProjectDocument.h"
#include "FileListTable.h"
#include "CTreeDirector.h"
#include "CTree.h"
#include "CPreprocessor.h"
#include "globals.h"
#include "ctagsRegex.h"
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kCtagsArgs =
	"--format=2 --excmd=number --sort=no --extras=+q "
	"--c-kinds=+p --php-kinds=-v --ant-kinds=t --javascript-kinds=cfgmGS "
	"--html-kinds=I --css-kinds= --make-kinds=tm --pod-kinds= "
	CtagsBisonDef
	CtagsBisonNonterminalDef
	CtagsBisonNonterminalDecl
	CtagsBisonTerminalDecl;

// JBroadcaster message types

const JUtf8Byte* SymbolList::kChanged = "Changed::SymbolList";

/******************************************************************************
 Constructor

 ******************************************************************************/

SymbolList::SymbolList
	(
	ProjectDocument* projDoc
	)
	:
	JContainer(),
	CtagsUser(kCtagsArgs)
{
	itsProjDoc                = projDoc;
	itsReparseAllFlag         = true;		// ReadSetup() clears this
	itsChangedDuringParseFlag = false;
	itsBeganEmptyFlag         = false;

	itsSymbolList = jnew JArray<SymbolInfo>(kBlockSize);
	assert( itsSymbolList != nullptr );
	itsSymbolList->SetSortOrder(JListT::kSortAscending);
	itsSymbolList->SetCompareFunction(CompareSymbols);

	InstallCollection(itsSymbolList);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SymbolList::~SymbolList()
{
	RemoveAllSymbols();
	jdelete itsSymbolList;
}

/******************************************************************************
 GetSymbol

	fullyQualifiedFileScope can be nullptr.

 ******************************************************************************/

const JString&
SymbolList::GetSymbol
	(
	const JIndex	symbolIndex,
	Language*		lang,
	Type*			type,
	bool*			fullyQualifiedFileScope
	)
	const
{
	const SymbolInfo info = itsSymbolList->GetElement(symbolIndex);
	*lang                 = info.lang;
	*type                 = info.type;

	if (fullyQualifiedFileScope != nullptr)
	{
		*fullyQualifiedFileScope = info.fullyQualifiedFileScope;
	}

	return *(info.name);
}

/******************************************************************************
 GetFile

 ******************************************************************************/

const JString&
SymbolList::GetFile
	(
	const JIndex	symbolIndex,
	JIndex*			lineIndex
	)
	const
{
	const SymbolInfo info = itsSymbolList->GetElement(symbolIndex);
	*lineIndex            = info.lineIndex;
	return (itsProjDoc->GetAllFileList())->GetFileName(info.fileID);
}

/******************************************************************************
 IsUniqueClassName

	Returns true if the given name is the name of a single class.

 ******************************************************************************/

bool
SymbolList::IsUniqueClassName
	(
	const JString&	name,
	Language*		lang
	)
	const
{
	const JSize symCount  = itsSymbolList->GetElementCount();
	const SymbolInfo* sym = itsSymbolList->GetCArray();

	bool found = false;
	for (JUnsignedOffset i=0; i<symCount; i++)
	{
		if (IsClass(sym[i].type) &&
			JString::Compare(*(sym[i].name), name, IsCaseSensitive(sym[i].lang)) == 0)
		{
			if (!found)
			{
				found = true;
				*lang = sym[i].lang;
			}
			else
			{
				return false;
			}
		}
	}

	return found;
}

/******************************************************************************
 FindSymbol

	Fills matchList with the symbol indices that match.

	If contextFileID is valid, it is used for context.
	If contextNamespace is not empty, the names are used for context.

	*** The contents of *ContextNamespaceList are altered.

 ******************************************************************************/

bool
SymbolList::FindSymbol
	(
	const JString&		name,
	const JFAID_t		contextFileID,
	const JString&		contextNamespace,
	const Language		contextLang,
	JPtrArray<JString>*	cContextNamespaceList,
	JPtrArray<JString>*	dContextNamespaceList,
	JPtrArray<JString>*	goContextNamespaceList,
	JPtrArray<JString>*	javaContextNamespaceList,
	JPtrArray<JString>*	phpContextNamespaceList,
	const bool			findDeclaration,
	const bool			findDefinition,
	JArray<JIndex>*		matchList
	)
	const
{
	// find all symbols that match

	matchList->RemoveAll();
	matchList->SetBlockSize(50);

	JArray<JIndex> allMatchList(50);

	JString s = name;
	SymbolInfo target;
	target.name = &s;
	JIndex startIndex;
	if (itsSymbolList->SearchSorted(target, JListT::kFirstMatch, &startIndex))
	{
		const JSize count = itsSymbolList->GetElementCount();
		for (JIndex i=startIndex; i<=count; i++)
		{
			const SymbolInfo info = itsSymbolList->GetElement(i);
			if (CompareSymbols(target, info) != JListT::kFirstEqualSecond)
			{
				break;
			}

			if (!IsCaseSensitive(info.lang) || *(info.name) == s)
			{
				if (contextFileID == info.fileID &&		// automatically fails if context is kInvalidID
					IsFileScope(info.type))
				{
					matchList->RemoveAll();
					matchList->AppendElement(i);
					break;
				}

				if ((findDeclaration || !IsPrototype(info.type)) &&
					(findDefinition  || !IsFunction(info.type)))
				{
					matchList->AppendElement(i);
				}
				allMatchList.AppendElement(i);
			}
		}
	}

	bool usedAllList = false;
	if (matchList->IsEmpty())
	{
		*matchList  = allMatchList;
		usedAllList = true;
	}

	// replace symbols with fully qualified versions

	if (!matchList->IsEmpty())
	{
		JString ns1, ns2;
		PrepareContextNamespace(contextNamespace, contextLang, &ns1, &ns2);
		PrepareCContextNamespaceList(cContextNamespaceList);
		PrepareDContextNamespaceList(dContextNamespaceList);
		PrepareGoContextNamespaceList(goContextNamespaceList);
		PrepareJavaContextNamespaceList(javaContextNamespaceList);
		PreparePHPContextNamespaceList(phpContextNamespaceList);

		JArray<JIndex> noContextList = *matchList;
		if (!ConvertToFullNames(&noContextList, matchList,
								ns1, ns2, contextLang,
								*cContextNamespaceList, *dContextNamespaceList,
								*goContextNamespaceList, *javaContextNamespaceList,
								*phpContextNamespaceList))
		{
			if (!usedAllList && !findDeclaration && findDefinition)
			{
				// if no definition, try declarations
				// (all: class decl, C++: pure virtual fn)

				JArray<JIndex> allNoContextList = allMatchList;
				if (ConvertToFullNames(&allNoContextList, &allMatchList,
									   ns1, ns2, contextLang,
									   *cContextNamespaceList, *dContextNamespaceList,
									   *goContextNamespaceList, *javaContextNamespaceList,
									   *phpContextNamespaceList))
				{
					*matchList = allMatchList;
				}
				else
				{
					*matchList = noContextList;		// honor original request for only defn
				}
			}
			else
			{
				*matchList = noContextList;
			}
		}

		// re-sort

		matchList->SetCompareFunction([this](const JIndex& s1, const JIndex& s2)
		{
			const SymbolList::SymbolInfo* info = itsSymbolList->GetCArray();
			return JCompareStringsCaseInsensitive(info[s1-1].name, info[s2-1].name);
		});

		matchList->SetSortOrder(itsSymbolList->GetSortOrder());
		matchList->Sort();
		matchList->ClearCompareFunction();

		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 ConvertToFullNames (private)

	Replaces name with *.name (Eiffel, Java) or *:name (C++: file: or
	class::).  Returns false if contextList is empty.

 ******************************************************************************/

bool
SymbolList::ConvertToFullNames
	(
	JArray<JIndex>*				noContextList,
	JArray<JIndex>*				contextList,
	const JString&				contextNamespace1,
	const JString&				contextNamespace2,
	const Language			contextLang,
	const JPtrArray<JString>&	cContextNamespaceList,
	const JPtrArray<JString>&	dContextNamespaceList,
	const JPtrArray<JString>&	goContextNamespaceList,
	const JPtrArray<JString>&	javaContextNamespaceList,
	const JPtrArray<JString>&	phpContextNamespaceList
	)
	const
{
	const bool useLangNSContext = !contextNamespace1.IsEmpty();
	const bool useCNSContext    = !cContextNamespaceList.IsEmpty();
	const bool useDNSContext    = !dContextNamespaceList.IsEmpty();
	const bool useGoNSContext   = !goContextNamespaceList.IsEmpty();
	const bool useJavaNSContext = !javaContextNamespaceList.IsEmpty();
	const bool usePHPNSContext  = !phpContextNamespaceList.IsEmpty();

	// substitute indicies of fully qualified symbols
	// and filter based on namespace context

	const JSize symCount  = itsSymbolList->GetElementCount();
	const SymbolInfo* sym = itsSymbolList->GetCArray();

	const JSize count = noContextList->GetElementCount();
	JString s1, s2;
	for (JIndex i=count; i>=1; i--)
	{
		const JIndex j               = noContextList->GetElement(i) - 1;
		const SymbolInfo& info       = sym[j];
		const JString::Case caseSens = IsCaseSensitive(info.lang);

		s1 = "." + *(info.name);
		s2 = ":" + *(info.name);
		for (JIndex k=0; k<symCount; k++)
		{
			if (k != j &&
				sym[k].fileID    == info.fileID &&
				sym[k].lineIndex == info.lineIndex &&
				((sym[k].name)->EndsWith(s1, caseSens) ||
				 (sym[k].name)->EndsWith(s2, caseSens)))
			{
				if ((info.lang == kCLang && useCNSContext &&
					 !InContext(*(sym[k].name), cContextNamespaceList, JString::kCompareCase)) ||
					(info.lang == kDLang && useDNSContext &&
					 !InContext(*(sym[k].name), dContextNamespaceList, JString::kCompareCase)) ||
					(info.lang == kGoLang && useGoNSContext &&
					 !InContext(*(sym[k].name), goContextNamespaceList, JString::kCompareCase)) ||
					(info.lang == kJavaLang && useJavaNSContext &&
					 !InContext(*(sym[k].name), javaContextNamespaceList, JString::kCompareCase)) ||
					(info.lang == kPHPLang && usePHPNSContext &&
					 !InContext(*(sym[k].name), phpContextNamespaceList, JString::kCompareCase)) ||
					(info.lang == contextLang && useLangNSContext &&
					 !(sym[k].name)->BeginsWith(contextNamespace1, caseSens) &&
					 !(sym[k].name)->Contains(contextNamespace2, caseSens)))
				{
					contextList->RemoveElement(i);
				}
				else
				{
					contextList->SetElement(i, k+1);
				}

				noContextList->SetElement(i, k+1);
				break;
			}
		}
	}

	return !contextList->IsEmpty();
}

/******************************************************************************
 PrepareContextNamespace (private)

 ******************************************************************************/

void
SymbolList::PrepareContextNamespace
	(
	const JString&		contextNamespace,
	const Language	lang,
	JString*			ns1,
	JString*			ns2
	)
	const
{
	if (!contextNamespace.IsEmpty() &&
		(lang == kDLang          ||
		 lang == kGoLang         ||
		 lang == kJavaLang       ||
		 lang == kJavaScriptLang ||
		 lang == kEiffelLang     ||
		 lang == kCSharpLang))
	{
		*ns1  = contextNamespace;	// name.
		*ns1 += ".";

		*ns2 = *ns1;				// .name.
		ns2->Prepend(".");
	}
	else
	{
		ns1->Clear();
		ns2->Clear();
	}
}

/******************************************************************************
 PrepareContextNamespaceList (private)

 ******************************************************************************/

void
SymbolList::PrepareCContextNamespaceList
	(
	JPtrArray<JString>* contextNamespace
	)
	const
{
	PrepareContextNamespaceList(contextNamespace, "::");
}

void
SymbolList::PrepareDContextNamespaceList
	(
	JPtrArray<JString>* contextNamespace
	)
	const
{
	PrepareContextNamespaceList(contextNamespace, ".");
}

void
SymbolList::PrepareGoContextNamespaceList
	(
	JPtrArray<JString>* contextNamespace
	)
	const
{
	PrepareContextNamespaceList(contextNamespace, ".");
}

void
SymbolList::PrepareJavaContextNamespaceList
	(
	JPtrArray<JString>* contextNamespace
	)
	const
{
	PrepareContextNamespaceList(contextNamespace, ".");
}

void
SymbolList::PreparePHPContextNamespaceList
	(
	JPtrArray<JString>* contextNamespace
	)
	const
{
	PrepareContextNamespaceList(contextNamespace, "\\");
}

void
SymbolList::PrepareContextNamespaceList
	(
	JPtrArray<JString>*	contextNamespace,
	const JUtf8Byte*	namespaceOp
	)
	const
{
	if (!contextNamespace->IsEmpty())
	{
		const JSize count = contextNamespace->GetElementCount();
		for (JIndex i=count; i>=1; i--)
		{
			JString* cns1 = contextNamespace->GetElement(i);	// name::
			*cns1 += namespaceOp;

			auto* cns2 = jnew JString(*cns1);					// ::name::
			assert( cns2 != nullptr );
			cns2->Prepend(namespaceOp);
			contextNamespace->InsertAtIndex(i+1, cns2);
		}
	}
}

/******************************************************************************
 InContext (private)

 ******************************************************************************/

bool
SymbolList::InContext
	(
	const JString&				fullName,
	const JPtrArray<JString>&	contextNamespace,
	const JString::Case			caseSensitive
	)
	const
{
	const JSize count = contextNamespace.GetElementCount();
	for (JIndex i=1; i<=count; i+=2)
	{
		const JString* cns1 = contextNamespace.GetElement(i);
		const JString* cns2 = contextNamespace.GetElement(i+1);
		if (fullName.BeginsWith(*cns1, caseSensitive) ||
			fullName.Contains(*cns2, caseSensitive))
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 ClosestMatch

	Returns the index of the closest match for the given name prefix,
	searching only the items in visibleList.

	This is rather ugly.  We search for zero, which can't be in the list,
	and the compare function uses this to figure out which item is the
	target and which is an index into the full symbol list.

 ******************************************************************************/

bool
SymbolList::ClosestMatch
	(
	const JString&	prefixStr,
	JArray<JIndex>&	visibleList,
	JIndex*			index
	)
	const
{
	visibleList.SetCompareFunction([&](const JIndex& s1, const JIndex& s2)
	{
		auto* prefix = const_cast<JString*>(&prefixStr);
		if (s1 == 0)
		{
			const SymbolList::SymbolInfo& info = itsSymbolList->GetCArray()[s2-1];
			return JCompareStringsCaseInsensitive(prefix, info.name);
		}
		else if (s2 == 0)
		{
			const SymbolList::SymbolInfo& info = itsSymbolList->GetCArray()[s1-1];
			return JCompareStringsCaseInsensitive(info.name, prefix);
		}
		else
		{
			assert_msg( 0, "SymbolList.cpp:ClosestMatchCompare::Compare() didn't get a zero" );
			return JListT::kFirstEqualSecond;
		}
	});

	visibleList.SetSortOrder(itsSymbolList->GetSortOrder());

	bool found;
	*index = visibleList.SearchSortedOTI(0, JListT::kFirstMatch, &found);
	if (*index > visibleList.GetElementCount())		// insert beyond end of list
	{
		*index = visibleList.GetElementCount();
	}

	visibleList.ClearCompareFunction();

	return *index > 0;
}

/******************************************************************************
 FileTypesChanged

	We can't use Receive() because this must be called -before-
	ProjectDocument updates everything.

 ******************************************************************************/

void
SymbolList::FileTypesChanged
	(
	const PrefsManager::FileTypesChanged& info
	)
{
	if (info.Changed(IsParsed))
	{
		itsReparseAllFlag = true;
	}
}

/******************************************************************************
 PrepareForUpdate

	Get ready to parse files that have changed or been created and to
	throw out symbols in files that no longer exist.

	*** This often runs in the update thread.

 ******************************************************************************/

void
SymbolList::PrepareForUpdate
	(
	const bool		reparseAll,
	JProgressDisplay&	pg
	)
{
	assert( !itsReparseAllFlag || reparseAll );

	if (reparseAll)
	{
		RemoveAllSymbols();
	}
	itsChangedDuringParseFlag = reparseAll;

	// sort more strictly when building the list

	itsSymbolList->SetCompareFunction(CompareSymbolsAndTypes);

	itsBeganEmptyFlag = itsSymbolList->IsEmpty();
}

/******************************************************************************
 UpdateFinished

	Cleans up after updating files.

	*** This often runs in the update thread.

 ******************************************************************************/

bool
SymbolList::UpdateFinished
	(
	const JArray<JFAID_t>&	deadFileList,
	JProgressDisplay&		pg
	)
{
	DeleteProcess();

	// reset to lenient search

	itsSymbolList->SetCompareFunction(CompareSymbols);

	// toss files that no longer exist

	const JSize fileCount = deadFileList.GetElementCount();
	if (fileCount > 0)
	{
		pg.FixedLengthProcessBeginning(fileCount, JGetString("CleaningUp::SymbolList"), false, true);

		for (JIndex i=1; i<=fileCount; i++)
		{
			RemoveFile(deadFileList.GetElement(i));
			pg.IncrementProgress();
		}

		pg.ProcessFinished();
	}

	if (itsChangedDuringParseFlag && !InUpdateThread())
	{
		itsReparseAllFlag = false;
		Broadcast(Changed());
	}

	return itsChangedDuringParseFlag;
}

/******************************************************************************
 RemoveAllSymbols (private)

 ******************************************************************************/

void
SymbolList::RemoveAllSymbols()
{
	const JSize count = itsSymbolList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		SymbolInfo info = itsSymbolList->GetElement(i);
		info.Free();
	}
	itsSymbolList->RemoveAll();
}

/******************************************************************************
 RemoveFile (private)

	Throws out all symbols that were defined in the given file.

 ******************************************************************************/

void
SymbolList::RemoveFile
	(
	const JIndex id
	)
{
	const JSize count = itsSymbolList->GetElementCount();
	for (JIndex i=count; i>=1; i--)
	{
		SymbolInfo info = itsSymbolList->GetElement(i);
		if (info.fileID == id)
		{
			info.Free();
			itsSymbolList->RemoveElement(i);
			itsChangedDuringParseFlag = true;
		}
	}
}

/******************************************************************************
 FileChanged

	Throws out all symbols that were in the given file and reparses it.

 ******************************************************************************/

void
SymbolList::FileChanged
	(
	const JString&			fileName,
	const TextFileType	fileType,
	const JFAID_t			id
	)
{
	if (IsParsed(fileType))
	{
		if (!itsBeganEmptyFlag)
		{
			RemoveFile(id);
		}
		ParseFile(fileName, fileType, id);
	}
}

/******************************************************************************
 ParseFile (private)

	Runs the specified file through ctags.

 ******************************************************************************/

void
SymbolList::ParseFile
	(
	const JString&		fileName,
	const TextFileType	fileType,
	const JFAID_t		id
	)
{
	JString data;
	Language lang;
	if (ProcessFile(fileName, fileType, &data, &lang))
	{
		icharbufstream input(data.GetBytes(), data.GetByteCount());
		ReadSymbolList(input, lang, fileName, id);
		itsChangedDuringParseFlag = true;
	}
}

/******************************************************************************
 InitCtags (virtual protected)

	C preprocessor macros only need to be defined once.

 ******************************************************************************/

void
SymbolList::InitCtags
	(
	std::ostream& output
	)
{
	itsProjDoc->GetCTreeDirector()->
		GetCTree()->GetCPreprocessor()->PrintMacrosForCTags(output);
}

/******************************************************************************
 ReadSymbolList (private)

 ******************************************************************************/

void
SymbolList::ReadSymbolList
	(
	std::istream&	input,
	const Language	lang,
	const JString&	fullName,
	const JFAID_t	fileID
	)
{
	JString path, fileName;
	JSplitPathAndName(fullName, &path, &fileName);

	input >> std::ws;
	while (input.peek() == '!')
	{
		JIgnoreLine(input);
		input >> std::ws;
	}

	JStringPtrMap<JString> flags(JPtrArrayT::kDeleteAll);
	while (true)
	{
		auto* name = jnew JString;
		assert( name != nullptr );

		input >> std::ws;
		*name = JReadUntil(input, '\t');		// symbol name
		if (input.eof() || input.fail())
		{
			jdelete name;
			break;
		}

		JIgnoreUntil(input, '\t');				// file name

		JIndex lineIndex;
		input >> lineIndex;						// line index

		ReadExtensionFlags(input, &flags);

		JUtf8Character typeChar(' ');
		JString* value;
		if (flags.GetElement("kind", &value) && !value->IsEmpty())
		{
			typeChar = value->GetFirstCharacter();
		}

		JString* signature = nullptr;
		if (flags.GetElement("signature", &value) && !value->IsEmpty())
		{
			signature = jnew JString(*value);
			assert( signature != nullptr );
			signature->Prepend(" ");
		}

		if (IgnoreSymbol(*name))
		{
			jdelete name;
		}
		else
		{
			const Type type = DecodeSymbolType(lang, typeChar.GetBytes()[0], flags);
			if (signature == nullptr &&
				(IsFunction(type) || IsPrototype(type)))
			{
				signature = jnew JString(" ( )");
				assert( signature != nullptr );
			}

			const SymbolInfo info(name, signature, lang, type,
								  false, fileID, lineIndex);
			itsSymbolList->InsertSorted(info);

			// add file:name

			if (IsFileScope(type))
			{
				auto* name1 = jnew JString(fileName);
				assert( name1 != nullptr );
				*name1 += ":";
				*name1 += *name;

				JString* sig1 = nullptr;
				if (signature != nullptr)
				{
					sig1 = jnew JString(*signature);
					assert( sig1 != nullptr );
				}

				const SymbolInfo info1(name1, sig1, lang, type,
									   true, fileID, lineIndex);
				itsSymbolList->InsertSorted(info1);
			}
		}
	}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
SymbolList::ReadSetup
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		symInput,
	const JFileVersion	symVers
	)
{
	std::istream* input     = (projVers <= 41 ? &projInput : symInput);
	const JFileVersion vers = (projVers <= 41 ? projVers   : symVers);
	if (input != nullptr)
	{
		ReadSetup(*input, vers);

		itsReparseAllFlag = vers < 89 || (itsSymbolList->IsEmpty() && IsActive());
	}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
SymbolList::ReadSetup
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	RemoveAllSymbols();
	itsReparseAllFlag = false;

	JSize symbolCount;
	input >> symbolCount;

	itsSymbolList->SetBlockSize(symbolCount+1);		// avoid repeated realloc

	for (JIndex i=1; i<=symbolCount; i++)
	{
		auto* name = jnew JString;
		assert( name != nullptr );
		input >> *name;

		long lang, type;
		input >> lang >> type;

		bool fullyQualifiedFileScope = false;
		if (vers >= 54)
		{
			input >> JBoolFromString(fullyQualifiedFileScope);
		}

		JFAID_t fileID;
		JIndex lineIndex;
		input >> fileID >> lineIndex;

		JString* signature = nullptr;
		if (vers > 63)
		{
			bool hasSignature;
			input >> JBoolFromString(hasSignature);

			if (hasSignature)
			{
				signature = jnew JString;
				assert( signature != nullptr );
				input >> *signature;
			}
		}

		itsSymbolList->AppendElement(
			SymbolInfo(name, signature, (Language) lang, (Type) type,
					   fullyQualifiedFileScope, fileID, lineIndex));
	}

	itsSymbolList->SetBlockSize(kBlockSize);

	if (vers < 51)
	{
		itsSymbolList->SetCompareFunction(CompareSymbolsAndTypes);
		itsSymbolList->Sort();
		itsSymbolList->SetCompareFunction(CompareSymbols);
	}

	Broadcast(Changed());
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
SymbolList::WriteSetup
	(
	std::ostream& projOutput,
	std::ostream* symOutput
	)
	const
{
	if (symOutput != nullptr)
	{
		const JSize symbolCount = itsSymbolList->GetElementCount();
		*symOutput << ' ' << symbolCount;

		for (JIndex i=1; i<=symbolCount; i++)
		{
			const SymbolInfo info = itsSymbolList->GetElement(i);
			*symOutput << ' ' << *(info.name);
			*symOutput << ' ' << (long) info.lang;
			*symOutput << ' ' << (long) info.type;
			*symOutput << ' ' << JBoolToString(info.fullyQualifiedFileScope);
			*symOutput << ' ' << info.fileID;
			*symOutput << ' ' << info.lineIndex;

			if (info.signature != nullptr)
			{
				*symOutput << ' ' << JBoolToString(true);
				*symOutput << ' ' << *(info.signature);
			}
			else
			{
				*symOutput << ' ' << JBoolToString(false);
			}
		}

		*symOutput << ' ';
	}
}

/******************************************************************************
 Receive (virtual protected)

	Required because of multiple inheritance.

 ******************************************************************************/

void
SymbolList::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JContainer::Receive(sender, message);
	CtagsUser::Receive(sender, message);
}

/******************************************************************************
 CompareSymbols (static)

 ******************************************************************************/

JListT::CompareResult
SymbolList::CompareSymbols
	(
	const SymbolInfo& s1,
	const SymbolInfo& s2
	)
{
	return JCompareStringsCaseInsensitive(s1.name, s2.name);
}

/******************************************************************************
 CompareSymbolsAndTypes (static)

 ******************************************************************************/

JListT::CompareResult
SymbolList::CompareSymbolsAndTypes
	(
	const SymbolInfo& s1,
	const SymbolInfo& s2
	)
{
	const JListT::CompareResult result =
		JCompareStringsCaseInsensitive(s1.name, s2.name);

	if (result == JListT::kFirstEqualSecond)
	{
		if (s1.type < s2.type)
		{
			return JListT::kFirstLessSecond;
		}
		else if (s1.type == s2.type)
		{
			return JListT::kFirstEqualSecond;
		}
		else
		{
			return JListT::kFirstGreaterSecond;
		}
	}
	else
	{
		return result;
	}
}

/******************************************************************************
 SymbolInfo functions (private)

 ******************************************************************************/

void
SymbolList::SymbolInfo::Free()
{
	jdelete name;
	name = nullptr;

	jdelete signature;
	signature = nullptr;
}

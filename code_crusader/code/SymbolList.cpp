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
#include "Class.h"
#include "globals.h"
#include "ctagsRegex.h"
#include <jx-af/jx/JXUrgentFunctionTask.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kCtagsArgs =
	"--format=2 --excmd=number --sort=no --extras=+q "
	"--c-kinds=+p --php-kinds=-v --ant-kinds=t "
	"--javascript-kinds=cfgmGS --typescript-kinds=cfgnim "
	"--html-kinds=I --css-kinds= --make-kinds=tm --pod-kinds= "
	CtagsBisonDef
	CtagsBisonNonterminalDef
	CtagsBisonNonterminalDecl
	CtagsBisonTerminalDecl;

// JBroadcaster message types

const JUtf8Byte* SymbolList::kChanged            = "Changed::SymbolList";
const JUtf8Byte* SymbolList::kUpdateFoundChanges = "UpdateFoundChanges::SymbolList";

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
	itsFirstSymbolUpdateFlag  = true;
	itsChangedDuringParseFlag = false;
	itsBeganEmptyFlag         = false;

	itsSymbolList = jnew JArray<SymbolInfo>(kMinSize);
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
	const SymbolInfo info = itsSymbolList->GetItem(symbolIndex);
	*lang                 = info.lang;
	*type                 = info.type;

	if (fullyQualifiedFileScope != nullptr)
	{
		*fullyQualifiedFileScope = info.fullyQualifiedFileScope;
	}

	return *info.name;
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
	const SymbolInfo info = itsSymbolList->GetItem(symbolIndex);
	*lineIndex            = info.lineIndex;
	return itsProjDoc->GetAllFileList()->GetFileName(info.fileID);
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
	const JSize symCount  = itsSymbolList->GetItemCount();
	const SymbolInfo* sym = itsSymbolList->GetCArray();

	bool found = false;
	for (JUnsignedOffset i=0; i<symCount; i++)
	{
		if (IsClass(sym[i].type) &&
			JString::Compare(*sym[i].name, name, IsCaseSensitive(sym[i].lang)) == 0)
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

	*** The contents of contextNamespaceList are altered.

 ******************************************************************************/

bool
SymbolList::FindSymbol
	(
	const JString&					name,
	const JFAID_t					contextFileID,
	const JArray<ContextNamespace>&	contextNamespaceList,
	const bool						findDeclaration,
	const bool						findDefinition,
	JArray<JIndex>*					matchList
	)
	const
{
	// find all symbols that match

	matchList->RemoveAll();
	matchList->SetMinSize(50);

	JArray<JIndex> allMatchList(50);

	SymbolInfo target;
	target.name = const_cast<JString*>(&name);
	JIndex startIndex;
	if (itsSymbolList->SearchSorted(target, JListT::kFirstMatch, &startIndex))
	{
		const JSize count = itsSymbolList->GetItemCount();
		for (JIndex i=startIndex; i<=count; i++)
		{
			const SymbolInfo info = itsSymbolList->GetItem(i);
			if (CompareSymbols(target, info) != std::weak_ordering::equivalent)
			{
				break;
			}

			if (!IsCaseSensitive(info.lang) || *info.name == name)
			{
				if (contextFileID == info.fileID &&		// automatically fails if context is kInvalidID
					IsFileScope(info.type))
				{
					matchList->RemoveAll();
					matchList->AppendItem(i);
					break;
				}

				if ((findDeclaration || !IsPrototype(info.type)) &&
					(findDefinition  || !IsFunction(info.type)))
				{
					matchList->AppendItem(i);
				}
				allMatchList.AppendItem(i);
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
		for (const auto cns : contextNamespaceList)
		{
			if (HasNamespace(cns.lang))
			{
				PrepareContextNamespaceList(cns.list, GetNamespaceOperator(cns.lang));
			}
			else
			{
				cns.list->CleanOut();
			}
		}

		JArray<JIndex> noContextList = *matchList;
		if (!ConvertToFullNames(&noContextList, matchList,
								contextNamespaceList))
		{
			if (!usedAllList && !findDeclaration && findDefinition)
			{
				// if no definition, try declarations
				// (all: class decl, C++: pure virtual fn)

				JArray<JIndex> allNoContextList = allMatchList;
				if (ConvertToFullNames(&allNoContextList, &allMatchList,
									   contextNamespaceList))
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

		Sort(matchList);
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
	JArray<JIndex>*					noContextList,
	JArray<JIndex>*					contextList,
	const JArray<ContextNamespace>&	contextNamespaceList
	)
	const
{
	// substitute indicies of fully qualified symbols
	// and filter based on namespace context

	const JSize symCount  = itsSymbolList->GetItemCount();
	const SymbolInfo* sym = itsSymbolList->GetCArray();

	const JSize count = noContextList->GetItemCount();
	JString s1, s2, s3;
	for (JIndex i=count; i>=1; i--)
	{
		const JIndex j               = noContextList->GetItem(i) - 1;
		const SymbolInfo& info       = sym[j];
		const JString::Case caseSens = IsCaseSensitive(info.lang);

		s1 = "."  + *info.name;
		s2 = ":"  + *info.name;
		s3 = "\\" + *info.name;
		for (JIndex k=0; k<symCount; k++)
		{
			if (k != j &&
				sym[k].fileID    == info.fileID &&
				sym[k].lineIndex == info.lineIndex &&
				(sym[k].name->EndsWith(s1, caseSens) ||
				 sym[k].name->EndsWith(s2, caseSens) ||
				 sym[k].name->EndsWith(s3, caseSens)))
			{
				bool removed = false;
				for (const auto cns : contextNamespaceList)
				{
					if (info.lang == cns.lang)
					{
						if (!cns.list->IsEmpty() &&
							!InContext(*sym[k].name, *cns.list, caseSens))
						{
							contextList->RemoveItem(i);
							removed = true;
						}
						break;
					}
				}

				if (!removed)
				{
					contextList->SetItem(i, k+1);
				}

				noContextList->SetItem(i, k+1);
				break;
			}
		}
	}

	return !contextList->IsEmpty();
}

/******************************************************************************
 PrepareContextNamespaceList (private)

 ******************************************************************************/

void
SymbolList::PrepareContextNamespaceList
	(
	JPtrArray<JString>*	contextNamespace,
	const JUtf8Byte*	namespaceOp
	)
	const
{
	const JSize count = contextNamespace->GetItemCount();
	for (JIndex i=count; i>=1; i--)
	{
		JString* cns1 = contextNamespace->GetItem(i);	// name::
		*cns1        += namespaceOp;

		auto* cns2 = jnew JString(*cns1);				// ::name::
		cns2->Prepend(namespaceOp);
		contextNamespace->InsertAtIndex(i+1, cns2);
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
	const JSize count = contextNamespace.GetItemCount();
	for (JIndex i=1; i<=count; i+=2)
	{
		const JString* cns1 = contextNamespace.GetItem(i);
		const JString* cns2 = contextNamespace.GetItem(i+1);
		if (fullName.StartsWith(*cns1, caseSensitive) ||
			fullName.Contains(*cns2, caseSensitive))
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 FindAllSymbols

	Fills matchList with all the symbols for the class and its ancestors.

 ******************************************************************************/

bool
SymbolList::FindAllSymbols
	(
	const Class*	theClass,
	const bool		findDeclaration,
	const bool		findDefinition,
	JArray<JIndex>*	matchList
	)
	const
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	theClass->GetAncestorList(&list);

	const Language lang               = theClass->GetLanguage();
	const bool hasNamespace           = HasNamespace(lang);
	const JString::Case caseSensitive = IsCaseSensitive(lang);

	// find all symbols that match

	matchList->RemoveAll();
	matchList->SetMinSize(50);

	JString prefix;
	for (const auto* name : list)
	{
		SymbolInfo target;
		target.name = const_cast<JString*>(name);
		JIndex startIndex;
		if (itsSymbolList->SearchSorted(target, JListT::kFirstMatch, &startIndex))
		{
			if (hasNamespace)
			{
				prefix = *name + GetNamespaceOperator(lang);
			}
			else
			{
				prefix.Clear();
			}

			const JSize count = itsSymbolList->GetItemCount();
			for (JIndex i=startIndex; i<=count; i++)
			{
				const SymbolInfo info = itsSymbolList->GetItem(i);
				if (info.lang == lang &&
					(findDeclaration || !IsPrototype(info.type)) &&
					(findDefinition  || !IsFunction(info.type)) &&
					(JString::Compare(*info.name, *name, caseSensitive) == 0 ||
					 (hasNamespace && info.name->StartsWith(prefix, caseSensitive))))
				{
					matchList->AppendItem(i);
				}
			}
		}
	}

	Sort(matchList);
	return !matchList->IsEmpty();
}

/******************************************************************************
 Sort (private)

 ******************************************************************************/

void
SymbolList::Sort
	(
	JArray<JIndex>* list
	)
	const
{
	list->SetCompareFunction([this](const JIndex& s1, const JIndex& s2)
	{
		const SymbolList::SymbolInfo* info = itsSymbolList->GetCArray();
		return JCompareStringsCaseInsensitive(info[s1-1].name, info[s2-1].name);
	});

	list->SetSortOrder(itsSymbolList->GetSortOrder());
	list->Sort();
	list->ClearCompareFunction();
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
	visibleList.SetCompareFunction([this, prefixStr](const JIndex& s1, const JIndex& s2)
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
			return std::weak_ordering::equivalent;
		}
	});

	visibleList.SetSortOrder(itsSymbolList->GetSortOrder());

	bool found;
	*index = visibleList.SearchSortedOTI(0, JListT::kFirstMatch, &found);
	if (*index > visibleList.GetItemCount())		// insert beyond end of list
	{
		*index = visibleList.GetItemCount();
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

 ******************************************************************************/

void
SymbolList::PrepareForUpdate
	(
	const bool reparseAll
	)
{
	assert( !itsReparseAllFlag || reparseAll );

	if (reparseAll)
	{
		RemoveAllSymbols();
		Broadcast(UpdateFoundChanges());
	}
	itsChangedDuringParseFlag = reparseAll;

	// sort more strictly when building the list

	itsSymbolList->SetCompareFunction(CompareSymbolsAndTypes);

	itsBeganEmptyFlag = itsSymbolList->IsEmpty();
}

/******************************************************************************
 UpdateThreadFinished

	Cleans up after updating files.

	*** This runs in the update thread.

 ******************************************************************************/

void
SymbolList::UpdateThreadFinished
	(
	const JArray<JFAID_t>&	deadFileList,
	JProgressDisplay&		pg
	)
{
	DeleteProcess();

	// reset to lenient search

	itsSymbolList->SetCompareFunction(CompareSymbols);

	// toss files that no longer exist

	const JSize fileCount = deadFileList.GetItemCount();
	if (fileCount > 0)
	{
		for (JIndex i=1; i<=fileCount; i++)
		{
			RemoveFile(deadFileList.GetItem(i));
			pg.IncrementProgress();
		}
	}

	// trim file scope paths

	JSize matchLength   = 0;
	const JString* prev = nullptr;
	for (const auto& info : *itsSymbolList)
	{
		if (info.fullyQualifiedFileScope)
		{
			matchLength =
				prev == nullptr ?
					info.name->GetCharacterCount() :
					JMin(matchLength, JString::CalcCharacterMatchLength(*prev, *info.name));

			prev = info.name;
		}
	}

	if (matchLength > 1)
	{
		for (const auto& info : *itsSymbolList)
		{
			if (info.fullyQualifiedFileScope)
			{
				JStringIterator iter(info.name);
				iter.SkipNext(matchLength);
				iter.RemoveAllPrev();
			}
		}

		itsSymbolList->Sort();
	}
}

/******************************************************************************
 UpdateFinished

	Cleans up after updating files.

 ******************************************************************************/

bool
SymbolList::UpdateFinished()
{
	itsReparseAllFlag = false;
	if (itsChangedDuringParseFlag || itsFirstSymbolUpdateFlag)
	{
		Broadcast(Changed());
	}

	itsFirstSymbolUpdateFlag = false;
	return itsChangedDuringParseFlag;
}

/******************************************************************************
 RemoveAllSymbols (private)

 ******************************************************************************/

void
SymbolList::RemoveAllSymbols()
{
	const JSize count = itsSymbolList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		SymbolInfo info = itsSymbolList->GetItem(i);
		info.Free();
	}
	itsSymbolList->RemoveAll();
}

/******************************************************************************
 RemoveFile (private)

	Throws out all symbols that were defined in the given file.

	*** This runs in the update thread.

 ******************************************************************************/

void
SymbolList::RemoveFile
	(
	const JIndex id
	)
{
	const JSize count = itsSymbolList->GetItemCount();
	for (JIndex i=count; i>=1; i--)
	{
		SymbolInfo info = itsSymbolList->GetItem(i);
		if (info.fileID == id)
		{
			if (!itsChangedDuringParseFlag)
			{
				auto* task = jnew JXUrgentFunctionTask(this, [this]()
				{
					Broadcast(UpdateFoundChanges());
				},
				"SymbolList::RemoveFile::UpdateFoundChanges");
				task->Go();
			}

			info.Free();
			itsSymbolList->RemoveItem(i);
			itsChangedDuringParseFlag = true;
		}
	}
}

/******************************************************************************
 FileChanged

	Throws out all symbols that were in the given file and reparses it.

	*** This runs in the update thread.

 ******************************************************************************/

void
SymbolList::FileChanged
	(
	const JString&		fileName,
	const TextFileType	fileType,
	const JFAID_t		id
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

	*** This runs in the update thread.

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
		if (!itsChangedDuringParseFlag)
		{
			auto* task = jnew JXUrgentFunctionTask(this, [this]()
			{
				Broadcast(UpdateFoundChanges());
			},
			"SymbolList::ParseFile::UpdateFoundChanges");
			task->Go();
		}

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

	JStringPtrMap<JString> flags(JPtrArrayT::kDeleteAll);
	while (true)
	{
		auto* name = jnew JString;

		input >> std::ws;
		while (input.peek() == '!')
		{
			JIgnoreLine(input);
			input >> std::ws;
		}

		*name = JReadUntil(input, '\t');		// symbol name
		if (input.eof() || input.fail())
		{
			jdelete name;
			break;
		}

		JStringIterator nameIter(name);
		while (nameIter.Next("\\\\"))
		{
			nameIter.ReplaceLastMatch("\\");
		}
		nameIter.Invalidate();

		JIgnoreUntil(input, '\t');				// file name

		JIndex lineIndex;
		input >> lineIndex;						// line index

		ReadExtensionFlags(input, &flags);

		if (IgnoreSymbol(*name))
		{
			jdelete name;
			continue;
		}

		JUtf8Character typeChar(' ');
		JString* value;
		if (flags.GetItem("kind", &value) && !value->IsEmpty())
		{
			typeChar = value->GetFirstCharacter();
		}

		JString* signature = nullptr;
		if (flags.GetItem("signature", &value) && !value->IsEmpty())
		{
			signature = jnew JString(*value);
			signature->Prepend(" ");
		}

		const Type type = DecodeSymbolType(lang, typeChar.GetBytes()[0], flags);
		if (signature == nullptr &&
			(IsFunction(type) || IsPrototype(type)))
		{
			signature = jnew JString(" ( )");
		}

		const SymbolInfo info(name, signature, lang, type,
							  false, fileID, lineIndex);
		itsSymbolList->InsertSorted(info);

		// add file:name

		if (IsFileScope(type))
		{
			auto* name1 = jnew JString(fullName);
			*name1 += ":";
			*name1 += *name;

			JString* sig1 = nullptr;
			if (signature != nullptr)
			{
				sig1 = jnew JString(*signature);
			}

			const SymbolInfo info1(name1, sig1, lang, type,
								   true, fileID, lineIndex);
			itsSymbolList->InsertSorted(info1);
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

		itsReparseAllFlag = vers < 101 || (itsSymbolList->IsEmpty() && IsActive());
	}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
SymbolList::ReadSetup
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	RemoveAllSymbols();
	itsReparseAllFlag = false;

	JSize symbolCount;
	input >> symbolCount;

	itsSymbolList->SetMinSize(symbolCount);		// avoid repeated realloc

	for (JIndex i=1; i<=symbolCount; i++)
	{
		auto* name = jnew JString;
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
				input >> *signature;
			}
		}

		itsSymbolList->AppendItem(
			SymbolInfo(name, signature, (Language) lang, (Type) type,
					   fullyQualifiedFileScope, fileID, lineIndex));
	}

	itsSymbolList->SetMinSize(kMinSize);

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
		const JSize symbolCount = itsSymbolList->GetItemCount();
		*symOutput << ' ' << symbolCount;

		for (JIndex i=1; i<=symbolCount; i++)
		{
			const SymbolInfo info = itsSymbolList->GetItem(i);
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

std::weak_ordering
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

std::weak_ordering
SymbolList::CompareSymbolsAndTypes
	(
	const SymbolInfo& s1,
	const SymbolInfo& s2
	)
{
	auto result = JCompareStringsCaseInsensitive(s1.name, s2.name);
	if (result == std::weak_ordering::equivalent)
	{
		result = s1.type <=> s2.type;
	}
	return result;
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

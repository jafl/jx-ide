/******************************************************************************
 MainDirector.cpp

	<Description>

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2002 by Glenn W. Bach.

 *****************************************************************************/

#include "MainDirector.h"
#include "App.h"
#include "PrefsManager.h"
#include "Class.h"
#include "FunctionTable.h"
#include "globals.h"

#include <jx-af/jx/JXChoosePathDialog.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXPathInput.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXWindow.h>

#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentPrefsVersion	= 1;

/******************************************************************************
 Constructor

 *****************************************************************************/

MainDirector::MainDirector
	(
	JXDirector*					supervisor,
	const JPtrArray<JString>&	argList
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GetPrefsManager(), kMainDirectorID)
{
	itsClass = jnew Class();

	JString outputPath;

	const JSize count = argList.GetItemCount();
	JString classname, filename;
	for (JIndex i = 2; i <= count; i++)
	{
		const JString* argName = argList.GetItem(i);
		if (*argName == "--output_path" && i < count)
		{
			outputPath = *argList.GetItem(i+1);
			i++;
		}
		else if (!argName->StartsWith("-"))
		{
			std::ifstream is(argName->GetBytes());
			if (is.good())
			{
				int version;
				is >> version;
				if (version == 0)
				{
					JSize count;
					is >> count;
					for (JIndex i = 1; i <= count; i++)
					{
						is >> classname >> filename;
						if (i == 1)
						{
							itsClass->AddBaseClass(classname, filename);
						}
						else
						{
							itsClass->AddAncestor(classname, filename);
						}
					}
				}
			}
		}
	}

	itsClass->Populate();

	BuildWindow(outputPath);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MainDirector::~MainDirector()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "gfg_main_window_icon.xpm"

void
MainDirector::BuildWindow
	(
	const JString& outputPath
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 650,460, JGetString("WindowTitle::MainDirector::JXLayout"));

	auto* classNameLabel =
		jnew JXStaticText(JGetString("classNameLabel::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,10, 90,20);
	classNameLabel->SetToLabel(false);

	auto* directoryLabel =
		jnew JXStaticText(JGetString("directoryLabel::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,10, 110,20);
	directoryLabel->SetToLabel(false);

	itsChooseButton =
		jnew JXTextButton(JGetString("itsChooseButton::MainDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 550,10, 80,20);
	itsChooseButton->SetShortcuts(JGetString("itsChooseButton::shortcuts::MainDirector::JXLayout"));

	auto* derivedLabel =
		jnew JXStaticText(JGetString("derivedLabel::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,45, 90,20);
	derivedLabel->SetToLabel(false);

	itsBaseClassTxt =
		jnew JXStaticText(JString::empty, window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 105,45, 110,20);
	itsBaseClassTxt->SetToLabel(false);

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,75, 610,300);
	assert( scrollbarSet != nullptr );

	itsTable =
		jnew FunctionTable(itsClass, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 610,280);

	auto* authorLabel =
		jnew JXStaticText(JGetString("authorLabel::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 15,390, 60,20);
	authorLabel->SetToLabel(false);

	auto* copyrightLabel =
		jnew JXStaticText(JGetString("copyrightLabel::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 250,390, 70,20);
	copyrightLabel->SetToLabel(false);

	itsStringsButton =
		jnew JXTextButton(JGetString("itsStringsButton::MainDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 550,390, 80,20);
	itsStringsButton->SetShortcuts(JGetString("itsStringsButton::shortcuts::MainDirector::JXLayout"));

	itsCancelButton =
		jnew JXTextButton(JGetString("itsCancelButton::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 130,425, 70,20);
	itsCancelButton->SetShortcuts(JGetString("itsCancelButton::shortcuts::MainDirector::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 285,425, 70,20);
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::shortcuts::MainDirector::JXLayout"));

	itsGenerateButton =
		jnew JXTextButton(JGetString("itsGenerateButton::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 440,425, 70,20);
	itsGenerateButton->SetShortcuts(JGetString("itsGenerateButton::shortcuts::MainDirector::JXLayout"));

	auto* colHeader =
		jnew JXColHeaderWidget(itsTable, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 610,20);
	assert( colHeader != nullptr );

	itsClassInput =
		jnew JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 105,10, 130,20);
	itsClassInput->SetIsRequired();
	itsClassInput->SetFont(JFontManager::GetDefaultMonospaceFont());

	itsDirInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 350,10, 200,20);
	itsDirInput->SetIsRequired(true);
	itsDirInput->ShouldAllowInvalidPath(false);
	itsDirInput->ShouldRequireWritable(true);

	itsAuthorInput =
		jnew JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 75,390, 160,20);

	itsCopyrightInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 320,390, 230,20);

// end JXLayout

	window->LockCurrentMinSize();
	window->SetIcon(jnew JXImage(GetDisplay(), gfg_main_window_icon));

	ListenTo(itsChooseButton, std::function([this](const JXButton::Pushed&)
	{
		if (itsDirInput != nullptr)
		{
			auto* dlog = JXChoosePathDialog::Create(JXChoosePathDialog::kAcceptReadable, itsDirInput->GetText()->GetText());
			if (dlog->DoDialog())
			{
				itsDirInput->GetText()->SetText(dlog->GetPath());
			}
		}
	}));

	ListenTo(itsCancelButton, std::function([this](const JXButton::Pushed&)
	{
		Close();
	}));

	ListenTo(itsHelpButton, std::function([](const JXButton::Pushed&)
	{
		JXGetHelpManager()->ShowTOC();
	}));

	ListenTo(itsGenerateButton, std::function([this](const JXButton::Pushed&)
	{
		GetPrefsManager()->SetAuthor(itsAuthorInput->GetText()->GetText());
		GetPrefsManager()->SetCopyright(itsCopyrightInput->GetText()->GetText());

		if (Write())
		{
			//Close();
		}
	}));

	ListenTo(itsStringsButton, std::function([](const JXButton::Pushed&)
	{
		GetPrefsManager()->EditPrefs();
	}));

	JString cname, fname;
	JString bases;
	const JSize count	= itsClass->GetBaseClassCount();
	for (JIndex i = 1; i <= count; i++)
	{
		itsClass->GetBaseClass(i, &cname, &fname);
		if (i > 1)
		{
			bases += ", ";
		}
		bases += cname;
	}

	const JFontStyle itsBaseClassTxt_style(true, false, 0, false, JColorManager::GetBlackColor());
	itsBaseClassTxt->SetFontStyle(itsBaseClassTxt_style);
	itsBaseClassTxt->GetText()->SetText(bases);

	colHeader->SetColTitle(FunctionTable::kFUsed, JGetString("UseLabel::MainDirector"));
	colHeader->SetColTitle(FunctionTable::kFReturnType, JGetString("ReturnTypeLabel::MainDirector"));
	colHeader->SetColTitle(FunctionTable::kFFunctionName, JGetString("FunctionLabel::MainDirector"));
	colHeader->SetColTitle(FunctionTable::kFSignature, JGetString("SignatureLabel::MainDirector"));

	itsDirInput->GetText()->SetText(outputPath);
	itsAuthorInput->GetText()->SetText(GetPrefsManager()->GetAuthor());
	itsCopyrightInput->GetText()->SetText(GetPrefsManager()->GetCopyright());

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
MainDirector::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;

	if (vers >= 1)
	{
		GetWindow()->ReadGeometry(input);
	}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
MainDirector::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion << ' ';
	GetWindow()->WriteGeometry(output);
}

/******************************************************************************
 Write (private)

 ******************************************************************************/

bool
MainDirector::Write()
{
	JString cname = itsClassInput->GetText()->GetText();
	if (cname.IsEmpty())
	{
		JGetUserNotification()->ReportError(JGetString("MissingClassName::MainDirector"));
		itsClassInput->Focus();
		return false;
	}
	itsClass->SetClassName(cname);

	JString dir;
	if (!itsDirInput->GetPath(&dir))
	{
		JGetUserNotification()->ReportError(JGetString("InvalidDestPath::MainDirector"));
		itsDirInput->Focus();
		return false;
	}
	JString headername	= cname + ".h";
	JString sourcename	= cname + ".cpp";

	JString headerfile	= JCombinePathAndName(dir, headername);
	JString sourcefile	= JCombinePathAndName(dir, sourcename);

	if (JFileExists(headerfile) ||
		JFileExists(sourcefile))
	{
		headername	= cname + "_tmp.h";
		sourcename	= cname + "_tmp.cpp";

		headerfile	= JCombinePathAndName(dir, headername);
		sourcefile	= JCombinePathAndName(dir, sourcename);
	}

	JString bcname, bfname;
	JString bases, basefiles, baseconstructors;

	const JSize count = itsClass->GetBaseClassCount();
	for (JIndex i = 1; i <= count; i++)
	{
		itsClass->GetBaseClass(i, &bcname, &bfname);
		if (i > 1)
		{
			bases += ", ";
		}
		bases += "public " + bcname;

		JString path, name;
		JSplitPathAndName(bfname, &path, &name);

		basefiles += "#include <" + name + ">";
		baseconstructors += "\t" + bcname + "()";
		if (i < count)
		{
			basefiles += "\n";
			baseconstructors += ",\n";
		}
	}

	// header file

	std::ofstream oh(headerfile.GetBytes());
	if (!oh.good())
	{
		JGetUserNotification()->ReportError(JGetString("CreateFileFailed::MainDirector"));
		return false;
	}

	JString comment = GetPrefsManager()->GetHeaderComment(cname);
	comment.TrimWhitespace();

	std::ostringstream publicHStream;
	itsClass->WritePublic(publicHStream, true);
	std::string publicHFns = publicHStream.str();

	std::ostringstream protectedHStream;
	itsClass->WriteProtected(protectedHStream, true);
	std::string protectedHFns = protectedHStream.str();

	const JUtf8Byte* hmap[] =
	{
		"comment",   comment.GetBytes(),
		"class",     cname.GetBytes(),
		"basefile",  basefiles.GetBytes(),
		"base",      bases.GetBytes(),
		"public",    publicHFns.c_str(),
		"protected", protectedHFns.c_str()
	};
	JGetString("CLASS_HEADER", hmap, sizeof(hmap)).Print(oh);

	oh.close();

	// source file

	std::ofstream os(sourcefile.GetBytes());
	if (!os.good())
	{
		JGetUserNotification()->ReportError(JGetString("CreateFileFailed::MainDirector"));
		return false;
	}

	comment = GetPrefsManager()->GetSourceComment(cname, bases);
	comment.TrimWhitespace();

	JString ctor = GetPrefsManager()->GetConstructorComment();
	ctor.TrimWhitespace();

	JString dtor = GetPrefsManager()->GetDestructorComment();
	dtor.TrimWhitespace();

	std::ostringstream publicSStream;
	itsClass->WritePublic(publicSStream);
	std::string publicSFns = publicSStream.str();

	std::ostringstream protectedSStream;
	itsClass->WriteProtected(protectedSStream);
	std::string protectedSFns = protectedSStream.str();

	const JUtf8Byte* smap[] =
	{
		"comment",   comment.GetBytes(),
		"class",     cname.GetBytes(),
		"ctor",      ctor.GetBytes(),
		"base",      baseconstructors.GetBytes(),
		"dtor",      dtor.GetBytes(),
		"public",    publicSFns.c_str(),
		"protected", protectedSFns.c_str()
	};
	JGetString("CLASS_SOURCE", smap, sizeof(smap)).Print(os);

	os.close();

	JString cmd	= CODE_CRUSADER_BINARY " " + headerfile + " " + sourcefile;

	JString errStr;
	JRunProgram(cmd, &errStr);

	return true;
}

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
#include "actionDefs.h"

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

#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kColHeaderHeight		= 20;
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
	assert(itsClass != nullptr);

	JString outputPath;

	const JSize count = argList.GetElementCount();
	JString classname, filename;
	for (JIndex i = 2; i <= count; i++)
	{
		const JString* argName = argList.GetElement(i);
		if (*argName == "--output_path" && i < count)
		{
			outputPath = *(argList.GetElement(i+1));
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

	auto* window = jnew JXWindow(this, 650,460, JString::empty);
	assert( window != nullptr );

	itsClassInput =
		jnew JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 105,10, 130,20);
	assert( itsClassInput != nullptr );

	auto* classNameLabel =
		jnew JXStaticText(JGetString("classNameLabel::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,10, 90,20);
	assert( classNameLabel != nullptr );
	classNameLabel->SetToLabel();

	auto* directoryLabel =
		jnew JXStaticText(JGetString("directoryLabel::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,10, 110,20);
	assert( directoryLabel != nullptr );
	directoryLabel->SetToLabel();

	itsChooseButton =
		jnew JXTextButton(JGetString("itsChooseButton::MainDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 550,10, 80,20);
	assert( itsChooseButton != nullptr );
	itsChooseButton->SetShortcuts(JGetString("itsChooseButton::MainDirector::shortcuts::JXLayout"));

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,75, 610,300);
	assert( scrollbarSet != nullptr );

	itsGenerateButton =
		jnew JXTextButton(JGetString("itsGenerateButton::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 440,425, 70,20);
	assert( itsGenerateButton != nullptr );
	itsGenerateButton->SetShortcuts(JGetString("itsGenerateButton::MainDirector::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 285,425, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::MainDirector::shortcuts::JXLayout"));

	itsCancelButton =
		jnew JXTextButton(JGetString("itsCancelButton::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 130,425, 70,20);
	assert( itsCancelButton != nullptr );
	itsCancelButton->SetShortcuts(JGetString("itsCancelButton::MainDirector::shortcuts::JXLayout"));

	itsDirInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 350,10, 200,20);
	assert( itsDirInput != nullptr );

	auto* derivedLabel =
		jnew JXStaticText(JGetString("derivedLabel::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,45, 90,20);
	assert( derivedLabel != nullptr );
	derivedLabel->SetToLabel();

	itsBaseClassTxt =
		jnew JXStaticText(JGetString("itsBaseClassTxt::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 105,45, 110,20);
	assert( itsBaseClassTxt != nullptr );
	const JFontStyle itsBaseClassTxt_style(true, false, 0, false, JColorManager::GetBlackColor());
	itsBaseClassTxt->SetFontStyle(itsBaseClassTxt_style);
	itsBaseClassTxt->SetToLabel();

	auto* authorLabel =
		jnew JXStaticText(JGetString("authorLabel::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 15,390, 60,20);
	assert( authorLabel != nullptr );
	authorLabel->SetToLabel();

	itsAuthorInput =
		jnew JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 75,390, 160,20);
	assert( itsAuthorInput != nullptr );

	itsStringsButton =
		jnew JXTextButton(JGetString("itsStringsButton::MainDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 550,390, 80,20);
	assert( itsStringsButton != nullptr );
	itsStringsButton->SetShortcuts(JGetString("itsStringsButton::MainDirector::shortcuts::JXLayout"));

	auto* copyrightLabel =
		jnew JXStaticText(JGetString("copyrightLabel::MainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 250,390, 70,20);
	assert( copyrightLabel != nullptr );
	copyrightLabel->SetToLabel();

	itsCopyrightInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 320,390, 230,20);
	assert( itsCopyrightInput != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::MainDirector"));
	window->LockCurrentMinSize();

	ListenTo(itsChooseButton);
	ListenTo(itsCancelButton);
	ListenTo(itsHelpButton);
	ListenTo(itsGenerateButton);
	ListenTo(itsStringsButton);

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

	itsBaseClassTxt->GetText()->SetText(bases);

	auto* image = jnew JXImage(GetDisplay(), gfg_main_window_icon);
	assert( image != nullptr );
	window->SetIcon(image);

	itsTable	=
		FunctionTable::Create(itsClass,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,kColHeaderHeight, 100,
			scrollbarSet->GetScrollEnclosure()->GetBoundsHeight() - kColHeaderHeight);
	assert(itsTable != nullptr);

	itsTable->FitToEnclosure(true, false);

	auto* widget =
		jnew JXColHeaderWidget(itsTable,
							  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  0,0, 100,kColHeaderHeight);
	assert(widget != nullptr);
	widget->FitToEnclosure(true, false);

	widget->SetColTitle(FunctionTable::kFUsed, JGetString("UseLabel::MainDirector"));
	widget->SetColTitle(FunctionTable::kFReturnType, JGetString("RTLabel::MainDirector"));
	widget->SetColTitle(FunctionTable::kFFunctionName, JGetString("FNLabel::MainDirector"));
	widget->SetColTitle(FunctionTable::kFConst, JGetString("ConstLabel::MainDirector"));
	widget->SetColTitle(FunctionTable::kFArgs, JGetString("ArgsLabel::MainDirector"));

	itsClassInput->SetIsRequired();
	itsDirInput->GetText()->SetText(outputPath);
	itsAuthorInput->GetText()->SetText(GetPrefsManager()->GetAuthor());
	itsCopyrightInput->GetText()->SetText(GetPrefsManager()->GetCopyright());

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
MainDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsChooseButton && message.Is(JXButton::kPushed))
	{
		if (itsDirInput != nullptr)
		{
			auto* dlog = JXChoosePathDialog::Create(JXChoosePathDialog::kAcceptReadable, itsDirInput->GetText()->GetText());
			if (dlog->DoDialog())
			{
				itsDirInput->GetText()->SetText(dlog->GetPath());
			}
		}
	}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowTOC();
	}
	else if (sender == itsCancelButton && message.Is(JXButton::kPushed))
	{
		Close();
	}
	else if (sender == itsGenerateButton && message.Is(JXButton::kPushed))
	{
		GetPrefsManager()->SetAuthor(itsAuthorInput->GetText()->GetText());
		GetPrefsManager()->SetCopyright(itsCopyrightInput->GetText()->GetText());

		if (Write())
		{
			//Close();
		}
	}
	else if (sender == itsStringsButton && message.Is(JXButton::kPushed))
	{
		GetPrefsManager()->EditPrefs();
	}
	else
	{
		JXWindowDirector::Receive(sender, message);
	}
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

	std::ofstream oh(headerfile.GetBytes());
	if (!oh.good())
	{
		JGetUserNotification()->ReportError(JGetString("CreateFileFailed::MainDirector"));
		return false;
	}

	JString bcname, bfname;
	JString bases;
	JString basefiles;
	JString baseconstructors;
	const JSize count	= itsClass->GetBaseClassCount();
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

	JString s	= GetPrefsManager()->GetHeaderComment(cname);
	s.Print(oh);

	const JUtf8Byte* map[] =
	{
		"class",    cname.GetBytes(),
		"basefile", basefiles.GetBytes(),
		"base",     bases.GetBytes()
	};
	s = JGetString("CLASS_HEADER_START", map, sizeof(map));

	s.Print(oh);

	oh << "\t";
	cname.Print(oh);
	oh << "();" << std::endl;

	oh << "\t~";
	cname.Print(oh);
	oh << "() override;" << std::endl << std::endl;

	itsClass->WritePublic(oh, true);

	oh << std::endl << std::endl;
	oh << "protected:" << std::endl << std::endl;

	itsClass->WriteProtected(oh, true);

	oh << std::endl << std::endl << "private:" << std::endl << std::endl;

	oh << "\t";
	cname.Print(oh);
	oh << "(const ";
	cname.Print(oh);
	oh << "& source);" << std::endl;

	oh << "\tconst ";
	cname.Print(oh);
	oh << "& operator=(const ";
	cname.Print(oh);
	oh << "& source);" << std::endl;

	oh << std::endl << std::endl << "};" << std::endl << std::endl << "#endif" << std::endl;

	oh.close();

	std::ofstream os(sourcefile.GetBytes());
	if (!os.good())
	{
		JGetUserNotification()->ReportError(JGetString("CreateFileFailed::MainDirector"));
		return false;
	}

	s	= GetPrefsManager()->GetSourceComment(cname, bases);
	s.Print(os);

	os << std::endl;
	os << "#include <";
	cname.Print(os);
	os << ".h>" << std::endl << std::endl;

	s	= GetPrefsManager()->GetConstructorComment();
	s.Print(os);

	const JUtf8Byte* map2[] =
	{
		"class",      cname.GetBytes(),
		"constructs", baseconstructors.GetBytes()
	};

	s = JGetString("CLASS_CONSTRUCTOR_DATA", map2, sizeof(map2));

	s.Print(os);

	s	= GetPrefsManager()->GetDestructorComment();
	s.Print(os);

	const JUtf8Byte* map3[] =
	{
		"class",      cname.GetBytes(),
		"constructs", baseconstructors.GetBytes()
	};

	s = JGetString("CLASS_DESTRUCTOR_DATA", map3, sizeof(map3));

	s.Print(os);

	itsClass->WritePublic(os);
	itsClass->WriteProtected(os);

	os.close();

	JString cmd	= "jcc " + headerfile + " " + sourcefile;

	JString errStr;
	JRunProgram(cmd, &errStr);

	return true;
}

/******************************************************************************
 FileDragSource.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_FileDragSource
#define _H_FileDragSource

#include <jx-af/jx/JXImageWidget.h>

class JXInputField;
class TextDocument;

class FileDragSource : public JXImageWidget
{
public:

	FileDragSource(TextDocument* doc, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	~FileDragSource();

	void	ProvideDirectSave(JXInputField* nameInput);

protected:

	void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;
	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	void	DNDInit(const JPoint& pt, const JXButtonStates& buttonStates,
					const JXKeyModifiers& modifiers) override;
	Atom	GetDNDAction(const JXContainer* target,
						 const JXButtonStates& buttonStates,
						 const JXKeyModifiers& modifiers) override;
	void	HandleDNDResponse(const JXContainer* target,
							  const bool dropAccepted, const Atom action) override;

private:

	TextDocument*	itsDoc;			// owns us
	JXInputField*	itsNameInput;	// nullptr => dragging for XDND
};


/******************************************************************************
 ProvideDirectSave

 ******************************************************************************/

inline void
FileDragSource::ProvideDirectSave
	(
	JXInputField* nameInput
	)
{
	itsNameInput = nameInput;
}

#endif

#ifndef _TEXTEDITINGFORM_H
#define _TEXTEDITINGFORM_H

#include <afxwin.h>
#include "Subject.h"

class Glyph;
class CharacterMetrics;
class Font;
class CaretController;
class ScrollController;
class Selection;

class TextEditingForm : public CFrameWnd, public Subject {
public:
	TextEditingForm();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	Glyph* note;
	Glyph* current;
	CharacterMetrics* characterMetrics;
	Font* font;
	CaretController* caretController;
	ScrollController* scrollController;
	Selection* selection;
protected:
	afx_msg void OnClose();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnImeComposition(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnImeChar(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnImeStartComposition(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cs, int cy);
	afx_msg void OnSetFocus(CWnd* pNewWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlag, CPoint point);
	afx_msg void OnMouseMove(UINT nFlag, CPoint point);
	afx_msg void OnCommandRange(UINT uID);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
public:
	BOOL GetIsComposing() const;
	void SetIsComposing(BOOL isComposing);
	char GetCurrentCharacter() const;
	void SetCurrentCharacter(char currentCharacter);
	TCHAR* GetCurrentBuffer() const;
	void SetCurrentBuffer(TCHAR(*currentBuffer));
private:
	BOOL isComposing;
	char currentCharacter;
	TCHAR currentBuffer[2];
};

inline BOOL TextEditingForm::GetIsComposing() const {
	return this->isComposing;
}

inline void TextEditingForm::SetIsComposing(BOOL isComposing) {
	this->isComposing = isComposing;
}

inline char TextEditingForm::GetCurrentCharacter() const {
	return this->currentCharacter;
}

inline void TextEditingForm::SetCurrentCharacter(char currentCharacter) {
	this->currentCharacter = currentCharacter;
}

inline TCHAR* TextEditingForm::GetCurrentBuffer() const {
	return const_cast<TCHAR*>(this->currentBuffer);
}

inline void TextEditingForm::SetCurrentBuffer(TCHAR(*currentBuffer)) {
	this->currentBuffer[0] = currentBuffer[0];
	this->currentBuffer[1] = currentBuffer[1];
}


#endif //_TEXTEDITINGFORM_H
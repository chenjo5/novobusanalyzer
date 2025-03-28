#ifndef BUSMASTER_APPLICATION_IMPORTLOGTOOLBAR_H_
#define BUSMASTER_APPLICATION_IMPORTLOGTOOLBAR_H_

#include "CNVTCToolBar.h"

// CImportLogToolbar

class CImportLogToolbar : public CNVTCToolBar {
    DECLARE_DYNAMIC(CImportLogToolbar)
private:
    HWND m_hWndScroll;
    CSliderCtrl m_omSliderMsgs;
public:
    CImportLogToolbar();
    virtual ~CImportLogToolbar();
    afx_msg void OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
    void SetSliderHandleWnd(const HWND& hWnd);
protected:
    DECLARE_MESSAGE_MAP()
};

#endif // BUSMASTER_APPLICATION_IMPORTLOGTOOLBAR_H_

// ***********************************************************************
// Code Created by James Michael Armstrong (https://github.com/BlazesRus)
// Tree Code based on https://www.codeproject.com/Articles/9887/CStaticTreeCtrl-A-CStatic-derived-custom-Tree-cont
// ***********************************************************************
#pragma once
#ifndef TreeView_Header
#define TreeView_Header

#include "StdBhvTree.h"
#include <string>
#include <typeinfo>
#include "..\BasicXMLGUI\UIntVector.h"
#include "DataNode.h"
#include "DataDictionary.h"
#include "UIntDic.h"
#include "APPDefines.h"

#include "ContextMenu.h"
#include "TreeNodeText.h"
//#include "TemplateMacros.h"
//#include "IniData/IndexedDictionary.h"
//#include "VariableLists/VariableTypeLists.h"
#include "..\DynMultiView\MultiViewDoc.h"

//#include "ArgList.h"
//#include "VariableTypeLists.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// <summary>
/// View for displaying information from .BHV format files converted into readable format
/// </summary>
/// <seealso cref="CView" />
class TreeView : public CView
{
	DECLARE_DYNCREATE(TreeView)
public:// Construction

	/// <summary>
	/// Initializes a new instance of the <see cref="TreeView"/> class.
	/// </summary>
	TreeView()
	{
		//m_pTopNode = new DataNode("No File Loaded");	// The tree top

		m_iIndent = 16;				// Indentation for tree branches
		m_iPadding = 4;				// Padding between tree and the control border

		m_bShowLines = TRUE;				// Show lines by default
		m_bScrollBarMessage = FALSE;			// Only relevant when calculating the scrollbar

		m_iDocHeight = 0;				// A polite yet meaningless default

		m_crDefaultTextColor = RGB(58, 58, 58);	// Some default
		m_crConnectingLines = RGB(128, 128, 128);	// Some default

		// Safeguards
		SetTextFont(8, FALSE, FALSE, "Arial Unicode MS");
		//dNode.Reset()

		//PrimaryTarget = "";
		//SecondaryTarget = "";
		
		//Test Code
		
	}

	/// <summary>
	/// Finalizes an instance of the <see cref="TreeView"/> class.
	/// </summary>
	virtual ~TreeView()
	{
		NodeBank.clear();
		NodeLinks.clear();

		m_Font.DeleteObject();
	}

	MultiViewDoc* GetDocument();

	// Attributes
protected:
	LOGFONT			m_lgFont;
	CFont			m_Font;
	COLORREF		m_crDefaultTextColor;
	COLORREF		m_crConnectingLines;

	BOOL			m_bShowLines;

	int				m_iDocHeight;
	BOOL			m_bScrollBarMessage;

	int				m_iLineHeight;
	int				m_iIndent;
	int				m_iPadding;
	
	UIntVector RootNodes;
	DataDictionary NodeBank;
	UIntDic NodeLinks;

	//Places all root-nodes at or below X coordinate of RootEnd
	long RootEnd;

	//string PrimaryTarget;
	//string SecondaryTarget;
public:
	/// <summary>
	/// Loads the data from file.
	/// </summary>
	/// <param name="FilePath">The file path.</param>
	/// <returns>bool</returns>
	bool LoadDataFromFile(std::string FilePath);

	/// <summary>
	/// Saves the loaded data to file.
	/// </summary>
	/// <param name="FilePath">The file path.</param>
	/// <returns>bool</returns>
	bool SaveDataToFile(std::string FilePath);

	/// <summary>
	/// Resets this instance.
	/// </summary>
	void Reset()
	{
		RootNodes.clear();
		NodeBank.clear();
		NodeLinks.clear();
	}

	// Operations
public:
	/// <summary>
	/// Sets the text font.
	/// </summary>
	/// <param name="nHeight">Height of the n.</param>
	/// <param name="bBold">The b bold.</param>
	/// <param name="bItalic">The b italic.</param>
	/// <param name="csFaceName">Name of the cs face.</param>
	virtual void SetTextFont(LONG nHeight, BOOL bBold, BOOL bItalic, const CString& csFaceName)
	{
		m_lgFont.lfHeight = -MulDiv(nHeight, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);
		m_lgFont.lfWidth = 0;
		m_lgFont.lfEscapement = 0;
		m_lgFont.lfOrientation = 0;
		m_lgFont.lfWeight = (bBold) ? FW_BOLD : FW_DONTCARE;
		m_lgFont.lfItalic = (BYTE)((bItalic) ? TRUE : FALSE);
		m_lgFont.lfUnderline = FALSE;
		m_lgFont.lfStrikeOut = FALSE;
		m_lgFont.lfCharSet = DEFAULT_CHARSET;
		m_lgFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		m_lgFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		m_lgFont.lfQuality = DEFAULT_QUALITY;
		m_lgFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

		STRCPY(m_lgFont.lfFaceName, csFaceName);

		if (m_Font.GetSafeHandle() != NULL)
			m_Font.DeleteObject();

		m_Font.CreateFontIndirect(&m_lgFont);

		// Calculate node height for this font
		CDC		*pDC = GetDC();
		int		iSaved = pDC->SaveDC();
		CFont*	pOldFont = pDC->SelectObject(&m_Font);

		// Calculate the height of this font with a character likely to be 'big'
		// and don't forget to add a little padding
		m_iLineHeight = pDC->GetTextExtent("X").cy + 4;

		pDC->SelectObject(pOldFont);
		pDC->RestoreDC(iSaved);
		ReleaseDC(pDC);
	}
	/// <summary>
	/// Sets the default color of the text.
	/// </summary>
	/// <param name="crText">The cr text.</param>
	virtual void SetDefaultTextColor(COLORREF crText)
	{
		m_crDefaultTextColor = crText;
	}

	/// <summary>
	/// Sets the text settings.
	/// </summary>
	/// <param name="nHeight">Height of the n.</param>
	/// <param name="bBold">The b bold.</param>
	/// <param name="bItalic">The b italic.</param>
	/// <param name="csFaceName">Name of the cs face.</param>
	/// <param name="crText">The cr text.</param>
	virtual void SetTextSettings(LONG nHeight, BOOL bBold, BOOL bItalic, const CString& csFaceName, COLORREF crText)
	{
		m_lgFont.lfHeight = -MulDiv(nHeight, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);
		m_lgFont.lfWidth = 0;
		m_lgFont.lfEscapement = 0;
		m_lgFont.lfOrientation = 0;
		m_lgFont.lfWeight = (bBold) ? FW_BOLD : FW_DONTCARE;
		m_lgFont.lfItalic = (BYTE)((bItalic) ? TRUE : FALSE);
		m_lgFont.lfUnderline = FALSE;
		m_lgFont.lfStrikeOut = FALSE;
		m_lgFont.lfCharSet = DEFAULT_CHARSET;
		m_lgFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		m_lgFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		m_lgFont.lfQuality = DEFAULT_QUALITY;
		m_lgFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

		STRCPY(m_lgFont.lfFaceName, csFaceName);

		if (m_Font.GetSafeHandle() != NULL)
			m_Font.DeleteObject();

		m_Font.CreateFontIndirect(&m_lgFont);

		// Calculate node height for this font
		CDC		*pDC = GetDC();
		int		iSaved = pDC->SaveDC();
		CFont*	pOldFont = pDC->SelectObject(&m_Font);

		// Calculate the height of this font with a character likely to be 'big'
		// and don't forget to add a little padding
		m_iLineHeight = pDC->GetTextExtent("X").cy + 4;

		pDC->SelectObject(pOldFont);
		pDC->RestoreDC(iSaved);
		ReleaseDC(pDC);
		m_crDefaultTextColor = crText;
	}


	/// <summary>
	/// Toggle node between open and closed
	/// </summary>
	/// <param name="pNode">The p node.</param>
	/// <param name="bInvalidate">The b invalidate.</param>
	void ToggleNode(DataNode* pNode, BOOL bInvalidate = FALSE)
	{
		ToggleNode(pNode, bInvalidate);
	}

protected:
	void DeleteNode(unsigned int nodeIndex, BOOL bInvalidate = FALSE)
	{
		UIntVector TargetNodes;
		TargetNodes.push_back(nodeIndex);
		DataNode* pNode = &NodeBank[nodeIndex];
		DataNode* targetNode;
        if (pNode->ParentIndex == 0)
        {
			RootNodes.DeleteFirstMatch(nodeIndex);
        }
		for(UIntVector::iterator targetNodeIndex = (pNode->ChildNodes).begin(), EndIndex = pNode->ChildNodes.end(); targetNodeIndex != EndIndex; ++targetNodeIndex)
		{
			targetNode = &this->NodeBank[*targetNodeIndex];
			AddAllSubNodes(targetNode, TargetNodes);
		}
		for(UIntVector::iterator targetNodeIndex = TargetNodes.begin(), EndIndex = TargetNodes.end(); targetNodeIndex != EndIndex; ++targetNodeIndex)
		{
			NodeBank.Remove(*targetNodeIndex);
		}
		if (bInvalidate)
			Invalidate();
	}

	void AddAllSubNodes(DataNode* pNode, UIntVector& TargetNodes)
	{
		DataNode* targetNode;
		for(UIntVector::iterator targetNodeIndex = pNode->ChildNodes.begin(), EndIndex = pNode->ChildNodes.end(); targetNodeIndex != EndIndex; ++targetNodeIndex)
		{
			targetNode = this->NodeBank[targetNodeIndex];
			AddAllSubNodes(targetNode, TargetNodes);
		}
	}

	int DrawNodesFromRoot(CDC* pDC)
	{

	}

	int DrawNodesRecursive(CDC* pDC, DataNode* pNode, int x, int y, CRect rFrame);

	
	int HowMuchTextFits(CDC* pDC, int iAvailableWidth, CString csText)
	{
		int iValidSoFar = csText.GetLength() - 1;					// Assume the entire text fits

		// If the text's pixel width is larger than what's available
		if (pDC->GetTextExtent(csText).cx > iAvailableWidth)
		{
			int iNextBlank = 0;	// Position of the next blank in text
			int iPixelWidth = 0;	// Text's pixel width

			// Loop until we can fit no more of the text
			while (iPixelWidth < iAvailableWidth)
			{
				iValidSoFar = iNextBlank;							// Record the char pos so far
				iNextBlank = csText.Find(' ', iNextBlank + 1);	// Advance one word at a time

				// Have reached the end of the string?
				if (iNextBlank == -1)
					iNextBlank = csText.GetLength();

				// Calculate the new width
				iPixelWidth = pDC->GetTextExtent(csText.Left(iNextBlank)).cx;
			}
		}

		return iValidSoFar;
	}
	
	void DrawLinesFromRoot(CDC* pDC)
	{

	}

	void DrawLinesRecursive(CDC* pDC, DataNode* pNode);


	void ResetScrollBar()
	{
		// Flag to avoid a call from OnSize while resetting the scrollbar
		m_bScrollBarMessage = TRUE;

		CRect rFrame;

		GetClientRect(rFrame);

		// Need for scrollbars?
		if (rFrame.Height() > m_iDocHeight + 8)
		{
			ShowScrollBar(SB_VERT, FALSE);	// Hide it
			SetScrollPos(SB_VERT, 0);
		}
		else
		{
			SCROLLINFO	si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE | SIF_RANGE;
			si.nPage = rFrame.Height();
			si.nMax = m_iDocHeight + 8;
			si.nMin = 0;

			SetScrollInfo(SB_VERT, &si);
			EnableScrollBarCtrl(SB_VERT, TRUE);
		}

		m_bScrollBarMessage = FALSE;
	}

	BOOL NodeTextDlg(CString& csText)
	{
		BOOL				bRet = FALSE;
		TreeNodeText	tntDlg;

		tntDlg.m_csItemText = csText;

		if (tntDlg.DoModal() == IDOK)
		{
			csText = tntDlg.m_csItemText;
			bRet = TRUE;
		}

		return bRet;
	}

	void AddNode(std::string TagName, unsigned int parentIndex=0, ArgList args)
	{
	
	}
	// Message handlers

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TreeView)
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_NCHITTEST || message == WM_NCLBUTTONDOWN || message == WM_NCLBUTTONDBLCLK)
			return ::DefWindowProc(m_hWnd, message, wParam, lParam);

		return CStatic::WindowProc(message, wParam, lParam);
	}
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	//{{AFX_MSG(TreeView)
	protected:
	virtual void OnDraw(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy)
	{
		// Setting the scroll sends its own size message. Prevent it thus avoiding an ugly loop.
		// Other than that, resizing the control means that the tree height may change (word-wrap).
		if (!m_bScrollBarMessage)
			ResetScrollBar();

		CStatic::OnSize(nType, cx, cy);
	}
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		int iScrollBarPos = GetScrollPos(SB_VERT);

		CRect rFrame;

		GetClientRect(rFrame);

		switch (nSBCode)
		{
		case SB_LINEUP:
			iScrollBarPos = max(iScrollBarPos - m_iLineHeight, 0);
			break;

		case SB_LINEDOWN:
			iScrollBarPos = min(iScrollBarPos + m_iLineHeight, GetScrollLimit(SB_VERT));
			break;

		case SB_PAGEUP:
			iScrollBarPos = max(iScrollBarPos - rFrame.Height(), 0);
			break;

		case SB_PAGEDOWN:
			iScrollBarPos = min(iScrollBarPos + rFrame.Height(), GetScrollLimit(SB_VERT));
			break;

		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
		{
			SCROLLINFO si;

			ZeroMemory(&si, sizeof(SCROLLINFO));

			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_TRACKPOS;

			if (GetScrollInfo(SB_VERT, &si, SIF_TRACKPOS))
				iScrollBarPos = si.nTrackPos;
			else
				iScrollBarPos = (UINT)nPos;
			break;
		}
		}

		SetScrollPos(SB_VERT, iScrollBarPos);

		Invalidate();

		CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
	}
	

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point)
	{
		//RecursiveNodeSearchByPoint(point);
		//switch(dNode.NodeDetected)
		//{
		//	case 1:ToggleNode(dNode.detectedDataNode, bInvalidate); break;
		//	default: 
		//		CStatic::OnLButtonUp(nFlags, point); break;
		//}
	}

	
	/// <summary>
	/// Called when [mouse wheel].
	/// </summary>
	/// <param name="nFlags">The n flags.</param>
	/// <param name="zDelta">The z delta.</param>
	/// <param name="pt">The pt.</param>
	/// <returns>BOOL.</returns>
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		// zDelta greater than 0, means rotating away from the user, that is, scrolling up
		OnVScroll((zDelta > 0) ? SB_LINEUP : SB_LINEDOWN, 0, NULL);

		return CStatic::OnMouseWheel(nFlags, zDelta, pt);
	}


	/// <summary>
	/// Called when [context menu].
	/// </summary>
	/// <param name="pWnd">The p WND.</param>
	/// <param name="point">The point.</param>
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point)
	{
		CPoint cp(point);

		// WM_CONTEXTMENU passes absolute coordinates, we need them local
		ScreenToClient(&cp);

		DataNode* TargetNode = nullptr;
		if(point.x<RootEnd)//Search for RootNode nearest to point
		{

		}
		else
		{

		}
		if(TargetNode!=nullptr)
		{
            ContextMenu contextMenuPopUp(&m_Font);

			contextMenuPopUp.CreatePopupMenu();

            // Get a device context so that it'll be possible for the context menu
            // to calculate the size of the menu item's text
            CDC		*pDC = GetDC();
            int		iSaved = pDC->SaveDC();
            CFont*	pOldFont = pDC->SelectObject(&m_Font);

            // ADDING MENU ITEMS - Start
            
			CString cs = TargetNode->TagName.c_str();
			cs = cs.Left(45) + ((TargetNode->TagName.size() > 45) ? _T("...") : _T(""));

			contextMenuPopUp.AppendMenuItem(MF_DISABLED, WM_APP, cs, pDC);
			contextMenuPopUp.AppendMenuItem(MF_SEPARATOR, 0, _T(""), pDC);
			switch(TargetNode->NodeType)
			{
			case 11:
				contextMenuPopUp.AppendMenuItem(MF_ENABLED, CM_DELETENODE, _T("Add New Event"), pDC);
				//contextMenuPopUp.AppendMenuItem(MF_ENABLED, CM_DELETENODE, _T("Copy Event to Other View"), pDC);
				break;
			case 12:
				contextMenuPopUp.AppendMenuItem(MF_ENABLED, CM_DELETENODE, _T("Add New Variable"), pDC);
				//contextMenuPopUp.AppendMenuItem(MF_ENABLED, CM_DELETENODE, _T("Copy Event to Other View"), pDC);
				contextMenuPopUp.AppendMenuItem(MF_ENABLED, CM_DELETENODE, _T("Delete Node"), pDC);
				break;
			case 13:
				break;
            case 95:
				contextMenuPopUp.AppendMenuItem(MF_ENABLED, CM_DELETENODE, _T("Load Tree from file"), pDC);
				contextMenuPopUp.AppendMenuItem(MF_ENABLED, CM_DELETENODE, _T("Save Tree to .xml file"), pDC);
                break;
			default:
				contextMenuPopUp.AppendMenuItem(MF_ENABLED, CM_DELETENODE, _T("Delete Node"), pDC);
				contextMenuPopUp.AppendMenuItem(MF_ENABLED, CM_MODIFYNODETEXT, _T("Modify Node Text"), pDC);
				//contextMenuPopUp.AppendMenuItem(MF_ENABLED, CM_TOGGLECONNECTINGLINES, _T("Toggle Connecting Lines"), pDC);
				break;
			}
            // ADDING MENU ITEMS - End

            // Display the context menu
            contextMenuPopUp.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

            // Clean up
            pDC->SelectObject(pOldFont);
            pDC->RestoreDC(iSaved);
            ReleaseDC(pDC);
		}
	}

	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


BEGIN_MESSAGE_MAP(TreeView, CStatic)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(CM_DELETENODE, OnCM_DeleteNode)
	//ON_COMMAND(CM_MODIFYNODETEXT, OnCM_ModifyNodeText)
	//ON_COMMAND(CM_TOGGLECONNECTINGLINES, OnCM_ToggleConnectingLines)
END_MESSAGE_MAP()
#endif

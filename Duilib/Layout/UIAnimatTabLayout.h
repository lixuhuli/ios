#ifndef __UIANIMATIONTABLAYOUT_H__
#define __UIANIMATIONTABLAYOUT_H__

namespace DuiLib
{
	class UILIB_API CAnimatTabLayoutUI 
		: public CTabLayoutUI
		, public CUIAnimation
	{
	public:
		CAnimatTabLayoutUI();
		~CAnimatTabLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		virtual bool SelectItem(int iIndex, bool bSetFocus = true);
		void AnimationSwitch();
		void DoEvent(TEventUI& event);
		void OnTimer( int nTimerID );

		virtual void OnAnimationStart(INT nAnimationID, BOOL bFirstLoop) {}
		virtual void OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
		virtual void OnAnimationStop(INT nAnimationID);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	protected:
		enum
		{
			TAB_ANIMATION_ID = 200,
			TAB_ANIMATION_ELLAPSE = 10,
			TAB_ANIMATION_FRAME_COUNT = 15,
		};

	private:
		bool m_bVertDirect;
		int m_nPosDirect;
		RECT m_rcCurPos;
		RECT m_rcItemOld;
		CControlUI* m_pCurrentControl;
		bool m_bVisibleFlag;
	};
}
#endif // __UIANIMATIONTABLAYOUT_H__
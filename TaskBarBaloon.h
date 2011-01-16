#include <wx/wx.h>
#include <wx/taskbar.h>
#include <wx/frame.h>

class TaskBarBaloon : public wxFrame
{
    public:
        TaskBarBaloon(wxString sTitle, wxString sMessage);
        virtual ~TaskBarBaloon() { delete timer; }
 
        /** painting bg */
        void OnPaint(wxPaintEvent& event);
        /** timer to close window */
        void OnTimerTick(wxTimerEvent & event);
        /** click on the baloon */
		void OnClick(wxMouseEvent & event);
		/** click on the baloon */
		void OnEscape(wxKeyEvent & event){};
 
        /** display the baloon and run the timer */
        void showBaloon(unsigned int iTimeout);
    private:
        wxTimer * timer;
 
        DECLARE_EVENT_TABLE();
 
};
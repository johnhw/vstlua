#ifndef __LineManager__
#define __LineManager__


#ifndef __vstgui__
#include "vstgui.sf/vstgui/vstgui.h"
#endif

#define N_LINES 1024

class LineManager
{
    public:
        LineManager(int lines, int wrap_width);
        void show(CRect size, int view_lines, int line_height,  CBitmap *background, CFrame *frame, CViewContainer *parent);
        void hide();
        ~LineManager();
        void clear();
        void set();
        void addLine(char *line, int iserror);            
        void down();
        void up();
        void pageup();    
        void pagedown();
    protected:
        void shiftLine();
        CTextLabel *outputLines[N_LINES];
        CViewContainer *container;
        int view_lines;
        int lines;
        char *line[N_LINES];
        int line_style[N_LINES];
        int curPtr;
        bool shown;

        int wrapLimit;
};

#endif

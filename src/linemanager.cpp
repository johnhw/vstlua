#include "vstlua.h"
#include "linemanager.h"


#include <stdio.h>
enum
{
    kNormalLine=0,
    kErrorLine=1,
};




CColor kNormalTextCColor = {255,255,255,255};

    void LineManager::show(CRect insize, int view_lines, int line_height,  CBitmap *background, CFrame *frame, CViewContainer *parent)
    {
        CRect newSize;
        
        //Get container size
        newSize(insize.left+5, insize.top+5, insize.right-5, insize.bottom-5);
        container = new CViewContainer(newSize, frame, background);
                             
        //find bmp offset
        CPoint pt;
        pt(0,0);
        pt = container->localToFrame(pt);        
        container->setBackgroundOffset(pt);
        CRect size;
        
        this->view_lines = view_lines;
        

        //Add the lines
        int cur_y = 0;
        int x = 0;
        int line_width = insize.right - insize.left;
        for(int i=0;i<view_lines;i++)
        {
                size(x,cur_y,line_width, cur_y+line_height);                
                
                cur_y+=line_height;
                outputLines[i] = new CTextLabel(size, "", 0, 0);    
                outputLines[i]->setBackColor(kTransparentCColor);
                outputLines[i]->setTransparency(true);                
                
                //outputLines[i]->setShadowColor(kWhiteCColor);            
                outputLines[i]->setHoriAlign(kLeftText);
                outputLines[i]->setFont(kNormalFontVerySmall);                                
                
                container->addView(outputLines[i]);                        
        }   
        
        curPtr = lines;
        parent->addView(container);
        set();        
        shown = true;
        

    }
    
    void LineManager::hide()
    {
       if(shown && container)
            container->removeAll();
        /*if(shown)
        {
            for(int i=0;i<view_lines;i++)            
                delete(outputLines[i]);
            
        shown = false;
        }        */
        
        shown = false;
    }
    
    
    LineManager::LineManager(int lines, int wrap_width)
    {    

        if(lines>N_LINES)
            lines = N_LINES;           
        this->lines = lines;
        
        wrapLimit = wrap_width;
        
        for(int i=0;i<lines;i++)        
            line[i] = new char[512];

        container=NULL;
        
        
        shown = false;
        clear();                
    }
    
    LineManager::~LineManager()
    {

        hide();
        
       for(int i=0;i<lines;i++)        
            delete [] line[i];
        
    }
    
    void LineManager::clear()
    {        

        for(int i=0;i<lines;i++)    
            {
                strncpy(line[i], "", 511);                       
                line_style[i] = kNormalLine;
            }
            set();
    }
    
    
    //Text has changed; update all the text boxes
    void LineManager::set()
    {
        
        //Compute the current view of the total lines
        int start, end;        
        end = curPtr;
        if(end>lines)
            end = lines;            
        start = end-view_lines;        
        if(start<0)
            start = 0;
        
        int k =0 ;
        for(int i=start;i<end;i++)        
        {            
            outputLines[k]->setText(line[i]);           
            
            
            if(line_style[i]==kNormalLine)            
                outputLines[k]->setFontColor(kBlackCColor);
            
            else       
            {
                outputLines[k]->setFontColor(kRedCColor);
                   outputLines[k]->setStyle(0);
            }
            
            k++;            
        }   
   
        
    }    
    
    //Scroll down one line
    void LineManager::down()
    {
        curPtr++;
        if(curPtr>lines)
            curPtr = lines;    
    }
    
    // Scroll up one line
    void LineManager::up()
    {
        curPtr--;
        if(curPtr<view_lines)
            curPtr = view_lines;    
    }
    
    
    //page up
    void LineManager::pageup()
    {
        curPtr-=view_lines;
        if(curPtr<view_lines)
            curPtr = view_lines;    
    }
    
    
    //page down
    void LineManager::pagedown()
    {
        curPtr+=view_lines;
        if(curPtr>lines)
            curPtr = lines;    
    }
    
    
    
    
    
    void LineManager::addLine(char *newline, int iserror)
    {
           
        
        
        int n = strlen(line[lines-1]);
        for(int i=0;i<strlen(newline);i++)
        {
            line_style[lines-1]=iserror;  
            if(newline[i]=='\n')
			{
                shiftLine();
				n = 0;
			}
			else if (newline[i] == '\t')
			{
				for (int j = 0; (!j) || (n & 3); j++)
				{
                line[lines-1][n] = ' ';
                line[lines-1][++n] = '\0';                                
				}
			}
            else if (newline[i] != '\r')
            {
                line[lines-1][n] = newline[i];
                line[lines-1][++n] = '\0';                                
            }
            
            if(n>wrapLimit)
			{
				n = 0;
                shiftLine();
			}
            
        }          
        if(shown)
           set();
    }
    
    void LineManager::shiftLine()
    {                
        //shift up
		char *p = line[0];
        for(int i=1;i<lines;i++)        
        {
			line[i - 1] = line[i];
            line_style[i-1] = line_style[i];          
         }   
		line[lines-1] = p;
        line[lines-1][0] = '\0';        
        line_style[lines-1] = 0;                        
        
        //reset scroll position to end
        curPtr=lines;
        
    }
    
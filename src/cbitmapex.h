#ifndef __CBitmapEx__
#define __CBitmapEx__

#include "public.sdk/source/vst2.x/aeffeditor.h"

#ifndef __vstgui__
#include "vstgui.sf/vstgui/vstgui.h"
#endif


class CBitmapEx : public CBitmap
{
public:
CBitmapEx(int) : CBitmap(resourceID) { };

#ifdef WINDOWS
bool loadFromFile(char *filename);
bool loadFromRes(int res, HMODULE handle);
#endif
};

CBitmapEx *loadBitmap(int resource, char *override=NULL);

#endif
// generated by Fast Light User Interface Designer (fluid) version 1.0110

#ifndef genmeta_h
#define genmeta_h

#include <lrtypes.h>

#include <map>
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Hold_Browser.H>
extern Fl_Hold_Browser *spriteui;
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Bar.H>
extern Fl_Menu_Item menu_[];
extern Fl_Double_Window *win;

class genmeta: public Fl_Widget {
private:
	u32 mx, my;
	u32 inx, iny;
	bool inside;
public:
	genmeta(int x, int y, int w, int h): Fl_Widget(x, y, w, h),
		mx(USHRT_MAX), my(USHRT_MAX),
		raw(NULL),
		scaled(NULL) {}

	int handle(int e);
	void draw();

	u8 *raw;
	u32 imgw, imgh;
	u32 scaledw, scaledh;

	Fl_RGB_Image *scaled;
};
extern genmeta *meta;

extern u8 tool;
extern u16 selected;

enum tooltype {
	SPR1x1,
	SPR2x1,
	SPR3x1,
	SPR4x1,
	SPR1x2,
	SPR2x2,
	SPR3x2,
	SPR4x2,
	SPR1x3,
	SPR2x3,
	SPR3x3,
	SPR4x3,
	SPR1x4,
	SPR2x4,
	SPR3x4,
	SPR4x4,
	MOVE
};

struct sprite {
	tooltype type;
	u16 x, y;
};

extern std::vector<sprite> spritelist;

void filechanged();

#endif

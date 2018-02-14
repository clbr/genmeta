// generated by Fast Light User Interface Designer (fluid) version 1.0110

#include "genmeta.h"
#include <lrtypes.h>

enum {
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

Fl_Browser *spritelist = (Fl_Browser *) 0;
static Fl_Double_Window *win;

static void newcb(Fl_Widget *, void *) {
}

static void savecb(Fl_Widget *, void *) {
}

static void opencb(Fl_Widget *, void *) {
}

static void quitcb(Fl_Widget *, void *) {
	win->hide();
}

class focusbutton: public Fl_Toggle_Button {
private:
	u8 who;
public:
	focusbutton(int x, int y, int w, int h, const char *lbl, u8 who):
		Fl_Toggle_Button(x, y, w, h, lbl) {
		this->who = who;
	}

	int handle(int e) {
		switch (e) {
			case FL_FOCUS:
				value(1);
				return 1;
			break;
			case FL_UNFOCUS:
				value(0);
				return 1;
			break;
		}
		return 0;
	}
};

Fl_Menu_Item menu_[] = {
	{"&File", 0, 0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
	{"&New", 0x4006e, newcb, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
	{"&Save", 0x40073, savecb, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{"&Open", 0x4006f, opencb, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
	{"&Quit", 0x40071, quitcb, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int main(int argc, char **argv)
{
	Fl::scheme("plastic");

	{
		win = new Fl_Double_Window(800, 565, "GenMeta");
		{
			Fl_Group *o = new Fl_Group(5, 25, 160, 535);
			{
				new focusbutton(5, 30, 40, 35, "1x1", SPR1x1);
			}	// focusbutton* o
			{
				new focusbutton(45, 30, 40, 35, "2x1", SPR2x1);
			}	// focusbutton* o
			{
				new focusbutton(85, 30, 40, 35, "3x1", SPR3x1);
			}	// focusbutton* o
			{
				new focusbutton(125, 30, 40, 35, "4x1", SPR4x1);
			}	// focusbutton* o
			{
				new focusbutton(5, 70, 40, 35, "1x2", SPR1x2);
			}	// focusbutton* o
			{
				new focusbutton(45, 70, 40, 35, "2x2", SPR2x2);
			}	// focusbutton* o
			{
				new focusbutton(85, 70, 40, 35, "3x2", SPR3x2);
			}	// focusbutton* o
			{
				new focusbutton(125, 70, 40, 35, "4x2", SPR4x2);
			}	// focusbutton* o
			{
				new focusbutton(5, 110, 40, 35, "1x3", SPR1x3);
			}	// focusbutton* o
			{
				new focusbutton(45, 110, 40, 35, "2x3", SPR2x3);
			}	// focusbutton* o
			{
				new focusbutton(85, 110, 40, 35, "3x3", SPR3x3);
			}	// focusbutton* o
			{
				new focusbutton(125, 110, 40, 35, "4x3", SPR4x3);
			}	// focusbutton* o
			{
				new focusbutton(5, 150, 40, 35, "1x4", SPR1x4);
			}	// focusbutton* o
			{
				new focusbutton(45, 150, 40, 35, "2x4", SPR2x4);
			}	// focusbutton* o
			{
				new focusbutton(85, 150, 40, 35, "3x4", SPR3x4);
			}	// focusbutton* o
			{
				new focusbutton(125, 150, 40, 35, "4x4", SPR4x4);
			}	// focusbutton* o
			{
				new focusbutton(5, 190, 64, 20, "Move", MOVE);
			}	// focusbutton* o
			{
				spritelist = new Fl_Browser(5, 215, 160, 340);
				Fl_Group::current()->resizable(spritelist);
			}	// Fl_Browser* spritelist
			o->end();
		}		// Fl_Group* o
		{
			genmeta *o = new genmeta(175, 29, 615, 527);
			o->box(FL_FLAT_BOX);
			o->color((Fl_Color) 41);
			Fl_Group::current()->resizable(o);
		}		// Fl_Box* o
		{
			Fl_Menu_Bar *o = new Fl_Menu_Bar(0, 0, 800, 20);
			o->menu(menu_);
		}		// Fl_Menu_Bar* o
		win->size_range(800, 565);
		win->end();
	}			// Fl_Double_Window* o
	win->show(argc, argv);
	return Fl::run();
}

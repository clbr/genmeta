// generated by Fast Light User Interface Designer (fluid) version 1.0110

#include "genmeta.h"
#include <png.h>

Fl_Browser *spritelist = (Fl_Browser *) 0;
genmeta *meta;
static Fl_Double_Window *win;
u8 tool;
char basefname[PATH_MAX];

static void newmeta(const char * const fname) {

	std::map<u8, u8> counter;
	std::map<u8, u8>::const_iterator it;

//	printf("Loading sprite %s\n", fname);
	spritelist->clear();
	free(meta->raw);
	meta->raw = NULL;

	FILE *f = fopen(fname, "r");
	if (!f) {
		fl_alert("Can't open %s", fname);
		return;
	}

	strcpy(basefname, fname);
	int len = strlen(fname);
	basefname[len - 4] = '\0';

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (!png_ptr) abort();
	png_infop info = png_create_info_struct(png_ptr);
	if (!info) abort();
	if (setjmp(png_jmpbuf(png_ptr))) abort();

	png_init_io(png_ptr, f);
	png_read_png(png_ptr, info,
		PNG_TRANSFORM_PACKING|PNG_TRANSFORM_STRIP_16|PNG_TRANSFORM_STRIP_ALPHA,
		NULL);

	const u32 imgw = png_get_image_width(png_ptr, info);
	const u32 imgh = png_get_image_height(png_ptr, info);
	const u8 type = png_get_color_type(png_ptr, info);
	const u8 depth = png_get_bit_depth(png_ptr, info);
	u8 **rows = png_get_rows(png_ptr, info);
	png_color *colors;
	int num_colors;

	if (type != PNG_COLOR_TYPE_PALETTE) {
		fl_alert("%s is not paletted", fname);
		goto out;
	} else if (depth != 8) {
		fl_alert("Depth not 8 (%u) - maybe you have old libpng?\n", depth);
		goto out;
	}

	png_get_PLTE(png_ptr, info, &colors, &num_colors);

	if (num_colors > 16) {
		fl_alert("Too many colors (%u)", num_colors);
		goto out;
	}

	u32 i;
	meta->raw = (u8 *) calloc(imgw, imgh);
	meta->imgw = imgw;
	meta->imgh = imgh;
	for (i = 0; i < imgh; i++) {
		u8 * const target = meta->raw + imgw * i;
		memcpy(target, &rows[i][0], imgw);
	}

	// Detect the bg pixel
	u8 px[4], found;
	px[0] = meta->raw[0];
	px[1] = meta->raw[imgw - 1];
	px[2] = meta->raw[(imgh - 1) * imgw];
	px[3] = meta->raw[(imgh - 1) * imgw + imgw - 1];

	counter[px[0]]++;
	counter[px[1]]++;
	counter[px[2]]++;
	counter[px[3]]++;

	found = 0;
	meta->bgpixel = 255;
	for (it = counter.begin(); it != counter.end(); it++) {
		if (it->second > 2) {
			found = 1;
			meta->bgpixel = it->first;
		}
	}

	if (!found)
		fl_alert("Couldn't determine background color from corners, finish detection won't work");

out:
	fclose(f);
	png_destroy_info_struct(png_ptr, &info);
	png_destroy_read_struct(&png_ptr, NULL, NULL);
}

static void newcb(Fl_Widget *, void *) {
	const char *name = fl_file_chooser("Load PNG sprite", "*.png", "", 1);
	if (name)
		newmeta(name);
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
				tool = who;
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
			meta = o;
		}		// Fl_Box* o
		{
			Fl_Menu_Bar *o = new Fl_Menu_Bar(0, 0, 800, 20);
			o->menu(menu_);
		}		// Fl_Menu_Bar* o
		win->size_range(800, 565);
		win->end();
	}			// Fl_Double_Window* o

	if (argc > 1 && strstr(argv[argc - 1], ".png"))
		newmeta(argv[argc - 1]);

	win->show(argc, argv);
	return Fl::run();
}

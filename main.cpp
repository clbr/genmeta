// generated by Fast Light User Interface Designer (fluid) version 1.0110

#include "genmeta.h"
#include <png.h>

Fl_Hold_Browser *spriteui = (Fl_Hold_Browser *) 0;
genmeta *meta;
Fl_Double_Window *win;
u8 tool;
u16 selected;
char basefname[PATH_MAX];
std::vector<sprite> spritelist;

static int num_colors;
png_color palette[16];

void filechanged() {
	win->label("GenMeta *");
}

void nukenewline(char buf[]) {

	u32 i;
	for (i = 0; buf[i]; i++) {
		if (buf[i] == '\n') {
			buf[i] = '\0';
			break;
		}
	}
}

static void newmeta(const char * const fname) {

//	printf("Loading sprite %s\n", fname);
	spriteui->clear();
	spritelist.clear();
	free(meta->raw);
	meta->raw = NULL;

	FILE *f = fopen(fname, "rb");
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

	memcpy(palette, colors, sizeof(png_color) * num_colors);

	u32 i;
	meta->raw = (u8 *) calloc(imgw, imgh);
	meta->imgw = imgw;
	meta->imgh = imgh;
	for (i = 0; i < imgh; i++) {
		u8 * const target = meta->raw + imgw * i;
		memcpy(target, &rows[i][0], imgw);
	}

	Fl_PNG_Image *png;
	png = new Fl_PNG_Image(fname);

	if (meta->scaled)
		delete meta->scaled;
	meta->scaled = (Fl_RGB_Image *) png->copy(imgw * 4, imgh * 4);
	meta->scaledw = imgw * 4;
	meta->scaledh = imgh * 4;

	delete png;

	meta->redraw();

out:
	fclose(f);
	png_destroy_info_struct(png_ptr, &info);
	png_destroy_read_struct(&png_ptr, NULL, NULL);

	win->redraw();
}

static void selectsprite(Fl_Widget *w, void *) {
	Fl_Hold_Browser *b = (Fl_Hold_Browser *) w;

	// Pick from list
	tool = MOVE;
	selected = b->value();

	if (!selected)
		selected = USHRT_MAX;
	else
		selected--;
}

static void newcb(Fl_Widget *, void *) {
	const char *name = fl_file_chooser("Load PNG sprite", "*.png", "", 1);
	if (name)
		newmeta(name);
}

static void copytile(const sprite &spr, u32 xs, u32 ys,
			u8 * const dst, const u32 tiles, const u32 curtile) {
	xs *= 8;
	ys *= 8;

	const u32 dstX = curtile * 8;

	u32 x, y, ox, oy;
	for (y = spr.y + ys, oy = 0; y < spr.y + ys + 8; y++, oy++) {
		for (x = spr.x + xs, ox = 0; x < spr.x + xs + 8; x++, ox++) {
			u8 px = 0;
			if (x < meta->imgw && y < meta->imgh)
				px = meta->raw[y * meta->imgw + x];

			dst[oy * tiles * 8 + dstX + ox] = px;
		}
	}
}

static void savepng(FILE *f, const u8 * const data, const u32 w) {

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) abort();
	png_infop info = png_create_info_struct(png_ptr);
	if (!info) abort();
	if (setjmp(png_jmpbuf(png_ptr))) abort();

	png_init_io(png_ptr, f);
	png_set_IHDR(png_ptr, info, w, 8, 4, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_set_PLTE(png_ptr, info, palette, num_colors);
	png_write_info(png_ptr, info);
	png_set_packing(png_ptr);

	u32 i;
	for (i = 0; i < 8; i++) {
		png_write_row(png_ptr, (png_byte *) data + i * w);
	}
	png_write_end(png_ptr, NULL);

	png_destroy_info_struct(png_ptr, &info);
	png_destroy_write_struct(&png_ptr, NULL);
}

static void savecb(Fl_Widget *, void *) {

	if (!spritelist.size()) {
		fl_alert("Nothing to save!");
		return;
	}

	char path[PATH_MAX];
	FILE *f;
	u32 i;

	const char *shortname = strrchr(basefname, '/');
	if (!shortname)
		shortname = basefname;

	// Save metasprite
	sprintf(path, "%s.meta", basefname);
	f = fopen(path, "wb");
	if (!f) {
		fl_alert("Can't open %s", path);
		return;
	}

	fprintf(f, "%s.png\n", basefname);

	for (i = 0; i < spritelist.size(); i++) {
		fprintf(f, "%s\n", spriteui->text(i + 1));
	}

	fclose(f);

	// Count tiles
	u32 tiles = 0;
	for (std::vector<sprite>::const_iterator it = spritelist.begin();
		it != spritelist.end(); it++) {
		tiles += (sprw[it->type] / 8) * (sprh[it->type] / 8);
	}

	// Save header TODO
	sprintf(path, "%s.h", basefname);
	f = fopen(path, "wb");
	if (!f) {
		fl_alert("Can't open %s", path);
		return;
	}

	fprintf(f, "#ifndef %s_sprite_h\n#define %s_sprite_h\n\n", shortname, shortname);
	fprintf(f, "// Format: x, y, size, offset. Set defines for OFFX, OFFY, and BASE.\n");
	fprintf(f, "const s16 %s_sprite[] = {\n", shortname);
	fprintf(f, "\t%u, // tiles\n", tiles);

	for (std::vector<sprite>::const_iterator it = spritelist.begin();
		it != spritelist.end(); it++) {

	}

	fprintf(f, "};\n\n#endif\n");
	fclose(f);

	// Save sprites
	u8 *data = (u8 *) calloc(tiles, 64);

	i = 0;
	for (std::vector<sprite>::const_iterator it = spritelist.begin();
		it != spritelist.end(); it++) {

		const u32 w = sprw[it->type] / 8;
		const u32 h = sprh[it->type] / 8;
		u32 x, y;

		for (x = 0; x < w; x++) {
			for (y = 0; y < h; y++, i++) {
				copytile(*it, x, y, data, tiles, i);
			}
		}
	}

	sprintf(path, "%s_sprite.png", basefname);
	f = fopen(path, "wb");
	if (!f) {
		fl_alert("Can't open %s", path);
		free(data);
		return;
	}

	savepng(f, data, tiles * 8);

	free(data);
	fclose(f);

	win->label("GenMeta");
}

static void opencb(Fl_Widget *, void *) {
	const char *name = fl_file_chooser("Open metasprite", "*.meta", "", 1);
	if (!name)
		return;

	FILE *f = fopen(name, "rb");
	if (!f) {
		fl_alert("Can't open %s", name);
		return;
	}

	char buf[1024];
	bool nameloaded = false;
	while (fgets(buf, 1024, f)) {
		nukenewline(buf);

		// Load image, list of things
		if (!nameloaded) {
			nameloaded = true;
			newmeta(buf);
			continue;
		}

		sprite s;
		s.type = MOVE;
		u32 w, h, i;
		if (sscanf(buf, "%ux%u %hu,%hu", &w, &h, &s.x, &s.y) != 4) {
			fl_alert("%s is corrupted!", name);
			return;
		}

		// Find type enum
		w *= 8;
		h *= 8;
		for (i = 0; i < MOVE; i++) {
			if (sprw[i] == w && sprh[i] == h) {
				s.type = (tooltype) i;
				break;
			}
		}

		if (s.type == MOVE) {
			fl_alert("%s is corrupted!", name);
			return;
		}

		spritelist.push_back(s);
		spriteui->add(buf);
	}

	fclose(f);
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
			case FL_PUSH:
				Fl::focus(this);
				handle(FL_FOCUS);
				return 1;
			break;
			case FL_FOCUS:
				value(1);
				tool = who;
				selected = USHRT_MAX;
				meta->redraw();
				return 1;
			break;
			case FL_UNFOCUS:
				value(0);
				return 1;
			break;
		}
		return Fl_Toggle_Button::handle(e);
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
				spriteui = new Fl_Hold_Browser(5, 215, 160, 340);
				spriteui->callback(selectsprite);
				Fl_Group::current()->resizable(spriteui);
			}	// Fl_Browser* spriteui
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

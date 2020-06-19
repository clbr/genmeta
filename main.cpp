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
	win->label("SNESMeta *");
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

static const u8 zeroes[16] = { 0 };

static void pack(u8 *packing, const sprite &spr, u32 &sx, u32 &sy, const u32 maxh,
			const u32 idx) {

	// Greedily find the next position where this sprite fits
	const u32 w = sprw[spr.type] / 8;
	const u32 h = sprh[spr.type] / 8;

	u32 x, y;
	for (y = 0; y < maxh; y++) {
		for (x = 0; x < 16 - w; x++) {
			u32 line;
			bool ok = true;
			for (line = 0; line < h; line++) {
				if (memcmp(zeroes, &packing[(y + line) * 16 + x], w)) {
					ok = false;
					break;
				}
			}

			if (ok) {
				// Place it
				for (line = 0; line < h; line++) {
					memset(&packing[(y + line) * 16 + x], 1 + idx, w);
				}

				sx = x * 8;
				sy = y * 8;
				return;
			} else {
				continue;
			}
		}
	}

	// Not reached, couldn't place it
	abort();
}

static void copysprite(const sprite &spr, u8 * const dst, const u32 dx, const u32 dy) {

	const u32 w = sprw[spr.type];
	const u32 h = sprh[spr.type];

	u32 y, x;
	for (y = 0; y < h; y++) {
		const u32 sy = spr.y + y;
		for (x = 0; x < w; x++) {
			u8 px = 0;

			const u32 sx = spr.x + x;
			if (sx < meta->imgw && sy < meta->imgh)
				px = meta->raw[sy * meta->imgw + sx];

			dst[(dy + y) * 128 + dx + x] = px;
		}
	}
}

static void savepng(FILE *f, const u8 * const data, const u32 h) {

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) abort();
	png_infop info = png_create_info_struct(png_ptr);
	if (!info) abort();
	if (setjmp(png_jmpbuf(png_ptr))) abort();

	png_init_io(png_ptr, f);
	png_set_IHDR(png_ptr, info, 128, h, 4, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_set_PLTE(png_ptr, info, palette, num_colors);
	png_write_info(png_ptr, info);
	png_set_packing(png_ptr);

	u32 i;
	for (i = 0; i < h; i++) {
		png_write_row(png_ptr, (png_byte *) data + i * 128);
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
	else
		shortname++;

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

	// Save header
	sprintf(path, "%s.h", basefname);
	f = fopen(path, "wb");
	if (!f) {
		fl_alert("Can't open %s", path);
		return;
	}

	fprintf(f, "#ifndef %s_sprite_h\n#define %s_sprite_h\n\n", shortname, shortname);
	fprintf(f, "#define %s_sprite_tiles %u\n", shortname, tiles);
	fprintf(f, "// Format: x, y, offset, attr. Set defines for (FLIP)OFFX, OFFY, and BASE.\n");

	fprintf(f, "const u8 %s_sprite[] = {\n", shortname);

	u32 t = 0;
	for (std::vector<sprite>::const_iterator it = spritelist.begin();
		it != spritelist.end(); it++) {

		fprintf(f, "\tOFFX + %u, OFFY + %u, BASE + %u, 0, // %ux%u\n",
			it->x, it->y, t,
			sprw[it->type], sprh[it->type]);

		t += (sprw[it->type] / 8) * (sprh[it->type] / 8);
	}

	fprintf(f, "\t128\n");
	fprintf(f, "};\n\n");

	fprintf(f, "const s16 %s_sprite_flip[] = {\n", shortname);

	t = 0;
	for (std::vector<sprite>::const_iterator it = spritelist.begin();
		it != spritelist.end(); it++) {

		fprintf(f, "\tFLIPOFFX + %d, OFFY + %u, BASE + %u, SPRITE_ATTR_FULL(0, 0, 0, 1, 0), // %ux%u\n",
			meta->imgw - it->x - sprw[it->type], it->y, t,
			sprw[it->type], sprh[it->type]);

		t += (sprw[it->type] / 8) * (sprh[it->type] / 8);
	}

	fprintf(f, "\t128\n");
	fprintf(f, "};\n\n#endif\n");
	fclose(f);

	// Save sprites
	const u32 maxh = tiles / 16 * 2;
	u8 *data = (u8 *) calloc(maxh, 64 * 16);
	u8 *packing = (u8 *) calloc(16, maxh);

	i = 0;
	for (std::vector<sprite>::const_iterator it = spritelist.begin();
		it != spritelist.end(); it++, i++) {

		u32 sx, sy;
		pack(packing, *it, sx, sy, maxh, i);
		copysprite(*it, data, sx, sy);
	}

	// Find the height
	u32 h;
	for (h = 0; h < maxh; h++) {
		if (!memcmp(zeroes, &packing[h * 16], 16))
			break;
	}

	#if 0
	// Print the packing
	for (i = 0; i < h; i++) {
		printf("%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u\n",
			packing[i * 16 + 0],
			packing[i * 16 + 1],
			packing[i * 16 + 2],
			packing[i * 16 + 3],
			packing[i * 16 + 4],
			packing[i * 16 + 5],
			packing[i * 16 + 6],
			packing[i * 16 + 7],
			packing[i * 16 + 8],
			packing[i * 16 + 9],
			packing[i * 16 + 10],
			packing[i * 16 + 11],
			packing[i * 16 + 12],
			packing[i * 16 + 13],
			packing[i * 16 + 14],
			packing[i * 16 + 15]);
	}
	#endif

	sprintf(path, "%s_sprite.png", basefname);
	f = fopen(path, "wb");
	if (!f) {
		fl_alert("Can't open %s", path);
		free(data);
		free(packing);
		return;
	}

	savepng(f, data, h * 8);

	free(data);
	free(packing);
	fclose(f);

	win->label("SNESMeta");
}

static void opencb(Fl_Widget *, void *ptr) {
	const char *name = ptr ? (const char *) ptr : fl_file_chooser("Open metasprite", "*.meta", "", 1);
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

static u8 strrdiff(const char * const str, const char * const end) {

	const u32 len = strlen(str);
	const u32 endlen = strlen(end);

	if (endlen > len)
		return 1;

	return strcmp(&str[len - endlen], end) != 0;
}

int main(int argc, char **argv)
{
	Fl::scheme("plastic");

	{
		win = new Fl_Double_Window(800, 565, "SNESMeta");
		{
			Fl_Group *o = new Fl_Group(5, 25, 160, 535);
			{
				new focusbutton(5, 30, 40, 35, "1x1", SPR1x1);
			}	// focusbutton* o
			{
				new focusbutton(45, 30, 40, 35, "2x2", SPR2x2);
			}	// focusbutton* o
			{
				new focusbutton(85, 30, 40, 35, "4x4", SPR4x4);
			}	// focusbutton* o
			{
				new focusbutton(125, 30, 40, 35, "8x8", SPR8x8);
			}	// focusbutton* o
			{
				new focusbutton(5, 70, 64, 20, "Move", MOVE);
			}	// focusbutton* o
			{
				spriteui = new Fl_Hold_Browser(5, 95, 160, 460);
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

	if (argc > 1) {
		if (!strrdiff(argv[argc - 1], ".png"))
			newmeta(argv[argc - 1]);
		if (!strrdiff(argv[argc - 1], ".meta"))
			opencb(NULL, argv[argc - 1]);
	}

	win->show(argc, argv);
	return Fl::run();
}

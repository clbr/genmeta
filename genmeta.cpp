#include "genmeta.h"
#include "colors.h"

static u32 spritebits[32 * 4 * 32 * 4];
static Fl_RGB_Image spriteimg((u8 *) spritebits, 32 * 4, 32 * 4, 4);

static const u8 sprw[MOVE] = {
	/*SPR1x1] = */ 8,
	/*SPR2x1] = */ 16,
	/*SPR3x1] = */ 24,
	/*SPR4x1] = */ 32,
	/*SPR1x2] = */ 8,
	/*SPR2x2] = */ 16,
	/*SPR3x2] = */ 24,
	/*SPR4x2] = */ 32,
	/*SPR1x3] = */ 8,
	/*SPR2x3] = */ 16,
	/*SPR3x3] = */ 24,
	/*SPR4x3] = */ 32,
	/*SPR1x4] = */ 8,
	/*SPR2x4] = */ 16,
	/*SPR3x4] = */ 24,
	/*SPR4x4] = */ 32,
};

static const u8 sprh[MOVE] = {
	/*SPR1x1] = */ 8,
	/*SPR2x1] = */ 8,
	/*SPR3x1] = */ 8,
	/*SPR4x1] = */ 8,
	/*SPR1x2] = */ 16,
	/*SPR2x2] = */ 16,
	/*SPR3x2] = */ 16,
	/*SPR4x2] = */ 16,
	/*SPR1x3] = */ 24,
	/*SPR2x3] = */ 24,
	/*SPR3x3] = */ 24,
	/*SPR4x3] = */ 24,
	/*SPR1x4] = */ 32,
	/*SPR2x4] = */ 32,
	/*SPR3x4] = */ 32,
	/*SPR4x4] = */ 32,
};

int genmeta::handle(int e) {

	const u32 sx = x() + (w() - scaledw) / 2;
	const u32 sy = y() + (h() - scaledh) / 2;

	switch (e) {
		case FL_ENTER:
			return 1;
		break;
		case FL_LEAVE:
			mx = my = 65535;
			return 1;
		break;
		case FL_MOVE:
			mx = Fl::event_x();
			my = Fl::event_y();

			if (mx >= sx && mx < sx + scaledw &&
				my >= sy && my < sy + scaledh) {
				inside = true;

				inx = (mx - sx) / 4;
				iny = (my - sy) / 4;
			} else {
				inside = false;
			}

			redraw();

			return 1;
		break;
		case FL_PUSH:
			if (inside) {
				if (tool == MOVE) {
					// TODO if selected from list, or pick below
				} else {
					// TODO place this sprite
				}
			}

			return 1;
		break;
	}

	return 0;
}

static u8 uncovered(const u32 x, const u32 y) {

	for (std::vector<sprite>::const_iterator it = spritelist.begin();
		it != spritelist.end(); it++) {
		if (x < it->x ||
			x > it->x + sprw[it->type] ||
			y < it->y ||
			y > it->y + sprh[it->type])
			continue;
		// It was covered by this sprite.
		return 0;
	}

	return 1;
}

static u8 checkok() {
	u32 x, y;

	if (!meta->raw)
		return 0;

	for (y = 0; y < meta->imgh; y++) {
		for (x = 0; x < meta->imgw; x++) {
			const u8 px = meta->raw[y * meta->imgw + x];

			if (px && uncovered(x, y))
				return 0;
		}
	}

	return 1;
}

static void fillcolor(u16 num, const u8 alpha) {
	num %= 63;

	const u32 col = uniqcolors[num * 3 + 0] |
			uniqcolors[num * 3 + 1] << 8 |
			uniqcolors[num * 3 + 2] << 16 |
			alpha << 24;

	u32 i;
	for (i = 0; i < 32 * 4 * 32 * 4; i++)
		spritebits[i] = col;

	spriteimg.uncache();
}

void genmeta::draw() {
	fl_rectf(x(), y(), w(), h(), checkok() ? FL_GREEN - 1 : color());

	if (!raw)
		return;

	fl_push_clip(x(), y(), w(), h());

	const u32 sx = x() + (w() - scaledw) / 2;
	const u32 sy = y() + (h() - scaledh) / 2;
	scaled->draw(sx, sy);

	// Draw black lines every 8 pixels
	fl_color(FL_GRAY0 + 2);
	u32 px, py;
	for (py = sy + 32; py < sy + scaledh; py += 32) {
		fl_line(sx, py, sx + scaledw, py);
	}
	for (px = sx + 32; px < sx + scaledw; px += 32) {
		fl_line(px, sy, px, sy + scaledh);
	}

	// Draw every placed sprite TODO
	for (std::vector<sprite>::const_iterator it = spritelist.begin();
		it != spritelist.end(); it++) {
	}

	// Draw the being-placed sprite
	if (inside && tool != MOVE) {
		fillcolor(spritelist.size(), 128);
		spriteimg.draw(sx + inx * 4, sy + iny * 4, sprw[tool] * 4, sprh[tool] * 4);
	}

	fl_pop_clip();
}

#include "genmeta.h"
#include "colors.h"

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
			mx = Fl::event_x() - x();
			my = Fl::event_y() - y();

			if (mx >= sx && mx < sx + scaledw &&
				my >= sy && my < sy + scaledh) {
				inside = true;

				inx = (mx - sx) / 4;
				iny = (my - sy) / 4;
			} else {
				inside = false;
			}

			redraw();
		break;
		case FL_PUSH:
		break;
	}

	return 0;
}

static u8 uncovered(const u32 x, const u32 y) {
	// TODO
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

	fl_pop_clip();
}

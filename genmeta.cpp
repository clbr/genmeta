#include "genmeta.h"
#include "colors.h"

int genmeta::handle(int e) {

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
}

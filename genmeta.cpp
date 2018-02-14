#include "genmeta.h"
#include "colors.h"

int genmeta::handle(int e) {

	return 0;
}

void genmeta::draw() {
	fl_rectf(x(), y(), w(), h(), color()); // TODO green if all ok
}

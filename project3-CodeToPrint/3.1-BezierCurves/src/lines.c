#include <cairo.h>
#include <cairo-svg.h>

#include <time.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH_PT 1728
#define HEIGHT_PT 288

cairo_t* cr;
cairo_surface_t* surface;

int main() {
    srand(time(0));

    // setup cairo
    surface = cairo_svg_surface_create("line.svg", WIDTH_PT, HEIGHT_PT);
    cr = cairo_create(surface);

    int originX = -3000;
    int originY = 0;

    int nsc = 0;

    while(originX < WIDTH_PT + 3000) {
        cairo_move_to(cr, originX, 0);
        cairo_curve_to(cr, (WIDTH_PT / 2) + originX + (rand() % 30) - 15, rand() % 20, rand() % 150, HEIGHT_PT, (WIDTH_PT / 2) + originX, HEIGHT_PT);
        originX += 10;
    }
    
    cairo_set_source_rgb(cr, 100, 100, 100);
    cairo_set_line_width(cr, 1);
    cairo_stroke(cr);

    cairo_surface_finish(surface);

    return 0;
}
#include <stdio.h>
#include <cairo.h>

#define IMAGE_WIDTH_PIXELS 11 * 300
#define IMAGE_HEIGHT_PIXELS 17 * 300

int main(int argc, char* argv[]) {

    if(argc < 2) {
        printf("Include a path to output to as the first argument.\n");
        return 2;
    }

    // Create a surface to write to
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, IMAGE_WIDTH_PIXELS, IMAGE_HEIGHT_PIXELS);
    cairo_t *c = cairo_create(surface);

    // Write the surface to final PNG
    // TODO: check this path is accessible
    cairo_status_t writeOperation = cairo_surface_write_to_png(surface, argv[1]);
    // TODO: error checking

	return 0;
}

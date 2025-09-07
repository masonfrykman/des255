#include <stdio.h>
#include <cairo.h>

#define IMAGE_WIDTH_PIXELS 11 * 300
#define IMAGE_HEIGHT_PIXELS 17 * 300

void printusage(char* execName) {
    printf("usage: %s outFile eyesDir\n", execName);
}

int main(int argc, char* argv[]) {

    if(argc < 3) {
        printusage(argv[0]);
        return 2;
    }

    // Create a surface to draw to
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, IMAGE_WIDTH_PIXELS, IMAGE_HEIGHT_PIXELS);
    cairo_t *c = cairo_create(surface);

    // Write the surface to final PNG
    cairo_status_t writeOperation = cairo_surface_write_to_png(surface, argv[1]);
    if(writeOperation == CAIRO_STATUS_WRITE_ERROR) {
        printf("failed to write the final png due to an i/o error (CAIRO_STATUS_WRITE_ERROR)\n");
    } else if(writeOperation == CAIRO_STATUS_NO_MEMORY) {
        printf("failed to write the final png due to a memory error (CAIRO_STATUS_NO_MEMORY)\n");
    } else if(writeOperation == CAIRO_STATUS_SURFACE_TYPE_MISMATCH) {
        printf("failed to write the final png due to the surface not being pixel-based (CAIRO_STATUS_SURFACE_TYPE_MISMATCH)\n");
    } else {
        printf("Successfully wrote the final png file!\n");
    }

    cairo_surface_destroy(surface);
    cairo_destroy(c);

	return 0;
}

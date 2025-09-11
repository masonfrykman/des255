#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <cairo.h>

#define IMAGE_WIDTH_PIXELS 11 * 300
#define IMAGE_HEIGHT_PIXELS 17 * 300
#define NODE_SIZE_PIXELS 50
#define NODE_CIRC_SPACE_BETWEEN 100

void printusage(char* execName) {
    printf("usage: %s outFile numNodes\n", execName);
}

int main(int argc, char* argv[]) {

    if(argc < 3) {
        printusage(argv[0]);
        return 2;
    }

	int numNodes = atoi(argv[2]);
	if(numNodes < 1) {
		printf("# of nodes must be an integer greater than or equal to 1.");
		return 2;
	}

    // Create a surface to draw to
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, IMAGE_WIDTH_PIXELS, IMAGE_HEIGHT_PIXELS);
    cairo_t *c = cairo_create(surface);

    // put a white background
    cairo_new_path(c);
    cairo_rectangle(c, 0, 0, IMAGE_WIDTH_PIXELS, IMAGE_HEIGHT_PIXELS);
    cairo_set_source_rgb(c, 255, 255, 255);
    cairo_fill(c);

    // The meat of the program
    
    srand(time(0));
	for(int i = 0; i < numNodes; i++) {
        
        // randomize the node's pos on the poster
        // the pos is the origin / center of the node.
        int x, y;
        x = rand() % (IMAGE_WIDTH_PIXELS - 200) + 100;
        y = rand() % (IMAGE_HEIGHT_PIXELS - 2000) + 200;

        // randomize the color of the nodes
        double red, g, b;
        red = (double)(rand() % 1000) / 1000;
        g = (double)(rand() % 1000) / 1000;
        b = (double)(rand() % 1000) / 1000;
        printf("%f %f %f\n", red, g, b);

        

        // draw the circles around the node
        cairo_new_path(c);
        for(int r = NODE_CIRC_SPACE_BETWEEN; r < IMAGE_WIDTH_PIXELS; r += NODE_CIRC_SPACE_BETWEEN) {
            
            cairo_arc(c, x, y, r, 0, M_PI * 2);
            cairo_set_source_rgb(c, red, g, b); // TODO: do cool colors based on each node
            cairo_stroke(c);
        }

        // draw the node's little square
        cairo_new_path(c);
        cairo_rectangle(c, x - (NODE_SIZE_PIXELS / 2), y - (NODE_SIZE_PIXELS / 2), NODE_SIZE_PIXELS, NODE_SIZE_PIXELS);
        cairo_set_source_rgb(c, red, g, b); // TODO: do cool colors based on each node
        cairo_fill(c);
    }

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

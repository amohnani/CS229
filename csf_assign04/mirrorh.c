//                                                                              
// Example plugin: it just swaps the blue and green color component             
// values for each pixel in the source image.                                   
//                                                                              

#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	// This plugin doesn't accept any command line arguments;               
	// just define a single dummy field.                                    
	int dummy;
};

const char *get_plugin_name(void) {
	return "mirrorh";
}

const char *get_plugin_desc(void) {
	return "mirror image horizontally";
}

void *parse_arguments(int num_args, char *args[]) {
        (void) args; // this is just to avoid a warning abo               

        if (num_args != 0) {
                return NULL;
        }
        return calloc(1, sizeof(struct Arguments));
}

struct Image *transform_image(struct Image *source, void *arg_data) {
        struct Arguments *args = arg_data;

        // Allocate a result Image                                           
        struct Image *out = img_create(source->width, source->height);
        if (!out) {
                free(args);
                return NULL;
	}

        
        for (unsigned i = 0; i < source->height; i++) {
	  for (unsigned j = 0; j < source->width; j++) {
	    out->data[i*source->width + j] =
	      source->data[i*source->width + source->width - j];
	  }
        }

	free(args);

        return out;
}

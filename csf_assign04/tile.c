#include <stdlib.h>
#include "image_plugin.h"
#include <stdio.h>
struct Arguments {
 
	int scale;
};

const char *get_plugin_name(void) {
	return "tile";
}

const char *get_plugin_desc(void) {
	return "tile source image in an NxN arrangement";
}

void *parse_arguments(int num_args, char *args[]) {
  int arg = (int) strtol(args[5], NULL, 10);
  if (num_args == 0) {
                return NULL;
        }
  struct Arguments *a = malloc(sizeof(struct Arguments));
  a->scale = arg;
  return (void *)a;
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
	      source->data[(j*args->scale % source->width) +
			   (i*args->scale % source->height)*source->width];
	    
	  }
        }

	free(args);

        return out;
}

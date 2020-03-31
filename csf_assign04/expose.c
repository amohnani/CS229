#include <stdlib.h>
#include "image.h"
#include "image_plugin.h"
#include <stdio.h>
#include <stdint.h>

struct Arguments {
 
	double scale;
};

const char *get_plugin_name(void) {
	return "tile";
}

const char *get_plugin_desc(void) {
	return "tile source image in an NxN arrangement";
}

void *parse_arguments(int num_args, char *args[]) {
  double arg = strtod(args[5], NULL);
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
	
        uint32_t *r = malloc(4);
	uint32_t *g = malloc(4);
	uint32_t *b = malloc(4);
        uint32_t *a = malloc(4);
       	for (unsigned i = 0; i < source->height; i++) {
	  for (unsigned j = 0; j < source->width; j++) {
	     img_unpack_pixel(source->data[i*source->width + j],
			      (uint8_t *)r, (uint8_t *)g,
			      (uint8_t *)b, (uint8_t *)a);
	     *r = (double)(*r)*args->scale; *g = (double)(*g)*args->scale;
             *b	= (double)(*b)*args->scale; 
	     if (*r  > 255) {
	       *r = 255;
	     }
	     if (*g > 255) {
	       *g = 255;
	     }
	     if (*b > 255) {
	       *b = 255;
	     }
	     if (*a > 255) {
	       *a = 255;
	     }
	     
	     out->data[i*source->width + j] =
	       img_pack_pixel(*r, *g,
			      *b, *a);
	  }
        }

	free(args);
	free(r);
	free(g);
	free(b);
	free(a);
        return out;
}

#include "image.h"
#include "pnglite.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>

struct Plugin {
    int valid;
    void *handle;
    const char *(*get_plugin_name)(void);
    const char *(*get_plugin_desc)(void);
    void *(*parse_arguments)(int num_args, char *args[]);
    struct Image *(*transform_image)(struct Image *source, void *arg_data);
};

void findValidPlugins(struct Plugin *plugins);
void loadPlugins(struct Plugin *plugins);
int parseArguments(struct Plugin *plugins, int argc, char **argv);
int listPlugins(struct Plugin *plugins);
int validNum(struct Plugin *plugins);
void execTransformation(struct Plugin *plugins, int i, char **argv);
void error();
void clear_plugins(struct Plugin *plugins);

int main(int argc, char **argv) {
  struct Plugin plugins[5];
  loadPlugins(plugins);
  if (parseArguments(plugins, argc, argv) < 0) {
    error();
  }
  clear_plugins(plugins);
  return 0;
  
}

int parseArguments(struct Plugin *plugins, int argc, char **argv) {
  if (argc == 1) {
    return -1;
  }
  if (strcmp(argv[1], "list") == 0) {
    return listPlugins(plugins);
  } else if (strcmp(argv[1], "exec") == 0) {
    if (strcmp(argv[2], "swapbg") == 0) {
      execTransformation(plugins, 3, argv);
    } else if (strcmp(argv[2], "tile") == 0) {
      execTransformation(plugins, 4, argv);
    } else if (strcmp(argv[2], "expose") == 0) {
      execTransformation(plugins, 0, argv);
    } else if (strcmp(argv[2], "mirrorh") == 0) {
      execTransformation(plugins, 1, argv);
    } else if (strcmp(argv[2], "mirrorv") == 0) {
      execTransformation(plugins, 2, argv);
    } else {
      return -1;
    }
  } else {
    return -1;
  }
  return 0;
}

void execTransformation(struct Plugin *plugins, int i, char **argv) {
  struct Image *source = img_read_png(argv[3]);
  if (source == NULL) {
    clear_plugins(plugins);
    error();
  }
  void *arg_data = plugins[i].parse_arguments(1, argv);
  struct Image *edited = plugins[i].transform_image(source, arg_data);
  img_destroy(source);
  if (img_write_png(edited, argv[4]) == 0) {
    clear_plugins(plugins);
    error();
  }
  img_destroy(edited);
}

int listPlugins(struct Plugin *plugins) {
  printf("Loaded %d plugin(s)\n", validNum(plugins));
  for (int i = 0; i < 5; i++) {
    if (plugins[i].valid == 1) {
      printf("%s: %s\n", plugins[i].get_plugin_name(),
	     plugins[i].get_plugin_desc());
    }
  }
  return 0;
}

void loadPlugins(struct Plugin *plugins) {
  findValidPlugins(plugins);

  for (int i = 0; i < 5; i++) {
    if (plugins[i].valid == 1) {
      *(void **) (&(plugins[i].get_plugin_name)) =
	 dlsym(plugins[i].handle, "get_plugin_name");
      
      *(void **) (&(plugins[i].get_plugin_desc)) =
	 dlsym(plugins[i].handle, "get_plugin_desc");

      *(void **) (&(plugins[i].parse_arguments)) =
	dlsym(plugins[i].handle, "parse_arguments");

      *(void **) (&(plugins[i].transform_image)) =
	dlsym(plugins[i].handle, "transform_image");
    }
  }
}

void findValidPlugins(struct Plugin *plugins) {
   for (int i = 0; i < 5; i++) {
    plugins[i].valid = 0;
  }
  DIR* directory = opendir("plugins");
  struct dirent *file = readdir(directory);
  while (file != NULL) {
    if (strcmp(file->d_name, "expose.so") == 0) {
      plugins[0].valid = 1;
      plugins[0].handle = dlopen("plugins/expose.so", RTLD_LAZY);
    } else if (strcmp(file->d_name, "mirrorh.so") == 0) {
      plugins[1].valid = 1;
      plugins[1].handle = dlopen("plugins/mirrorh.so", RTLD_LAZY);
    } else if (strcmp(file->d_name, "mirrorv.so") == 0) {
      plugins[2].valid = 1;
      plugins[2].handle = dlopen("plugins/mirrorv.so", RTLD_LAZY);
    } else if (strcmp(file->d_name, "swapbg.so") == 0) {
      plugins[3].valid = 1;
      plugins[3].handle = dlopen("plugins/swapbg.so", RTLD_LAZY);
    } else if (strcmp(file->d_name, "tile.so") == 0) {
      plugins[4].valid = 1;
      plugins[4].handle = dlopen("plugins/tile.so", RTLD_LAZY);
    }
		      
    file = readdir(directory);
  }
  closedir(directory);
}

int validNum(struct Plugin *plugins) {
  int count = 0;
  for (int i = 0; i < 5; i++) {
    if (plugins[i].valid == 1) {
      count++;
    }
  }
  return count;
}

void error() {
  printf("ERROR: Invalid arguments\n");
  exit(1);
}

void clear_plugins(struct Plugin *plugins) {
  for (int i = 0; i < 5; i++) {
    if (plugins[i].valid == 1) {
      dlclose(plugins[i].handle);
    }
  }
}

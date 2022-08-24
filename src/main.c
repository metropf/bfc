#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "bfc.h"

static char *read_file(const char *file_name);
static char *bf_to_c_extension(const char *file_name);
static uint32_t get_extension_index(const char *file_name);
static uint32_t get_file_name_in_path_index(const char *file_name); // gives the index of the first letter after the last / in the path

static void write_file(const char *file_name, const char *src);

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "error: no program arguments\n");
		return 1;
	}

	for (uint32_t i = 1; i < argc; ++i) {
		const char *file_name = argv[i];
		char *src = read_file(file_name);
		char *output = bfc_compile(src);
		free(src);

		char *out_file_name = bf_to_c_extension(file_name + get_file_name_in_path_index(file_name));
		write_file(out_file_name, output);

		free(output);
		free(out_file_name);
	}

	return 0;
}

static char *read_file(const char *file_name)
{
	FILE *file = fopen(file_name, "r");
	if (file == NULL) {
		fprintf(stderr, "error: %s: file does not exist\n", file_name);
		exit(EXIT_FAILURE);
	}

	uint32_t extension_index = get_extension_index(file_name);
	if (extension_index != 0 && strcmp(file_name + extension_index, "bf") != 0) {
		fprintf(stderr, "error: %s: unrecognised file format\n", file_name);
		exit(EXIT_FAILURE);
	} 

	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buffer = malloc(file_size + 1);
	fread(buffer, sizeof(char), file_size, file);

	fclose(file);

	return buffer;
}

static char *bf_to_c_extension(const char *file_name)
{
	char *c_file_name;

	uint32_t extension_index = get_extension_index(file_name);
	if (extension_index == 0) { // No file extension
		size_t c_file_name_len = strlen(file_name) + strlen("c");

		c_file_name = strcpy(malloc(c_file_name_len + 1), file_name);
		sprintf(c_file_name + strlen(file_name), "c");
	}
	else {
		// 'bf' has 2 characters and 'c' has 1 so 'c' = 'bf' - 1
		size_t c_file_name_len = strlen(file_name) - 1; 

		c_file_name = strcpy(malloc(c_file_name_len + 1), file_name);
		sprintf(c_file_name + extension_index, "c");
	}

	return c_file_name;
}

static void write_file(const char *file_name, const char *src)
{
	FILE *file = fopen(file_name, "w+");
	fwrite(src, sizeof(char), strlen(src), file);
	fclose(file);
}

static uint32_t get_extension_index(const char *file_name)
{
	uint32_t i;
	for (i = strlen(file_name) - 1; i != 0 && file_name[i] != '.'; --i);
	return ++i;
}

static uint32_t get_file_name_in_path_index(const char *file_name)
{
	uint32_t i;
	for (i = strlen(file_name) - 1; i != 0 && file_name[i] != '/'; --i);
	return ++i;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "bfc.h"

static char *add_instruction(char *dest, size_t *dest_size, const char *src);

char *bfc_compile(const char *src)
{
	uint64_t index = 0;
	char current;

	size_t output_size = 512;
	char *output = calloc(output_size, sizeof(char));

	output = add_instruction(output, &output_size, "#include<stdio.h>\n"
	                                      "#include<stdlib.h>\n"
	                                      "#include<stdint.h>\n"
	                                      "#include<string.h>\n"
	                                      "void d(uint8_t*c,uint8_t*p,size_t*s)"        // double cells size if needed
	                                      "{"
	                                       "if(p-c==*s-1){"
	                                        "p-=(uintptr_t)c;"
	                                        "c=realloc(c,*s*2);"
	                                        "memset(c+*s,0,*s-1);"
	                                        "*s*=2;"
	                                        "p+=(uintptr_t)c;"
	                                       "}"
	                                       "++p;"
	                                      "}"
	                                      "int main(void)"
	                                      "{"
	                                       "size_t s=256;"                              // cells size
	                                       "uint8_t*c=calloc(s,sizeof(unsigned char));" // cells
	                                       "uint8_t*p=c;");                             // data pointer

	while ((current = src[index++]) != 0) {
		switch (current) {
			case '+':
				output = add_instruction(output, &output_size, "++*p;");
				break;
			case '-':
				output = add_instruction(output, &output_size, "--*p;");
				break;
			case '<':
				output = add_instruction(output, &output_size, /*"if(p-c==0) "
				                                      "fputs(\"Error: Out of range\",stderr);"
				                                      "exit(EXIT_FAILURE);"
				                                      "}"*/
				                                      "--p;");
				break;
			case '>':
				output = add_instruction(output, &output_size, "d(c,p,&s);"
				                                      "++p;");
				break;
			case '.':
				output = add_instruction(output, &output_size, "putchar(*p);");
				break;
			case ',':
				output = add_instruction(output, &output_size, "*p=getchar();");
				break;
			case '[':
				output = add_instruction(output, &output_size, "while(*p){");
				break;
			case ']':
				output = add_instruction(output, &output_size, "}");
				break;
		}
	}

	output = add_instruction(output, &output_size,  "free(c);"
	                                      "}\n");

	return output;
}

static char *add_instruction(char *dest, size_t *dest_size, const char *src)
{
	if (strlen(dest) + 1 + strlen(src) >= *dest_size) {
		size_t src_length = strlen(src);
		dest = realloc(dest, *dest_size * 2 + src_length + 1);
		memset(dest + *dest_size, 0, *dest_size + src_length + 1);
		*dest_size = *dest_size * 2 + src_length + 1;
	}

	strcat(dest, src);

	return dest;
}

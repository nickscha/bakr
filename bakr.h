/* bakr.h - v0.1 - public domain data structures - nickscha 2025

A C89 standard compliant, single header, nostdlib (no C Standard Library) util that bakes files into C files.

USAGE

  Only one method is required to call to bake your supplied files into a C89 compliant header file.

  bakr_recipe recipies[] = {
      {"test.txt", "test"},
      {"example.txt", "example"}};

  bakr_cook(recipies, BAKR_ARRAY_SIZE(recipies), "bakr_bin.h", "2025", "nickscha");

LICENSE

  Placed in the public domain and also MIT licensed.
  See end of file for detailed license information.

*/
#ifndef BAKR_H
#define BAKR_H

/* #############################################################################
 * # COMPILER SETTINGS
 * #############################################################################
 */
/* Check if using C99 or later (inline is supported) */
#if __STDC_VERSION__ >= 199901L
#define BAKR_INLINE inline
#define BAKR_API extern
#elif defined(__GNUC__) || defined(__clang__)
#define BAKR_INLINE __inline__
#define BAKR_API static
#elif defined(_MSC_VER)
#define BAKR_INLINE __inline
#define BAKR_API static
#else
#define BAKR_INLINE
#define BAKR_API static
#endif

#define BAKR_VERSION "0.1"
#define BAKR_ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef struct bakr_llu_type_internal
{
    unsigned long low;
    unsigned long high;

} bakr_llu_type_internal;

typedef struct bakr_internal_file
{
    unsigned long size;
    bakr_llu_type_internal time_created;
    bakr_llu_type_internal time_modified;
    bakr_llu_type_internal time_accessed;
    const char *name;
    char *command;
    unsigned char *content;

} bakr_internal_file;

typedef struct bakr_recipe
{
    const char *name_file;
    const char *name_normalized;

} bakr_recipe;

#ifdef _WIN32
int GetFileAttributesExA(const char *lpFileName, void *fInfoLevelId, void *lpFileInformation);

typedef struct WIN32_FILE_ATTRIBUTE_DATA
{
    unsigned long dwFileAttributes;
    bakr_llu_type_internal ftCreationTime;
    bakr_llu_type_internal ftLastAccessTime;
    bakr_llu_type_internal ftLastWriteTime;
    unsigned long nFileSizeHigh;
    unsigned long nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA;

BAKR_API BAKR_INLINE void bakr_win32_internal_file_read_timestat(bakr_internal_file *file)
{
    bakr_llu_type_internal time_created;
    bakr_llu_type_internal time_modified;
    bakr_llu_type_internal time_accessed;

    WIN32_FILE_ATTRIBUTE_DATA fad;
    GetFileAttributesExA(file->name, 0, &fad);
    time_created.low = fad.ftCreationTime.low;
    time_created.high = fad.ftCreationTime.high;
    time_modified.low = fad.ftLastWriteTime.low;
    time_modified.high = fad.ftLastWriteTime.high;
    time_accessed.low = fad.ftLastAccessTime.low;
    time_accessed.high = fad.ftLastAccessTime.high;

    file->time_created = time_created;
    file->time_modified = time_modified;
    file->time_accessed = time_accessed;
}
#endif

BAKR_API BAKR_INLINE void bakr_internal_file_read_timestat(bakr_internal_file *file)
{
#ifdef _WIN32
    bakr_win32_internal_file_read_timestat(file);
#else
    (void)file;
#endif
}

#include "stdlib.h"
#include "stdio.h"

BAKR_API BAKR_INLINE bakr_internal_file bakr_internal_file_read(const char *filename)
{
    FILE *file;

    bakr_internal_file result = {0};
    result.name = filename;

    printf("[bakr] baking into header file: %s\n", filename);

    file = fopen(filename, "rb");
    if (!file)
    {
        printf("[bakr] file not found: %s\n", filename);
        return (result);
    }

    fseek(file, 0, SEEK_END);
    result.size = (unsigned long)ftell(file);
    rewind(file);

    /* Allocate memory for file content */
    result.content = (unsigned char *)malloc(result.size + 1);
    if (!result.content)
    {
        fclose(file);
        return (result);
    }

    if (fread(result.content, 1, result.size, file) != result.size)
    {
        free(result.content);
        fclose(file);
        return (result);
    }
    result.content[result.size] = '\0'; /* Null-terminate in case it's text */

    fclose(file);

    bakr_internal_file_read_timestat(&result);

    return (result);
}

void bakr_internal_file_free(bakr_internal_file *file_data)
{
    free(file_data->content);
}

const char *bakr_internal_header_comment =
    "/* bakr.h - v%s - public domain data structures - %s %s\n"
    "\n"
    "A C89 standard compliant, single header, nostdlib (no C Standard Library) util that bakes files into C code.\n"
    "\n"
    "See https://github.com/nickscha/bakr for more information.\n"
    "*/\n";

BAKR_API BAKR_INLINE void bakr_internal_cook_header(FILE *output, const char *version, const char *year, const char *author)
{
    fprintf(output, bakr_internal_header_comment, version, year, author);
    fprintf(output, "#ifndef BAKR_BIN_H\n");
    fprintf(output, "#define BAKR_BIN_H\n");
}

BAKR_API BAKR_INLINE void bakr_internal_cook_definitions(FILE *output)
{
    fprintf(output, "#define BAKR_VERSION \"%s\"\n", BAKR_VERSION);
    fprintf(output, "\n");
    fprintf(output, "typedef struct bakr_llu_type\n{\n  unsigned long low;\n  unsigned long high;\n\n} bakr_llu_type;\n\n");
    fprintf(output, "typedef struct bakr_file\n{\n  unsigned long size;\n  bakr_llu_type time_created;\n  bakr_llu_type time_modified;\n  bakr_llu_type time_accessed;\n  char *name;\n  char *command;\n  unsigned char *content;\n\n} bakr_file;\n");
    fprintf(output, "\n");
}

BAKR_API BAKR_INLINE void bakr_internal_cook_file(FILE *output, bakr_internal_file file, bakr_recipe recipe)
{
    unsigned long i;

    /* Content */
    fprintf(output, "static unsigned char bakr_file_%s_content[] = {", recipe.name_normalized);
    for (i = 0; i < file.size - 1; ++i)
    {
        fprintf(output, "0x%02X", file.content[i]);
        if (i != file.size - 2)
        {
            fprintf(output, ", ");
        }
    }
    fprintf(output, "};\n");

    /* File struct */
    fprintf(output, "static const bakr_file bakr_file_%s = {", recipe.name_normalized);
    fprintf(output, "%li, ", file.size);
    fprintf(output, "{%luLU, %luLU}, ", file.time_created.low, file.time_created.high);
    fprintf(output, "{%luLU, %luLU}, ", file.time_modified.low, file.time_modified.high);
    fprintf(output, "{%luLU, %luLU}, ", file.time_accessed.low, file.time_accessed.high);
    fprintf(output, "\"%s\", ", file.name);
    fprintf(output, "\"bakr %s\", ", file.name);
    fprintf(output, "bakr_file_%s_content", recipe.name_normalized);
    fprintf(output, "};\n");
}

BAKR_API BAKR_INLINE void bakr_internal_cook_files_info(FILE *output, bakr_recipe *recipies, int recipies_count)
{
    int i;

    fprintf(output, "\n");
    fprintf(output, "#define BAKR_NUM_FILES (%i)\n", recipies_count);
    fprintf(output, "static const bakr_file bakr_files[BAKR_NUM_FILES] = {");
    for (i = 0; i < recipies_count; ++i)
    {
        fprintf(output, "bakr_file_%s", recipies[i].name_normalized);
        if (i != recipies_count - 1)
        {
            fprintf(output, ", ");
        }
    }
    fprintf(output, "};\n");
}

BAKR_API BAKR_INLINE void bakr_internal_cook_footer(FILE *output)
{
    fprintf(output, "\n#endif /* BAKR_BIN_H */\n");
}

BAKR_API BAKR_INLINE void bakr_cook(bakr_recipe *recipies, int recipies_count, const char *output_filename, const char *year, const char *author)
{
    int i;

    FILE *output = fopen(output_filename, "w");

    bakr_internal_cook_header(output, BAKR_VERSION, year, author);
    bakr_internal_cook_definitions(output);

    for (i = 0; i < recipies_count; i++)
    {
        bakr_internal_file f1 = bakr_internal_file_read(recipies[i].name_file);
        bakr_internal_cook_file(output, f1, recipies[i]);
        bakr_internal_file_free(&f1);
    }

    bakr_internal_cook_files_info(output, recipies, recipies_count);
    bakr_internal_cook_footer(output);

    fclose(output);

    printf("[bakr] executables baked successfully into: %s", output_filename);
}

#endif /* BAKR_H */

/*
   ------------------------------------------------------------------------------
   This software is available under 2 licenses -- choose whichever you prefer.
   ------------------------------------------------------------------------------
   ALTERNATIVE A - MIT License
   Copyright (c) 2025 nickscha
   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is furnished to do
   so, subject to the following conditions:
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
   ------------------------------------------------------------------------------
   ALTERNATIVE B - Public Domain (www.unlicense.org)
   This is free and unencumbered software released into the public domain.
   Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
   software, either in source code form or as a compiled binary, for any purpose,
   commercial or non-commercial, and by any means.
   In jurisdictions that recognize copyright laws, the author or authors of this
   software dedicate any and all copyright interest in the software to the public
   domain. We make this dedication for the benefit of the public at large and to
   the detriment of our heirs and successors. We intend this dedication to be an
   overt act of relinquishment in perpetuity of all present and future rights to
   this software under copyright law.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   ------------------------------------------------------------------------------
*/

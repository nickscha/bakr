# bakr (in development)
A C89 standard compliant, single header, nostdlib (no C Standard Library) util that bakes files into C89 header files.

For more information please look at the "bakr.h" file or take a look at the "examples" or "tests" folder.

## Quick Start

Download or clone bakr.h and include it in your project.

```C
#include "bakr.h"

int main() {

    /* The files to bake into the C89 compliant header file */
    bakr_recipe recipies[] = {
        {"test.txt", "test"},
        {"example.txt", "example"}};

    /* Generate the C89 compliant source code */
    bakr_cook(
        recipies, 
        BAKR_ARRAY_SIZE(recipies), 
        "bakr_bin.h", 
        "2025", 
        "nickscha"
    );

    return 0;
}
```

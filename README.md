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

## "nostdlib" Motivation & Purpose

nostdlib is a lightweight, minimalistic approach to C development that removes dependencies on the standard library. The motivation behind this project is to provide developers with greater control over their code by eliminating unnecessary overhead, reducing binary size, and enabling deployment in resource-constrained environments.

Many modern development environments rely heavily on the standard library, which, while convenient, introduces unnecessary bloat, security risks, and unpredictable dependencies. nostdlib aims to give developers fine-grained control over memory management, execution flow, and system calls by working directly with the underlying platform.

### Benefits

#### Minimal overhead
By removing the standard library, nostdlib significantly reduces runtime overhead, allowing for faster execution and smaller binary sizes.

#### Increased security
Standard libraries often include unnecessary functions that increase the attack surface of an application. nostdlib mitigates security risks by removing unused and potentially vulnerable components.

#### Reduced binary size
Without linking to the standard library, binaries are smaller, making them ideal for embedded systems, bootloaders, and operating systems where storage is limited.

#### Enhanced performance
Direct control over system calls and memory management leads to performance gains by eliminating abstraction layers imposed by standard libraries.

#### Better portability
By relying only on fundamental system interfaces, nostdlib allows for easier porting across different platforms without worrying about standard library availability.

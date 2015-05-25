# kprintf
This is a minimalist formatted print function for debugging purpose in an embedded system with limited memory and processing power.
* Reentrant: only local buffers, no global state.
* Requires no `malloc()`.
* Supported format specifiers: `%d`, `%i`, `%u`, `%x`, `%p`, `%c`, `%s`

## Limitations
* Only for 32bit systems.
* No formatting options for padding, length or precision.
* No 64bit integer, no `float`, no `double`.

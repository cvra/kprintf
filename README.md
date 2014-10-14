# kprintf

This is a simple printf implementation for debugging.
* reentrant
* no `malloc()`
* supported types: int, char, string, pointer

## Note
* Only for 32bit systems.
* not supported type will be ignored.
* Using any formatting option for padding, length or precision will result in a wrong output.

# circular-buffers_c
Simple implementation of a Circular Buffer structure, ready for user applications programming. Written in C, requires the GNU C Library.

They are allocated in the heap as arrays of _void *_, of given size. It is possible to make single read/write operations, as well as transfer entire blocks of data with *copy* or *paste* functions. They are intended as a FIFO data structure, without the possibility to overwrite old data if no room is left. Care must be taken while transferring data smaller than a _void *_ (operating with single bytes is highly suggested).

## Can I use this?

If you stumbled upon here and find this suitable for your project, or think this might save you some work, sure!
Just credit the use of this code, and although I used this stuff in projects and tested it, there's no 100% guarantee that it's bug-free. Feel free to open an issue if you see something wrong.

Also, this code is protected by the MIT license as per the attached *LICENSE* file.
# Linux-Shell
This is my implementation of a Linux shell using C programming language

This is a Linux shell implementation in C which supports all normal commands with switches separated by :,|| or &&.
The beahivour of these separators are as documented in the Linux documentation.
This also supports single and double pipes and simple I/O redirection.
Example commands supported with I/O redirection and pipe are:

cat c.txt|grep a|grep z
cat c.txt|grep a>out.txt
sort<a.txt>out.txt

The shell.c file can be compiled as gcc shell.c -lreadline

The shell has remembrance capability per session i.e by pressing the up key we can have the previous commands.

It can also execute files containing these commands that is passed by command-line argument.
For example: ./a.out filename will execute commands from the file.
I have provided an example file a.sh in this repository.



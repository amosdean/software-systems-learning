# C Shell

A simple Unix-style command shell written in C as a systems programming learning project.

## Features

* Program execution
* Basic file path movement with `cd`
* Shell variables and environment variables
* I/O redirection (`<`, `>`, `>>`)
* Command pipelines (`|`)
* Built-in commands such as `pwd`, `quit`, and `exit`

## Concepts

This project explores fundamental Unix systems programming concepts, including:

* Processes and process creation
* `fork()` and `execvp()`
* File descriptors
* `dup2()` and I/O redirection
* Pipes and inter-process communication
* Basic command parsing

## Build

```bash
make
```

Run with:

```bash
./shell
```

## Purpose

Built as a hands-on project to develop practical experience with C, Linux system calls, processes, and inter-process communication.

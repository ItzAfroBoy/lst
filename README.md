<div align="center">
<pre>
 __       ____    ______   
/\ \     /\  _`\ /\__  _\  
\ \ \    \ \,\L\_\/_/\ \/  
 \ \ \  __\/_\__ \  \ \ \  
  \ \ \L\ \ /\ \L\ \ \ \ \ 
   \ \____/ \ `\____\ \ \_\
    \/___/   \/_____/  \/_/
<br>
A shitty file manager written in C99
<br>
<img alt="GitHub License" src="https://img.shields.io/github/license/ItzAfroBoy/lst"> <img alt="GitHub tag (with filter)" src="https://img.shields.io/github/v/tag/ItzAfroBoy/lst?label=version"> <a href="https://www.codefactor.io/repository/github/itzafroboy/lst"><img src="https://www.codefactor.io/repository/github/itzafroboy/lst/badge" alt="CodeFactor" /></a> <img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/ItzAfroBoy/lst">
</pre>
</div>

## Building

You will need:

* Linux or MacOS
* GCC or Clang
* Make

[`txt`](https://github.com/ItzAfroBoy/txt) used to be needed but you can now use your editor of choice by setting the `EDITOR` env variable  

Windows is unsupported due to some header files only available on Linux and MacOS (`termios.h` & `sys/ioctl.h`)

To build:

1. Clone this repo
2. Enter the directory
3. Run `make` to create the binary
4. Place the binary on your PATH
5. Enjoy

## Usage

```bash-session
$ lst -h
lst [options] <path>
a shitty file manager

OPTIONS
        -h      :: show help
        -v      :: show version
        -n      :: show numbers
        -i      :: show file info
        -p      :: show path at exit

OPERATIONS
         q      :: exit lst
         r      :: rename a file
         d      :: delete a file
         c      :: create a new file
        ^c      :: create a new folder
$ lst -tni ~/.config
```

> Exiting `lst`:

1. `Q`
2. `Esc`

> Config file:

The config file uses the `ini` file format and utilises [`ini`](https://github.com/rxi/ini) which  
reads and parses the file. The format of file will be specified [`here`](https://github.com/ItzAfroBoy/lst/wiki).

## Features

1. Lists files and directories
2. Opens files
    * Grab my shitty text editor while you're here: [`txt`](https://github.com/ItzAfroBoy/txt)
3. Inline file size (`-i`)
4. Line numbers (`-n`)
6. Status & Message Bar
    * File size
    * Current line
    * Current directory
    * Total entries
    * Clock
7. Reads a config file

More is to come in the future. It's Work In Progress

### In depth & Upcoming features

> Size calculations

`lst` shows the size of the file that is highlighted in the UI. Directories will always  
return as 0 bytes in the status bar and shows `>` to indicate it is a directory when you  
enable the info flag (`-i`).

> Opening directories & files

When pressing `Enter` or `->` key, `lst` will check if the highlighted item is a file.  
If it is, `txt` or your default ediort will take over so you can edit the file. Otherwise, it will open the directory.

> File & directory operations

In `lst`, you can rename, delete and create files. Directories can also be renamed and created, however they  
can't be deleted as that the directories have to empty to be deleted. This will be implemented soon.  
Moving files, grouped file operations and deleting directories are currently in the works.

> Changing directory on exit [Work In Progress]

I need to find an efficient way to do this so this will be coming in the near future.  
The `-p` flag will print final directory on exit. So you'll probs need a wrapper function.

## Updates

```markdown
* 1.1.1 Flags & input timeout fixed
* 1.1.0 New features introduced
* 1.0.0 Initial Release
```

## License

[GNU GPLv3](https://github.com/ItzAfroBoy/lst/blob/main/LICENSE)

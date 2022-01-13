# lst

A shitty file manager

[![GitHub](https://img.shields.io/github/license/ItzAfroBoy/lst)](https://github.com/ItzAfroBoy/lst/blob/main/LICENSE)
[![GitHub file size in bytes](https://img.shields.io/github/size/ItzAfroBoy/lst/lst.c?label=lst.c)](https://github.com/ItzAfroBoy/lst/blob/main/lst.c)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/ItzAfroBoy/lst)](https://github.com/ItzAfroBoy/lst/releases/tag/1.0.0)
[![CodeFactor](https://www.codefactor.io/repository/github/itzafroboy/lst/badge/main)](https://www.codefactor.io/repository/github/itzafroboy/lst/overview/main)


## Building

You will need:

* A linux machine
* gcc
* make
* [txt](https://github.com/ItzAfroBoy/txt)

`txt` is needed as it is hard-coded into the file manager to use it when editing files.  
As lst is worked on, it will be able to use your personal choice of editor as a fallback.  

A linux machine is needed as lst uses two header files that are not available on Windows:

* sys/ioctl.h
* termios.h

To build:

1. Clone this repo
2. Enter the directory
3. Run `make` to create the binary
4. Run `make install` to copy the binary to path
5. Enjoy

## Usage

```bash-session
$ lst -h
lst [options] <path>
a shitty file manager

OPTIONS
        -h      :: show help
        -v      :: show version
        -t      :: show tildes
        -c      :: show colors
        -n      :: show numbers
        -i      :: show file info

OPERATIONS
         r      :: rename a file
         d      :: delete a file
         c      :: create a new file
        ^c      :: create a new folder
        ^q      :: exit lst
$ lst -tni ~/.config
```

> Exiting `lst`:

1. `Ctrl-Q`
2. `Esc`

> Config file:

The config file uses the `.ini` / `ini` file format and utilises [`ini`](https://github.com/rxi/ini) which  
reads and parses the file. The format of file will be specified [`here`](https://github.com/ItzAfroBoy/lst/wiki).

## Features

1. Lists files and directories
2. Opens files
    * It does depend on my shitty text editor: [`txt`](https://github.com/ItzAfroBoy/txt)
3. Inline file size (`-i`)
4. Line numbers (`-n`)
5. Tiles on empty lines (`-t`)
6. Status & Message Bar
    * File size
    * Current line
    * Current Dir
    * Total Dirs
    * Clock
7. Reads a config file (WIP)

More is to come in the future. It's Work In Progress

### In depth & Upcoming features

> Size calculations

`lst` shows the size of the file that is highlighted in the UI. Directories will always  
return as 0 bytes in the status bar and shows `>` to indicate it is a directory when you  
enable the info flag (`-i`).

> Extensions

`lst` has what I am calling extensions. They add features that you might want in a file manager  
and are enabled through flags and config files. These can be toggled when running `lst`.  
They are all disabled by default, giving a minimal-ish file manager.

> Opening directories & files

When pressing `Enter` or `->` key, `lst` will check if the highlighted item is a file.  
If it is, `txt` will take over so you can edit the file. Otherwise, it will open the directory.  
`txt` is hardcoded as text editor of choice but that will soon be able to customised in the config file.

> File & directory operations

In `lst`, you can rename, delete and create files. Directories can also be renamed and created, however they  
can't be deleted as that the directories have to empty to be deleted. This will be implemented soon.  
To edit files, this requires you to have txt, which `lst` is hardcoded to use. Moving files,  
grouped file operations and deleting directories are currently in the works.

> Config file [Work In Progress]

`lst` will soon be able to read a config file that will be able to enable extensions  
on start up with out specifying flags for extra convenience. You will also able to customize the  
colors `lst` uses for the status bar, highlighted item, etc.

> Changing directory on exit [Work In Progress]

I need to find an efficient way to do this so this will be coming in the near future.

## Updates

```markdown
* 1.1.0 New features introduced
* 1.0.0 Initial Release
```

## License

[GNU GPLv3](https://github.com/ItzAfroBoy/lst/blob/main/LICENSE)

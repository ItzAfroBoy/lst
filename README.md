# lst

A shitty file manager

## Building

You will need:

* A linux machine
* gcc
* make
* [txt](https://github.com/ItzAfroBoy/txt)

`txt` is needed as it is hard-coded into the file manager to use it to edit files.  
As lst is worked on, it will be able to use your personal choice of editor as a fallback.  

A linux machine is needed as lst uses two header files that are not available on Windows:

* sys/ioctl.h
* termios.h

To build:

1. Download this repo
2. Enter the directory
3. Run `make` to create the binary
4. Run `make install` to copy the binary to path
5. Enjoy

## Features

1. Lists files and directories
2. Opens files
	- It does depend on my shitty text editor: [`txt`](https://github.com/ItzAfroBoy/txt)
3. More is to come in the future. It's WIP

## Updates

```markdown
* 1.0.0 Initial Release
```

## License

[GNU GPLv3](https://github.com/ItzAfroBoy/txt/blob/main/LICENSE)

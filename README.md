![preview.gif](https://raw.github.com/AlxHnr/interface/master/preview.gif)

A launcher for a video game console I tried to build out of an old pc. It
would boot straight into this interface, allowing games to be launched,
installed and uninstalled. This interface doesn't wrap any existing package
managers. Instead, it mounts a USB thumb drive by spawning shell commands.
The thumb drive can contain arbitrary setup scripts, which will be listed
as installable packages. It will not reboot or shutdown your machine
directly, but returns
[exit codes](https://github.com/AlxHnr/interface/blob/master/main.c#L11).

## Building

This program requires the development files of the following libraries:

* SDL (_not_ SDL2)
* SDL\_image
* SDL\_mixer
* SDL\_gfx
* SDL\_ttf

Build and run the program using `make run`.

## Assets used

* [Crystal icon theme](https://www.opendesktop.org/p/1002590)
* [Elementary icon theme](https://github.com/elementary/icons)
* [Glass icon theme](https://www.gnome-look.org/content/show.php/Glass+Icons+Theme?content=32146)
* [Oxygen icon theme](https://github.com/KDE/oxygen-icons)
* [Tango icon theme](https://github.com/elementary/icons)
* [Artwiz fonts - Expanded set](http://artwizaleczapka.sourceforge.net)
* [DejaVu font family](https://dejavu-fonts.github.io)

Some of these have been resized or tinted. If you find some assets used in
this project, which I've forgot to list here, file an issue with the link
to the original work. It was not possible for me to find the sources of the
sound effects. I have modified them heavily years ago. For each sound
effect used, I can find 10 almost identical sound files from different
authors, not knowing which the original is. If you can help, please file an
issue.

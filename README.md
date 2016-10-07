BingWallpaper
=============

A Simple C App To download the "Wallpaper of the day" on www.bing.com and
to set it as the Desktop Wallpaper.

##Requirements
* curl

BingWallpaper supports several desktop environments:
- GNOME 3+
- Unity 7+
- XFCE 4+
- Any window manager that supports `feh`

##Instructions

* Download the source.  
* Change into source directory  
* Run :   
```
./configure  
make  
sudo make install
```
* Now you can change wallpaper to the current bing wallpaper by just running   
the _bingit_ command on the terminal.  

__Play around with cron and crontab to make it change wallpaper automatically every morning :D__

###Crontab Help

* install crontab (cronie maybe)
* Add `5 0 * * * /usr/local/bin/bingit` to crontab using `crontab -e`

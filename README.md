# Proxy Master

## Warning: BEFORE USING THIS APP YOU MUST ADD `source ~/.config/proxy-set.sh` TO YOUR `.bashrc`


## How to use

- add `source ~/.config/proxy-set.sh` to your `bashrc`
- open the app and enter the IP address and port
- click on the main switch (big OFF/ON icon on top)

## What it does

It's a simple app to change your proxy settings everywhere. It automatically changes your proxy settings on:

- KDE: this is enough for chromium-based web browsers
- gsettings: some apps and browsers only use gsettings to get their proxy settings and simply ignore KDE's
- environment variables: curl, Qt apps, etc.
- sudo: adds `-E` flag to `sudo` so package managers like `apt` and `pacman` use the system proxy
- git: git does not use environment variables. Proxy Master can handle that as well.
- yt-dlp: adds appropriate flags to guide yt-dlp to use the system's proxy
- snap: has its own settings (but sometimes changing it can be really slow for some reason)

## How to build 

This app uses TGUI and C++26 (with hardend `std` and other safety features)

You need gcc15 to build it:

```bash
mkdir build
cd build 
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```


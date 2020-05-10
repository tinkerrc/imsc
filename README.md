# `imsc` - Scoring Engine for Linux
**This project is still under development.**

# Introduction
This is an attempt at making a scoring engine for practice images (Ubuntu w/ gdm3) for
CyberPatriot. This is not a daemon, instead it should be run periodically
 (e.g., cron job). The user needs to initialize the image by supplying `imsc`
 a personal token. `imsc` will then tell the server the token and send an initial scoring report
  to prevent an early start. On each run, `imsc` scores the image and spits out the report (text file)
  on desktop. After scoring stops, `imsc` will also generate a final report on desktop 
  and send one to server as well.
  
This program is by no means secure and cannot eliminate cheating.  
It is trivially exploitable and it only keeps the honest people away.
 
The engine can be configured using 3 files:
- The checklist file (TOML) is used to give points 
 or penalties depending on the exit code of the command specified
 in the rule.
- The config file stores time-related variables and flags so that
 the program can save them and use in the next run. In retrospect I should've
 named it something else.
- `config.h` specifies some constants that is hardcoded into the binary
 use `cmake -Dvar1=value -Dvar2=value ...` to define the constants

There will be sample files to play around with when I finish making this.

Note that both files have to be encrypted before hand. I will make scripts
to help with that too. (Note: right now there is no encryption)

This scoring engine also requires a server to prevent second attempt. 
The server-side script haven't been implemented yet. If you have HTTPS
 you can also transmit the encryption key that way (but you will need
 to modify the code; see the `Client` and `ScoringManager` class).

# Build
## Install libraries
```bash
$ sudo apt update
$ sudo apt install -y libboost-all-dev libnotify-dev libcrypto++-dev \
                    libcurlpp-dev libcurl4-openssl-dev
$ git clone https://github.com/ToruNiina/toml11
$ cd toml11
$ mkdir build
$ cd build
$ cmake ..
$ make && sudo make install
```

## Build options
Options that are not required have default values. See `CMakeLists.txt`
- `IMSC_USER`: ***(required)*** default user of vm
- `IMSC_URL`: ***(required)*** `imsc` server URL
- `IMSC_SECRET`: **(recommended)** key that is **hardcoded** into the binary for now
- `IMSC_CONFIG_FILE`: path for the config file
- `IMSC_CHECKLIST_FILE`: path for the checklist file
- `IMSC_LAST_REPORT_FILE`: path for the last (encrypted) report file

## Build `imsc`
Your `cmake` arguments may differ
```bash
$ git clone https://www.github.com/oakrc/imsc
$ cd imsc
$ mkdir build
$ cd build
$ cmake -DIMSC_SECRET=super_strong_key -DIMSC_USER=abc -DIMSC_URL=https://example.com .. 
$ make
```
### Note
I haven't tested this so I have no idea
 but the binary might need to be built on the target VM... 
 (assuming you own a Linux machine)

# License
GPL v3.0, see LICENSE

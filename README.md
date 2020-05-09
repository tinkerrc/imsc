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

## Create config.h
Define these macros in `config.h`

```c++
#ifndef SECRET
#define SECRET "SECRET_PASSWORD" // hmmm hardcoded into the binary for now

#define HOME_DIR "/home/user" // home folder of the default user in the vm

#define CONFIG_FILE "/etc/imsc1.dat"
#define CHECKLIST_FILE "/etc/imsc2.dat"
#define LAST_REPORT_FILE "/etc/imsc3.dat"
#define LOG_FILE HOME_DIR "/scoring-log.txt"

#define IMSC_URL "http://imsc.example.com" // imsc server

#define SCORING_INTVL_MINS 3 // minutes to wait before next report
#endif
```
In the future I will probably use CMake to set them instead.

## Build `imsc`
```bash
$ git clone https://www.github.com/oakrc/imsc
$ cd imsc
$ mkdir build
$ cd build
$ cmake ..
$ make
```

I haven't tested this so I have no idea
 but the binary might need to be built on the target VM...

# License
GPL v3.0, see LICENSE

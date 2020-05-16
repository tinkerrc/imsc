# `imsc` - Scoring Engine for Linux
**This project is still under development.**

# Introduction
This is an attempt at making a scoring engine for tryout images (Linux) for
CyberPatriot. The user needs to run `imsc TOKEN` after he/she boots up the image.
The token is sent by the proctor beforehand.

# How does it work
A tryout image is defined using 2 files
- The checklist file (TOML) contains a list of rules (vulns or penalties)
- The config file stores image name and time constraints. It is created on server-side.
The proctor needs to upload the checklist to the server since they shouldn't
be stored on client-side. (Why store an encrypted file when you have SSL)

I haven't made the server-side script yet. You will need to host the server yourself when it's finished.

There will be sample checklists to play with when I finish making this.

# How to Build
```bash
$ sudo apt update
$ sudo apt install -y libcurlpp-dev libcurl4-openssl-dev
$ git clone --recurse-submodules https://www.github.com/oakrc/imsc
$ mkdir build
$ cd build
$ cmake .. -DIMSC_URL=https://imsc.example.com # **CHANGE THIS**
$ make && sudo make install
```

# License
GPL v3.0, see LICENSE

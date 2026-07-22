extract markers from an audio file. Libsndfile (https://libsndfile.github.io/libsndfile/) does most of the heavy lifting. 

This is an updated re-hash of a similar utility I created in 2004, which was specific for files prepped for the Max ~groove object. This version has a default option suitable to get a marker list in ms to use with ChucK's sndbuff ugen. Cut n' paste from the terminal.

---
ATM, this src only compiles (if you use make) for Intel. You can get a Silicon-compatible version by adding arch flags and ensuring that libsndfile is installed via MacPorts (or whatever you used to obtain it) as universal:
> sudo port install libsndfile +universal

... The Makefile needs some tinkering w. arch detection to make this work. I will get to this

this app is intended to run across several Mac I use:
M1pro 26.2 - note about Silicon above
Intel 10.15,
Intel 10.14 & 
Intel 10.13


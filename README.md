# SWIG Python Bindings for TMK Video Hashing

# Installation 

## tl;dr
```
git clone https://github.com/meedan/tmkpy
cd tmkpy
python setup.py build
python setup.py install

```

## Longer version
Ensure swig is installed (`sudo apt install swig`) and then build this extension.

`setup.py` will first build the TMK C++ code per the instructions at tmk/cpp/ (namely run `make` in that directory). This will build a file called `libtmk.a`

It will then run swig and generate `tmkpy.py` and `tmkpy_wrap.cpp`. It will then compile everything.
To install system wide, run `python setup.py install`

# Usage


```
import tmkpy

#Hash a video
vid=tmkpy.hashVideo("test/chair-19-sd-bar.mp4","/usr/bin/ffmpeg")

#Write the hash to a file (second argument is a string used in error messages to identify the program and can be anything)
vid.writeToOutputFile("output.tmk","anything_here")

#Get the 256-dimensional vector that is used to compute level-1 scores. Level-1 scores are the cosine similarity of these vectors.
l1features=vid.getPureAverageFeature()

#Compute level-2 scores against other tmk files on disk
import glob
haystack=[f for f in glob.glob("test/*.tmk")]
scores=tmkpy.query(vid,haystack,1)
scores=[(x,y) for x,y in zip(haystack,scores)]
print(scores)
```

tmkpy.query(needle,haystack,threads) expects needle to an actual TMK object or the name of a tmkfile. Haystack is a list of tmk filenames. The function computes all level-2 scores and returns them in a list equal to the length of, and in the same order as, haystack.  It does not compute level-1 scores: it just computes all level-2 scores for the files in haystack. If a file is invalid, the score will be -1. There is no need to filter invalid / missing filenames. 

# Error handling

`tmkpy.query` generally sollows errors quietly if there is a missing or invlaid filename in haystack. The one excpetion is if you are passing in a string for the needle and a file with that string name does not exist. In this case, `tmkpy.query` will throw an invalid_argument exception. This is raised in Python as a `RuntimeError` and can be handle in the normal Python way. E.g.,

```
try:
	scores=tmkpy.query("invalid-missing-file.tmk",["hs1","hs2"],1)
except RuntimeError as e:
	print(e)
```

Generates
```
fopen: No such file or directory
tmkpy: could not open "invalid-missing-file.tmk" for read.
tmkpy: failed to read needle "invalid-missing-file.tmk".
Failed to read needle from supplied filename
```
The first three lines are written to standard error in C++. The last line is the result of `print(e)` in Python

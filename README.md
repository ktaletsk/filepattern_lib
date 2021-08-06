## File tree

Library:
- functions.h
- functions.cpp

Main file (demo):
- filepattern.cpp

## Build

```bash
docker build . -t filepattern
```

This will install dependencies (CMake, Apache Arrow binaries), copy the sources, compile filepattern binary, download the test data (Small_Fluorescent_Test_Dataset)

## Run

```bash
docker run filepattern
```

This will run filepattern on the test data with pattern `img_r0{yy}_c00{x+}.tif` and print out Arrow Table containing parsed test data
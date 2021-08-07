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

Prepare the data collection to parse in `<path_to_data>`, then run

```bash
docker run -v <path_to_data>:/data filepattern <filepattern> /data
```

This will run filepattern on the data collection with pattern `<filepattern>` and print out Arrow Table containing parsed test data
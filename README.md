# ppc2cpp
(work in progress) reverse engineering library and tool.

### Requirements
- protobuf
  - Debian: `sudo apt install -y protobuf-compiler libprotobuf-dev libz-dev`

### Building
```
cmake -B build
cmake --build build -j
```

## Tools
### ppc2cpp create
Create a ppc2cpp project from a series of ELF or RVL (DOL and REL) format binary files
### ppc2cpp viz
Visualize a function's data flow graph
### ppc2cpp checkflow (experimental)
Compare the equivalence between the version of a function in two ppc2cpp projects using their data flow graph. This ignores regalloc, scheduling and certain dead code
### ppc2cpp importppcdis
Import symbol and relocation information from ppcdis
### ppc2pp compare (experimental)
Compare two ppc2cpp programs. This takes a symbol and reference based approach, which allows symbols to be rearranged arbitrarily. This requires symbol name, type and size information, as well as relocations.

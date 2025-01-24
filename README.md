# ppc2cpp
PowerPC binary reverse engineering library and tool. The current functionality revolves around checking for program and function equivalence

### Requirements
- protobuf
  - Debian: `sudo apt install -y protobuf-compiler libprotobuf-dev libz-dev`
If you only care about using, there are cross platform builds in the GitHub releases

### Building
```
cmake -B build
cmake --build build -j
```

## Usage
ppc2cpp operates on "project files" created by `pp2cpp create`. If symbol and relocation information are available, that project file can then be used by the rest of the tools.

## Tools
`ppc2cpp --help` can be run to get the list of verbs. `ppc2cpp <verb> -- help` offers instructions for that verb specifically.
### ppc2cpp create
Create a ppc2cpp project from a series of ELF or RVL (DOL and REL) format binary files.

Example:

```bash
$ ppc2cpp create -o myProject.ppc2cpp myBinary.o
```
### ppc2cpp viz
Visualize a function's data flow graph. This tool creates a graphviz file that can be rendered to an image using a tool like neato.

Example:

```bash
$ ppc2cpp viz -i myProject.ppc2cpp -o graph.dot
```
### ppc2cpp checkflow
Compare the equivalence between the version of a function in two ppc2cpp projects using their data flow graph. This ignores regalloc, scheduling and certain dead code. Returns exit code 0 on equivalence, non zero otherwise

Example:

```bash
$ ppc2cpp checkflow project1.ppc2cpp project2.ppc2cpp function1 function2 ...
```
### ppc2cpp importppcdis
Import symbol and relocation information from ppcdis. This is useful if any of the binaries passed to `ppc2cpp create` did not contain symbol information.

Each `-p` flag indicates a triplet of ppcdis files: the binary descriptor YAML, label pickle file and relocation pickle file.
Example:
```bash
$ ppc2cpp importppcdis -s symbols.yml -i myProject.ppc2cpp -p ppcdis_dir/dol.yaml ppcdis_dir/dol_labels.pickle ppcdis_dir/dol_relocs.pickle -p ppcdis_dir/rel.yaml ppcdis_dir/rel_labels.pickle ppcdis_dir/rel_relocs.pickle
```
### ppc2pp compare (experimental)
Compare two ppc2cpp programs. This takes a symbol and reference based approach, which allows symbols to be rearranged arbitrarily. This requires symbol name, type and size information, as well as relocations.

Example:

```bash
$ ppc2cpp checkflow project1.ppc2cpp project2.ppc2cpp
```

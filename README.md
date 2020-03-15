# FilePacker
Library to pack several files into only one and retrieve one file among several.

File created starts has the header "JaJa" then the following architecture:
1. 16 bits for file count
2. For each file
    2.1. 16 bits for file name
    2.2. file name (relative to the archive)
    2.3. 64 bits file size
3. Files content

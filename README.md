# ts-mini
 
A lightweight MPEG Transport Stream packetizer and parser written in C++.
 
Built to demonstrate understanding of how broadcast systems process media transport streams.
 
## What it does
 
```
Raw file → Packetizer → .ts stream (188-byte packets) → Parser → Restored file + Stats
```
 
- **Packetizer** — Splits any file into 184-byte chunks, wraps each in a 4-byte TS header (sync byte, PID, continuity counter)
- **Parser** — Scans for sync bytes, parses headers, extracts payloads, detects packet loss via continuity counter validation
- **Roundtrip** — Packs and parses in-memory, verifies data integrity byte-for-byte
 
## Build
 
```bash
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
```
 
Requires CMake 3.16+ and a C++20 compiler (GCC 11+).
Google Test is fetched automatically.
 
## Usage
 
```bash
# Roundtrip test — pack then parse, verify integrity
./ts-mini round <input_file>
```
 
Example:
 
```
$ echo "Hello MPEG-TS!" > test.txt
$ ./ts-mini round test.txt
Original file: test.txt (15 bytes)
 
[Pack]  1 packets generated
[Pack]  TS stream: 188 bytes
 
[Parse] Packets parsed: 1
[Parse] Sync losses: 0
[Parse] CC errors: 0
 
[Verify] Original size:  15 bytes
[Verify] Restored size:  184 bytes
[Verify] Data integrity: PASS
```

Generate videos:
```
$ ffmpeg -f lavfi -i testsrc=duration=10:size=320x240:rate=25 -c:v libx264 test10s.ts
$ vlc --avcodec-hw none test10s.ts # play the video
$ ./ts-mini round test10s.ts
Original file: test10s.ts (112800 bytes)
[Pack]  614 packets generated
[Pack]  TS stream: 115432 bytes

[Parse] Packets parsed: 614
[Parse] Sync losses: 0
[Parse] CC errors: 0

[Verify] Original size:  112800 bytes
[Verify] Restored size:  112976 bytes
[Verify] Data integrity: PASS

```
 
## Tests
 
```bash
./ts_tests
```
 
10 tests covering TSPacket bit manipulation, Packetizer chunking and CC wrapping, Parser roundtrip and sync loss detection.
 
## Project structure
 
```
ts-mini/
├── CMakeLists.txt
├── include/
│   ├── ts_packet.hpp      TS packet with bit-level header accessors
│   ├── packetizer.hpp      Raw data → TS packets
│   └── parser.hpp          TS stream → restored data + stats
├── src/
│   ├── ts_packet.cpp
│   ├── packetizer.cpp
│   ├── parser.cpp
│   └── main.cpp            CLI (roundtrip command)
└── test/
    └── test_ts.cpp
```
 
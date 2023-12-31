# {{ Your Puzzle Name }} based on Tempus-Pico
This repo should be cloned with `--recursive` or `git submodule update --init --recursive` should be ran after clone it setup all the submodules

# Template
This is a template for building RP2040 based puzzles that use the Tempus Pico Library to connect to the Tempus Clue System.  
Update the following variables in the `tempus_pico_puzzle.cpp` file:-  

```c
// Define this RoomScreens details
const char *room_slug = "template-room";            // max length ROOM_STRING_SIZE
const char *component_name = "TemplateRoomPuzzle";  // max length NAME_STRING_SIZE
// optional define a PuzzleData store
const char *store_name = "template-room:puzzle";    // max length STORE_NAME_STRING_SIZE
```

You can also update the `set(NAME tempus_pico_puzzle)` in the top level `CMakeLisits.txt` and rename the `src/*_main.cpp` file to match

# Build
The pico-sdk needs to be cloned separately and it's path set in your environment.

```bash
mkdir build
cd build
cmake ..
make
````

# Flash

```bash
picotool load -f src/tempus_pico_puzzle.uf2
```

# Pins

| Pin  | Alt use    | Usage            | Notes                                  |
|------|------------|------------------|----------------------------------------|
| GP0  |            |                  |                                        |
| GP1  |            |                  |                                        |
| GP2  |            |                  |                                        |
| GP3  |            |                  |                                        |
| GP4  |            |                  |                                        |
| GP5  |            |                  |                                        |
| GP6  |            |                  |                                        |
| GP7  |            |                  |                                        |
| GP8  |            |                  |                                        |
| GP9  |            |                  |                                        |
| GP10 |            |                  |                                        |
| GP11 |            |                  |                                        |
| GP12 |            |                  |                                        |
| GP13 |            |                  |                                        |
| GP14 |            |                  |                                        |
| GP15 |            |                  |                                        |
| GP16 | spi0       | MISO             | Wiznet                                 |
| GP17 |            | CSn              | Wiznet                                 |
| GP18 | spi0       | SCLK             | Wiznet                                 |
| GP19 | spi0       | MOSI             | Wiznet                                 |
| GP20 |            | RSTn             | Wiznet                                 |
| GP21 |            | INTn             | Wiznet                                 |
| GP22 |            |                  |                                        |
| GP23 |            |                  |                                        |
| GP24 |            |                  |                                        |
| GP25 |            | On Board LED     |                                        |
| GP26 |            |                  |                                        |
| GP27 |            |                  |                                        |
| GP28 |            |                  |                                        |

# Puzzle Data
Store Name `template-room:puzzle`

| Key           | Type   | Default           | Notes                            |
|---------------|--------|-------------------|----------------------------------|
|               |        |                   |                                  |

# Custom Puzzle Topics
These topics are prefixed by `tempus/room/{room_slug}/puzzle/{component_name}/`  
Topic max length is 50 (`PUZZLE_CUSTOM_TOPIC_SIZE`)
Payload should be json.  
A maximum of 2 subscribes are allowed by default (`CONFIG_MQTT_MAX_PUZZLE_CUSTOM_SUBSCRIBES`)

| Topic             | S/P | Value / Usage                                       |
|-------------------|-----|-----------------------------------------------------|
|                   |     |                                                     |

# Logic
TODO

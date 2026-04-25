# Motion Interpolation

Motion Interpolation is a small two-player network simulation for comparing interpolation techniques in an online game setting. Each client controls a local player, receives the other player's networked positions through a UDP server, and reconstructs the remote player's motion using the selected interpolation method.

The project is intentionally visual: it lets you switch interpolation methods, choose predefined movement paths, adjust the number of polynomial samples, and inspect the resulting motion through fading player trails.

## Features

- Two SFML clients connected through a lightweight UDP server.
- Runtime interpolation selection:
  - Linear interpolation
  - Lagrange interpolation
  - Newton divided-difference interpolation
- Configurable sample count for polynomial interpolation.
- Predefined movement tests:
  - Sinusoidal path
  - Bounded spiral path
  - Square path
- Manual player movement when the path test is paused.
- WASD and AZERTY-friendly ZQSD keyboard controls.
- Fading trails behind both local and remote players to visualize path shape.
- Side-by-side client windows for easier comparison.

## Project Structure

```text
motion-interpolation/
├── client/
│   ├── include/
│   └── src/
├── server/
│   ├── include/
│   └── src/
├── assets/
│   └── fonts/
├── CMakeLists.txt
└── README.md
```

Important files:

- `client/src/Game.cpp`: client UI, update loop, interpolation selection, trails, and path test control.
- `client/src/Interpolate.cpp`: linear, Lagrange, and Newton interpolation implementations.
- `client/src/Player.cpp`: manual movement and predefined path movement.
- `client/src/NetworkClient.cpp`: UDP client send/receive logic.
- `server/src/Server.cpp`: UDP server that registers clients and forwards packets.

## Requirements

- CMake 3.10 or newer
- A C++17 compiler
- Git
- SFML dependencies required by your platform

SFML itself is fetched automatically through CMake `FetchContent` using SFML `3.0.2`.

On Ubuntu/WSL, you may need common graphics/audio/network development packages for SFML to build and run correctly.

## Build

From the repository root:

```bash
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
```

The executables are generated in:

```text
server/build/server
client/build/client
```

## Run

Open three terminals from the repository root.

Start the server:

```bash
./server/build/server
```

Start client 1:

```bash
./client/build/client 0
```

Start client 2:

```bash
./client/build/client 1
```

Client `0` opens on the left side of the screen. Client `1` opens on the right side.

## Using The Client

The client window contains a small control panel.

Interpolation controls:

- `Linear`: use linear interpolation between remote samples.
- `Lagrange`: use Lagrange polynomial interpolation.
- `Newton`: use Newton divided-difference polynomial interpolation.

Path controls:

- `Sinusoidal`: smooth wave-like path.
- `Spiral`: bounded out-and-back spiral centered in the window.
- `Square`: path with sharp turns for testing corner behavior.

Other controls:

- `Start Test`: starts the selected predefined movement path.
- `Pause Test`: pauses the predefined path and returns to manual movement.
- `-` / `+`: decreases or increases the number of samples used by Lagrange and Newton interpolation.

Manual movement:

- QWERTY: `W`, `A`, `S`, `D`
- AZERTY: `Z`, `Q`, `S`, `D`

Manual movement is active when the predefined path test is paused.

## How The Simulation Works

Each client sends its local player's position to the server at a fixed interval. The server forwards each packet to the other registered clients. The receiving client stores the remote samples in a buffer and renders the remote player using the selected interpolation method.

Current timing values:

- Send interval: `250 ms`
- Interpolation delay: `0.6 s`
- Render limit: `60 FPS`

The intentionally low send rate makes interpolation artifacts easier to see. Linear interpolation should usually look stable, while Lagrange and Newton can overshoot or jitter on sparse data, especially on sharp paths such as the square.

## Interpolation Notes

Linear interpolation is the most stable baseline. It does not attempt to curve between samples, so it usually avoids overshoot.

Lagrange interpolation fits a polynomial through the selected samples. It can look smoother on some curved paths, but it is sensitive to sparse samples and can overshoot.

Newton interpolation evaluates an equivalent polynomial form using divided differences. In this simulation it is useful for comparison with Lagrange because both are polynomial methods but use different formulations.

The `Samples` control changes how many buffered samples Lagrange and Newton use. Higher values can create smoother curves in ideal cases, but they can also increase overshoot and instability.

## Known Limitations

- The networking layer is intentionally simple and intended for local experimentation.
- The server forwards UDP packets without authentication or client cleanup.
- Remote sample times are based on receive time, not sender simulation time.
- The protocol sends only positions, not velocity or path metadata.
- Polynomial interpolation is experimental here and not necessarily suitable for production game networking.


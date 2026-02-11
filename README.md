# Terminal Velocity

[![Homebrew](https://img.shields.io/badge/brew%20install-rjchicago%2Ftap%2Ftermv-blue)](https://github.com/rjchicago/homebrew-tap)
[![Docker](https://img.shields.io/badge/docker-rjchicago%2Ftermv-blue?logo=docker)](https://hub.docker.com/r/rjchicago/termv)

A high-performance, terminal-based game written in C with ncurses.

## Quick Start

### Docker (no install required)

```bash
docker run -it rjchicago/termv
```

### Docker Compose (build from source)

```bash
docker compose run --build termv
```

### Homebrew (macOS)

```bash
brew install rjchicago/tap/termv
termv
```

### Build from source

Requires `gcc` and `ncurses-dev`:

```bash
make
./termv
```

## Controls

| Key          | Action             |
|--------------|--------------------|
| Left Arrow   | Move left          |
| Right Arrow  | Move right         |
| Down Arrow   | Soft drop (hold)   |
| Z / Up Arrow | Rotate counter-CW  |
| X / Shift+Up | Rotate clockwise   |
| Space        | Hard drop          |
| T            | Cycle theme        |
| P            | Pause / Resume     |
| Q / Esc      | Quit               |

## Features

- 10x20 visible playfield (10x40 internal board with hidden buffer)
- All 7 standard tetrominoes (I, O, T, S, Z, J, L)
- 7-bag randomizer for fair piece distribution
- Wall kick rotation system
- Ghost piece preview (shows where hard drop will land)
- Soft drop (hold Down) and hard drop (Space)
- Lock delay with reset on move/rotate
- Line clears with row collapse
- Scoring: line clears, soft drop, hard drop
- Level progression (speed increases every 10 lines)
- Next piece preview
- Color-coded pieces with 3 themes (Pastel, Retro, Matrix) — press T to cycle
- Pause and game over states
- Deterministic RNG with optional seed: `./termv <seed>`

## Build from Source

Requires `gcc` and `ncurses-dev`:

```bash
# macOS (ncurses is built-in)
make

# Ubuntu/Debian
sudo apt install libncursesw5-dev
make

# Alpine
apk add gcc musl-dev ncurses-dev make
make
```

Run:

```bash
./termv
```

Optional seed for deterministic games:

```bash
./termv 42
```

Check version:

```bash
./termv --version
```

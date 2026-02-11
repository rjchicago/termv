# Contributing to Terminal Velocity

Thanks for your interest in contributing to termv!

## Development Setup

### Prerequisites

- `gcc`
- `ncurses-dev` (built-in on macOS)
- `make`
- Docker (optional, for containerized builds)

### Build

```bash
make
./termv
```

Or with Docker:

```bash
docker compose run --build termv
```

### Clean

```bash
make clean
```

## Project Structure

```
termv/
├── src/
│   ├── main.c         # Entry point, game loop
│   ├── board.c/h      # Playfield logic
│   ├── piece.c/h      # Tetromino definitions and rotation
│   ├── game.c/h       # Game state, scoring, gravity
│   ├── render.c/h     # ncurses rendering
│   ├── input.c/h      # Input handling
│   ├── theme.c/h      # Color themes
│   └── version.h      # Version define
├── Makefile
├── Dockerfile
├── docker-compose.yml
└── docker-compose.ci.yml
```

## Making Changes

1. Fork the repo and create a branch from `main`
2. Make your changes
3. Test locally: `make clean && make && ./termv`
4. Submit a pull request

## Code Style

- C99 standard
- `-Wall -Wextra` clean (no warnings)
- Keep functions small and focused
- Comment non-obvious logic

## Releasing

Releases are fully automated. When a version tag is pushed, GitHub Actions will:

1. Create a GitHub Release with auto-generated notes
2. Build and push a Docker image to Docker Hub (`rjchicago/termv`)
3. Update the Homebrew formula in [homebrew-tap](https://github.com/rjchicago/homebrew-tap)

### To release a new version:

```bash
git tag v0.2.0
git push origin v0.2.0
```

That's it. The rest happens automatically:

- `termv --version` will reflect the new version (baked in at build time)
- `brew upgrade termv` will pick up the new release
- `docker pull rjchicago/termv` will pull the latest image

### Version format

Follow [Semantic Versioning](https://semver.org/): `vMAJOR.MINOR.PATCH`

- **PATCH**: bug fixes
- **MINOR**: new features, backward compatible
- **MAJOR**: breaking changes

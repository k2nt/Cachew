# RedisDiy

RedisDiy is a small Redis-compatible, in-memory key–value server written in C++23. It is a learning project: the goal is to understand networking, protocol parsing, and server design by building each piece by hand.

It speaks a subset of RESP2 over TCP, so `redis-cli` can talk to it.

> **Status:** Milestone 0 scaffold. The executable starts and exits; networking and commands are not implemented yet. See [DESIGN.md](./DESIGN.md) for the full design and milestone plan.

## Planned commands

| Command | Behavior |
|---|---|
| `PING [msg]` | Replies `PONG`, or echoes `msg` |
| `ECHO msg` | Returns `msg` |
| `SET key value` | Stores the value, overwrites any existing one, clears any TTL |
| `GET key` | Returns the value, or nil if missing or expired |
| `DEL key [key ...]` | Returns how many keys were removed |
| `EXISTS key [key ...]` | Returns how many of the keys exist |
| `EXPIRE key seconds` | Sets a TTL. Returns 1 if the key exists, else 0 |
| `TTL key` | Remaining seconds, `-1` if no TTL, `-2` if the key is missing |

Command names are case-insensitive. Planned later: `SET` options `EX`, `PX`, `NX`, `XX`.

## Not supported

Persistence, replication, clustering, auth, TLS, pub/sub, transactions, lists/hashes/sets, inline (plain-text) commands, and multithreaded command execution. RedisDiy has no auth and binds to localhost only; do not expose it to a network.

## Build and run

Requires GNU GCC 16 (`gcc-16` and `g++-16` on `PATH`), CMake 3.25+, Ninja, and Git.
The first configure downloads GoogleTest, so internet access is required.

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
./build/debug/RedisDiy
```

Replace `debug` with `release` for an optimized build.
If switching from a previously configured compiler, configure with
`cmake --fresh --preset debug`.

The presets select GNU g++ directly, without invoking Homebrew. For a different
installed GNU compiler version, override it when configuring:

```sh
cmake --fresh --preset debug -DCMAKE_C_COMPILER=gcc-15 -DCMAKE_CXX_COMPILER=g++-15
```

`redis-cli` will be needed for manual testing once networking is implemented.

## Docs

See [DESIGN.md](./DESIGN.md) for the architecture, testing strategy, and milestones.




# skyroads

An endless runner built in C++ and OpenGL 3.3, inspired by the classic Skyroads game. The game layer is original work; the engine layer (window, input, GPU buffers, shader loader) is the ACS framework provided by the Faculty of Automatic Control and Computers, University Politehnica of Bucharest.

> This project was the motivation to build [pingu-engine](https://github.com/valentinpopescu98/pingu-engine) — a full 3D graphics engine from scratch.

*All rights to the framework reserved to Universitatea Politehnica din București, Faculty of Automatic Control and Computers, and all framework authors.*

---

## Gameplay

You control a sphere rolling across floating platforms in 5 parallel lanes. Platforms scroll toward you — jump between lanes, manage your fuel, and survive as long as possible.

**Controls:**

| Key | Action |
|---|---|
| W / S | Increase / decrease speed |
| A / D | Move left / right between lanes |
| Space | Jump (only when on a platform) |
| C | Toggle first-person / third-person camera |

---

## Features

### Platforms & Power-ups

Platforms are color-coded by their effect when touched:

| Color | Effect | Probability |
|---|---|---|
| Blue | No effect | 50% |
| Green | +Fuel | 20% |
| Yellow | −Fuel | 10% |
| Orange | Speed boost (15 units, 5 sec) | 15% |
| Red | Instant death | 5% |
| Purple | Currently active (any type) | — |

Platform lengths are randomized per spawn. When a platform scrolls off screen, it respawns at the back with a new random length and power-up.

### Physics

- Gravity accumulates on `vitezaCadere` each frame, clamped to min/max fall speed
- Landing on a platform zeroes fall speed and resets jump availability
- Falling below the platform level (or running out of fuel) triggers the death sequence
- Ball shrinks to 0 on death animation

### Fuel System

- Fuel drains continuously over time (`0.05 * deltaTime`)
- Green platforms refuel; yellow platforms drain; red platforms empty immediately
- Displayed as a vertical bar (outline + fill) in screen space on the left side of the viewport

### Deformation Power-up

When a power-up platform is touched, a 1-second deformation effect activates on the player sphere. Implemented in the vertex shader — each vertex is displaced along its normal by a pseudo-random value derived from texture coordinates:

```glsl
gl_Position = Projection * View * Model * vec4(v_position - v_normal * random(tex_coord), 1.0);
```

### Camera

- **Third-person** (default): position `(0, 1.6, 2.5)` behind and above the ball
- **First-person**: position `(0, 0.4, -0.5)` at ball level

---

## Tech Stack

| Layer | Technology |
|---|---|
| Language | C++ |
| Graphics API | OpenGL 3.3 |
| Framework | ACS / UPB lab framework |
| Math | GLM |

---

## Known issues / possible improvements

- Power-up deformation visual could be smoother
- Fuel counter border renders with diagonal lines from box mesh triangles (no hollow mesh available in the framework)

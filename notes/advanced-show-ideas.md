# Advanced Light Show Ideas (Yun + 1 or 2 Matrices)

This document captures advanced concepts for evolving the July 4th matrix project from a single panel to synchronized dual-panel shows.

## High-Impact Visual Ideas

1. Panorama mode (side by side)
   - Treat two 32x8 panels as one 64x8 canvas.
   - Use long scroll text, flag waves, skyline silhouettes, and wide firework trails.

2. Vertical totem mode (one on top)
   - Treat two 32x8 panels as one 32x16 canvas.
   - Use launch-from-bottom and burst-at-top fireworks.

3. Dual-role choreography
   - Left/bottom panel as launcher/base.
   - Right/top panel as burst/echo with slight delay for depth.

4. Call-and-response scenes
   - One panel animates first, second answers 200-500 ms later.
   - Works well for rhythmic scene timing.

5. Symmetry and break
   - Begin with mirrored effects.
   - Break symmetry in the finale for impact.

6. Layer illusion
   - Bottom panel as city line/ground.
   - Top panel as sky with stars, comets, and fireworks.

## Advanced Control Concepts

1. Master/slave sync over WiFi
   - Master sends scene ID, start timestamp, and random seed.
   - Slave computes the same scene locally for smooth sync.

2. Deterministic randomness
   - Same seed on both boards keeps sparkles/bursts aligned.
   - Reduces network traffic.

3. BPM timeline engine
   - Drive transitions on beat boundaries.
   - Makes scene changes feel intentional and music-ready.

4. Live trigger API
   - Endpoints like /scene/fireworks and /mode/finale.
   - Quick testing from phone or browser.

5. Role switching command
   - Flip panel roles without reflashing.
   - Useful when physical placement changes.

## July 4th Show Concepts

1. Countdown split
   - One panel shows countdown.
   - Second panel shows increasing pyro intensity.

2. Flag unfurl
   - Tight stripes on one panel.
   - Expand into cross-panel full-width wave.

3. History cards
   - Short cards such as 1776 and USA.
   - Transitions sweep between panels.

4. Grand finale
   - Alternating strobes.
   - Cross-panel comet sweeps.
   - Synchronized white burst at the end.

## Practical Build Path

1. If panels are physically close
   - Prefer one Yun driving both panels.
   - Lower complexity and easier timing.

2. If using two Yuns
   - Build timestamped scene sync first.
   - Then add complex visuals.

3. Keep effects resolution-agnostic
   - Write effects against width/height config.
   - Reuse on 32x8, 64x8, and 32x16 layouts.

## Next Design Step (Optional)

Define two architecture options in code docs:
1. Single-controller dual-panel wiring and pin map.
2. Dual-controller sync protocol with message format and timing tolerance.

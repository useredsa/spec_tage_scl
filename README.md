# Speculative TAGE-SC-L Interfaces to Computer Architecture Simulators

This repository contains an adapted version of
the TAGE-SC-L branch predictor implementation of the [Scarab] simulator
with interfaces for [ChampSim], [Gem5] and [MBPlib].

The reason for having this repository is to have a
correct and unique implementation that can be used in different simulators.
For example, (at the moment of writing) Gem5 has a TAGE-SC-L implementation,
but its speculative execution is incorrect,
and its performance is that of a tournament predictor
rather than a TAGE predictor.

We chose to start with the implementation in the Scarab simulator
because it was mostly correct.
We will make merge requests to the repository with the changes we performed.

[Scarab]: https://github.com/hpsresearchgroup/scarab
[ChampSim]: https://github.com/ChampSim/ChampSim
[Gem5]: https://github.com/gem5/gem5
[MBPlib]: https://github.com/useredsa/MBPlib

## Interfaces

You can see the available interfaces in the [ifaces] folder.
Check the README of each interface to read about the installation process
and other details.

[ifaces]: /include/tagescl/ifaces/

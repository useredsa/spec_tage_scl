# MBPlib Wrapper Interface for TAGE-SC-L

To use the interface,
create a symlink of this folder
or add it to your include path.
The file to include is `tage_sc_l_sim_only.hpp`.

## Simulation-only Interface

This implementation allows to simulate with MBPlib,
but does not comply with the MBPlib interface.
The reason is that the original implementation needs to predict a branch
before trying to track its outcome,
and needs to return a branch id to the user.
This implementation is fine to use as long as
each call to `predict` is followed by a call to
one and only one of `train` and `track`,
and `train` is never called without calling `predict` first.

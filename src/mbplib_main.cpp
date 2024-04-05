#include <mbp/sim/simulator.hpp>

#include "mbplib_compat_layer.hpp"

#if TAGE_SC_L_SIZE == 64
static TageScl<TAGE_SC_L_CONFIG_64KB> branchPredictor(1);
#elif TAGE_SC_L_SIZE == 80
static TageScl<TAGE_SC_L_CONFIG_80KB> branchPredictor(1);
#else
#error Unsupported TAGE_SC_L_SIZE setting.
#endif

int main(int argc, char** argv) {
  return mbp::SimMain(argc, argv, &branchPredictor);
}

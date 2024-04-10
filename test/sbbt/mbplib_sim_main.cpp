#include <mbp/sim/simulator.hpp>

#include "tagescl/ifaces/mbplib/tage_sc_l_sim_only.hpp"

#if TAGE_SC_L_SIZE == 64
static tagescl::MbpTageScl<tagescl::CONFIG_64KB> branchPredictor(1);
#elif TAGE_SC_L_SIZE == 80
static tagescl::MbpTageScl<tagescl::CONFIG_80KB> branchPredictor(1);
#else
#error Unsupported TAGE_SC_L_SIZE setting.
#endif

int main(int argc, char** argv) {
  return mbp::SimMain(argc, argv, &branchPredictor);
}

#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <mbp/sim/sbbt_reader.hpp>
#include <mbp/sim/simulator.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_set>
#include <vector>

#include "template_lib/tagescl.h"

#ifndef MAX_WRONG_PATH_BRANCHES
#error MAX_WRONG_PATH_BRANCHES not defined
#endif

constexpr int kMaxWrongPathBranches = MAX_WRONG_PATH_BRANCHES;

template <class CONFIG>
mbp::json SimulateWithRandomWrongPath(Tage_SC_L<CONFIG>& branchPredictor,
                                      const mbp::SimArgs& args) {
  const auto& [tracepath, warmupInstrs, simInstr, stopAtInstr] = args;
  mbp::SbbtReader trace{tracepath};
  struct BranchInfo {
    std::int64_t occurrences, misses;
  };
  std::unordered_set<uint64_t> branchIps;
  std::int64_t numBranches = 0;
  std::int64_t mispredictions = 0;
  std::vector<std::string> errors;

  auto startTime = std::chrono::high_resolution_clock::now();
  mbp::Branch b;
  std::int64_t instrNum = 0;
  std::srand(1000);
  while ((instrNum = trace.nextBranch(b)) < stopAtInstr) {
    branchIps.insert(b.ip());
    tagescl::Branch_Type bType;
    bType.is_conditional = b.isConditional();
    bType.is_indirect = b.isIndirect();
    int bId = bp.get_new_branch_id();
    bool predictedTaken = b.isTaken();
    bool prediction = bp.get_prediction(bId, b.ip());
    if (b.isConditional()) {
      predictedTaken = prediction;
    }
    branchPredictor.update_speculative_state(bId, b.ip(), bType, predictedTaken,
                                             b.target());
    if (b.isConditional()) {
      bool mispredicted = predictedTaken != b.isTaken();
      if (mispredicted) {
        int wrongPathBranches = std::rand() % (1 + kMaxWrongPathBranches);
        while (wrongPathBranches--) {
          Branch_Type rndBranchType;
          rndBranchType.is_conditional = std::rand() % 2;
          rndBranchType.is_indirect = (std::rand() % 20) == 0;
          std::uint64_t rndBranchIp = b.ip() + std::rand();
          std::uint64_t rndBranchTarget = rndBranchIp + std::rand();
          int rndBranchId = branchPredictor.get_new_branch_id();
          bool rndBranchPred =
              branchPredictor.get_prediction(rndBranchId, rndBranchIp);
          branchPredictor.update_speculative_state(rndBranchId, rndBranchIp,
                                                   rndBranchType, rndBranchPred,
                                                   rndBranchTarget);
        }
        branchPredictor.flush_branch_and_repair_state(bId, b.ip(), bType,
                                                      b.isTaken(), b.target());
      }
      branchPredictor.commit_state(bId, b.ip(), bType, b.isTaken());
      if (instrNum >= warmupInstrs) {
        numBranches += 1;
        mispredictions += mispredicted;
      }
    }
    branchPredictor.commit_state_at_retire(bId, b.ip(), bType, b.isTaken(),
                                           b.target());
  }
  auto endTime = std::chrono::high_resolution_clock::now();
  double simulationTime =
      std::chrono::duration<double>(endTime - startTime).count();
  // See Note 0.
  std::int64_t metricInstr =
      simInstr == 0 ? trace.numInstructions() - warmupInstrs : simInstr;
  if (simInstr != 0 && trace.eof()) {
    std::string errMsg = "The trace did not contain " +
                         std::to_string(simInstr) + " instructions, only " +
                         std::to_string(trace.lastInstrRead());
    errors.emplace_back(errMsg);
  }

  mbp::json j = {
      {"metadata",
       {
           {"simulator", "SBBT trace with rnd spec path on misps."},
           {"simulator_version", "v0.1.0"},
           {"simulator_max_wrong_path_depth", kMaxWrongPathBranches},
           {"trace", tracepath},
           {"warmup_instr", warmupInstrs},
           {"simulation_instr", metricInstr},
           {"exhausted_trace", trace.eof()},
           {"num_conditonal_branches", numBranches},
           {"num_branch_instructions", branchIps.size()},
           {"predictor", {{"name", "Adapter of Scarab's TAGE-SC-L to MBPlib"}}},
       }},
      {"metrics",
       {
           {"mpki", 1000.0 * mispredictions / metricInstr},
           {"mispredictions", mispredictions},
           {"accuracy",
            static_cast<double>(numBranches - mispredictions) / numBranches},
           {"simulation_time", simulationTime},
       }},
      {"predictor_statistics", {}},
      {"errors", errors},
  };
  return j;
}

#if TAGE_SC_L_SIZE == 64
static Tage_SC_L<TAGE_SC_L_CONFIG_64KB> branchPredictor(1 +
                                                        kMaxWrongPathBranches);
#elif TAGE_SC_L_SIZE == 80
static Tage_SC_L<TAGE_SC_L_CONFIG_80KB> branchPredictor(1 +
                                                        kMaxWrongPathBranches);
#else
#error Unsupported TAGE_SC_L_SIZE setting.
#endif

int main(int argc, char** argv) {
  auto args = mbp::ParseCmdLineArgs(argc, argv);
  mbp::json output = SimulateWithRandomWrongPath(branchPredictor, args);
  std::cout << std::setw(2) << output << std::endl;
  return output["errors"].empty() ? 0 : mbp::ERR_SIMULATION_ERROR;
}

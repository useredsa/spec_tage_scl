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

#include "tagescl/tagescl.hpp"

#ifndef NUM_CORRECT_PATH_INSTRS
#error NUM_CORRECT_PATH_INSTRS not defined
#endif
#ifndef NUM_WRONG_PATH_BRANCHES
#error NUM_WRONG_PATH_BRANCHES not defined
#endif

constexpr int kNumCorrectPathInstrs = NUM_CORRECT_PATH_INSTRS + 1;
constexpr int kNumWrongPathBranches = NUM_WRONG_PATH_BRANCHES;

static_assert(kNumCorrectPathInstrs >= 1,
              "NUM_CORRECT_PATH_INSTRS shall be non-negative");
static_assert(kNumWrongPathBranches >= 0,
              "NUM_WRONG_PATH_BRANCHES shall be non-negative");

struct RobEntry {
  std::uint32_t bId;
  std::int64_t instrNum;
  mbp::Branch b;
};

constexpr tagescl::Branch_Type Type(const mbp::Branch& b) {
  tagescl::Branch_Type type{};
  type.is_conditional = b.isConditional();
  type.is_indirect = b.isIndirect();
  return type;
}

template <class CONFIG>
mbp::json Sim(tagescl::Tage_SC_L<CONFIG>& bp, const mbp::SimArgs& args) {
  const auto& [tracepath, warmupInstrs, simInstr, stopAtInstr] = args;
  mbp::SbbtReader trace{tracepath};
  struct BranchInfo {
    std::int64_t occurrences, misses;
  };
  std::unordered_set<uint64_t> branchIps;
  std::int64_t numBranches = 0;
  std::int64_t mispredictions = 0;
  std::vector<std::string> errors;

  std::vector<RobEntry> rob(1 + kNumCorrectPathInstrs + kNumWrongPathBranches);
  std::size_t front = 0;
  std::size_t back = 0;
  std::srand(1000);
  auto startTime = std::chrono::high_resolution_clock::now();
  mbp::Branch b;
  bool mispredicted = false;

  while (true) {
    std::int64_t instrNum = trace.nextBranch(b);
    while (front != back && (mispredicted || instrNum - rob[front].instrNum >=
                                                 kNumCorrectPathInstrs)) {
      const auto& r = rob[front];
      if (r.b.isConditional()) {
        bp.commit_state(r.bId, r.b.ip(), Type(r.b), r.b.isTaken());
      }
      bp.commit_state_at_retire(r.bId, r.b.ip(), Type(r.b), r.b.isTaken(),
                                r.b.target());
      front = front + 1 < rob.size() ? front + 1 : 0;
    }
    if (instrNum >= stopAtInstr) break;
    branchIps.insert(b.ip());
    std::uint32_t bId = bp.get_new_branch_id();
    rob[back].instrNum = instrNum;
    rob[back].bId = bId;
    rob[back].b = b;
    back = back + 1 < rob.size() ? back + 1 : 0;
    bool prediction = bp.get_prediction(bId, b.ip());
    if (b.isConditional()) {
      mispredicted = prediction != b.isTaken();
      bp.update_speculative_state(bId, b.ip(), Type(b), prediction, b.target());
      if (mispredicted) {
        for (int i = 0; i < kNumWrongPathBranches; ++i) {
          tagescl::Branch_Type rndType;
          rndType.is_conditional = std::rand() % 2;
          rndType.is_indirect = (std::rand() % 20) == 0;
          std::uint64_t rndIp = b.ip() + std::rand();
          std::uint64_t rndTgt = rndIp + std::rand();
          std::uint32_t rndId = bp.get_new_branch_id();
          bool rndPred = bp.get_prediction(rndId, rndIp);
          bp.update_speculative_state(rndId, rndIp, rndType, rndPred, rndTgt);
        }
        bp.flush_branch_and_repair_state(bId, b.ip(), Type(b), b.isTaken(),
                                         b.target());
      }
      if (instrNum >= warmupInstrs) {
        numBranches += 1;
        mispredictions += mispredicted;
      }
    } else {
      bp.update_speculative_state(bId, b.ip(), Type(b), b.isTaken(),
                                  b.target());
    }
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
           {"simulator", "SBBT trace with late commit and wrong path."},
           {"simulator_version", "v0.1.0"},
           {"simulator_num_correct_path_instrs", kNumCorrectPathInstrs},
           {"simulator_num_wrong_path_branches", kNumWrongPathBranches},
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
static tagescl::Tage_SC_L<tagescl::CONFIG_64KB> branchPredictor(
    kNumCorrectPathInstrs + kNumWrongPathBranches);
#elif TAGE_SC_L_SIZE == 80
static tagescl::Tage_SC_L<tagescl::CONFIG_80KB> branchPredictor(
    kNumCorrectPathInstrs + kNumWrongPathBranches);
#else
#error Unsupported TAGE_SC_L_SIZE setting.
#endif

int main(int argc, char** argv) {
  auto args = mbp::ParseCmdLineArgs(argc, argv);
  mbp::json output = Sim(branchPredictor, args);
  std::cout << std::setw(2) << output << std::endl;
  return output["errors"].empty() ? 0 : mbp::ERR_SIMULATION_ERROR;
}

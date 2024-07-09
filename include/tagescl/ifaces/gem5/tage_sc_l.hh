#ifndef __CPU_PRED_TAGE_SC_L_HH__
#define __CPU_PRED_TAGE_SC_L_HH__

#include <vector>

#include "base/types.hh"
#include "cpu/pred/bpred_unit.hh"
#include "cpu/pred/tage_base.hh"
#include "params/TAGE_SC_L.hh"
#include "cpu/pred/tagescl/tagescl.hpp"

namespace gem5
{

namespace branch_prediction
{

class TAGE_SC_L: public BPredUnit
{
  private:
    tagescl::Tage_SC_L<tagescl::CONFIG_64KB> tage;

  protected:
    virtual bool predict(ThreadID tid, Addr branch_pc, bool cond_branch,
                         void* &b);

    struct TageSclBranchInfo
    {
        uint32_t id;
        Addr pc;
        tagescl::Branch_Type br_type;
        TageSclBranchInfo()
        {}
    };

  public:

    TAGE_SC_L(const TAGE_SC_LParams &params);

    // Base class methods.
    bool lookup(ThreadID tid, Addr pc, void* &bp_history) override;
    void updateHistories(ThreadID tid, Addr pc, bool uncond, bool taken,
                         Addr target,  void * &bp_history) override;
    void update(ThreadID tid, Addr pc, bool taken,
                void * &bp_history, bool squashed,
                const StaticInstPtr & inst, Addr target) override;
    virtual void squash(ThreadID tid, void * &bp_history) override;
};

} // namespace branch_prediction

} // namespace gem5

#endif // __CPU_PRED_TAGE_SC_L_HH__

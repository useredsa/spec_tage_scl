# Add the following to the BranchPredictor.py
class TAGE_SCL(BranchPredictor):
    type = "TAGE_SCL"
    cxx_class = "gem5::branch_prediction::TAGE_SC_L"
    cxx_header = "cpu/pred/tage_sc_l.hh"

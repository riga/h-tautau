/*! Definiton of analysis::FlatEventInfo class.
This file is part of https://github.com/hh-italian-group/h-tautau. */

#pragma once

#include <TLorentzVector.h>
#include <TMatrixD.h>

#include "SyncTree.h"
#include "AnalysisTypes.h"
#include "exception.h"

namespace analysis {

struct SyncEventInfo {
    typedef std::pair<size_t, size_t> BjetPair;

    static size_t NumberOfCombinationPairs(size_t n_bjets)
    {
        return n_bjets * (n_bjets - 1) / 2;
    }

    static size_t CombinationPairToIndex(const BjetPair& pair, size_t n_bjets)
    {
        const size_t min = std::min(pair.first, pair.second);
        const size_t max = std::max(pair.first, pair.second);
        if(n_bjets < 2 || min == max || min >= n_bjets || max >= n_bjets)
            throw exception("bad combination pair (%1%, %2%) for n b-jets = %3%.")
                % pair.first % pair.second % n_bjets;
        return max - 1 + min * (2 * n_bjets - 3 - min) / 2;
    }

    static BjetPair CombinationIndexToPair(size_t index, size_t n_bjets)
    {
        if(n_bjets < 2 || index >= n_bjets * (n_bjets - 1) / 2)
            throw exception("bad combination index = %1% for n b-jets = %2%.") % index % n_bjets;

        for(size_t min = 0;; ++min) {
            const size_t l = CombinationPairToIndex(BjetPair(min, n_bjets - 1), n_bjets);
            if(l >= index) {
                const size_t max = index + n_bjets - 1 - l;
                return BjetPair(min, max);
            }
        }
    }

 const ntuple::Sync* event;
 analysis::Channel channel;
 std::vector<TLorentzVector> lepton_momentums;
 std::vector<TLorentzVector> bjet_momentums;
 BjetPair selected_bjets;
 bool has_bjet_pair;
 Float_t npu;
 Int_t npv;
 TLorentzVector MET, Htt, Htt_MET, Hbb, resonance;
 TMatrixD MET_covariance;

 SyncEventInfo(const ntuple::Sync& _event, const BjetPair& _selected_bjets)
     : event(&_event),
       channel(static_cast<analysis::Channel>(analysis::Channel::MuTau)),
       lepton_momentums(2), bjet_momentums(_event.pt_jets.size()), selected_bjets(_selected_bjets),
       has_bjet_pair(false), npu(-1), npv(-1), MET_covariance(2, 2)
 {
     lepton_momentums.at(0).SetPtEtaPhiM(event->pt_1, event->eta_1, event->phi_1, event->m_1);
     lepton_momentums.at(1).SetPtEtaPhiM(event->pt_2, event->eta_2, event->phi_2, event->m_2);
     npu = event->npu;
     npv = event->npv;

     for(size_t n = 0; n < bjet_momentums.size(); ++n)
         bjet_momentums.at(n).SetPtEtaPhiE(event->pt_jets.at(n), event->eta_jets.at(n), event->phi_jets.at(n),
                                           event->energy_jets.at(n));

     MET.SetPtEtaPhiM(event->met, 0, event->metphi, 0);
     MET_covariance(0, 0) = event->metcov00;
     MET_covariance(1, 0) = event->metcov10;
     MET_covariance(0, 1) = event->metcov01;
     MET_covariance(1, 1) = event->metcov11;

     Htt = lepton_momentums.at(0) + lepton_momentums.at(1);
     Htt_MET = Htt + MET;
     has_bjet_pair = selected_bjets.first < bjet_momentums.size() && selected_bjets.second < bjet_momentums.size();
     if(has_bjet_pair) {
         Hbb = bjet_momentums.at(selected_bjets.first) + bjet_momentums.at(selected_bjets.second);
         resonance = Htt_MET + Hbb;
     }
 }

};

using SyncEventInfoPtr = std::shared_ptr<SyncEventInfo>;

} // namespace analysis

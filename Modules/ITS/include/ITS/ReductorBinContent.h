// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file   ReductorBinContent.h
/// \author 
///
#ifndef QUALITYCONTROL_REDUCTORBINCONTENT_H
#define QUALITYCONTROL_REDUCTORBINCONTENT_H

#include "QualityControl/Reductor.h"
#include <vector>

namespace o2::quality_control_modules::its
{

class ReductorBinContent : public quality_control::postprocessing::Reductor
{
   public:
      ReductorBinContent() = default;
      ~ReductorBinContent() = default;

      void* getBranchAddress() override;
      const char* getBranchLeafList() override;
      void update(TObject* obj) override;

   private:
      struct BinContentInfo {
         double binContent {0.}; // Bin content in the slice
      };

      std::vector<BinContentInfo> mStats;
};

} // namespace o2::quality_control_modules::its

#endif // QUALITYCONTROL_REDUCTORBINCONTENT_H

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
/// \file    TrendingTaskITSFEE.h
/// \author   on the structure from Piotr Konopka
///

#ifndef QUALITYCONTROL_TRENDINGTASKITSFEE_H
#define QUALITYCONTROL_TRENDINGTASKITSFEE_H

#include "ITS/TrendingTaskConfigITS.h"
#include "QualityControl/PostProcessingInterface.h"
#include "QualityControl/Reductor.h"
// #include "ITS/ReductorBinContent.h"

#include <memory>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <TTree.h>
#include <TColor.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TGraph.h>

namespace o2::quality_control::repository
{
class DatabaseInterface;
}

namespace o2::quality_control::postprocessing
{

class TrendingTaskITSFEE : public PostProcessingInterface/*, private quality_control_modules::its::ReductorBinContent*/
{
   public:
      /// \brief Constructor.
      TrendingTaskITSFEE() = default;
      /// \brief Destructor.
      ~TrendingTaskITSFEE() override = default;

      /// \brief Post-processing methods inherited from 'PostProcessingInterface'.
      void configure(std::string name, const boost::property_tree::ptree& config) override;
      void initialize(Trigger, framework::ServiceRegistry&) override;
      void update(Trigger, framework::ServiceRegistry&) override;
      void finalize(Trigger, framework::ServiceRegistry&) override;

   private:
      struct MetaData{
         Int_t runNumber = 0;
      };

      void trendValues(const Trigger& t, repository::DatabaseInterface& qcdb);

      void drawCanvasMO(const std::string& var, const std::string& name, const std::string& opt);

      void getTrendVariables(const std::string& inputvar, std::string& sourceName, std::string& variableName, std::string& trend);
      void generatePlots();

      // Graph style ...
      TrendingTaskConfigITS mConfig;
      MetaData mMetaData;
      UInt_t mTime;

      std::unique_ptr<TTree> mTrend;
      std::unordered_map<std::string, std::unique_ptr<Reductor>> mReductors;
      // std::unordered_map<std::string, std::vector<BinContentInfo>*> mSources;
      // std::unordered_map<std::string, int> mNumberPads;
};

} // namespace o2::quality_control::postprocessing

#endif // QUALITYCONTROL_TRENDINGTASKITSFEE_H

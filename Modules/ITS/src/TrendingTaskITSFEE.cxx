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
/// \file    TrendingTaskITSFEE.cxx
/// \author
///

#include "ITS/TrendingTaskITSFEE.h"
#include "ITS/ReductorBinContent.h"

#include "QualityControl/RootClassFactory.h"
#include "QualityControl/DatabaseInterface.h"
#include "QualityControl/MonitorObject.h"
#include "QualityControl/QcInfoLogger.h"
#include "QualityControl/Reductor.h"
#include "QualityControl/ObjectMetadataKeys.h"
#include "ITS/ReductorBinContent.h"

#include <TCanvas.h>
#include <TDatime.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TGraph.h>
#include <TH1.h>
#include <TStyle.h>
#include <TLegend.h>

#include <string>
#include <map>

using namespace o2::quality_control;
using namespace o2::quality_control::core;
using namespace o2::quality_control::postprocessing;
using namespace o2::quality_control::repository;
using namespace o2::quality_control_modules::its;

void TrendingTaskITSFEE::configure(std::string name, const boost::property_tree::ptree& config)
{
  mConfig = TrendingTaskConfigITS(name, config);
}

void TrendingTaskITSFEE::initialize(Trigger, framework::ServiceRegistry&)
{
   // Preparing data structure of TTree
   ILOG(Info, Support) << "Generating new TTree for ITS Fee Trending Task" << ENDM;
   mTrend = std::make_unique<TTree>();
   mTrend->SetName(PostProcessingInterface::getName().c_str());

   // mTrend->Branch("meta", &mMetaData, "runNumber/I");
   mTrend->Branch("runNumber", &mMetaData.runNumber);
   mTrend->Branch("time", &mTime);
   // mTrend->Branch("ntreeentries", &ntreeentries);

   // for (const auto& source : mConfig.dataSources) {
      // mSources[source.name] = new std::vector<BinContentInfo>();
      // mTrend->Branch(source.name.c_str(), &mSources[source.name]);
   // }

   // Reductor
   for (const auto& source : mConfig.dataSources) {
      std::unique_ptr<Reductor> reductor(root_class_factory::create<Reductor>(source.moduleName, source.reductorName));

      mTrend->Branch(source.name.c_str(), reductor->getBranchAddress(), reductor->getBranchLeafList());

      mReductors[source.name] = std::move(reductor);
   }
}

void TrendingTaskITSFEE::update(Trigger t, framework::ServiceRegistry& services)
{
   auto& qcdb = services.get<repository::DatabaseInterface>();

   trendValues(t, qcdb);

   //Comment out for some time
   // storePlots(qcdb);
   // storeTrend(qcdb);
}

void TrendingTaskITSFEE::finalize(Trigger t, framework::ServiceRegistry& services)
{
  auto& qcdb = services.get<repository::DatabaseInterface>();

  //Comment out for some time
  // storePlots(qcdb);
  // storeTrend(qcdb);

  generatePlots();

  // for (const auto& source : mConfig.dataSources) {
  //    delete mSources[source.name];
  //    mSources[source.name] = nullptr;
  // }
}

void TrendingTaskITSFEE::trendValues(const Trigger& t, repository::DatabaseInterface& qcdb)
{
   mTime = t.timestamp / 1000; // ROOT expects seconds since epoch.
   mMetaData.runNumber = t.activity.mId;

   for(auto& dataSource : mConfig.dataSources) {

      // mNumberPads[dataSource.name] = 0;
      // mSources[dataSource.name]->clear();

      if(dataSource.type == "repository") {

         auto mo = qcdb.retrieveMO(dataSource.path, dataSource.name, t.timestamp, t.activity);

         TObject* obj = mo ? mo->getObject() : nullptr;

         if(obj) {
            mReductors[dataSource.name]->update(obj);
         }
      } else {
         ILOG(Error, Support) << "Data source '" << dataSource.type << "' is not of type repository." << ENDM;
      }
   }
   mTrend->Fill();
} // void SliceTrendingTask::trendValues(const Trigger& t, repository::DatabaseInterface& qcdb)

//----------------------------------------------------------------------------------------------
void TrendingTaskITSFEE::drawCanvasMO(const std::string& var, const std::string& name, const std::string& opt)
{
  // Determine the order of the plot (1 - histo, 2 - graph, ...)
  // const size_t plotOrder = std::count(var.begin(), var.end(), ':') + 1;

  // Prepare the strings for the dataSource and its trending quantity.
  std::string varName, typeName, trendType;
  getTrendVariables(var, varName, typeName, trendType);

  // Setup the tree reader with the needed values.
  // TTreeReader myReader(mTrend.get());
  // TTreeReaderValue<UInt_t> retrieveTime(myReader, "time");
  // TTreeReaderValue<std::vector<BinContentInfo>> dataRetrieveVector(myReader, varName.data());

  // const int nuPa = mNumberPads[varName];
  // const int nEntries = mTrend->GetEntriesFast();
  //
  //   for (int p = 0; p < nuPa; p++) {
  //
  //     while (myReader.Next()) {
  //
  //       const double timeStamp = (double)(*retrieveTime);
  //       const double dataPoint = (dataRetrieveVector->at(p)).retrieveValue(typeName);
  //
  //       std::cout << "timeStamp: " << timeStamp << std::endl;
  //       std::cout << "dataPoint: " << dataPoint << std::endl;
  //
  //     }
  //     myReader.Restart();
  //   }
} // void TrendingTaskITSFEE::drawCanvasMO

void TrendingTaskITSFEE::getTrendVariables(const std::string& inputvar, std::string& sourceName, std::string& variableName, std::string& trend)
{
  const std::size_t posEndVar = inputvar.find(".");  // Find the end of the dataSource.
  const std::size_t posEndType = inputvar.find(":"); // Find the end of the quantity.
  sourceName = inputvar.substr(0, posEndVar);
  variableName = inputvar.substr(posEndVar + 1, posEndType - posEndVar - 1);
  trend = inputvar.substr(posEndType + 1, -1);
} // void TrendingTaskITSFEE::getTrendVariables

void TrendingTaskITSFEE::generatePlots()
{
  if (mTrend->GetEntries() < 1) {
    ILOG(Info, Support) << "No entries in the trend so far, no plot generated." << ENDM;
    return;
  }

  ILOG(Info, Support) << "Generating " << mConfig.plots.size() << " plots." << ENDM;
  for (const auto& plot : mConfig.plots) {
     // Delete the existing plots before regenerating them.
     // if (mPlots.count(plot.name)) {
     //   getObjectsManager()->stopPublishing(plot.name);
     //   delete mPlots[plot.name];
     // }

     // Postprocess each pad (titles, axes, flushing buffers).
     const std::size_t posEndVar = plot.varexp.find("."); // Find the end of the dataSource.
     const std::string varName(plot.varexp.substr(0, posEndVar));

     // Draw the trending on a new canvas.
     drawCanvasMO(plot.varexp, plot.name, plot.option);
  }
} // void TrendingTaskITSFEE::generatePlots

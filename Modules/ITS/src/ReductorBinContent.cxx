// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file   ReductorBinContent.cxx
/// \author
///

#include "QualityControl/QcInfoLogger.h"
#include <TH1.h>
#include "ITS/ReductorBinContent.h"
#include <iostream>
#include <fmt/format.h>

namespace o2::quality_control_modules::its
{
void* ReductorBinContent::getBranchAddress()
{
  return &mStats;
}

const char* ReductorBinContent::getBranchLeafList()
{
  return "qq";
}

void ReductorBinContent::update(TObject* obj)
{

   // Get the number of pads, and their list in case of an input canvas.
   // std::cout << "obj->IsA(): " << obj->InheritsFrom("TH1") << std::endl; //Использовал в cluster задании для QCanalysis

   // ILOG(Info, Support) << "Number of input histograms for the trending of " << obj->GetName() << " : " << numberPads << ENDM;

   // Access the histograms embedded in 'obj'.
   auto histo = static_cast<TH1*>(obj);
   std::cout << "================REDUCTOR=========================" << std::endl;

   if(histo){
      int numberOfBins = histo->GetXaxis()->GetNbins();
      std::cout << "numberOfBins: " << numberOfBins << std::endl;

      // finalNumberPads = numberOfBins; //надобность для моего задания под вопросом. Используется при "рисовании"

      // Get the trending quantities defined in 'SlicerInfo'.
      for(int j = 1; j < numberOfBins; j++){ // количество слайсов определяется по размеру массива, заданного в json в разделе "axisDivision"
         mStats.push_back(BinContentInfo());
         mStats[j - 1].binContent = histo->GetBinContent(j);

         std::cout << "mStats.binContent: " << mStats[j - 1].binContent << std::endl;
      }
   } else {
      ILOG(Error, Support) << "Error: 'histo' not found." << ENDM;
   }

}
} // namespace o2::quality_control_modules::its

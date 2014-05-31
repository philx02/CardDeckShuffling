#include "ShuffleAlgorithmPerformance.h"
#include "as_range.h"

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <random>
#include <fstream>
#include <deque>

namespace ba = boost::accumulators;

static const std::size_t sDeckSize = 60;
static const std::size_t sNumberOfIterations = 1000000;

template< typename Iter, typename URNG >
void oneMashShuffle(Iter iBegin, Iter iEnd, URNG &iRng, double iDeckBreakPointStdDev)
{
  static const std::uniform_int<> wUniformInt(0, 1);
  std::normal_distribution<> wNormalDist(static_cast< double >(iEnd - iBegin) / 2.0, iDeckBreakPointStdDev);
  Iter wBreakingPoint;
  do
  {
    wBreakingPoint = iBegin + static_cast< std::size_t >(wNormalDist(iRng));
  } while (wBreakingPoint < iBegin && wBreakingPoint >= iEnd);

  std::deque< std::size_t > wFirstStack(iBegin, wBreakingPoint);
  std::deque< std::size_t > wSecondStack(wBreakingPoint, iEnd);
  auto wDequeue = [](std::deque< std::size_t > &iDeque) {auto wValue = iDeque.front(); iDeque.pop_front(); return wValue; };
  for (auto &wCardId : as_range(iBegin, iEnd))
  {
    if (wFirstStack.empty())
    {
      wCardId = wDequeue(wSecondStack);
    }
    else if (wSecondStack.empty())
    {
      wCardId = wDequeue(wFirstStack);
    }
    else
    {
      wCardId = wUniformInt(iRng) == 0 ? wDequeue(wFirstStack) : wDequeue(wSecondStack);
    }
  }
}

int main()
{
  std::random_device wRandomDevice;
  std::mt19937 wRandomNumberGenerator(wRandomDevice());

  std::vector< ShuffleAlgorithmPerformance > wShuffleAlgorithmPerformances;

  wShuffleAlgorithmPerformances.emplace_back(testRandomShuffle("sorted", wRandomNumberGenerator, [](std::vector< std::size_t >::iterator iBegin, std::vector< std::size_t >::iterator iEnd, decltype(wRandomNumberGenerator) & iRandomNumberGenerator)
  {
  }));
  wShuffleAlgorithmPerformances.emplace_back(testRandomShuffle("std_shuffle (computerized)", wRandomNumberGenerator, [](std::vector< std::size_t >::iterator iBegin, std::vector< std::size_t >::iterator iEnd, decltype(wRandomNumberGenerator) & iRandomNumberGenerator)
  {
    std::shuffle(iBegin, iEnd, iRandomNumberGenerator);
  }));
  for (std::size_t wNumberOfMashShuffle = 0; wNumberOfMashShuffle < 10; ++wNumberOfMashShuffle)
  {
    wShuffleAlgorithmPerformances.emplace_back(testRandomShuffle(std::to_string(wNumberOfMashShuffle) + " MashShuffle(s)", wRandomNumberGenerator, [=](std::vector< std::size_t >::iterator iBegin, std::vector< std::size_t >::iterator iEnd, decltype(wRandomNumberGenerator) & iRandomNumberGenerator)
    {
      for (std::size_t wIter = 0; wIter < wNumberOfMashShuffle; ++wIter)
      {
        oneMashShuffle(iBegin, iEnd, iRandomNumberGenerator, 5.0);
      }
    }));
  }

  std::ofstream wCsvResults("Results.csv");
  ShuffleAlgorithmPerformance::printTitles(wCsvResults);
  for (const auto &iShuffleAlgorithmPerformance : wShuffleAlgorithmPerformances)
  {
    wCsvResults << iShuffleAlgorithmPerformance;
    std::ofstream wCardOccurence(iShuffleAlgorithmPerformance.getName() + ".csv");
    iShuffleAlgorithmPerformance.dumpCardOccurences(wCardOccurence);
    wCardOccurence.close();
  }
  wCsvResults.close();

  return 0;
}

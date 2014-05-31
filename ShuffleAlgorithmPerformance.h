#include <string>
#include <vector>

class ShuffleAlgorithmPerformance
{
public:
  ShuffleAlgorithmPerformance(std::string iAlgorithmName, double iMeanOfMeans, double iStdDevOfMeans, double iMeanOfStdDevs, double iStdDevOfStdDevs, std::vector< std::vector< std::size_t > > &&iCardOccurenceCounter)
    : mAlgorithmName(std::move(iAlgorithmName))
    , mMeanOfMeans(iMeanOfMeans)
    , mStdDevOfMeans(iStdDevOfMeans)
    , mMeanOfStdDevs(iMeanOfStdDevs)
    , mStdDevOfStdDevs(iStdDevOfStdDevs)
    , mCardOccurenceCounter(std::move(iCardOccurenceCounter))
  {}

  const std::string &getName() const
  {
    return mAlgorithmName;
  }

  static void printTitles(std::ostream &oStream)
  {
    oStream << "AlgorithmName" << ", ";
    oStream << "MeanOfMeans" << ", ";
    oStream << "StdDevOfMeans" << ", ";
    oStream << "MeanOfStdDevs" << ", ";
    oStream << "StdDevOfStdDevs" << ", ";
    oStream << std::endl;
  }

  void dumpCardOccurences(std::ostream &oStream) const
  {
    for (const auto &iRow : mCardOccurenceCounter)
    {
      for (const auto &iCell : iRow)
      {
        oStream << iCell << ',';
      }
      oStream << '\n';
    }
  }

private:
  friend std::ostream & operator<<(std::ostream &, const ShuffleAlgorithmPerformance &);
  std::string mAlgorithmName;
  double mMeanOfMeans;
  double mStdDevOfMeans;
  double mMeanOfStdDevs;
  double mStdDevOfStdDevs;
  std::vector< std::vector< std::size_t > > mCardOccurenceCounter;
};

std::ostream & operator<<(std::ostream &oStream, const ShuffleAlgorithmPerformance &iShuffleAlgorithmPerformance)
{
  oStream << iShuffleAlgorithmPerformance.mAlgorithmName << ", ";
  oStream << iShuffleAlgorithmPerformance.mMeanOfMeans << ", ";
  oStream << iShuffleAlgorithmPerformance.mStdDevOfMeans << ", ";
  oStream << iShuffleAlgorithmPerformance.mMeanOfStdDevs << ", ";
  oStream << iShuffleAlgorithmPerformance.mStdDevOfStdDevs << ", ";
  oStream << std::endl;
  return oStream;
}

template< typename ShuffleAlgo, typename URNG >
ShuffleAlgorithmPerformance testRandomShuffle(const std::string &iAlgorithmName, URNG &iRandomNumberGenerator, ShuffleAlgo &&iShuffleAlgo)
{
  std::vector< std::size_t > wInitialDeck;
  for (std::size_t wCard = 0; wCard < sDeckSize; ++wCard)
  {
    wInitialDeck.emplace_back(wCard);
  }
  std::vector< std::size_t > wDeck;
  wDeck.reserve(sDeckSize);

  std::vector< ba::accumulator_set< std::size_t, ba::stats< ba::tag::variance > > > wVectorOfAccumulators(sDeckSize);
  std::vector< std::vector< std::size_t > > wCardOccurenceCounter(sDeckSize, std::vector< std::size_t >(sDeckSize, 0));

  for (std::size_t wIteration = 0; wIteration < sNumberOfIterations; ++wIteration)
  {
    wDeck = wInitialDeck;
    iShuffleAlgo(std::begin(wDeck), std::end(wDeck), iRandomNumberGenerator);
    auto wAccumulatorIter = std::begin(wVectorOfAccumulators);
    std::for_each(std::begin(wDeck), std::end(wDeck), [&](decltype(*std::begin(wDeck)) && iCardId) {(*(wAccumulatorIter++))(iCardId); });
    auto wCardOccurenceCounterIter = wCardOccurenceCounter.begin();
    for (const auto &wCardId : wDeck)
    {
      ++(wCardOccurenceCounterIter++)->at(wCardId);
    }
  }

  ba::accumulator_set< std::size_t, ba::stats< ba::tag::variance > > wMeanAccumulator;
  ba::accumulator_set< std::size_t, ba::stats< ba::tag::variance > > wStdDevAccumulator;

  for (const auto &wAccumulator : wVectorOfAccumulators)
  {
    wMeanAccumulator(ba::mean(wAccumulator));
    wStdDevAccumulator(std::sqrt(ba::variance(wAccumulator)));
  }

  return ShuffleAlgorithmPerformance(iAlgorithmName, ba::mean(wMeanAccumulator), std::sqrt(ba::variance(wMeanAccumulator)), ba::mean(wStdDevAccumulator), std::sqrt(ba::variance(wStdDevAccumulator)), std::move(wCardOccurenceCounter));
}

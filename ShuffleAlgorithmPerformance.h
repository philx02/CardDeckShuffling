#include <string>

class ShuffleAlgorithmPerformance
{
public:
  ShuffleAlgorithmPerformance(std::string iAlgorithmName, double iMeanOfMeans, double iStdDevOfMeans, double iMeanOfStdDevs, double iStdDevOfStdDevs)
    : mAlgorithmName(std::move(iAlgorithmName))
    , mMeanOfMeans(iMeanOfMeans)
    , mStdDevOfMeans(iStdDevOfMeans)
    , mMeanOfStdDevs(iMeanOfStdDevs)
    , mStdDevOfStdDevs(iStdDevOfStdDevs)
  {}

  static void printTitles(std::ostream &oStream)
  {
    oStream << "AlgorithmName" << ", ";
    oStream << "MeanOfMeans" << ", ";
    oStream << "StdDevOfMeans" << ", ";
    oStream << "MeanOfStdDevs" << ", ";
    oStream << "StdDevOfStdDevs" << ", ";
    oStream << std::endl;
  }

private:
  friend std::ostream & operator<<(std::ostream &, const ShuffleAlgorithmPerformance &);
  std::string mAlgorithmName;
  double mMeanOfMeans;
  double mStdDevOfMeans;
  double mMeanOfStdDevs;
  double mStdDevOfStdDevs;
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

  for (std::size_t wIteration = 0; wIteration < sNumberOfIterations; ++wIteration)
  {
    wDeck = wInitialDeck;
    iShuffleAlgo(std::begin(wDeck), std::end(wDeck), iRandomNumberGenerator);
    auto wAccumulatorIter = std::begin(wVectorOfAccumulators);
    std::for_each(std::begin(wDeck), std::end(wDeck), [&](decltype(*std::begin(wDeck)) && iCardId) {(*(wAccumulatorIter++))(iCardId); });
  }

  ba::accumulator_set< std::size_t, ba::stats< ba::tag::variance > > wMeanAccumulator;
  ba::accumulator_set< std::size_t, ba::stats< ba::tag::variance > > wStdDevAccumulator;

  for (const auto &wAccumulator : wVectorOfAccumulators)
  {
    wMeanAccumulator(ba::mean(wAccumulator));
    wStdDevAccumulator(std::sqrt(ba::variance(wAccumulator)));
  }

  return ShuffleAlgorithmPerformance(iAlgorithmName, ba::mean(wMeanAccumulator), std::sqrt(ba::variance(wMeanAccumulator)), ba::mean(wStdDevAccumulator), std::sqrt(ba::variance(wStdDevAccumulator)));
}

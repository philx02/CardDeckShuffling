#pragma once

#include <utility>

template< class Iter >
struct iter_pair_range : std::pair< Iter, Iter >
{
  iter_pair_range(std::pair< Iter, Iter > const& x)
    : std::pair< Iter, Iter >(x)
  {
  }
  
  Iter begin() const
  {
    return this->first;
  }

  Iter end() const
  {
    return this->second;
  }
};

template< class Iter >
inline iter_pair_range< Iter > as_range(std::pair< Iter, Iter > const &x)
{
  return iter_pair_range< Iter >(x);
}

template< class Iter >
inline iter_pair_range< Iter > as_range(Iter iBegin, Iter iEnd)
{
  return as_range(std::make_pair(iBegin, iEnd));
}

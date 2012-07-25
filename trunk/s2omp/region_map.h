/*
 * region_map.h
 *
 *  Created on: Jul 24, 2012
 *      Author: cbmorrison
 */

#ifndef REGION_MAP_H_
#define REGION_MAP_H_

typedef std::map<const uint64, int16_t> region_dict;
typedef region_dict::iterator region_iterator;
typedef std::pair<region_iterator, region_iterator> region_pair;
typedef std::map<const int16_t, double> region_area;

namespace s2omp {
class region_map {
  // This class provides the functionality for dividing the area subtended by a
  // BaseMap-derived object into roughly equal-area, equal-sized regions.  The
  // class is not intended to be instantiated outside of the BaseMap class.

public:
  region_map();
  virtual ~region_map();

  uint16_t initialize(uint16_t n_region, pixelized_bound* bound);
  uint16_t
      initialize(uint16_t n_region, uint32_t level, pixelized_bound* bound);

  bool initialize(pixelized_bound& reference_bound, pixelized_bound* bound);

  int16_t find_region(const point& p);

  int16_t find_region(const pixel& pix);

  void clear();

  void region_covering(int16_t region, pixel_vector* pixels);

  // Given a region index, return the area associated with that region.
  double region_area(int16_t region);

  // Some getter methods to describe the state of the RegionMap.
  inline uint16_t n_region() const {
    return n_region_;
  }
  inline uint32_t level() const {
    return level_;
  }
  inline bool is_initialized() const {
    return !region_map_.empty();
  }

  // Return iterators for the set of RegionMap objects.
  inline region_iterator begin() {
    return region_map_.begin();
  }
  inline region_iterator end() {
    return region_map_.end();
  }

private:
  int find_regionation_level(analytic_bound* bound, uint16_t n_region);

  void regionate(analytic_bound* bound, uint16_t n_region, int level);

  bool verify_regionation(uint16_t n_region);

  region_dict region_map_;
  region_area region_area_;
  int level_;
  uint16_t n_region_;
};

} // end namespace s2omp


#endif /* REGION_MAP_H_ */
// Copyright 2010  All Rights Reserved.
// Author: ryan.scranton@gmail.com (Ryan Scranton)

// STOMP is a set of libraries for doing astrostatistical analysis on the
// celestial sphere.  The goal is to enable descriptions of arbitrary regions
// on the sky which may or may not encode futher spatial information (galaxy
// density, CMB temperature, observational depth, etc.) and to do so in such
// a way as to make the analysis of that data as algorithmically efficient as
// possible.
//
// This header file contains a single class for dealing with angular annuli
// on the sky.  There is no heavy coding here (indeed, all of the class code is
// in this header), but the functionality here is necessary for the angular
// correlation operations in angular_correlation.h as well as the pair finding
// in tree_union and scalar_union.

#ifndef ANGULAR_BIN_H_
#define ANGULAR_BIN_H_

#include <vector>
#include <math.h>
#include "core.h"
#include "pixel.h"

namespace s2omp {

class angular_bin {
  // Class object for holding the data associated with a single angular
  // annulus.  Each instance of the class contains a lower and upper angular
  // limit that defines the annulus as well as methods for testing against
  // those limits and data fields that are used for calculating angular
  // auto-correlations and cross-correlations via the AngularCorrelation
  // class described in stomp_angular_correlation.h.

public:
  enum Counter {
    GAL_GAL, GAL_RAND, RAND_GAL, RAND_RAND
  };

  virtual ~angular_bin();

  // The simplest angular_bin object needs only a minimum and maximum angular
  // range (generally denoted by _theta_ in the literature).  Theta is taken to
  // be in degrees.
  inline angular_bin(double theta_deg_min, double theta_deg_max);

  // A common method for calculating the error on angular correlations is to
  // divide the survey area up into equal area regions and use jack-knife
  // methods to estimate the variance on the correlation function.  This
  // constructor sets up the angular_bin object for that sort of operation.
  inline angular_bin(double theta_deg_min, double theta_deg_max, int n_regions);

  // These two methods do the work of clearing and initializing the variables
  // used to store the region-related data.
  inline void clear_regions();
  inline void init_regions(int n_regions);
  inline bool regions_initialized() const;

  // There are two different methods for calculating the angular correlation
  // function, w(theta).  One is based on counting pairs separated by a given
  // angular distance.  The other pixelizes the survey area and sums the product
  // of over-densities for pixels separated by a given angular distance.  To
  // maximize the efficiency of the latter method, the resolution of the pixel
  // map needs to be matched to the angular scale of interest.  By storing this
  // resolution in the angular_bin, we can tell the AngularCorrelation object
  // which maps to use for this bin.  Alternatively, by setting this value to
  // an illegal resolution, we can signal that this angular scale is better
  // suited to the pair-based method.
  inline void set_level(int level) {
    level_ = level;
  }

  // We can also calculate the appropriate resolution to use given the span
  // of our angular limits.  The idea here is to find the largest possible
  // resolution that still resolves this bin's angular scale.
  inline void find_level();

  // Setting the angular minima, maxima and mid-point.
  //
  // Depending on whether we're using linear or logarithmic angular binning,
  // we'll need to set the mid-point of the angular bin by hand.
  inline void set_theta_min(double theta_min);
  inline void set_theta_max(double theta_max);
  inline void set_theta(double theta) {
    theta_ = theta;
  }

  // A set of methods for determining if a given angular scale is within our
  // bounds.  Depending on the application, this scale may be most easily
  // given to us in terms of theta, sin(theta) or cos(theta).
  inline bool is_within_bounds(double theta);
  inline bool is_within_sin2_bounds(double sin2theta);
  inline bool is_within_cos_bounds(double costheta);

  // Since we know the inner and outer angular radii for our bin, we can
  // calculate the area of the bin and, given a density of objects per square
  // degree, the expected Poisson noise.
  inline double area();
  inline double poisson_noise(double objects_per_square_degree,
      double survey_area);

  // For the pixel-based w(theta), we use two internal variables:
  //
  //  * PixelWtheta, which stores the sum of the products of the overdensities
  //  * PixelWeight, which stores the number of such pixel pairs.
  //
  // w(theta) is then the ratio of these two numbers.
  inline void add_to_pixel_wtheta(double dwtheta, double dweight);
  inline void add_to_pixel_wtheta(double dwtheta, double dweight, int region_a,
      int region_b);

  // For the pair-counting, we use the methods in the tree_pixel and tree_union
  // classes.  Those methods are oblivious to the particular data sets they
  // are operating on, so they store the values in weight (for the sum of the
  // products of the object weights) and counter, which stores the raw number
  // of point pairs.
  inline void add_to_weight(double weight);
  inline void add_to_weight(double weight, int region_a, int region_b);
  inline void add_to_counter(long step);
  inline void add_to_counter(long step, int region_a, int region_b);
  inline void add_to_pair_wtheta(double weight, long step);
  inline void add_to_pair_wtheta(double weight, long step, int region_a,
      int region_b);

  // For calculating the pair-based w(theta), we use the Landy-Szalay estimator.
  // In the general case of a cross-correlation between two galaxy data sets,
  // there are four terms:
  //
  //  * galaxy-galaxy: the number of pairs between the two data sets.
  //  * random-random: the number of pairs between a randomized version of each
  //                   data set.
  //  * galaxy-random: the number of pairs between the first data set and a
  //                   randomized version of the second data set.
  //  * random-galaxy: the complement of galaxy-random.
  //
  // In the case of an autocorrelation, the last two terms are identical.  Once
  // we have the values of one of these combinations in the Weight and Counter
  // values, we can shift those values to the appropriate internal variable.
  inline void move_weight(Counter c);

  // If the number of random points is not equal to the number of data points,
  // we will need to rescale the number of pairs accordingly.
  inline void rescale_pair_counts(Counter c, double scale);

  // Methods for reseting some or all of our internal data.
  inline void reset();
  inline void reset_pixel_wtheta();
  inline void reset_weight();
  inline void reset_counter();
  inline void reset_pair_counts(Counter c);

  // Some basic getters for the internal data.
  inline int level() const {
    return level_;
  }
  inline int n_region() const {
    return n_region_;
  }
  inline double theta() const {
    return theta_;
  }
  inline double theta_min() const {
    return theta_min_;
  }
  inline double theta_max() const {
    return theta_max_;
  }
  inline double sin2_theta_min() const {
    return sin2theta_min_;
  }
  inline double sin2_theta_max() const {
    return sin2theta_max_;
  }
  inline double cos_theta_min() const {
    return costheta_min_;
  }
  inline double cos_theta_max() const {
    return costheta_max_;
  }

  // For the angular correlation values, we can access the value for the
  // entire survey (if the method is called without any arguments) or for a
  // given region.
  inline double wtheta() const;
  inline double wtheta(int region) const;
  inline double wtheta_error() const;
  inline double wtheta_error(int region) const;
  inline double weighted_cross_correlation() const;
  inline double weighted_cross_correlation(int region) const;

  // The getters for our data fields.
  inline double pixel_wtheta() const {
    return pixel_wtheta_;
  }
  inline double pixel_wtheta(int region) const;
  inline double pixel_weight() const {
    return pixel_weight_;
  }
  inline double pixel_weight(int region) const;
  inline double pair_weight() const {
    return pair_weight_;
  }
  inline double pair_weight(int region) const;
  inline long pair_counts() const {
    return pair_count_;
  }
  inline long pair_counts(int region) const;
  inline double pair_weight(Counter c) const;
  inline double pair_weight(Counter c, int region) const;

  // For these getters, we calculate the average value over all of the region
  // measurements.
  inline double mean_wtheta() const;
  inline double mean_wtheta_error() const;
  inline double mean_weighted_cross_correlation() const;
  inline double mean_weighted_cross_correlation_error() const;
  inline double mean_weight() const;
  inline double mean_counter() const;
  inline double mean_pair_counts(Counter c) const;

  // Finally, some static methods which the AngularCorrelation method will use
  // to order its vectors of angular_bin objects.
  inline static bool theta_order(const angular_bin& theta_a,
      const angular_bin& theta_b);
  inline static bool sin_theta_order(const angular_bin& theta_a,
      const angular_bin& theta_b);
  inline static bool reverse_level_order(const angular_bin& theta_a,
      const angular_bin& theta_b);

private:
  angular_bin();
  double theta_min_, theta_max_, theta_;
  double costheta_min_, costheta_max_, sin2theta_min_, sin2theta_max_;
  double pair_weight_, gal_gal_, gal_rand_, rand_gal_, rand_rand_;
  double pixel_wtheta_, pixel_weight_, wtheta_, wtheta_error_;
  long pair_count_;
  std::vector<double> pair_weight_region_, gal_gal_region_;
  std::vector<double> gal_rand_region_, rand_gal_region_, rand_rand_region_;
  std::vector<double> pixel_wtheta_region_, pixel_weight_region_;
  std::vector<double> wtheta_region_, wtheta_error_region_;
  std::vector<long> pair_counts_region_;
  int level_;
  int n_region_;
  bool set_wtheta_error_, set_wtheta_;
};

inline angular_bin::angular_bin(double theta_min, double theta_max) {
  set_theta_min(theta_min);
  set_theta_max(theta_max);
  pair_weight_ = gal_gal_ = gal_rand_ = rand_gal_ = rand_rand_ = 0.0;
  pixel_wtheta_ = pixel_weight_ = wtheta_ = wtheta_error_ = 0.0;
  pair_count_ = 0;
  clear_regions();
  level_ = -1;
  set_wtheta_ = false;
  set_wtheta_error_ = false;
}

inline angular_bin::angular_bin(double theta_min, double theta_max,
    int n_regions) {
  set_theta_min(theta_min);
  set_theta_max(theta_max);
  pair_weight_ = 0.0;
  pair_count_ = 0;
  gal_gal_ = gal_rand_ = rand_gal_ = rand_rand_ = 0.0;
  pixel_wtheta_ = pixel_weight_ = 0.0;
  wtheta_ = wtheta_error_ = 0.0;
  clear_regions();
  init_regions(n_regions);
  level_ = -1;
  set_wtheta_ = false;
  set_wtheta_error_ = false;
}

inline angular_bin::~angular_bin() {
  theta_min_ = theta_max_ = sin2theta_min_ = sin2theta_max_ = 0.0;
  pair_weight_ = 0.0;
  pair_count_ = 0;
  gal_gal_ = gal_rand_ = rand_gal_ = rand_rand_ = 0.0;
  pixel_wtheta_ = pixel_weight_ = 0.0;
  wtheta_ = wtheta_error_ = 0.0;
  clear_regions();
  level_ = 0;
  set_wtheta_ = false;
  set_wtheta_error_ = false;
}

inline void angular_bin::clear_regions() {
  pair_weight_region_.clear();
  pair_counts_region_.clear();

  gal_gal_region_.clear();
  gal_rand_region_.clear();
  rand_gal_region_.clear();
  rand_rand_region_.clear();

  pixel_wtheta_region_.clear();
  pixel_weight_region_.clear();

  wtheta_region_.clear();
  wtheta_error_region_.clear();

  n_region_ = 0;
}

inline void angular_bin::init_regions(int n_regions) {
  clear_regions();

  if (n_regions < 0) {
    return;
  }

  n_region_ = n_regions;
  pair_weight_region_.reserve(n_regions);
  pair_counts_region_.reserve(n_regions);

  gal_gal_region_.reserve(n_regions);
  gal_rand_region_.reserve(n_regions);
  rand_gal_region_.reserve(n_regions);
  rand_rand_region_.reserve(n_regions);

  pixel_wtheta_region_.reserve(n_regions);
  pixel_weight_region_.reserve(n_regions);

  wtheta_region_.reserve(n_regions);
  wtheta_error_region_.reserve(n_regions);

  for (uint k = 0; k < n_regions; k++) {
    pair_weight_region_.push_back(0.0);
    pair_counts_region_.push_back(0);

    gal_gal_region_.push_back(0.0);
    gal_rand_region_.push_back(0.0);
    rand_gal_region_.push_back(0.0);
    rand_rand_region_.push_back(0.0);

    pixel_wtheta_region_.push_back(0.0);
    pixel_weight_region_.push_back(0.0);

    wtheta_region_.push_back(0.0);
    wtheta_error_region_.push_back(0.0);
  }
}

bool angular_bin::regions_initialized() const {
  if (pair_weight_region_.size() != n_region_) {
    std::cout << "pair weight uninitialized\n";
    exit(1);
  }
  if (pair_counts_region_.size() != n_region_) {
    std::cout << "pair count uninitialized\n";
    exit(1);
  }
  if (gal_gal_region_.size() != n_region_) {
    std::cout << "gal_gal uninitialized\n";
    exit(1);
  }
  if (gal_rand_region_.size() != n_region_) {
    std::cout << "gal_rand uninitialized\n";
    exit(1);
  }
  if (rand_gal_region_.size() != n_region_) {
    std::cout << "rand_gal uninitialized\n";
    exit(1);
  }
  if (rand_rand_region_.size() != n_region_) {
    std::cout << "rand_rand uninitialized\n";
    exit(1);
  }
  if (pixel_weight_region_.size() != n_region_) {
    std::cout << "pixel_weight uninitialized\n";
    exit(1);
  }
  if (pixel_wtheta_region_.size() != n_region_) {
    std::cout << "pixel_wtheta uninitialized\n";
    exit(1);
  }

  return true;
}


inline void angular_bin::find_level() {
  level_ = -1;
  for (int level = 0; level < MAX_LEVEL; level++) {
    // Regardless of the projection, the ratio of the largest pixel area to
    // the smallest for a given level should be < 2, so we choose that as a
    // worst case scenario for determining the scale we need to resolve.
    double scale = sqrt(2.0 * pixel::average_area(level));
    if (is_within_bounds(scale) || scale < theta_min_) {
      level_ = level;
      break;
    }
  }
}

inline void angular_bin::set_theta_min(double theta_min) {
  theta_min_ = theta_min;
  sin2theta_min_ = sin(theta_min_ * DEG_TO_RAD) * sin(theta_min_ * DEG_TO_RAD);
  costheta_max_ = cos(theta_min_ * DEG_TO_RAD);
}

inline void angular_bin::set_theta_max(double theta_max) {
  theta_max_ = theta_max;
  sin2theta_max_ = sin(theta_max_ * DEG_TO_RAD) * sin(theta_max_ * DEG_TO_RAD);
  costheta_min_ = cos(theta_max_ * DEG_TO_RAD);
}

inline bool angular_bin::is_within_bounds(double theta) {
  return (double_ge(theta, theta_min_) && double_le(theta, theta_max_));
}

inline bool angular_bin::is_within_sin2_bounds(double sin2theta) {
  return (double_ge(sin2theta, sin2theta_min_) && double_le(sin2theta,
      sin2theta_max_));
}

inline bool angular_bin::is_within_cos_bounds(double costheta) {
  return (double_ge(costheta, costheta_min_) && double_le(costheta,
      costheta_max_));
}

inline double angular_bin::area() {
  return (costheta_max_ - costheta_min_) * 2.0 * PI * STRAD_TO_DEG2;
}

inline double angular_bin::poisson_noise(double objects_per_square_degree,
    double survey_area) {
  return 1.0 / sqrt(objects_per_square_degree * objects_per_square_degree
      * survey_area * area());
}

inline void angular_bin::add_to_pixel_wtheta(double dwtheta, double dweight) {
  pixel_wtheta_ += dwtheta;
  pixel_weight_ += dweight;
}

inline void angular_bin::add_to_pixel_wtheta(double dwtheta, double dweight,
    int region_a, int region_b) {
  pixel_wtheta_ += dwtheta;
  pixel_weight_ += dweight;

  if ((region_a != -1) && (region_b != -1)) {
    for (int k = 0; k < n_region_; k++) {
      if ((k != region_a) && (k != region_b)) {
        pixel_wtheta_region_[k] += dwtheta;
        pixel_weight_region_[k] += dweight;
      }
    }
  }
}

inline void angular_bin::add_to_weight(double weight) {
  pair_weight_ += weight;
}

inline void angular_bin::add_to_weight(double weight, int region_a,
    int region_b) {
  pair_weight_ += weight;

  if (region_a != -1 && region_b != -1) {
    for (int k = 0; k < n_region_; k++) {
      if (k != region_a && k != region_b)
        pair_weight_region_[k] += weight;
    }
  }
}

inline void angular_bin::add_to_counter(long step) {
  pair_count_ += step;
}

inline void angular_bin::add_to_counter(long step, int region_a, int region_b) {
  pair_count_ += step;

  if (region_a != -1 && region_b != -1) {
    for (int k = 0; k < n_region_; k++) {
      if (k != region_a && k != region_b)
        pair_counts_region_[k] += step;
    }
  }
}

inline void angular_bin::add_to_pair_wtheta(double weight, long step) {
  pair_weight_ += weight;
  pair_count_ += step;
}

inline void angular_bin::add_to_pair_wtheta(double weight, long step,
    int region_a, int region_b) {
  pair_weight_ += weight;
  pair_count_ += step;

  if (region_a != -1 && region_b != -1) {
    for (int k = 0; k < n_region_; k++) {
      if (k != region_a && k != region_b) {
        pair_weight_region_[k] += weight;
        pair_counts_region_[k] += step;
      }
    }
  }
}

inline void angular_bin::move_weight(Counter c) {
  switch (c) {
  case GAL_GAL:
    gal_gal_ += pair_weight_;
    break;
  case GAL_RAND:
    gal_rand_ += pair_weight_;
    break;
  case RAND_GAL:
    rand_gal_ += pair_weight_;
    break;
  case RAND_RAND:
    rand_rand_ += pair_weight_;
    break;
  }
  pair_weight_ = 0.0;

  for (int k = 0; k < n_region_; k++) {
    switch (c) {
    case GAL_GAL:
      gal_gal_region_[k] += pair_weight_region_[k];
      break;
    case GAL_RAND:
      gal_rand_region_[k] += pair_weight_region_[k];
      break;
    case RAND_GAL:
      rand_gal_region_[k] += pair_weight_region_[k];
      break;
    case RAND_RAND:
      rand_rand_region_[k] += pair_weight_region_[k];
      break;
    }
    pair_weight_region_[k] = 0.0;
  }
}

inline void angular_bin::rescale_pair_counts(Counter c, double scale) {
  switch (c) {
  case GAL_GAL:
    gal_gal_ /= scale;
    for (int k = 0; k < n_region_; k++)
      gal_gal_region_[k] /= scale;
    break;
  case GAL_RAND:
    gal_rand_ /= scale;
    for (int k = 0; k < n_region_; k++)
      gal_rand_region_[k] /= scale;
    break;
  case RAND_GAL:
    rand_gal_ /= scale;
    for (int k = 0; k < n_region_; k++)
      rand_gal_region_[k] /= scale;
    break;
  case RAND_RAND:
    rand_rand_ /= scale;
    for (int k = 0; k < n_region_; k++)
      rand_rand_region_[k] /= scale;
    break;
  }
}

inline void angular_bin::reset() {
  pair_weight_ = 0.0;
  pair_count_ = 0;
  gal_gal_ = gal_rand_ = rand_gal_ = rand_rand_ = 0.0;
  pixel_wtheta_ = pixel_weight_ = 0.0;
  wtheta_ = wtheta_error_ = 0.0;
  if (n_region_ > 0) {
    for (int k = 0; k < n_region_; k++) {
      pair_weight_region_[k] = 0.0;
      pair_counts_region_[k] = 0;

      gal_gal_region_[k] = 0.0;
      gal_rand_region_[k] = 0.0;
      rand_gal_region_[k] = 0.0;
      rand_rand_region_[k] = 0.0;

      pixel_wtheta_region_[k] = 0.0;
      pixel_weight_region_[k] = 0.0;

      wtheta_region_[k] = 0.0;
      wtheta_error_region_[k] = 0.0;
    }
  }
}

inline void angular_bin::reset_pixel_wtheta() {
  pixel_wtheta_ = 0.0;
  pixel_weight_ = 0.0;
  if (n_region_ > 0) {
    for (int k = 0; k < n_region_; k++) {
      pixel_wtheta_region_[k] = 0.0;
      pixel_weight_region_[k] = 0.0;
    }
  }
}

inline void angular_bin::reset_weight() {
  pair_weight_ = 0.0;
  if (n_region_ > 0)
    for (int k = 0; k < n_region_; k++)
      pair_weight_region_[k] = 0.0;
}

inline void angular_bin::reset_counter() {
  pair_count_ = 0;
  if (n_region_ > 0)
    for (int k = 0; k < n_region_; k++)
      pair_counts_region_[k] = 0;
}

inline void angular_bin::reset_pair_counts(Counter c) {
  switch (c) {
  case GAL_GAL:
    gal_gal_ = 0.0;
    if (n_region_ > 0)
      for (int k = 0; k < n_region_; k++)
        gal_gal_region_[k] = 0.0;
    break;
  case GAL_RAND:
    gal_rand_ = 0.0;
    if (n_region_ > 0)
      for (int k = 0; k < n_region_; k++)
        gal_rand_region_[k] = 0.0;
    break;
  case RAND_GAL:
    rand_gal_ = 0.0;
    if (n_region_ > 0)
      for (int k = 0; k < n_region_; k++)
        rand_gal_region_[k] = 0.0;
    break;
  case RAND_RAND:
    rand_rand_ = 0.0;
    if (n_region_ > 0)
      for (int k = 0; k < n_region_; k++)
        rand_rand_region_[k] = 0.0;
    break;
  }
}

inline double angular_bin::wtheta() const {
  if (set_wtheta_) {
    return wtheta_;
  } else {
    return level_ == -1 ? (gal_gal_ - gal_rand_ - rand_gal_ + rand_rand_)
        / rand_rand_ : pixel_wtheta_ / pixel_weight_;
  }
}

inline double angular_bin::wtheta(int region) const {
  if (set_wtheta_) {
    return (region == -1 ? wtheta_ : (region < n_region_
        ? wtheta_region_[region] : -1.0));
  } else {
    if (level_ == -1) {
      return (region == -1 ? (gal_gal_ - gal_rand_ - rand_gal_ + rand_rand_)
          / rand_rand_ : (region < n_region_ ? (gal_gal_region_[region]
          - gal_rand_region_[region] - rand_gal_region_[region]
          + rand_rand_region_[region]) / rand_rand_region_[region] : -1.0));
    } else {
      return (region == -1 ? pixel_wtheta_ / pixel_weight_ : (region
          < n_region_ ? pixel_wtheta_region_[region]
          / pixel_weight_region_[region] : -1.0));
    }
  }
}

inline double angular_bin::wtheta_error() const {
  if (set_wtheta_error_) {
    return wtheta_error_;
  } else {
    return level_ == -1 ? 1.0 / sqrt(gal_gal_) : 1.0 / sqrt(pixel_weight_);
  }
}

inline double angular_bin::wtheta_error(int region) const {
  if (set_wtheta_error_) {
    return (region == -1 ? wtheta_error_ : (region < n_region_
        ? wtheta_error_region_[region] : -1.0));
  } else {
    if (level_ == 0) {
      return (region == -1 ? 1.0 / sqrt(gal_gal_) : (region < n_region_ ? 1.0
          / sqrt(gal_gal_region_[region]) : -1.0));
    } else {
      return (region == -1 ? 1.0 / sqrt(pixel_weight_) : (region < n_region_
          ? 1.0 / sqrt(pixel_weight_region_[region]) : -1.0));
    }
  }
}

inline double angular_bin::weighted_cross_correlation() const {
  return pair_weight_ / pair_count_;
}

inline double angular_bin::weighted_cross_correlation(int region) const {
  return (region == -1 ? pair_weight_ / pair_count_ : (region < n_region_
      ? pair_weight_region_[region] / pair_counts_region_[region] : -1.0));
}

inline double angular_bin::pixel_wtheta(int region) const {
  return (region == -1 ? pixel_wtheta_ : (region < n_region_
      ? pixel_wtheta_region_[region] : -1.0));
}

inline double angular_bin::pixel_weight(int region) const {
  return (region == -1 ? pixel_weight_ : (region < n_region_
      ? pixel_weight_region_[region] : -1.0));
}

inline double angular_bin::pair_weight(int region) const {
  return (region == -1 ? pair_weight_ : (region < n_region_
      ? pair_weight_region_[region] : -1.0));
}

inline long angular_bin::pair_counts(int region) const {
  return (region == -1 ? pair_count_ : (region < n_region_
      ? pair_counts_region_[region] : -1));
}

inline double angular_bin::pair_weight(Counter c) const {
  switch (c) {
  case GAL_GAL:
    return gal_gal_;
  case GAL_RAND:
    return gal_rand_;
  case RAND_GAL:
    return rand_gal_;
  case RAND_RAND:
    return rand_rand_;
  }
}

inline double angular_bin::pair_weight(Counter c, int region) const {
  switch (c) {
  case GAL_GAL:
    return (region == -1 ? gal_gal_ : (region < n_region_
        ? gal_gal_region_[region] : -1.0));
  case GAL_RAND:
    return (region == -1 ? gal_rand_ : (region < n_region_
        ? gal_rand_region_[region] : -1.0));
  case RAND_GAL:
    return (region == -1 ? rand_gal_ : (region < n_region_
        ? rand_gal_region_[region] : -1.0));
  case RAND_RAND:
    return (region == -1 ? rand_rand_ : (region < n_region_
        ? rand_rand_region_[region] : -1.0));
  }
}

inline double angular_bin::mean_wtheta() const {
  double mean_wtheta = 0.0;
  for (int k = 0; k < n_region_; k++)
    mean_wtheta += wtheta(k) / (1.0 * n_region_);
  return mean_wtheta;
}

inline double angular_bin::mean_wtheta_error() const {
  double avg_wtheta = mean_wtheta();
  double mean_wtheta_error = 0.0;
  for (int k = 0; k < n_region_; k++)
    mean_wtheta_error += (avg_wtheta - wtheta(k)) * (avg_wtheta - wtheta(k));
  return (n_region_ == 0 ? 0.0 : (n_region_ - 1.0) * sqrt(mean_wtheta_error)
      / n_region_);
}

inline double angular_bin::mean_weighted_cross_correlation() const {
  double mean_weight_cross_correlation = 0.0;
  for (int k = 0; k < n_region_; k++)
    mean_weight_cross_correlation += 1.0 * pair_weight_region_[k]
        / pair_counts_region_[k] / (1.0 * n_region_);
  return mean_weight_cross_correlation;
}

inline double angular_bin::mean_weighted_cross_correlation_error() const {
  double avg_weighted_cross_correlation = mean_weighted_cross_correlation();
  double mean_weighted_cross_correlation_error = 0.0;
  for (int k = 0; k < n_region_; k++)
    mean_weighted_cross_correlation_error += (avg_weighted_cross_correlation
        - weighted_cross_correlation(k)) * (avg_weighted_cross_correlation
        - weighted_cross_correlation(k));
  return (n_region_ == 0 ? 0.0 : (n_region_ - 1.0) * sqrt(
      mean_weighted_cross_correlation_error) / n_region_);
}

inline double angular_bin::mean_weight() const {
  double mean_weight = 0.0;
  for (int k = 0; k < n_region_; k++)
    mean_weight += pair_weight_region_[k] / (1.0 * n_region_);
  return mean_weight;
}

inline double angular_bin::mean_counter() const {
  double mean_counter = 0.0;
  for (int k = 0; k < n_region_; k++)
    mean_counter += 1.0 * pair_counts_region_[k] / (1.0 * n_region_);
  return mean_counter;
}

inline double angular_bin::mean_pair_counts(Counter c) const {
  double mean_pair_counts = 0.0;
  switch (c) {
  case GAL_GAL:
    for (int k = 0; k < n_region_; k++)
      mean_pair_counts += gal_gal_region_[k] / (1.0 * n_region_);
    break;
  case GAL_RAND:
    for (int k = 0; k < n_region_; k++)
      mean_pair_counts += gal_rand_region_[k] / (1.0 * n_region_);
    break;
  case RAND_GAL:
    for (int k = 0; k < n_region_; k++)
      mean_pair_counts += rand_gal_region_[k] / (1.0 * n_region_);
    break;
  case RAND_RAND:
    for (int k = 0; k < n_region_; k++)
      mean_pair_counts += rand_rand_region_[k] / (1.0 * n_region_);
    break;
  }
  return mean_pair_counts;
}

inline bool angular_bin::theta_order(const angular_bin& theta_a,
    const angular_bin& theta_b) {
  return (theta_a.theta_min() < theta_b.theta_min() ? true : false);
}

inline bool angular_bin::sin_theta_order(const angular_bin& theta_a,
    const angular_bin& theta_b) {
  return (theta_a.sin2_theta_min() < theta_b.sin2_theta_min() ? true : false);
}

inline bool angular_bin::reverse_level_order(const angular_bin& theta_a,
    const angular_bin& theta_b) {
  return (theta_b.level() < theta_a.level() ? true : false);
}

} // end namespace s2omp

#endif /* ANGULAR_BIN_H_ */

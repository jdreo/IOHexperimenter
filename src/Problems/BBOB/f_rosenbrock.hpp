/// \file f_rosenbrock.hpp
/// \brief cpp file for class Rosenbrock.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_rosenbrock.c"
///
/// \author Furong Ye
/// \date 2019-09-12
#ifndef _F_ROSENBROCK_HPP
#define _F_ROSENBROCK_HPP

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation_vars.hpp"
#include "bbob_common_used_functions/coco_transformation_objs.hpp"

class Rosenbrock : public IOHprofiler_problem<double> {
public:
  Rosenbrock() {
    IOHprofiler_set_problem_name("Rosenbrock");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
  }
  Rosenbrock(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Rosenbrock");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    Initilize_problem(dimension);
  }
  ~Rosenbrock() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  std::vector<double> xopt;
  std::vector<double> minus_one;
  double fopt, factor;
  void prepare_problem() {
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (8 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(8, this->IOHprofiler_get_instance_id());

    for (int i = 0; i < n; ++i) {
      minus_one.push_back(-1.0);
      xopt[i] *= 0.75;
    }

    factor = 1.0 > (sqrt((double) n) / 8.0) ? 1 : (sqrt((double) n) / 8.0);
  }

  double internal_evaluate(const std::vector<double> &x) {
    std::vector<double> temp_x = x;
    int n = temp_x.size();
    size_t i = 0;
    std::vector<double> result(1);
    double s1 = 0.0, s2 = 0.0, tmp;

    transform_vars_shift_evaluate_function(temp_x,minus_one);
    transform_vars_scale_evaluate(temp_x,factor);
    transform_vars_shift_evaluate_function(temp_x,xopt);

    for (i = 0; i < n - 1; ++i) {
      tmp = (temp_x[i] * temp_x[i] - temp_x[i + 1]);
      s1 += tmp * tmp;
      tmp = (temp_x[i] - 1.0);
      s2 += tmp * tmp;
    }
    result[0] = 100.0 * s1 + s2;

    transform_obj_shift_evaluate_function(result,fopt);

    return result[0];
  };
  
  static Rosenbrock * createInstance() {
    return new Rosenbrock();
  };

  static Rosenbrock * createInstance(int instance_id, int dimension) {
    return new Rosenbrock(instance_id, dimension);
  };
};

#endif
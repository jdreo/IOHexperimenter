/// \file f_schaffers10.hpp
/// \brief cpp file for class Schaffers10.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_schaffers.c"
///
/// \author Furong Ye
/// \date 2019-09-12
#ifndef _F_SCHAFFERSTEN_HPP
#define _F_SCHAFFERSTEN_HPP

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation_vars.hpp"
#include "bbob_common_used_functions/coco_transformation_objs.hpp"

class Schaffers10 : public IOHprofiler_problem<double> {
public:
  Schaffers10() {
    IOHprofiler_set_problem_name("Schaffers10");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
  }
  Schaffers10(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Schaffers10");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    Initilize_problem(dimension);
  }
  ~Schaffers10() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  std::vector<double> xopt;
  double fopt;
  std::vector<std::vector<double>> M;
  std::vector<std::vector<double>> M1;
  std::vector<double> b;
  std::vector<double> b1;
  const double penalty_factor = 10.0;
  const double conditioning = 10;
  void prepare_problem() {
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (17 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(17, this->IOHprofiler_get_instance_id());
    
    /* compute M and b */
    M = std::vector<std::vector<double>> (n);
    M1 = std::vector<std::vector<double>> (n);
    for (int i = 0; i != n; i++) {
      M[i] = std::vector<double> (n);
      M1[i] = std::vector<double> (n);
    }
    b = std::vector<double> (n);
    b1 = std::vector<double> (n);
    std::vector<std::vector<double>> rot1;
    std::vector<std::vector<double>> rot2;
    bbob2009_compute_rotation(rot1, rseed + 1000000, n);
    bbob2009_compute_rotation(rot2, rseed, n);
    for (int i = 0; i < n; ++i) {
      b[i] = 0.0;
      for (int j = 0; j < n; ++j) {
        double exponent = 1.0 * (int) i / ((double) (long) n - 1.0);
        M[i][j] = rot2[i][j] * pow(sqrt(conditioning), exponent);
      }
    }
    bbob2009_copy_rotation_matrix(rot1,M1,b1,n);
  }

  double internal_evaluate(const std::vector<double> &x) {
    std::vector<double> temp_x = x;
    int n = temp_x.size();
    size_t i = 0;
    std::vector<double> result(1);

    transform_vars_affine_evaluate_function(temp_x,M,b);
    transform_vars_asymmetric_evaluate_function(temp_x,0.5);
    transform_vars_affine_evaluate_function(temp_x,M1,b1);
    transform_vars_shift_evaluate_function(temp_x,xopt);

    /* Computation core */
    result[0] = 0.0;
    for (i = 0; i < n - 1; ++i) {
      const double tmp = temp_x[i] * temp_x[i] + temp_x[i + 1] * temp_x[i + 1];
      if (isinf(tmp) && isnan(sin(50.0 * pow(tmp, 0.1))))  /* sin(inf) -> nan */
        /* the second condition is necessary to pass the integration tests under Windows and Linux */
        return tmp;
      result[0] += pow(tmp, 0.25) * (1.0 + pow(sin(50.0 * pow(tmp, 0.1)), 2.0));
    }
    result[0] = pow(result[0] / ((double) (long) n - 1.0), 2.0);

    transform_obj_shift_evaluate_function(result,fopt);
    transform_obj_penalize_evaluate(temp_x,this->IOHprofiler_get_lowerbound(),this->IOHprofiler_get_upperbound(),penalty_factor,result);
    return result[0];
  };
  
  static Schaffers10 * createInstance() {
    return new Schaffers10();
  };

  static Schaffers10 * createInstance(int instance_id, int dimension) {
    return new Schaffers10(instance_id, dimension);
  };
};

#endif
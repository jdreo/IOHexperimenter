#pragma once
#include "bbob_base.hpp"

namespace ioh {
    namespace problem {
        namespace bbob {
            class Gallagher101 : public bbob_base {
                typedef struct f_gallagher_permutation_t1 {
                    double value;
                    size_t index;
                } F_GALLAGHER_PERMUTATION_T1;


                static int f_gallagher_compare_doubles1(
                    const void *a, const void *b_) {
                    const auto temp =
                        (*(f_gallagher_permutation_t1 *)a).value - (*(
                            f_gallagher_permutation_t1 *)b_).value;
                    if (temp > 0)
                        return 1;
                    if (temp < 0)
                        return -1;
                    return 0;
                }

                const size_t number_of_peaks = 101;
                std::vector<std::vector<double>> rotation;
                std::vector<std::vector<double>> arr_scales;
                std::vector<std::vector<double>> x_local;
                std::vector<double> peak_values;


            public:
                Gallagher101(int instance_id = IOH_DEFAULT_INSTANCE,
                             int dimension = IOH_DEFAULT_DIMENSION)
                    : bbob_base(21, "Gallagher101", instance_id, dimension) {
                    set_number_of_variables(dimension);
                }


                void prepare_problem() override {
                    size_t i, j;
                    const auto maxcondition = 1000.;
                    /* maxcondition1 satisfies the old code and the doc but seems wrong in that it is, with very high
                    * probability, not the largest condition level!!! */
                    auto maxcondition1 = 1000.;
                    double fitvalues[2] = {1.1, 9.1};

                    std::vector<double> random_numbers;
                    arr_scales = std::vector<std::vector<double>>(
                        number_of_peaks);
                    for (i = 0; i != number_of_peaks; ++i) {
                        arr_scales[i] = std::vector<double>(n_);
                    }

                    x_local = std::vector<std::vector<double>>(n_);
                    for (i = 0; i != n_; ++i) {
                        x_local[i] = std::vector<double>(number_of_peaks);
                    }

                    /* Allocate temporary storage and space for the rotation matrices */

                    maxcondition1 = sqrt(maxcondition1);
                    const auto b_ = 10.;
                    const auto c = 5.;

                    transformation::coco::bbob2009_compute_rotation(
                        rotation, rseed_, n_);

                    /* Initialize all the data of the inner problem */
                    transformation::coco::bbob2009_unif(
                        random_numbers, number_of_peaks - 1,
                        rseed_);
                    //rperm = std::vector<f_gallagher_permutation_t1> (number_of_peaks - 1);
                    auto rperm = static_cast<f_gallagher_permutation_t1 *>(
                        malloc(
                            (number_of_peaks - 1) * sizeof(
                                f_gallagher_permutation_t1)));

                    for (i = 0; i < number_of_peaks - 1; ++i) {
                        rperm[i].value = random_numbers[i];
                        rperm[i].index = i;
                    }
                    //std::sort(rperm.begin(), rperm.end(), compareRperm);
                    qsort(rperm, number_of_peaks - 1,
                          sizeof(f_gallagher_permutation_t1),
                          f_gallagher_compare_doubles1);

                    /* Random permutation */
                    auto arrCondition = std::vector<double>(number_of_peaks);
                    arrCondition[0] = maxcondition1;
                    peak_values = std::vector<double>(number_of_peaks);
                    peak_values[0] = 10;
                    for (i = 1; i < number_of_peaks; ++i) {
                        arrCondition[i] = pow(maxcondition,
                                              static_cast<double>(rperm[i - 1].
                                                  index) /
                                              static_cast<double>(
                                                  number_of_peaks - 2));
                        peak_values[i] =
                            static_cast<double>(i - 1) / static_cast<double>(
                                number_of_peaks - 2) * (
                                fitvalues[1] - fitvalues[0])
                            + fitvalues[0];
                    }

                    free(rperm);
                    rperm = static_cast<f_gallagher_permutation_t1 *>(malloc(
                        n_ * sizeof(f_gallagher_permutation_t1)));

                    //rperm = std::vector<f_gallagher_permutation_t1> (n_);
                    for (i = 0; i < number_of_peaks; ++i) {
                        transformation::coco::bbob2009_unif(random_numbers, n_,
                            rseed_ + static_cast<long>(1000 * i));
                        for (j = 0; j < n_; ++j) {
                            rperm[j].value = random_numbers[j];
                            rperm[j].index = j;
                        }
                        //std::sort(rperm.begin(), rperm.end(), compareRperm);
                        qsort(rperm, n_, sizeof(f_gallagher_permutation_t1),
                              f_gallagher_compare_doubles1);
                        for (j = 0; j < n_; ++j) {
                            arr_scales[i][j] = pow(arrCondition[i],
                                                   /* Lambda^alpha_i from the doc */
                                                   static_cast<double>(rperm[j].
                                                       index) / static_cast
                                                   <double>(n_ - 1) -
                                                   0.5);
                        }
                    }
                    free(rperm);
                    transformation::coco::bbob2009_unif(
                        random_numbers, n_ * number_of_peaks,
                        rseed_);
                    std::vector<double> best_variables(n_);
                    for (i = 0; i < n_; ++i) {
                        xopt_[i] = 0.8 * (b_ * random_numbers[i] - c);
                        best_variables[i] = 0.8 * (b_ * random_numbers[i] - c);
                        for (j = 0; j < number_of_peaks; ++j) {
                            x_local[i][j] = 0.;
                            for (size_t k = 0; k < n_; ++k) {
                                x_local[i][j] += rotation[i][k] * (
                                    b_ * random_numbers[j * n_ + k] - c
                                );
                            }
                            if (j == 0) {
                                x_local[i][j] *= 0.8;
                            }
                        }
                    }
                    set_best_variables(best_variables);
                }


                double internal_evaluate(
                    const std::vector<double> &x) override {
                    size_t i, j; /* Loop over dim */
                    std::vector<double> tmx(n_);
                    const auto a = 0.1;
                    double f = 0., tmp, f_pen = 0., f_true = 0.;

                    const auto fac = -0.5 / static_cast<double>(n_);

                    /* Boundary handling */
                    for (i = 0; i < n_; ++i) {
                        tmp = fabs(x[i]) - 5.;
                        if (tmp > 0.) {
                            f_pen += tmp * tmp;
                        }
                    }
                    const auto f_add = f_pen;
                    /* Transformation in search space */
                    /* TODO: this should rather be done in f_gallagher */

                    for (i = 0; i < n_; i++) {
                        tmx[i] = 0;
                        for (j = 0; j < n_; ++j) {
                            tmx[i] += rotation[i][j] * x[j];
                        }
                    }
                    /* Computation core*/
                    for (i = 0; i < number_of_peaks; ++i) {
                        auto tmp2 = 0.;
                        for (j = 0; j < n_; ++j) {
                            tmp = tmx[j] - x_local[j][i];
                            tmp2 += arr_scales[i][j] * tmp * tmp;
                        }
                        tmp2 = peak_values[i] * exp(fac * tmp2);
                        f = f > tmp2 ? f : tmp2;
                    }

                    f = 10. - f;
                    if (f > 0) {
                        f_true = log(f) / a;
                        f_true = pow(
                            exp(f_true + 0.49 * (sin(f_true) + sin(
                                                     0.79 * f_true))), a);
                    } else if (f < 0) {
                        f_true = log(-f) / a;
                        f_true = -pow(
                            exp(f_true + 0.49 * (sin(0.55 * f_true) + sin(
                                                     0.31 * f_true))), a);
                    } else {
                        f_true = f;
                    }

                    f_true *= f_true;
                    f_true += f_add;

                    return f_true;
                }

                static Gallagher101 *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Gallagher101(instance_id, dimension);
                }
            };
        }
    }
}

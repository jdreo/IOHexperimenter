#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{
    class Katsuura final : public BBOB<Katsuura>

    {
        std::vector<double> raw_x_;
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            static const auto double_n = static_cast<double>(meta_data_.n_variables);
            static const auto exponent = 10. / pow(double_n, 1.2);
            static const auto factor = 10. / double_n / double_n;

            std::vector<double> result = { 1.0 };
            for (auto i = 0; i < meta_data_.n_variables; ++i)
            {
                double z = 0;
                for (size_t j = 1; j < 33; ++j)
                    z += fabs(transformation_state_.exponents.at(j) * x.at(i)
                            - floor(transformation_state_.exponents.at(j) * x.at(i) + 0.5))
                        / transformation_state_.exponents.at(j);

                result[0] *= pow(1.0 + (static_cast<double>(i) + 1) * z, exponent);
            }
            result[0] = factor * (-1. + result.at(0));
            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            raw_x_ = x;
            transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
            transform_vars_affine_evaluate_function(
                x, transformation_state_.second_transformation_matrix,
                transformation_state_.second_transformation_base);
            return x;
        }

        std::vector<double> transform_objectives(std::vector<double> y) override
        {
            using namespace transformation::coco;
            static const auto penalty_factor = 1.0;
            transform_obj_shift_evaluate_function(y, meta_data_.objective.y.at(0));
            transform_obj_penalize_evaluate(raw_x_, constraint_.lb.at(0),
                                            constraint_.ub.at(0), penalty_factor, y);
            return y;
        }

    public:
        Katsuura(const int instance, const int n_variables) :
            BBOB(23, instance, n_variables, "Katsuura", sqrt(100.0)),
            raw_x_(n_variables)
        {
            transformation_state_.exponents.resize(33);
            for (auto i = 1; i < 33; ++i)
                transformation_state_.exponents[i] = pow(2., static_cast<double>(i));
        }
    };
}

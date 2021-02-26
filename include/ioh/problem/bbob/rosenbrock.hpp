#pragma once

#include "ioh/problem/bbob/bbob_base.hpp"

namespace ioh::problem::bbob
{
    template<typename T>
    class RosenbrockBase : public BBOB<T>
    {
    protected:
        std::vector<double> evaluate(std::vector<double> &x) override
        {
            auto sum1 = 0.0, sum2 = 0.0;

            for (size_t i = 0; i < x.size() - 1; ++i) {
                sum1 += pow(x.at(i) * x.at(i) - x.at(i + 1), 2.0);
                sum2 += pow(x.at(i) - 1.0, 2.0);
            }
            return { 100.0 * sum1 + sum2 };
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            static const auto factor = std::max(1.0, std::sqrt(meta_data_.n_variables) / 8.0);
            static const auto negative_one = std::vector<double>(meta_data_.n_variables, -1);
            transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
            transform_vars_scale_evaluate(x, factor);
            transform_vars_shift_evaluate_function(x, negative_one);
            return x;
        }

    public:
        RosenbrockBase(const int problem_id, const int instance, const int n_variables, const std::string& name) :
            BBOB(problem_id, instance, n_variables, name)
        {
        }
    };

    class Rosenbrock final: public RosenbrockBase<Rosenbrock>
    {
    public:
        Rosenbrock(const int instance, const int n_variables) :
            RosenbrockBase(8, instance, n_variables, "Rosenbrock")
        {
            for (auto& e : meta_data_.objective.x)
                e *= 0.75;
        }
    };

  
}

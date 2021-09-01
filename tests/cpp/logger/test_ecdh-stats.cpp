#include "../utils.hpp"

#include "ioh/logger/ecdh.hpp"
#include "ioh/suite.hpp"

TEST_F(BaseTest, ecdh_stats)
{
    using namespace ioh::logger;

    size_t runs = 10;
    size_t sample_size = 100;
    size_t buckets = 10;

    size_t pb_start = 2;
    size_t pb_end = 10;
    ioh::suite::BBOB suite({ 1, 2 }, { 1, 2 }, { 2, 10 });
    ECDH ecdh(0, 6e7, buckets, 0, sample_size, buckets);
    suite.attach_logger(ecdh);

    for (const auto& pb : suite) {
        for (size_t run = 0; run < runs; ++run) {
            // FIXME how to indicate different runs to the logger?
            // ecdh.update_run_info(pb->meta_data());
            for (size_t s = 0; s < sample_size; ++s) {
                (*pb)(ioh::common::random::pbo::uniform(pb->meta_data().n_variables, 0));
            } // s
            pb->reset();
        } // run
    } // pb

    EXPECT_GT(ecdh::stat::sum(ecdh), 0);

    // Histogram
    ecdh::stat::Histogram histo;
    ecdh::stat::Histogram::Mat m = histo(ecdh);
    EXPECT_EQ(histo.nb_attainments(), runs * (pb_end - pb_start));
    
    // buckets * buckets matrix
    EXPECT_EQ(m.size(), buckets);
    for (const auto& row : m) {
        EXPECT_EQ(row.size(), buckets);
    }

    EXPECT_EQ(ecdh::stat::histogram(ecdh), m);
    
    // Distribution
    ecdh::stat::Distribution::Mat d = ecdh::stat::distribution(ecdh);
    
    // buckets * buckets matrix
    EXPECT_EQ(d.size(), buckets);
    for (const auto& row : d) {
        EXPECT_EQ(row.size(), buckets);
    }

    // Volume under curve
    EXPECT_GE(ecdh::stat::under_curve::volume(ecdh), 0);
    EXPECT_LE(ecdh::stat::under_curve::volume(ecdh), 1);
}

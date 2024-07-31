#include <gtest/gtest.h>

#include "gsdecon/gsdecon.hpp"
#include "scran_tests/scran_tests.hpp"

class GsdeconSimpleTest : public ::testing::TestWithParam<std::tuple<int, bool> > {
protected:
    static inline std::shared_ptr<tatami::NumericMatrix> matrix;

    static void SetUpTestSuite() {
        auto vec = scran_tests::simulate_vector(20000, scran_tests::SimulationParameters());
        matrix.reset(new tatami::DenseRowMatrix<double, int>(50, 400, std::move(vec)));
    }

    struct ReferenceResults {
        std::vector<double> weights, scores;
    };

    static ReferenceResults reference(const tatami::NumericMatrix& mat, int rank, bool scale) {
        scran_pca::SimplePcaOptions sopt;
        sopt.number = rank;
        sopt.scale = scale;
        auto res = scran_pca::simple_pca(mat, sopt);

        ReferenceResults output;
        Eigen::VectorXd weighted = (res.rotation.array() * res.rotation.array()).rowwise().sum();
        size_t NR = mat.nrow();
        output.weights.resize(NR);
        for (size_t r = 0; r < NR; ++r) {
            output.weights[r] = std::sqrt(weighted[r] / rank);
        }

        // Explicitly computing the low-rank approximation.
        Eigen::MatrixXd lowrank = res.rotation * res.components;
        if (scale) {
            lowrank.array().colwise() *= res.scale.array();
        }
        lowrank.colwise() += res.center;
        Eigen::VectorXd colmeans = lowrank.colwise().sum() / NR;
        output.scores.insert(output.scores.end(), colmeans.begin(), colmeans.end());

        return output;
    }
};

TEST_P(GsdeconSimpleTest, Reference) {
    auto param = GetParam();
    auto rank = std::get<0>(param);
    auto scale = std::get<1>(param);

    gsdecon::Options opt;
    opt.rank = rank;
    opt.scale = scale;
    auto obs = gsdecon::compute(*matrix, opt);

    auto ref = reference(*matrix, rank, scale);
    scran_tests::compare_almost_equal(obs.weights, ref.weights);
    scran_tests::compare_almost_equal(obs.scores, ref.scores);

    // Same results with multiple threads.
    opt.num_threads = 3;
    obs = gsdecon::compute(*matrix, opt);
    scran_tests::compare_almost_equal(obs.weights, ref.weights);
    scran_tests::compare_almost_equal(obs.scores, ref.scores);
}

INSTANTIATE_TEST_SUITE_P(
    Gsdecon,
    GsdeconSimpleTest,
    ::testing::Combine(
        ::testing::Values(1, 5), // number of PCs.
        ::testing::Values(false, true) // with or without scaling
    )
);

/******************************************************
 ******************************************************/

TEST(Gsdecon, EdgeCases) {
    {
        tatami::DenseRowMatrix<double, int> mat(0, 400, std::vector<double>());
        gsdecon::Options opt;
        auto res = gsdecon::compute(mat, opt);
        EXPECT_TRUE(res.weights.empty());
        EXPECT_EQ(res.scores, std::vector<double>(400));
    }

    {
        tatami::DenseRowMatrix<double, int> mat(1, 400, std::vector<double>(400, 1));
        gsdecon::Options opt;
        auto res = gsdecon::compute(mat, opt);
        EXPECT_EQ(res.weights, std::vector<double>(1, 1));
        EXPECT_EQ(res.scores, std::vector<double>(400, 1));
    }

    {
        tatami::DenseRowMatrix<double, int> mat(100, 0, std::vector<double>());
        gsdecon::Options opt;
        auto res = gsdecon::compute(mat, opt);
        EXPECT_EQ(res.weights, std::vector<double>(100));
        EXPECT_TRUE(res.scores.empty());
    }

    {
        tatami::DenseRowMatrix<double, int> mat(100, 200, std::vector<double>(20000));
        gsdecon::Options opt;
        opt.rank = 0;
        auto res = gsdecon::compute(mat, opt);
        EXPECT_EQ(res.scores, std::vector<double>(200));
        EXPECT_EQ(res.weights, std::vector<double>(100));

        res = gsdecon::compute_blocked(mat, static_cast<const int*>(NULL), opt);
        EXPECT_EQ(res.scores, std::vector<double>(200));
        EXPECT_EQ(res.weights, std::vector<double>(100));
    }
}

/******************************************************
 ******************************************************/

class GsdeconBlockTest : public ::testing::TestWithParam<std::tuple<int, bool> > {
protected:
    static inline std::shared_ptr<tatami::NumericMatrix> matrix;

    static void SetUpTestSuite() {
        auto vec = scran_tests::simulate_vector(20000, scran_tests::SimulationParameters());
        matrix.reset(new tatami::DenseRowMatrix<double, int>(50, 400, std::move(vec)));
    }
};

TEST_P(GsdeconBlockTest, Reference) {
    auto param = GetParam();
    auto rank = std::get<0>(param);
    auto scale = std::get<1>(param);

    gsdecon::Options opt;
    opt.rank = rank;
    opt.scale = scale;
    auto ref = gsdecon::compute(*matrix, opt);

    std::vector<int> batch(matrix->ncol());
    auto obs = gsdecon::compute_blocked(*matrix, batch.data(), opt);
    scran_tests::compare_almost_equal(ref.weights, obs.weights);
    scran_tests::compare_almost_equal(ref.scores, obs.scores);
}

TEST_P(GsdeconBlockTest, ShiftSanity) {
    auto param = GetParam();
    auto rank = std::get<0>(param);
    auto scale = std::get<1>(param);
    size_t ncells = matrix->ncol();

    // Shifting everything up in one batch. This should manifest as a
    // corresponding shift in the scores for that batch. 
    const double CONSTANT = 5.6;
    auto added = tatami::make_DelayedUnaryIsometricOperation(matrix, tatami::DelayedUnaryIsometricArithmeticScalar<tatami::ArithmeticOperation::ADD, false, double, double>(CONSTANT));
    auto combined = tatami::make_DelayedBind(std::vector<std::shared_ptr<tatami::NumericMatrix> >{ matrix, added }, false);

    gsdecon::Options opt;
    opt.rank = rank;
    opt.scale = scale;
    auto ref = gsdecon::compute(*matrix, opt);
    double ref_position = std::accumulate(ref.scores.begin(), ref.scores.end(), 0.0) / ncells;

    std::vector<int> batch(ncells * 2);
    std::fill(batch.begin() + ncells, batch.end(), 1);
    auto obs = gsdecon::compute_blocked(*combined, batch.data(), opt);
    scran_tests::compare_almost_equal(ref.weights, obs.weights);
    EXPECT_EQ(obs.scores.size(), ncells * 2);

    // Checking that the recovery of the low-rank approximation worked correctly.
    std::vector<double> first_half(obs.scores.begin(), obs.scores.begin() + ncells);
    double first_position = std::accumulate(first_half.begin(), first_half.end(), 0.0) / ncells;
    for (auto& x : first_half) {
        x += (ref_position - first_position);
    }
    scran_tests::compare_almost_equal(first_half, ref.scores); 

    std::vector<double> second_half(obs.scores.begin() + ncells, obs.scores.end());
    double second_position = std::accumulate(second_half.begin(), second_half.end(), 0.0) / ncells;
    for (auto& x : second_half) {
        x += (ref_position - second_position);
    }
    scran_tests::compare_almost_equal(second_half, ref.scores);

    scran_tests::compare_almost_equal(second_position - first_position, CONSTANT);
}

TEST_P(GsdeconBlockTest, ScaleSanity) {
    auto param = GetParam();
    auto rank = std::get<0>(param);
    auto scale = std::get<1>(param);
    size_t ncells = matrix->ncol();

    // Scaling everything up in one batch. This should manifest as a
    // corresponding scaling in the scores for that batch, regardless
    // of whether options.scale is true or not.
    const double CONSTANT = 1.45;
    auto scaled = tatami::make_DelayedUnaryIsometricOperation(matrix, tatami::DelayedUnaryIsometricArithmeticScalar<tatami::ArithmeticOperation::MULTIPLY, false, double, double>(CONSTANT));
    auto combined = tatami::make_DelayedBind(std::vector<std::shared_ptr<tatami::NumericMatrix> >{ matrix, scaled }, false);

    gsdecon::Options opt;
    opt.rank = rank;
    opt.scale = scale;
    auto ref = gsdecon::compute(*matrix, opt);
    double ref_position = std::accumulate(ref.scores.begin(), ref.scores.end(), 0.0) / ncells;

    std::vector<int> batch(ncells * 2);
    std::fill(batch.begin() + ncells, batch.end(), 1);
    auto obs = gsdecon::compute_blocked(*combined, batch.data(), opt);
    scran_tests::compare_almost_equal(ref.weights, obs.weights);
    EXPECT_EQ(obs.scores.size(), ncells * 2);

    // Checking that the recovery of the low-rank approximation worked correctly.
    std::vector<double> first_half(obs.scores.begin(), obs.scores.begin() + ncells);
    double first_position = std::accumulate(first_half.begin(), first_half.end(), 0.0) / ncells;
    for (auto& x : first_half) {
        x += (ref_position - first_position);
    }
    scran_tests::compare_almost_equal(first_half, ref.scores); 

    std::vector<double> second_half(obs.scores.begin() + ncells, obs.scores.end());
    double second_position = std::accumulate(second_half.begin(), second_half.end(), 0.0) / ncells;
    for (auto& x : second_half) {
        x -= second_position;
        x /= CONSTANT;
        x += ref_position;
    }
    scran_tests::compare_almost_equal(second_half, ref.scores);
}

INSTANTIATE_TEST_SUITE_P(
    Gsdecon,
    GsdeconBlockTest,
    ::testing::Combine(
        ::testing::Values(1, 5), // number of PCs.
        ::testing::Values(false, true) // with or without scaling
    )
);


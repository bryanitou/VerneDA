/**
 * DACE_VSOD (VERY SIMPLE ATTITUDE DETERMINATION): aims to solve a problem where the initial orientation is known and
 * we want to compute the future attitude_ of the spacecraft after a given time using DACE.
 */
// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "scv.h"
#include "integrator.h"
#include "base/constants.h"
#include "problems.h"
#include "delta.h"
#include "tools/io.h"
#include "solids/solid.h"
#include "quaternion.h"

/**
 * Main entry point
 */
int main(int argc, char* argv[])
{
    // Number of variables and order
    int n_var = 7;
    int n_ord = 2;

    // Initialize DACE with 6 variables
    DACE::DA::init(n_ord, n_var);

    // Declare a geometric figure: cylinder
    auto upper_base_center = point(0.0, 0.0, 2.0);
    auto lower_base_center = point(0.0, 0.0, -2.0);
    std::vector<point> point_list = {upper_base_center, lower_base_center};
    auto cylinder = std::make_unique<solid>();
    cylinder->addpoints(point_list);
    double radius = 2.0;

    // Stuff to compute the inertia: EXAMPLE: CYLINDER
    double r = 2;
    double h = 6;
    double mass = 1000;
    double Jx = (mass * h * h) / 12 + (mass * r * r) / 4;
    double Jy = (mass * h * h) / 12 + (mass * r * r) / 4;
    double Jz = (mass * r * r) / 4;

    // Inertia matrix of the object
    double inertia[3][3] = {
            {Jx, 0.0, 0.0},
            {0.0, Jy, 0.0},
            {0.0, 0.0, Jz}
    };

    // Initial conditions of attitude
    double roll = 0.00;
    double pitch = 0.00;
    double yaw = 0.00;

    // Error in attitude
    double error_att = M_PI / 2;
    double stddev_att = 0.1;
    double error_vel = 0.1;
    double stddev_vel = 0.01;

    // Get initial quaternion
    // TODO: Error should be computed here
    auto q_control = quaternion::euler2quaternion(roll, pitch,  yaw);
    auto q_with_error = quaternion::euler2quaternion(roll + error_att, pitch + error_att, yaw + error_att);
    auto q_error = quaternion::q8_multiply(q_with_error, q_control);

    // Declare state control vector without DA
    std::vector<DACE::DA> scv_no_DA = {
            q_control[0],   // q0
            q_control[1],   // q1
            q_control[2],   // q2
            q_control[3],   // q3
            0.01,           // w1 -> rotation around 1 axis
            0.0 ,           // w2 -> rotation around 2 axis
            0.0 };          // w3 -> rotation around 3 axis

    // Declare the state control vector with DA
    std::vector<DACE::DA> scv0 = {
            scv_no_DA[0] +  DACE::DA(1) * q_error[0],   // q0
            scv_no_DA[1] +  DACE::DA(2) * q_error[1],   // q1
            scv_no_DA[2] +  DACE::DA(3) * q_error[2],   // q2
            scv_no_DA[3] +  DACE::DA(4) * q_error[3],   // q3
            scv_no_DA[4] +  DACE::DA(5) * error_vel,    // w1 -> rotation around 1 axis
            scv_no_DA[5] +  DACE::DA(6) * error_vel,    // w2 -> rotation around 2 axis
            scv_no_DA[6] +  DACE::DA(7) * error_vel };  // w3 -> rotation around 3 axis

    // Declare and initialize class
    auto s0 = std::make_unique<scv>(scv0);
    auto s0_no_da = std::make_unique<scv>(scv_no_DA);

    // Now, should initialize all the dace variables from the initial conditions
    auto scv0_DA = s0->get_state_vector_copy();

    // Initial and final time
    // Initial time is zero
    double const t0 = 0.0;

    // How many periods do we want to integrate?
    double const tf = 100;

    // Initialize integrator
    auto objIntegrator = std::make_unique<integrator>(INTEGRATOR::RK4, 0.1);

    // Declare the problem object
    auto prob = problems();

    // Insert the inertia matrix to the problem
    prob.set_inertia_matrix(inertia);

    // Set the problem in the integrator
    objIntegrator->set_problem_object(&prob);

    // Apply integrator
    auto xf_DA =
            objIntegrator->integrate(scv0_DA, t0, tf);

    // Now we have to evaluate the deltas (little displacements in the initial position)
    auto scvf_DA = std::make_shared<scv>(xf_DA);

    // TODO: Add this to the delta set
    //  Debug line
    std::cout << scvf_DA->get_state_vector_copy().cons()<< std::endl;
    std::cout << scvf_DA->get_state_vector_copy().cons().extract(0, 3).vnorm() << std::endl;
    std::cout << xf_DA.cons() << std::endl;
    std::cout << xf_DA.cons().extract(0, 3).vnorm() << std::endl;

    // -----------------------------------------DELTAS ENGINE STUFF ----------------------------------------------------
    // Build deltas class
    auto deltas_engine = std::make_shared<delta>(*scvf_DA, xf_DA);

    // Set mean state
    deltas_engine->set_mean_quaternion_option(q_control);

    // Set distribution
    deltas_engine->set_constants(stddev_att, stddev_vel);

    // Set options for the generator
    deltas_engine->set_bool_option(DELTA_GENERATOR_OPTION::ATTITUDE, true);
    deltas_engine->set_bool_option(DELTA_GENERATOR_OPTION::QUAT2EULER, true);
    deltas_engine->set_sampling_option(QUATERNION_SAMPLING::OMPL_GAUSSIAN);

    // Generate deltas
    deltas_engine->generate_deltas(DISTRIBUTION::GAUSSIAN, 10000);

    // Insert nominal delta
    deltas_engine->insert_nominal(n_var);

    // Evaluate deltas
    deltas_engine->evaluate_deltas();

    // Set output path for results
    std::filesystem::path output_path_avd = "./out/attp/taylor_expression_RK4.avd";
    std::filesystem::path output_eval_deltas_path_dd = "./out/attp/eval_deltas_expression_RK4.dd";
    std::filesystem::path output_non_eval_deltas_path_dd = "./out/attp/non_eval_deltas_expression.dd";

    // Dump final info
    tools::io::dace::dump_algebraic_vector(xf_DA, output_path_avd);

    // Dump non evaluated deltas
    tools::io::dace::dump_non_eval_deltas(deltas_engine.get(), output_non_eval_deltas_path_dd);

    // Dump evaluated deltas
    tools::io::dace::dump_eval_deltas(deltas_engine.get(), output_eval_deltas_path_dd);

    // Prepare arguments for python call
    std::unordered_map<std::string, std::string> py_args = {
            {"file",    output_eval_deltas_path_dd},
            {"plot_type", PYPLOT_ATTITUDE},
            {"metrics", "deg"},
    };


    // Draw plots
    tools::io::plot_variables(PYPLOT_BANANA, py_args, true);
}
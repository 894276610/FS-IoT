// #include "utils-metric.h"
// #include <catch2/catch_test_macros.hpp>


// TEST_CASE(" KPacket could be compared properly", "[single-file]")
// {
//     std::vector<std::string> true_labels = {
//         "amazon echo", "belkin wemo", "tp-link plug", 
//         "amazon echo", "belkin wemo", "tp-link plug"
//     };
    
//     std::vector<std::string> pred_labels = {
//         "amazon echo", "belkin wemo", "belkin wemo", 
//         "amazon echo", "tp-link plug", "tp-link plug"
//     };

//     auto metrics = calculate_metrics(true_labels, pred_labels);

//     REQUIRE( metrics.accuracy == ((double)2)/3);
//     REQUIRE( metrics.macro_f1 == ((double)2)/3);
//     REQUIRE( metrics.micro_f1 == ((double)2)/3);
    
// }
#include <mlpack.hpp>

using namespace mlpack;

int main()
{
    arma::mat dataset;
    // 要添加的新数据（两行，每行 3 个元素）
    arma::mat new_rows = {{7, 8, 9}}; 
    arma::rowvec new_row = {7, 8, 99};  

    // 在最后插入新行（行索引=当前行数）
    dataset.insert_rows(dataset.n_rows, new_row);

    dataset.print("dataset");
}

// int main()
// {
//   arma::mat dataset;
//   data::Load("/home/kunling/BurstIoT/covertype-small.csv", dataset);

//   dataset.head_cols(1).print("dataset");
  
//   std::cout << "---" << std::endl;
//   // Labels are the last row.\n",
//   // The dataset stores labels from 1 through 7, but we need 0 through 6\n",
//   // (in mlpack labels are zero-indexed), so we subtract 1.\n",
//   arma::Row<size_t> labels = arma::conv_to<arma::Row<size_t>>::from(dataset.row(dataset.n_rows - 1)) - 1;
//   dataset.shed_row(dataset.n_rows - 1);

//   arma::mat trainSet, testSet;
//   arma::Row<size_t> trainLabels, testLabels;

//   // Split dataset randomly into training set and test set.\n",
//   data::Split(dataset, labels, trainSet, testSet, trainLabels, testLabels, 0.3 
//       /* Percentage of dataset to use for test set. */);

//   RandomForest<> rf(trainSet, trainLabels, 7 /* Number of classes in dataset */, 10 /* 10 trees */);
//   // Predict the labels of the test points.,
//   arma::Row<size_t> output;
//   rf.Classify(testSet, output);
//   // Now print the accuracy. The 'probabilities' output could also be used to\n",
//   // generate an ROC curve.\n",
//   const size_t correct = arma::accu(output == testLabels);
//   std::cout << "first 1w prediction:" << output[10000] << "first 1w label:" << testLabels[10000] << std::endl;
//   std::cout << correct
//             << " correct out of "
//             << testLabels.n_elem << "\n"
//             << 100.0 * correct / testLabels.n_elem
//             << "%)." << std::endl;
// }
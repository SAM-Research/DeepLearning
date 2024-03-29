//
// Created by dewe on 12/9/21.
//

//#define DEBUG_VISION

#include "vision/conv_net.h"
#include "basic/fcnn.h"
#include "common/builder.h"

// Where to find the MNIST dataset.
const char* kDataRoot = "examples/data/mnist";

// The batch size for training.
const int64_t kTrainBatchSize = 64;

// The batch size for testing.
const int64_t kTestBatchSize = 1000;

// The number of epochs to train.
const int64_t kNumberOfEpochs = 10;

// After how many batches to log a new update with the loss value.
const int64_t kLogInterval = 10;


template <typename DataLoader>
void train(
        size_t epoch,
        torch::nn::Sequential model,
        torch::Device device,
        DataLoader& data_loader,
        torch::optim::Optimizer& optimizer,
        size_t dataset_size) {
    model->train();
    size_t batch_idx = 0;
    for (auto& batch : data_loader) {
        auto data = batch.data.to(device), targets = batch.target.to(device);
        optimizer.zero_grad();
        auto output = model->forward(data);
        auto loss = torch::cross_entropy_loss(output, targets);
        AT_ASSERT(!std::isnan(loss.template item<float>()));
        loss.backward();
        optimizer.step();

        if (batch_idx++ % kLogInterval == 0) {
            std::printf(
                    "\rTrain Epoch: %ld [%5ld/%5ld] Loss: %.4f",
                    epoch,
                    batch_idx * batch.data.size(0),
                    dataset_size,
                    loss.template item<float>());
        }
    }
}

template <typename DataLoader>
void test(
        torch::nn::Sequential model,
        torch::Device device,
        DataLoader& data_loader,
        size_t dataset_size) {
    torch::NoGradGuard no_grad;
    model->eval();
    double test_loss = 0;
    int32_t correct = 0;
    for (const auto& batch : data_loader) {
        auto data = batch.data.to(device), targets = batch.target.to(device);
        auto output = model->forward(data);
        test_loss += torch::cross_entropy_loss(
                output,
                targets,
                /*weight=*/{},
                torch::Reduction::Sum)
                .template item<float>();
        auto pred = output.argmax(1);
        correct += pred.eq(targets).sum().template item<int64_t>();
    }

    test_loss /= dataset_size;
    std::printf(
            "\nTest set: Average loss: %.4f | Accuracy: %.3f\n",
            test_loss,
            static_cast<double>(correct) / dataset_size);
}


int main(){

    torch::manual_seed(1);

    torch::DeviceType device_type = torch::kCPU;
//    if (torch::cuda::is_available()) {
//        std::cout << "CUDA available! Training on GPU." << std::endl;
//        device_type = torch::kCUDA;
//    } else {
//        std::cout << "Training on CPU." << std::endl;
//        device_type = torch::kCPU;
//    }
    torch::Device device(device_type);

    sam_dn::InputShapes shapes;
    sam_dn::Builder modelbuilder;
    auto modules = modelbuilder.compile("examples/model/mnist_resnet.yaml", shapes);

    torch::nn::Sequential model = modules["mnist"];

//    sam_dn::CNNOption cnn_opt;
//    cnn_opt.activations = {"relu", "relu"};
//    cnn_opt.filters = {10, 20};
//    cnn_opt.kernels = {5, 5};
//    cnn_opt.strides = {1, 1};
//    cnn_opt.padding = {"valid", "valid"};
//    cnn_opt.Input({1, 28, 28});
//    sam_dn::CNN cnn(cnn_opt);
//
//    sam_dn::FCNNOption opt;
//    opt.dims = {50, 10};
//    opt.act_fn = "relu";
//    opt.Input( { torch::tensor(cnn->outputSize()).prod(0).item<long>() } );

//    model->push_back("feature_extractor", cnn);
//    model->push_back("predictor", sam_dn::FCNN(sam_dn::FCNNOption(opt)));

    model->to(device);
    std::cout << *model << "\n";

    auto train_dataset = torch::data::datasets::MNIST(kDataRoot)
            .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
            .map(torch::data::transforms::Stack<>());
    const size_t train_dataset_size = train_dataset.size().value();
    auto train_loader =
            torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
                    std::move(train_dataset), kTrainBatchSize);

    auto test_dataset = torch::data::datasets::MNIST(
            kDataRoot, torch::data::datasets::MNIST::Mode::kTest)
            .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
            .map(torch::data::transforms::Stack<>());
    const size_t test_dataset_size = test_dataset.size().value();
    auto test_loader =
            torch::data::make_data_loader(std::move(test_dataset), kTestBatchSize);

    torch::optim::SGD optimizer(
            model->parameters(), torch::optim::SGDOptions(0.01).momentum(0.5));

    for (size_t epoch = 1; epoch <= kNumberOfEpochs; ++epoch) {
        train(epoch, model, device, *train_loader, optimizer, train_dataset_size);
        test(model, device, *test_loader, test_dataset_size);
    }
}
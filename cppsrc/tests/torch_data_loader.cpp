#include <torch/torch.h>
#include <iostream>

/** Experimenting with torch data loaders
 * We have a torch tensor as the data
 * How do we load it into a torch data loader?
 */

class TensorDataset : public torch::data::Dataset<TensorDataset> {
	private:
		torch::Tensor data_;
		torch::Tensor targets_;
	public:
		TensorDataset(torch::Tensor data, torch::Tensor targets) {
			data_ = data;
			targets_ = targets;
		}

		torch::data::Example<> get(size_t index) override {
			return {data_[index], targets_[index]};
		}

		torch::optional<size_t> size() const override {
			return data_.size(0);
		}
};

int main() {

	int M = 10; // Dataset size
	int kBatchSize = 5;
	
	torch::Tensor data = torch::rand({M,3,3}); // 2 channel image fo 3x3
	torch::Tensor targets = torch::rand({M, 3}); // 3 targets for each data
	std::cout << data << std::endl;
	std::cout << targets << std::endl;

	auto dataset = TensorDataset(data, targets).map(torch::data::transforms::Stack<>());
	auto data_loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
			std::move(dataset),
			torch::data::DataLoaderOptions().batch_size(kBatchSize).workers(2).enforce_ordering(false));

	for (torch::data::Example<>& batch : *data_loader) {
		std::cout << "Batch size: " << batch.data.size(0) << " | Labels: ";
		for (int64_t i = 0; i < batch.data.size(0); ++i) {
			std::cout << batch.target[i] << " ";
		}
		std::cout << std::endl;
	}
	// In a for loop you can now use your data.
	/* for (auto& batch : data_loader) { */
	/* 	auto data = batch.data; */
	/* 	auto labels = batch.target; */
	/* 	std::cout << "Batch data: " << data << std::endl; */
	/* 	std::cout << "Batch labels: " << labels << std::endl; */
	/* } */
}

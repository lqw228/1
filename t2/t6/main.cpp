#include <vector>
#include <stdio.h>
template<typename T>
class CircularQueue 
{
public:
	CircularQueue(const size_t& size) : size_(size + 1), read_index_(0), write_index_(0) {
		//data_.reserve(size_);
		data_.resize(size_);
	}
	bool Push(const T& element) {
		size_t next_index = (write_index_ + 1) % size_;
		if (next_index == read_index_) {
			return false;
		} else {
			data_[write_index_] = element;
			write_index_ = next_index;
		}
	}
	bool Pop(T* element) {
		if (read_index_ == write_index_) {
			return false;
		} else {
			*element = data_[read_index_];
			read_index_ = (read_index_ + 1) % size_;

		}
	}
private:
	size_t read_index_;
	size_t write_index_;
	size_t size_;
	std::vector<T> data_;
};

int main(int argc, char** argv) 
{
	CircularQueue<int> circular_queue(100);
	int index = 0;
	while (circular_queue.Push(index)) 
	{
		printf("%d\n", index);
		index++;
	}
	int value;
	while (circular_queue.Pop(&value)) 
	{
		printf("--%d--\n", value);
	}
}
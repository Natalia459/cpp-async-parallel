#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#include <future>
#include "log_duration.h"

using namespace std;
int amount = 0;

template <typename It>
void PrintRange(It range_begin, It range_end) {
	for (auto it = range_begin; it != range_end; ++it)
	{
		cout << *it << " "s;
	}
	cout << endl;
}

template <typename RandomIt>
void MergeSortSeq(RandomIt range_begin, RandomIt range_end) {
	// 1. Если диапазон содержит меньше 2 элементов, выходим из функции
	int range_length = range_end - range_begin;
	if (range_length < 2) {
		return;
	}

	// 2. Создаём вектор, содержащий все элементы текущего диапазона
	vector elements(range_begin, range_end);
	// Тип элементов — typename iterator_traits<RandomIt>::value_type

	// 3. Разбиваем вектор на две равные части
	auto mid = elements.begin() + range_length / 2;

	// 4. Вызываем функцию MergeSort от каждой половины вектора
	MergeSortSeq(elements.begin(), mid);
	MergeSortSeq(mid, elements.end());

	// 5. С помощью алгоритма merge сливаем отсортированные половины
	// в исходный диапазон
	merge(elements.begin(), mid, mid, elements.end(), range_begin);
}

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
	// 1. Если диапазон содержит меньше 2 элементов, выходим из функции
	int range_length = range_end - range_begin;

	if (range_length < 2) { return; }

	// 2. Разбиваем вектор на две равные части
	auto mid = range_begin + range_length / 2;

	// 3. Вызываем функцию MergeSort от каждой половины вектора
	if (amount < 3)
	{
		++amount;
		auto first_range = async([=] {return MergeSort<RandomIt>(range_begin, mid); });
		MergeSort(mid, range_end);

		first_range.get();
		--amount;
	}
	else
	{
		MergeSort(range_begin, mid);
		MergeSort(mid, range_end);
	}

	// 4. С помощью алгоритма inplace_merge сливаем отсортированные половины в исходный диапазон
	std::inplace_merge(range_begin, mid, range_end);
}

int main() {
	mt19937 generator;
	vector<int> test_vector(4'000'000);

	iota(test_vector.begin(), test_vector.end(), 1);
	shuffle(test_vector.begin(), test_vector.end(), generator);

	{
		LOG_DURATION_STREAM("MergeSortSeq", cerr);
		MergeSortSeq(test_vector.begin(), test_vector.end());
	}

	{
		LOG_DURATION_STREAM("MergeSort", cerr);
		// Проверка можно ли передать указатели
		MergeSort(test_vector.data(), test_vector.data() + test_vector.size());
	}

	return 0;
}

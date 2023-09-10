// I wrote this during a practice coding interview.

#include <cmath>
#include <iostream>
#include <set>

using Position = long;

bool isInRange(Position house, Position station, Position radius) {
	return std::abs(house - station) <= radius;
}

size_t placeStations(const std::set<Position> &houses, Position radius) {
	if (houses.empty())
		return 0;

	size_t station_count = 1;

	auto iter = houses.begin();

	Position current_position = *iter++ + radius;

	while (iter != houses.end()) {
		const Position house_position = *iter++;
		if (isInRange(house_position, current_position, radius))
			continue;
		current_position = house_position + radius;
		++station_count;
	}

	return station_count;
}

Position getRadius(const std::set<Position> &houses, size_t max_stations) {
	if (houses.empty())
		return 0;

	const auto max_radius = static_cast<Position>(std::ceil(static_cast<double>(*houses.rbegin() - *houses.begin())
		/ max_stations / 2.0));

	Position lower = 0;
	Position upper = max_radius;
	Position radius = max_radius / 2;

	for (;;) {
		size_t stations = placeStations(houses, radius);

		const Position old_radius = radius;

		if (stations > max_stations) {
			lower = old_radius;
		} else {
			upper = old_radius;
		}

		radius = lower + (upper - lower) / 2;

		if (radius == old_radius)
			return radius;
	}
}

// 1, 2, 7, 10, 15, radius = 2

int main() {
	std::cout << getRadius({1, 2, 7, 10, 15, 16, 17, 18, 20, 200}, 3) << '\n';
}

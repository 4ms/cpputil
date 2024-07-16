#pragma once

template<typename T, int num_updates>
struct InterpParam {
	InterpParam()
		: cur_val{0}
		, step_size{0} {
		static_assert(num_updates > 0);
	}

	void set_new_value(T new_val) {
		T d = new_val - cur_val;
		step_size = d / num_updates;
	}

	T next() {
		cur_val += step_size;
		return cur_val;
	}

	T get_step_size() {
		return step_size;
	}

	T cur_val;

private:
	T step_size;
};

template<typename T>
struct InterpParamVariable {
	InterpParamVariable(unsigned num_updates = 256)
		: cur_val{0}
		, step_size{0}
		, num_updates{num_updates} {
	}

	void set_new_value(T new_val) {
		T d = new_val - cur_val;
		step_size = d / T(num_updates);
	}

	T next() {
		cur_val += step_size;
		return cur_val;
	}

	T get_step_size() {
		return step_size;
	}

	void set_num_updates(unsigned num_updates) {
		this->num_updates = num_updates;
	}

	T cur_val;

private:
	T step_size;
	unsigned num_updates;
};

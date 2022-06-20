#include "csv.h"
csv_op csv_op_init(sscan sscan_op, print print_op, size_t struct_size) {
	csv_op parrayop = {
		.sscan_from = sscan_op,
		.print_to = print_op,
		.struct_size = struct_size,
	};
	return parrayop;
}

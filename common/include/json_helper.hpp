#pragma once

#include "Poco/JSON/Parser.h"

namespace js = Poco::JSON;


namespace engine
{
	using records_array_type = js::Array;
	using records_array_ptr_type = js::Array::Ptr;

	std::string serialize(const records_array_type& records);
	records_array_ptr_type deserialize(const std::string& dump);
	records_array_ptr_type create_records_array_ptr();
}
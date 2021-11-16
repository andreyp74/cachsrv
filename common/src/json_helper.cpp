
#include "Poco/Logger.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/JSONException.h"

#include "json_helper.hpp"

using namespace Poco::JSON;
using Poco::Logger;


namespace engine
{
	std::string serialize(const records_array_type& records)
	{
		std::ostringstream oss;
		Stringifier::stringify(records, oss);
		return oss.str();
	}

	records_array_ptr_type deserialize(const std::string& dump)
	{
		Parser parser;
		Poco::DynamicAny result;
		try
		{
			result = parser.parse(dump);
			return result.extract<Array::Ptr>();
		}
		catch (JSONException& err)
		{
			Logger::get("cachesrv").error(err.message());
		}
		return Array::Ptr();
	}

	records_array_ptr_type create_records_array_ptr()
	{
		return js::Array::Ptr(new js::Array());
	}
}
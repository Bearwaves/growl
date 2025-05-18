#include "growl/core/network/http.h"
#include "growl/core/error.h"
#include "zlib.h"
#include <cstring>

using Growl::HttpMethod;
using Growl::Result;

std::string Growl::httpMethodString(HttpMethod method) {
	switch (method) {
	case HttpMethod::GET:
		return "GET";
	case HttpMethod::POST:
		return "POST";
	case HttpMethod::PUT:
		return "PUT";
	case HttpMethod::PATCH:
		return "PATCH";
	case HttpMethod::OPTIONS:
		return "OPTIONS";
	case HttpMethod::HEAD:
		return "HEAD";
	case HttpMethod::CONNECT:
		return "CONNECT";
	case HttpMethod::TRACE:
		return "TRACE";
	}
}

Result<std::string> Growl::gzip(std::string& data) {
	z_stream zs;
	memset(&zs, 0, sizeof(zs));
	if (deflateInit2(
			&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 | 16, 8,
			Z_DEFAULT_STRATEGY) != Z_OK) {
		return Error(
			std::make_unique<GenericError>("Failed to init zlib stream"));
	}
	zs.next_in = reinterpret_cast<Bytef*>(data.data());
	zs.avail_in = data.size();

	int ret;
	char outbuffer[10240];
	std::string outstring;

	do {
		zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
		zs.avail_out = sizeof(outbuffer);

		ret = deflate(&zs, Z_FINISH);

		if (outstring.size() < zs.total_out) {
			outstring.append(outbuffer, zs.total_out - outstring.size());
		}
	} while (ret == Z_OK);

	deflateEnd(&zs);

	if (ret != Z_STREAM_END) {
		return Error(
			std::make_unique<GenericError>(
				"Failed to gzip: " + std::string(zs.msg)));
	}

	return outstring;
}

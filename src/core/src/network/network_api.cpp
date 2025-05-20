#include "growl/core/api/network_api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/error.h"
#include <sodium.h>
#include <zlib.h>

using Growl::Error;
using Growl::NetworkAPI;
using Growl::NetworkAPIInternal;
using Growl::Result;

Error NetworkAPIInternal::initCrypto() {
	if (sodium_init() < 0) {
		return std::make_unique<GenericError>("Failed to init crypto");
	}
	return nullptr;
}

std::vector<unsigned char>
NetworkAPI::hmac256(std::string& body, std::string& key) {
	unsigned char out[crypto_auth_hmacsha256_BYTES];

	crypto_auth_hmacsha256_state state;
	crypto_auth_hmacsha256_init(
		&state, reinterpret_cast<const unsigned char*>(key.data()), key.size());
	crypto_auth_hmacsha256_update(
		&state, reinterpret_cast<const unsigned char*>(body.data()),
		body.size());
	crypto_auth_hmacsha256_final(&state, out);

	return std::vector<unsigned char>(out, out + crypto_auth_hmacsha256_BYTES);
}

std::string NetworkAPI::base64enc(const unsigned char* data, int len) {
	auto buf = std::vector<char>(
		sodium_base64_encoded_len(len, sodium_base64_VARIANT_ORIGINAL));
	if (!sodium_bin2base64(
			buf.data(), buf.size(), data, len,
			sodium_base64_VARIANT_ORIGINAL)) {
		return "";
	}
	return std::string(buf.data());
}

Result<std::string> NetworkAPI::gzip(std::string& data) {
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

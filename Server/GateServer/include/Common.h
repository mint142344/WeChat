#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <fmt/core.h>
#include <fmt/base.h>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = net::ip::tcp;
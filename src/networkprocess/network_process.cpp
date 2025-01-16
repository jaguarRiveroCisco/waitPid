#include "network_process.h"
#include <asio.hpp>
#include <thread>
#include <unistd.h>
#include "logger_instance.h"

namespace process
{
    void network()
    {
        try
        {
            asio::io_context io_context;

            // Resolve the host
            asio::ip::tcp::resolver               resolver(io_context);
            asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("www.example.com", "80");

            // Create a socket and connect to the host
            asio::ip::tcp::socket socket(io_context);
            asio::connect(socket, endpoints);

            // Form the HTTP request
            std::string request = "GET / HTTP/1.1\r\n";
            request += "Host: www.example.com\r\n";
            request += "Connection: close\r\n\r\n";

            // Send the request
            asio::write(socket, asio::buffer(request));

            // Read the response
            asio::streambuf response;
            asio::read_until(socket, response, "\r\n");

            // Parse the HTTP response status line
            std::istream response_stream(&response);
            std::string  http_version;
            response_stream >> http_version;
            unsigned int status_code;
            response_stream >> status_code;
            std::string status_message;
            std::getline(response_stream, status_message);

            if (!response_stream || http_version.substr(0, 5) != "HTTP/")
            {
                tools::LoggerManager::getInstance() << "Invalid response";
                tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
                return;
            }
            if (status_code != 200)
            {
                tools::LoggerManager::getInstance() << "Response returned with status code " << status_code;
                tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
                return;
            }

            // Read the headers
            asio::read_until(socket, response, "\r\n\r\n");
            std::string header;
            while (std::getline(response_stream, header) && header != "\r")
            {
                tools::LoggerManager::getInstance() << header;
                tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
            }

            // Write the remaining response to the output
            if (response.size() > 0)
            {
                std::istream remaining_stream(&response);
                std::string  remaining_content((std::istreambuf_iterator<char>(remaining_stream)),
                                               std::istreambuf_iterator<char>());
                tools::LoggerManager::getInstance() << remaining_content;
                tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
            }

            // Read until EOF
            asio::error_code error;
            while (asio::read(socket, response, asio::transfer_at_least(1), error))
            {
                std::istream response_stream(&response);
                std::string  content((std::istreambuf_iterator<char>(response_stream)),
                                     std::istreambuf_iterator<char>());
                tools::LoggerManager::getInstance() << content;
                tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
            }
            if (error != asio::error::eof)
            {
                throw asio::system_error(error);
            }
        }
        catch (const std::exception &e)
        {
            tools::LoggerManager::getInstance() << "Exception: " << e.what();
            tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
        }
    }

    void NetworkProcess::work()
    {
        preWork();
        network();
        postWork();
        return;
        // while(continue_)
        {
            CURL *curl = curl_easy_init();
            if (curl)
            {
                CURLcode res;
                curl_easy_setopt(curl, CURLOPT_URL, "http://www.example.com");
                res = curl_easy_perform(curl);

                if (res != CURLE_OK)
                {
                    tools::LoggerManager::getInstance() << "curl_easy_perform() failed: " << curl_easy_strerror(res);
                    tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
                }
                else
                {
                    tools::LoggerManager::getInstance() << "Request successful.";
                    tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
                }

                curl_easy_cleanup(curl);
            }
            else
            {
                std::cerr << "Failed to initialize curl." << std::endl;
                tools::LoggerManager::getInstance() << "Failed to initialize curl.";
                tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Simulate some work
        }
        postWork();
    }

}
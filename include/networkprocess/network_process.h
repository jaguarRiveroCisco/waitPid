#pragma once
// network_process.h
#pragma once
#include <curl/curl.h>
#include <iostream>
#include <stdexcept>
#include "process.h"

namespace process
{

    class NetworkProcess : public Process 
    {
    public:
        ~NetworkProcess() override = default;

        void work() override
        {
            preWork();
            CURL *curl = curl_easy_init();
            if (curl)
            {
                CURLcode res;
                curl_easy_setopt(curl, CURLOPT_URL, "http://www.example.com");
                res = curl_easy_perform(curl);

                if (res != CURLE_OK)
                {
                    std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                }
                else
                {
                    std::cout << "Request successful." << std::endl;
                }

                curl_easy_cleanup(curl);
            }
            else
            {
                std::cerr << "Failed to initialize curl." << std::endl;
            }
            postWork();
        }
    };

} // namespace process

#include <iostream>
#include <curl/curl.h>
#include <string>
#include <nlohmann/json.hpp>
#include <random>
#include <stdexcept>
#include <vector>
#include <stdio.h>

using json = nlohmann::json;

// Callback function to write received data into our string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

// wtf, allat just to random
namespace utils {
    std::mt19937 rng(std::random_device{}());

    std::string random_choice(const std::vector<std::string>& vec) {
        if (vec.empty()) throw std::invalid_argument("empty vector");
        std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);
        return vec[dist(rng)];
    }
}

struct IpOutput {
    std::string ip;
    std::string hostname;
    std::string city;
    std::string region;
    std::string country;
    std::string loc;
    std::string organization;
    std::string zip;
    std::string timezone;
};

namespace toriel {
    void get(std::string ip) {
        curl_global_init(CURL_GLOBAL_ALL);

        CURL* curl = curl_easy_init();

        std::string responseString;
        json data;

        IpOutput output;

        if (curl) {
            std::string url = "https://ipinfo.io/" + ip + "/json";
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:149.0) Gecko/20100101 Firefox/149.0");
            // Tell libcurl how to write the data
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

            // Perform the request
            CURLcode res = curl_easy_perform(curl);

            std::cout << "Trying to contact IpInfo.io..." << "\n\n";

            if (res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            }
            else {
                data = json::parse(responseString);

                output.ip = data.value("ip", "");
                output.hostname = data.value("hostname", "");
                output.city = data.value("city", "");
                output.region = data.value("region", "");
                output.country = data.value("country", "");
                output.loc = data.value("loc", "");
                output.organization = data.value("org", "");
                output.zip = data.value("postal", "");
                output.timezone = data.value("timezone", "");
                    
                std::cout << "IP           : " << output.ip << "\n";
                std::cout << "Hostname     : " << output.hostname << "\n";
                std::cout << "City         : " << output.city << "\n";
                std::cout << "Region/State : " << output.region << "\n";
                std::cout << "Country      : " << output.country << "\n";
                std::cout << "LOC          : " << output.loc << "\n";
                std::cout << "Organization : " << output.organization << "\n";
                std::cout << "Zip Code     : " << output.zip << "\n";
                std::cout << "Timezone     : " << output.timezone << std::endl;
            }
            // Cleanup the handle
            curl_easy_cleanup(curl);
        }

        // Final global cleanup
        curl_global_cleanup();
    }
}

int main() {
    std::string ip;

    std::cout << "Enter an IP: ";
    std::cin >> ip;

    toriel::get(ip);
    return 0;
}
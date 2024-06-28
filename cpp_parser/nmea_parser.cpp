#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cstring>
#include <curl/curl.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Utility function to split a string by a delimiter
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(std::move(token));
    }
    return tokens;
}

// Function to calculate the checksum of an NMEA sentence
unsigned char calculate_checksum(const std::string& sentence) {
    unsigned char checksum = 0;
    for (char ch : sentence) {
        checksum ^= ch;
    }
    return checksum;
}

// Function to parse GGA sentence
json parse_gga(const std::vector<std::string>& fields) {
    if (fields.size() < 13) {
        std::cerr << "Invalid GGA sentence format: insufficient fields" << std::endl;
        return json({});
    }
    return {
        {"Type", "GGA"},
        {"Time", fields[1]},
        {"Latitude", fields[2] + " " + fields[3]},
        {"Longitude", fields[4] + " " + fields[5]},
        {"Fix Quality", fields[6]},
        {"Number of Satellites", fields[7]},
        {"Horizontal Dilution", fields[8]},
        {"Altitude", fields[9] + " " + fields[10]},
        {"Geoid Height", fields[11] + " " + fields[12]}
    };
}

// Function to parse GLL sentence
json parse_gll(const std::vector<std::string>& fields) {
    if (fields.size() < 7) {
        std::cerr << "Invalid GLL sentence format: insufficient fields" << std::endl;
        return json({});
    }
    return {
        {"Type", "GLL"},
        {"Latitude", fields[1] + " " + fields[2]},
        {"Longitude", fields[3] + " " + fields[4]},
        {"Time", fields[5]},
        {"Status", fields[6]}
    };
}

// Function to perform HTTP POST request to Node.js server
CURLcode perform_http_post(const json& data) {
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize libcurl!" << std::endl;
        return CURLE_FAILED_INIT;
    }

    // URL for POST request, replace with your server endpoint
    std::string url = "http://localhost:3000/receive_data";

    // Set URL for HTTP POST request
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Set HTTP POST data (convert JSON to string)
    std::string json_data = data.dump();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());

    // Perform HTTP POST request
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    }

    // Clean up libcurl resources
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return res;
}

// Main function to parse NMEA sentences and perform HTTP POST requests
void process_nmea_sentences(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Could not open the file!" << std::endl;
        return;
    }

    std::string line;
   // Inside process_nmea_sentences function
while (std::getline(infile, line)) {
    // Parse NMEA sentence and extract attributes
    std::vector<std::string> fields = split(line, ',');
    if (fields.empty()) {
        std::cerr << "Empty line encountered." << std::endl;
        continue;
    }
    if (fields[0].size() < 3) {
        std::cerr << "Invalid NMEA sentence format: " << fields[0] << std::endl;
        continue;
    }
    std::string sentence_type = fields[0].substr(2); // Ensure fields[0] has at least 3 characters

    if (sentence_type == "GGA") {
        json j = parse_gga(fields);
        if (!j.empty()) {
            // Print parsed JSON to console
            std::cout << "Parsed GGA JSON: " << j.dump() << std::endl;

            // Perform POST request with JSON data
            perform_http_post(j);
        }
    } else if (sentence_type == "GLL") {
        json j = parse_gll(fields);
        if (!j.empty()) {
            // Print parsed JSON to console
           std::cerr << "Parsed GLL JSON: " << j.dump() << std::endl << std::flush;

            // Perform POST request with JSON data
            perform_http_post(j);
        }
    }
}

}

int main() {
    std::ifstream infile("nmea.txt");
    if (!infile) {
        std::cerr << "Could not open the file!" << std::endl;
        return 1;
    }

    process_nmea_sentences("nmea.txt");
    return 0;
}

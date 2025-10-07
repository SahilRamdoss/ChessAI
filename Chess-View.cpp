#include <curl/curl.h>
#include <iostream>

int main() {
    CURL* curl = curl_easy_init();
    if(curl) {
        std::cout << "libcurl is working!\n";
        curl_easy_cleanup(curl);
    } else {
        std::cout << "Failed to init libcurl\n";
    }
    return 0;
}

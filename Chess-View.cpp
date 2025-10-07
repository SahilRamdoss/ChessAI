#include <iostream>
#include <string>
#include <curl/curl.h>
#include "nlohmann-json.hpp"

using json = nlohmann::json;

// Callback to collect data from libcurl
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

int main()
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    // Example FEN: starting position
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    curl = curl_easy_init();
    if (curl)
    {
        // URL-encode the FEN string
        char *encodedFEN = curl_easy_escape(curl, fen.c_str(), fen.length());
        std::string url = "https://lichess.org/api/cloud-eval?fen=" + std::string(encodedFEN);
        curl_free(encodedFEN);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // User-Agent required by Lichess
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "ChessProjectBot/1.0");

        // WARNING: only for local testing. Disables SSL cert verification.
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: "
                      << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            try
            {
                json j = json::parse(readBuffer);

                // Check if pvs exists and is not empty
                if (!j.contains("pvs") || j["pvs"].empty())
                {
                    std::cerr << "No evaluation returned." << std::endl;
                }
                else
                {
                    std::string bestMove = j["pvs"][0]["moves"];
                    int eval = 0;
                    if (j["pvs"][0].contains("cp"))
                        eval = j["pvs"][0]["cp"];

                    std::cout << "Best move: " << bestMove << std::endl;
                    std::cout << "Evaluation (centipawns): " << eval << std::endl;
                }
            }
            catch (json::parse_error &e)
            {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Failed to initialize curl." << std::endl;
    }

    return 0;
}

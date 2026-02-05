#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static size_t write_cb(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* out = static_cast<std::string*>(userdata);
    out->append(ptr, size * nmemb);
    return size * nmemb;
}

static bool has_flag(const std::vector<std::string>& args, const std::string& key) {
    for (const auto& a : args) if (a == key) return true;
    return false;
}

static bool parse_arg(const std::vector<std::string>& args, const std::string& key, std::string& value) {
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == key) {
            if (i + 1 >= args.size()) return false;
            value = args[i + 1];
            return true;
        }
    }
    return false;
}

static void usage(const char* argv0) {
    std::cerr
        << "ppp0_authorize_demo\n\n"
        << "Equivalent of:\n"
        << "  curl http://ttft.uxp.ru/api/pump/authorize -H \"Content-Type: application/json\" \\\n"
        << "    -d '{\"CardUid\":3000,\"PumpControllerUid\":1}'\n\n"
        << "but forces egress via a chosen interface (default: ppp0) using libcurl CURLOPT_INTERFACE.\n\n"
        << "Usage:\n"
        << "  " << argv0 << " [--url URL] [--ppp-if IFACE] [--card UID] [--pump UID]\n"
        << "         [--timeout-sec N] [--insecure]\n";
}

int main(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) args.emplace_back(argv[i]);

    if (has_flag(args, "-h") || has_flag(args, "--help")) {
        usage(argv[0]);
        return 0;
    }

    std::string url = "http://ttft.uxp.ru/api/pump/authorize";
    std::string iface = "ppp0";
    std::string card_uid = "3000";
    std::string pump_uid = "1";
    std::string timeout_s = "30";
    bool insecure = has_flag(args, "--insecure");

    (void)parse_arg(args, "--url", url);
    (void)parse_arg(args, "--ppp-if", iface);
    (void)parse_arg(args, "--card", card_uid);
    (void)parse_arg(args, "--pump", pump_uid);
    (void)parse_arg(args, "--timeout-sec", timeout_s);

    std::ostringstream body;
    body << "{\"CardUid\":" << card_uid << ",\"PumpControllerUid\":" << pump_uid << "}";

    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        std::cerr << "curl_global_init failed\n";
        return 2;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "curl_easy_init failed\n";
        curl_global_cleanup();
        return 2;
    }

    std::string response;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.str().c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)body.str().size());

    // Key requirement: force egress via ppp0 even if eth0 is default route
    curl_easy_setopt(curl, CURLOPT_INTERFACE, iface.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "orange-pi-ppp0-demo/1.0");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, std::stol(timeout_s));

    if (insecure) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }

    CURLcode rc = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    if (rc != CURLE_OK) {
        std::cerr << "Request failed: " << curl_easy_strerror(rc) << "\n";
        std::cerr << "HTTP code (if any): " << http_code << "\n";
        std::cerr << "Interface: " << iface << "\n";
        std::cerr << "URL: " << url << "\n";
        std::cerr << "Body: " << body.str() << "\n";
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    std::cout << "HTTP " << http_code << "\n";
    std::cout << response << "\n";

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return (http_code >= 200 && http_code < 300) ? 0 : 3;
}

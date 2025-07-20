#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include <optional>

// =================================================================================
// Data Structures to represent the Gemini API parameters
// =================================================================================

struct GenerationConfig {
    std::optional<float> temperature;
    std::optional<float> topP;
    std::optional<int> topK;
    std::optional<int> candidateCount;
    std::optional<int> maxOutputTokens;
    std::optional<int> thinking_budget;
    // Note: stopSequences is a vector of strings, omitted for brevity in this minimal example
};

struct SafetySetting {
    std::string category;
    std::string threshold;
};

// For this minimal example, we'll only support the codeExecution tool.
struct Tool {
    bool enableCodeExecution = false;
};


// =================================================================================
// Helper and Core Functions
// =================================================================================

// Callback function to write the response from libcurl to a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    } catch (std::bad_alloc& e) {
        // Handle memory allocation errors
        return 0;
    }
    return newLength;
}

// Function to read the API key from a file
std::string readApiKey(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open API key file: " << filename << std::endl;
        return "";
    }
    std::string apiKey;
    std::getline(file, apiKey);
    return apiKey;
}

/**
 * @brief Queries the Gemini API with extended parameters.
 *
 * @param model The model to use (e.g., "gemini-2.5-pro").
 * @param prompt The text prompt to send to the model.
 * @param apiKey Your Gemini API key.
 * @param config Optional generation configuration.
 * @param safetySettings Optional vector of safety settings.
 * @param tools Optional tool configuration.
 * @return The response from the API as a string.
 */
std::string queryGemini(
    const std::string& model,
    const std::string& prompt,
    const std::string& apiKey,
    const std::optional<GenerationConfig>& config,
    const std::optional<std::vector<SafetySetting>>& safetySettings,
    const std::optional<Tool>& tool)
{
    CURL* curl;
    CURLcode res;
    std::string responseString;
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + model + ":generateContent";

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        // =========================================================================
        // Construct the JSON request body dynamically
        // For a real-world application, a dedicated JSON library is recommended
        // =========================================================================
        std::stringstream jsonStream;
        jsonStream << "{";
        
        // 1. Add mandatory contents
        jsonStream << "\"contents\":[{\"parts\":[{\"text\":\"" << prompt << "\"}]}]";

        // 2. Add optional generationConfig
        if (config) {
            jsonStream << ",\"generationConfig\": {";
            bool first = true;
            if (config->temperature) { jsonStream << "\"temperature\":" << *config->temperature; first = false; }
            if (config->topP) { if (!first) jsonStream << ","; jsonStream << "\"topP\":" << *config->topP; first = false; }
            if (config->topK) { if (!first) jsonStream << ","; jsonStream << "\"topK\":" << *config->topK; first = false; }
            if (config->candidateCount) { if (!first) jsonStream << ","; jsonStream << "\"candidateCount\":" << *config->candidateCount; first = false; }
            if (config->maxOutputTokens) { if (!first) jsonStream << ","; jsonStream << "\"maxOutputTokens\":" << *config->maxOutputTokens; }
            if (config->thinking_budget) {
                if (!first) jsonStream << ",";
                jsonStream << "\"thinkingConfig\":{\"thinking_budget\":" << *config->thinking_budget << "}";
            }
            jsonStream << "}";
        }
        
        // 3. Add optional safetySettings
        if (safetySettings && !safetySettings->empty()) {
            jsonStream << ",\"safetySettings\": [";
            for (size_t i = 0; i < safetySettings->size(); ++i) {
                jsonStream << "{\"category\":\"" << (*safetySettings)[i].category << "\",";
                jsonStream << "\"threshold\":\"" << (*safetySettings)[i].threshold << "\"}";
                if (i < safetySettings->size() - 1) {
                    jsonStream << ",";
                }
            }
            jsonStream << "]";
        }

        // 4. Add optional tools
        if (tool && tool->enableCodeExecution) {
            jsonStream << ",\"tools\": [{\"codeExecution\": {}}]";
        }

        jsonStream << "}";
        std::string jsonData = jsonStream.str();
        // std::cout << "DEBUG JSON Request: " << jsonData << std::endl; // Uncomment for debugging

        // Set the headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string authHeader = "x-goog-api-key: " + apiKey;
        headers = curl_slist_append(headers, authHeader.c_str());

        // Set libcurl options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

        // Perform the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return responseString;
}


int main() {
    std::string apiKey = readApiKey("api_key.txt");
    if (apiKey.empty()) {
        return 1;
    }

    // --- Setup the request parameters ---
    std::string model = "gemini-2.5-pro";
    std::string prompt = "Explain why the sky is blue in a simple paragraph.";

    // 1. Configure generation for a more deterministic and shorter response
    GenerationConfig config;
    config.thinking_budget = 4096;

    // To turn off thinking:
    // config.thinking_budget = 0;

    // To use dynamic thinking:
    // config.thinking_budget = -1;
    
    // 2. Configure safety settings to block dangerous content
    std::vector<SafetySetting> safety;
    safety.push_back({"HARM_CATEGORY_DANGEROUS_CONTENT", "BLOCK_ONLY_HIGH"});

    // 3. Enable the code execution tool
    Tool tool;
    tool.enableCodeExecution = true; // Set to true to allow the model to use its code interpreter

    // --- Make the API call with all parameters ---
    std::string response = queryGemini(model, prompt, apiKey, config, safety, tool);

    std::cout << "Response from Gemini API:" << std::endl;
    std::cout << response << std::endl;

    return 0;
}
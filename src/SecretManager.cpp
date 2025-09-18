#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <filesystem>
#include "../include/SecretManager.hpp"  


namespace fs = std::filesystem;

    std::unordered_map<std::string, std::string> secrets;
    std::string secrets_path;

    SecretManager::SecretManager(const std::string& path = "/run/secrets/") : secrets_path(path) {}
    
    void SecretManager::load_secret(const std::string& secret_name) {
        std::string full_path = secrets_path + secret_name;
        
        if (!fs::exists(full_path)) {
            throw std::runtime_error("Secret file not found: " + full_path);
        }
        
        std::ifstream file(full_path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open secret file: " + full_path);
        }
        
        std::string value;
        std::getline(file, value);
        
        // Remove trailing whitespace
        value.erase(value.find_last_not_of(" \n\r\t") + 1);
        
        secrets[secret_name] = value;
        file.close();
    }
    
    void SecretManager::load_all_secrets() {
        if (!fs::exists(secrets_path)) {
            throw std::runtime_error("Secrets directory not found: " + secrets_path);
        }
        
        for (const auto& entry : fs::directory_iterator(secrets_path)) {
            if (entry.is_regular_file()) {
                std::string secret_name = entry.path().filename();
                load_secret(secret_name);
            }
        }
    }
    
    std::string SecretManager::get_secret(const std::string& secret_name) {
        auto it = secrets.find(secret_name);
        if (it == secrets.end()) {
            throw std::runtime_error("Secret not loaded: " + secret_name);
        }
        return it->second;
    }
    
    bool SecretManager::has_secret(const std::string& secret_name) const {
        return secrets.find(secret_name) != secrets.end();
    }

    const std::unordered_map<std::string, std::string>& SecretManager::get_all_secrets() const {
        return secrets;
    };
    

#ifndef SECRET_HPP  
#define SECRET_HPP 

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <stdexcept>

class SecretManager {

private:
    std::unordered_map<std::string, std::string> secrets;
    std::string secrets_path;

public:
    SecretManager(const std::string& path);
    void load_secret(const std::string& secret_name);
    void load_all_secrets();
    std::string get_secret(const std::string& secret_name);
    bool has_secret(const std::string& secret_name)const;
    const std::unordered_map<std::string, std::string>& get_all_secrets() const ;
};

#endif // --SECRET_HPP

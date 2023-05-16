/**
 * @file Header_Info.h
 * @brief Contains common information for all Dynamo programs
 */
#pragma once

#include <string>

// Functions
class HeaderInfo
{

public: // CONSTRUCTOR

    /**
     * @brief Builds header info object with the compilation information
     * @param program_name              [in] [std::string]
     * @param code_version              [in] [std::string]
     * @param git_branch                [in] [std::string]
     * @param git_hash                  [in] [std::string]
     * @param user_name                 [in] [std::string]
     * @param os_version                [in] [std::string]
     * @param date                      [in] [std::string]
     * @param time                      [in] [std::string]
     */
    explicit HeaderInfo(std::string program_name, std::string code_version, std::string git_branch,
                        std::string git_hash, std::string user_name,
                        std::string os_version, std::string date, std::string time);

private: // PROPERTIES

    // Local copies of the STATIC_FLAGS
    std::string local_flag_program_name{};
    std::string local_flag_code_version{};
    std::string local_flag_git_branch{};
    std::string local_flag_git_hash{};
    std::string local_flag_user_name{};
    std::string local_flag_os_version{};
    std::string local_flag_date{};
    std::string local_flag_time{};

public: // METHODS (static queries)

    // Get hostname
    static std::string get_hostname();

    // Get current username
    static std::string get_username();

    // Get current date
    static std::string get_current_date();

public: // METHODS

    /**
     * @brief Builds the DYNAMO banner
     * @return std::string
     */
    static std::string get_banner();

    /**
     * @brief Builds a huge std::string with all the compilation information
     * @return std::String
     */
    std::string get_compilation_info();

    /**
    * @brief Prints in stdout the compilation info
    */
    void print_compilation_info();

    /**
     * @brief Builds a huge std::string with all the header information
     * @return std::String
     */
    std::string get_header_info();

    /**
     * @brief Prints in stdout the header info (banner + compilation info)
     */
    void print_header_info();

private: // METHODS

    /**
     * @brief Gets sudo greetings
     * @return std::string
     */
    static std::string get_sudo_greeting();

    /**
     * @brief Gets program name
     * @return std::string
     */
    std::string get_program_name()
    {
        return this->local_flag_program_name;
    };

    /**
    * @brief Gets code version
    * @return std::string
    */
    std::string get_code_version()
    {
        return this->local_flag_code_version;
    };

    /**
    * @brief Gets git branch
    * @return std::string
    */
    std::string get_git_branch()
    {
        return this->local_flag_git_branch;
    };

    /**
    * @brief Gets git hash
    * @return std::string
    */
    std::string get_git_hash()
    {
        return this->local_flag_git_hash;
    };

    /**
    * @brief Gets user name
    * @return std::string
    */
    std::string get_user_name()
    {
        return this->local_flag_user_name;
    };

    /**
    * @brief Gets os version
    * @return std::string
    */
    std::string get_os_version()
    {
        return this->local_flag_os_version;
    };

    /**
    * @brief Gets compilation date
    * @return std::string
    */
    std::string get_date()
    {
        return this->local_flag_date;
    };

    /**
    * @brief Gets time
    * @return std::string
    */
    std::string get_time()
    {
        return this->local_flag_time;
    };

};

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

#include "httplib.h"
#include <myhtml/api.h>

struct AttributeData {
	std::map<std::string, std::string> data;
};

struct InputData {
	myhtml_tree_node_t* node = nullptr;
	AttributeData attributes;
};

struct FormData {
	myhtml_tree_node_t* node = nullptr;
	AttributeData attributes;
	std::vector<InputData> inputs;
};

enum class RequestFormValid { Valid, Invalid, None };

struct LoginRequest {
	RequestFormValid mValid = RequestFormValid::None;
	std::string endpointAttribute;
	std::string requestType; // POST
	std::string usernameAttribute;
	std::string passwordAttribute;
};

AttributeData collect_node_attr(myhtml_tree_node_t* node);

void collect_tree_data(myhtml_tree_t* tree, myhtml_tree_node_t* node, size_t inc, std::vector<FormData>& formData);
void collect_tree_data(myhtml_tree_t* tree, myhtml_tree_node_t* node, size_t inc, std::vector<InputData>& inputData);
std::string ExtractAttribute(const std::vector<InputData>& inputs, const std::string attName);
std::vector<std::string> ExtractMultipleAttributes(const std::vector<InputData>& inputs, const std::vector<std::string> guesses);
std::vector<std::string> ExtractUsernameAttribute(const std::vector<InputData>& inputs);
std::vector<std::string> ExtractPasswordAttribute(const std::vector<InputData>& inputs);

std::vector<LoginRequest> FormulateLoginRequest(FormData input);
std::vector<FormData> ProcessFormData(myhtml_tree_t* tree, myhtml_tree_node_t* node);

std::vector<LoginRequest> FormulateLoginRequests(const std::vector<FormData> forms);
std::vector<LoginRequest> ScanForLogin(std::string inputHTML);

std::string GetLoginWebsite(const std::string targetURL, const std::string endpoint);

std::vector<std::string> AttemptLogin(const std::string targetURL, const std::vector<LoginRequest> candidateRequests);

void task();
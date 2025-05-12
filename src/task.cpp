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

AttributeData collect_node_attr(myhtml_tree_node_t* node)
{
	// copy-pasted from myhtml library examples, with small modifications
	AttributeData output;

	myhtml_tree_attr_t* attr = myhtml_node_attribute_first(node);

	while (attr) {
		const char* name = myhtml_attribute_key(attr, NULL);

		if (name) {
			const char* value = myhtml_attribute_value(attr, NULL);
			output.data.emplace(std::make_pair(std::string(name), std::string(value)));
		}

		attr = myhtml_attribute_next(attr);
	}

	return output;
}

void collect_tree_data(myhtml_tree_t* tree, myhtml_tree_node_t* node, size_t inc, std::vector<FormData>& formData)
{
	// copy-pasted from myhtml library examples, with small modifications
	while (node)
	{
		const char* tag_name = myhtml_tag_name_by_id(tree, myhtml_node_tag_id(node), NULL);

		if (tag_name == std::string("form")) {
			formData.emplace_back(FormData{ node });
		}

		myhtml_tag_id_t tag_id = myhtml_node_tag_id(node);

		// print children
		collect_tree_data(tree, myhtml_node_child(node), (inc + 1), formData);
		node = myhtml_node_next(node);
	}
}

void collect_tree_data(myhtml_tree_t* tree, myhtml_tree_node_t* node, size_t inc, std::vector<InputData>& inputData)
{
	// copy-pasted from myhtml library examples, with small modifications
	while (node)
	{
		// print current element
		const char* tag_name = myhtml_tag_name_by_id(tree, myhtml_node_tag_id(node), NULL);

		if (tag_name == std::string("input")) {
			inputData.emplace_back(InputData{ node });
		}

		myhtml_tag_id_t tag_id = myhtml_node_tag_id(node);

		// print children
		collect_tree_data(tree, myhtml_node_child(node), (inc + 1), inputData);
		node = myhtml_node_next(node);
	}
}

std::string ExtractAttribute(const std::vector<InputData>& inputs, const std::string attName) {
	for (const auto& item : inputs) {
		auto& attributes = item.attributes;
		for (const auto& attItem : attributes.data) {
			if (attItem.second == attName) {
				return attName;
			}
		}
	}
	return "";
}

std::vector<std::string> ExtractMultipleAttributes(const std::vector<InputData>& inputs, const std::vector<std::string> guesses) {
	std::vector<std::string> output;
	for (const auto name : guesses) {
		std::string attr = ExtractAttribute(inputs, name);
		if (attr.size()) {
			output.emplace_back(attr);
		}
	}
	return output;
}

std::vector<std::string> ExtractUsernameAttribute(const std::vector<InputData>& inputs) {
	std::vector<std::string> possibleUsernameAttributes = { "uname", "username" };
	return ExtractMultipleAttributes(inputs, possibleUsernameAttributes);
}

std::vector<std::string> ExtractPasswordAttribute(const std::vector<InputData>& inputs) {
	std::vector<std::string> possiblePasswordAttributes = { "password", "pass" };
	return ExtractMultipleAttributes(inputs, possiblePasswordAttributes);
}

std::vector<LoginRequest> FormulateLoginRequest(FormData input) {
	auto actionName = input.attributes.data.find("name");

	if (actionName == input.attributes.data.cend()) {
		return {};
	}

	if (actionName->second != "loginform") {
		return {};
	}

	const std::string method = input.attributes.data.find("method")->second; // post
	const std::string endpoint = input.attributes.data.find("action")->second; // endpoint

	const auto usernames = ExtractUsernameAttribute(input.inputs);
	const auto password = ExtractPasswordAttribute(input.inputs);

	std::vector<LoginRequest> output;

	for (const auto u : usernames) {
		for (const auto p : password) {
			output.emplace_back(LoginRequest{ RequestFormValid::Valid, endpoint, "POST", u, p });
		}
	}

	return output;
}

std::vector<FormData> ProcessFormData(myhtml_tree_t* tree, myhtml_tree_node_t* node) {
	std::vector<FormData> output;

	// collect data about forms
	collect_tree_data(tree, myhtml_node_child(node), 0, output);

	// collect the forms' attributes
	for (auto& item : output) {
		item.attributes = collect_node_attr(item.node);

		std::vector<InputData> inputs;
		collect_tree_data(tree, item.node, 0, inputs);

		item.inputs = inputs;
		for (auto& inputItem : item.inputs) {

			inputItem.attributes = collect_node_attr(inputItem.node);
		}
	}

	return output;
}

std::vector<LoginRequest> FormulateLoginRequests(const std::vector<FormData> forms) {
	std::vector<LoginRequest> output;
	for (auto& item : forms) {
		auto loginRequests = FormulateLoginRequest(item);

		for (const auto req : loginRequests) {
			switch (req.mValid) {
			case RequestFormValid::Valid: {
				output.emplace_back(req);
				break;
			}
			case RequestFormValid::Invalid: {
				break;
			}
			default: {
				break;
			}
			}
		}
	}

	return output;
}

std::vector<LoginRequest> ScanForLogin(std::string inputHTML) {

	myhtml_t* myhtml = myhtml_create();
	myhtml_init(myhtml, MyHTML_OPTIONS_DEFAULT, 1, 0);

	// first tree init
	myhtml_tree_t* tree = myhtml_tree_create();
	myhtml_tree_init(tree, myhtml);

	// parse html
	myhtml_parse(tree, MyENCODING_ISO_8859_2, inputHTML.c_str(), strlen(inputHTML.c_str()));

	myhtml_tree_node_t* node = myhtml_tree_get_document(tree);

	try {
		const std::vector<FormData> formData = ProcessFormData(tree, node);
		const std::vector<LoginRequest> output = FormulateLoginRequests(formData);
		return output;
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
	}

	myhtml_tree_destroy(tree);
	myhtml_destroy(myhtml);

	return {};
}

std::string GetLoginWebsite(const std::string targetURL, const std::string endpoint) {

	httplib::Client cli(targetURL);

	auto res = cli.Get("/" + endpoint);

	if (res.error() != httplib::Error::Success) {
		std::cout << to_string(res.error()) << std::endl;
		return "";
	}

	if (res->status != httplib::OK_200) {
		throw std::runtime_error(std::to_string(res->status));
	}

	return res->body;
}

std::vector<std::string> AttemptLogin(const std::string targetURL, const std::vector<LoginRequest> candidateRequests) {
	std::vector<std::string> output;

	httplib::Client cli(targetURL);
	
	// following the website's suggestion, could be a list of "common" usernames and passwords
	const std::string username = "test";
	const std::string password = "test";

	for (const auto login : candidateRequests) {

		std::stringstream ss;
		ss << login.usernameAttribute.c_str() << "=" << username.c_str() << "&" << login.passwordAttribute.c_str() << "=" << password.c_str();

		const std::string encodedInputs = ss.str();

		const auto loginResult = cli.Post("/" + login.endpointAttribute, encodedInputs.c_str(), "application/x-www-form-urlencoded");

		// we logged in - woo!
		if ((loginResult->status == httplib::OK_200) && loginResult->body.size()) {
			output.emplace_back(loginResult->body);
		}
	}

	return output;
}

void task() {
	const std::string targetURL = "http://testphp.vulnweb.com";

	const auto loginHTML = GetLoginWebsite(targetURL, "login.php");

	const auto loginCandidates = ScanForLogin(loginHTML);

	const auto results = AttemptLogin(targetURL, loginCandidates);

	// display the HTML, if any were obtained
	for (const auto& res : results) {
		std::cout << res << std::endl;
	}
}


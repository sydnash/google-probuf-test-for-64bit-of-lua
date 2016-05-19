#include <iostream>
#include <fstream>
#include <string>
#include "addressbook.pb.h"
using namespace std;

// This function fills in a Person message based on user input.
void PromptForAddress(tutorial::Person* person) {
	cout << "Enter person ID number: ";
	int id;
	cin >> id;
	person->set_id(id);
	cin.ignore(256, '\n');

	cout << "Enter name: ";
	getline(cin, *person->mutable_name());

	cout << "Enter email address (blank for none): ";
	string email;
	getline(cin, email);
	if (!email.empty()) {
		person->set_email(email);
	}

	while (true) {
		cout << "Enter a phone number (or leave blank to finish): ";
		string number;
		getline(cin, number);
		if (number.empty()) {
			break;
		}

		tutorial::Person::PhoneNumber* phone_number = person->add_phone();
		phone_number->set_number(number);

		cout << "Is this a mobile, home, or work phone? ";
		string type;
		getline(cin, type);
		if (type == "mobile") {
			phone_number->set_type(tutorial::Person::MOBILE);
		}
		else if (type == "home") {
			phone_number->set_type(tutorial::Person::HOME);
		}
		else if (type == "work") {
			phone_number->set_type(tutorial::Person::WORK);
		}
		else {
			cout << "Unknown phone type.  Using default." << endl;
		}
	}
}

// Main function:  Reads the entire address book from a file,
//   adds one person based on user input, then writes it back out to the same
//   file.
int main(int argc, char* argv[]) {
	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	if (argc != 2) {
		cerr << "Usage:  " << argv[0] << " ADDRESS_BOOK_FILE" << endl;
		//return -1;
	}

	{
		auto infile = "testproto.txt";
		fstream input(infile, std::fstream::in | std::fstream::binary);
		tutorial::Person person;
		input.seekg(0, input.end);
		int length = input.tellg();
		input.seekg(0, input.beg);

		char * buffer = new char[length + 1];
		input.read(buffer, length);
		std::string tmp(buffer, length);
		person.ParseFromArray(buffer, length);
		printf("%s", person.DebugString().c_str());
	}

	auto outfile = "myaddressbook.txt";
	tutorial::AddressBook address_book;

	{
		// Read the existing address book.
		fstream input(outfile, ios::in | ios::binary);
		if (!input) {
			cout << outfile << ": File not found.  Creating a new file." << endl;
		}
		else if (!address_book.ParseFromIstream(&input)) {
			cerr << "Failed to parse address book." << endl;
			return -1;
		}
		printf("%s", address_book.DebugString().c_str());
	}

	// Add an address.
	PromptForAddress(address_book.add_person());

	{
		// Write the new address book back to disk.
		fstream output(outfile, ios::out | ios::trunc | ios::binary);
		std::string outStr = address_book.DebugString();
		printf("%s", outStr.c_str());
		if (!address_book.SerializeToOstream(&output)) {
			cerr << "Failed to write address book." << endl;
			return -1;
		}
	}

	// Optional:  Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}